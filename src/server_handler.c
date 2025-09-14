#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../include/server_handler.h"
#include "../include/server_setup.h"
#include "../include/http_handler.h"
#include "../include/server_log.h"

void log_write_ssl_errors(void) {
    unsigned long err;
    char buf[256];
    while((err = ERR_get_error()) != 0) {
        ERR_error_string_n(err, buf, sizeof(buf));
        log_write("[WARN] %s", buf);
    }
}

#ifdef _WIN32
DWORD sock_timeout = 10000;
#else
struct timeval sock_timeout = { .tv_sec = 10, .tv_usec = 0};
#endif

void handle_client(client_t *client) {
    setsockopt(client->sock, SOL_SOCKET, SO_RCVTIMEO,
#ifdef _WIN32
        (const char *)
#endif
        &sock_timeout, sizeof(sock_timeout));
    setsockopt(client->sock, SOL_SOCKET, SO_SNDTIMEO,
#ifdef _WIN32
        (const char *)
#endif
        &sock_timeout, sizeof(sock_timeout));

    if(client->is_https) {
        client->ssl = SSL_new(get_ssl_context());
        SSL_set_fd(client->ssl, client->sock);
        if(SSL_accept(client->ssl) <= 0) {
            log_write_ssl_errors();
            goto CLEANUP;
        }
    }

    const char content[] =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 2\r\n"
        "Connection: close\r\n\r\n"
        "OK";

    send_all(client, content, strlen(content));

CLEANUP:
    if(client->is_https) {
        SSL_shutdown(client->ssl);
        SSL_free(client->ssl);
    }
    CLOSE_SOCK(client->sock);
}

void client_addr_info(client_t *client, struct sockaddr_storage *addr) {
    if(addr->ss_family == AF_INET) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        inet_ntop(AF_INET, &addr4->sin_addr, client->ip, sizeof(client->ip));
    }
}