#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#endif
#include "../include/server_tpool.h"
#include "../include/server_def.h"
#include "../include/server_log.h"
#include "../include/server_handler.h"

static int pool_len = 10;

void pool_len_init(void) {
    const char *const env = getenv("POOL_LEN");
    if(env != NULL) {
        int len = atoi(env);
        if(len > 0 && len <= 999 && len != pool_len) pool_len = len;
    }
}

typedef struct {
    int is_free;    // 1 = livre, 0 = ocupado
    mutex_t mutex;
    cond_t cond;
    client_t client; 
} slot_t;

static thread_t *threads = NULL;
static slot_t *slots = NULL;

static mutex_t global_mutex;
static cond_t global_cond;
static int slots_free;
static int stop = 0;

#ifdef _WIN32
DWORD WINAPI tpool_handle(LPVOID arg)
#else
void *tpool_handle(void *arg)
#endif
{
    int idx = *((int *)arg);
    free(arg);

    slot_t *slot = &slots[idx];

    while(!stop) {
        mutex_lock(&slot->mutex);
        while(slot->is_free && !stop) {
            cond_wait(&slot->cond, &slot->mutex);
        }
        if(stop) {
            mutex_unlock(&slot->mutex);
            break;
        }

        handle_client(&slot->client);

        slot->is_free = 1;
        mutex_lock(&global_mutex);
        slots_free++;
        cond_broadcast(&global_cond);
        mutex_unlock(&global_mutex);
        mutex_unlock(&slot->mutex);
    }
    return
#ifdef _WIN32
        0;
#else
        NULL;
#endif
}

status_t tpool_init(void) {
    mutex_init(&global_mutex);
    cond_init(&global_cond);

    threads = malloc(sizeof(thread_t) * pool_len);
    slots = malloc(sizeof(slot_t) * pool_len);
    if(!threads || !slots) return FAILED_ALLOC;

    slots_free = pool_len;
    for(int i = 0; i < pool_len; i++) {
        slots[i].is_free = 1;
        mutex_init(&slots[i].mutex);
        cond_init(&slots[i].cond);

        slots[i].client.pg = PQconnectdb("");   // Todas as configurações do PostgreSQL devem estar em variáveis de ambiente
        if(PQstatus(slots[i].client.pg) != CONNECTION_OK) {
            log_write("[ERROR] %s", PQerrorMessage(slots[i].client.pg));
            return PG_ERROR;
        }

        int *arg = malloc(sizeof(int));
        *arg = i;
        thread_create(&threads[i], tpool_handle, arg);
    }
    return STATUS_OK;
}

status_t tpool_put(socket_t sock, int is_https) {
    status_t status = STATUS_OK;
    mutex_lock(&global_mutex);
    while(slots_free == 0 && !stop) {
        cond_wait(&global_cond, &global_mutex);
    }
    if(stop) {
        goto ENDPOOL;
    }

    for(int i = 0; i < pool_len; i++) {
        if(!slots[i].is_free) {
            continue;
        }
        mutex_lock(&slots[i].mutex);
        struct sockaddr_storage addr;
        socklen_t addrlen = sizeof(addr);
        slots[i].client.sock = accept(sock, (struct sockaddr *)&addr, &addrlen);
        if(slots[i].client.sock == INVALID_SOCK) {
            status = CLIENT_ACCEPT_FAILURE;
        } else {
            slots[i].client.is_https = is_https;
            client_addr_info(&slots[i].client, &addr);
            slots[i].is_free = 0;
            slots_free--;
            cond_broadcast(&slots[i].cond);
        }
        mutex_unlock(&slots[i].mutex);
        break;
    }

ENDPOOL:
    mutex_unlock(&global_mutex);
    return status;
}

status_t tpool_destroy(void) {
    stop = 1;

    for(int i = 0; i < pool_len; i++) {
        cond_broadcast(&slots[i].cond);
        mutex_lock(&slots[i].mutex);
        PQfinish(slots[i].client.pg);
        mutex_unlock(&slots[i].mutex);
        thread_join(threads[i]);
    }

    if(slots) free(slots);
    if(threads) free(threads);

    return STATUS_OK;
}