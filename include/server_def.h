#ifndef SERVER_DEF_H
#define SERVER_DEF_H

/**
 * @file    server_def.h
 * @brief   Arquivo central de macros e definições globais.
 *
 */

/* Gera enum a partir de X-macro */
#define GENERATE_ENUM(ENUM_NAME, MACRO_NAME) \
    typedef enum ENUM_NAME { \
        MACRO_NAME(GENERATE_ENUM_CASE) \
        MACRO_NAME##_MAX \
    } ENUM_NAME##_t;

#define GENERATE_ENUM_CASE(code, msg) code,

/**
 * @brief   Gera um array de strings associado a um enum
 * 
 * @note    O enum correspondente deve já ter sido definido.
 */
#define GENERATE_ARRAY(ARRAY_NAME, MACRO_NAME) \
    static const char *ARRAY_NAME[MACRO_NAME##_MAX] = { \
        MACRO_NAME(GENERATE_ARRAY_CASE) \
    };

#define GENERATE_ARRAY_CASE(code, msg) [code] = msg,

/* Definições de abstração para compatibilidade entre sistemas */
#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define INVALID_SOCK INVALID_SOCKET
#define SOCK_ERR WSAGetLastError
#define CLOSE_SOCK closesocket
#define THANDLE _In_ LPTHREAD_START_ROUTINE
typedef SOCKET socket_t;
typedef LPVOID targ_t;
typedef HANDLE thread_t;
typedef CRITICAL_SECTION mutex_t;
typedef CONDITION_VARIABLE cond_t;
#else
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <pthread.h>
typedef int socket_t;
#define INVALID_SOCK -1
#define SOCK_ERR errno
#define CLOSE_SOCK close
#define THANDLE void*
typedef void* targ_t;
typedef pthread_t thread_t;
typedef pthread_mutex_t mutex_t;
typedef pthread_cond_t cond_t;
#endif

static inline void thread_create(thread_t *t, THANDLE h, targ_t a) {
#ifdef _WIN32
    *t = CreateThread(NULL, 0, h, a, 0, NULL);
#else
    pthread_create(t, NULL, h, a);
#endif
}
static inline void thread_join(thread_t t) {
#ifdef _WIN32
    WaitForSingleObject(t, INFINITE); CloseHandle(t);
#else
    pthread_join(t, NULL);
#endif
}
static inline void mutex_init(mutex_t *m) {
#ifdef _WIN32
    InitializeCriticalSection(m);
#else
    pthread_mutex_init(m, NULL);
#endif
}
static inline void mutex_lock(mutex_t *m) {
#ifdef _WIN32
    EnterCriticalSection(m);
#else
    pthread_mutex_lock(m);
#endif
}
static inline void mutex_unlock(mutex_t *m) {
#ifdef _WIN32
    LeaveCriticalSection(m);
#else
    pthread_mutex_unlock(m);
#endif
}
static inline void cond_init(cond_t *c) {
#ifdef _WIN32
    InitializeConditionVariable(c);
#else
    pthread_cond_init(c, NULL);
#endif
}
static inline void cond_wait(cond_t *c, mutex_t *m) {
#ifdef _WIN32
    SleepConditionVariableCS(c, m, INFINITE);
#else
    pthread_cond_wait(c, m);
#endif
}
static inline void cond_signal(cond_t *c) {
#ifdef _WIN32
    WakeConditionVariable(c);
#else
    pthread_cond_signal(c);
#endif
}
static inline void cond_broadcast(cond_t *c) {
#ifdef _WIN32
    WakeAllConditionVariable(c);
#else
    pthread_cond_broadcast(c);
#endif
}

#include <openssl/ssl.h>
#include <postgresql/libpq-fe.h>

typedef struct {
    char ip[INET6_ADDRSTRLEN];
    int port;
    int is_https;
    SSL *ssl;
    socket_t sock;
    PGconn *pg;
    char buffer[4096];
} client_t;

#endif /* SERVER_DEF_H */