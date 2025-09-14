#include <stdlib.h>
#include <openssl/err.h>
#ifdef _WIN32
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif
#include "../include/server_setup.h"
#include "../include/server_log.h"

static int port_http = 0;
static int port_https = 0;
static const char *CERT_FILE = NULL;
static const char *KEY_FILE = NULL;

void port_init(void) {
    CERT_FILE = getenv("CERT_FILE");
    KEY_FILE = getenv("KEY_FILE");
    const char *http_env = getenv("HTTP_PORT");
    const char *https_env = getenv("HTTPS_PORT");
    if(http_env) {
        int port = atoi(http_env);
        if(port > 0 && port <= 65535) port_http = port;
    }
    if(CERT_FILE && KEY_FILE && https_env) {
        int port = atoi(https_env);
        if(port > 0 && port <= 65535) port_https = port;
    }
    if(port_http == 0 && port_https == 0) {
        port_http = 8080;
    }
}

#ifdef _WIN32
static WSADATA wsa_data;
static int wsa_initialized = 0;
static const char opt_on = 1;
static const char opt_off = 0;
#else
static int opt_on = 1;
static int opt_off = 0;
#endif

static socket_t listener = INVALID_SOCK,
                writer = INVALID_SOCK,
                sock_http = INVALID_SOCK,
                sock_https = INVALID_SOCK;

static SSL_CTX *ssl_context = NULL;

status_t sock_setup(void) {
    port_init();


#ifdef _WIN32
    if(WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        return ERROR_INIT_WINSOCKET;
    }
    wsa_initialized = 1;
#endif

    struct sockaddr_in6 sock_addr = {
        .sin6_family = AF_INET6, .sin6_addr = in6addr_any
    };

    if(port_http > 0) {
        sock_http = socket(AF_INET6, SOCK_STREAM, 0);
        if(sock_http == INVALID_SOCK) {
            return ERROR_CREATING_SOCK;
        }

        setsockopt(sock_http, IPPROTO_IPV6, IPV6_V6ONLY, &opt_off, sizeof(opt_off));
        setsockopt(sock_http, SOL_SOCKET, SO_REUSEADDR, &opt_on, sizeof(opt_on));

        sock_addr.sin6_port = htons(port_http);

        if(bind(sock_http, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0) {
            return ERROR_ASSOCIATING_SOCK_WITH_PORT;
        }

        if(listen(sock_http, SOMAXCONN) < 0) {
            return ERROR_LISTENING_SOCK_FOR_CONNECTIONS;
        }

        log_write("[INFO] HTTP server started on port %d", port_http);
    }
    if(port_https > 0) {
        SSL_library_init();
        SSL_load_error_strings();

        ssl_context = SSL_CTX_new(TLS_server_method());
        if(!ssl_context) {
            return COULD_NOT_CREATE_SSL_CTX;
        }
        if(SSL_CTX_use_certificate_file(ssl_context, CERT_FILE, SSL_FILETYPE_PEM) <= 0) {
            return ERROR_LOADING_CERT;
        }
        if(SSL_CTX_use_PrivateKey_file(ssl_context, KEY_FILE, SSL_FILETYPE_PEM) <= 0) {
            return ERROR_LOADING_KEY;
        }

        sock_https = socket(AF_INET6, SOCK_STREAM, 0);
        if(sock_https == INVALID_SOCK) {
            return ERROR_CREATING_SOCK;
        }

        setsockopt(sock_https, IPPROTO_IPV6, IPV6_V6ONLY, &opt_off, sizeof(opt_off));
        setsockopt(sock_https, SOL_SOCKET, SO_REUSEADDR, &opt_on, sizeof(opt_on));

        sock_addr.sin6_port = htons(port_https);

        if(bind(sock_https, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0) {
            return ERROR_ASSOCIATING_SOCK_WITH_PORT;
        }

        if(listen(sock_https, SOMAXCONN) < 0) {
            return ERROR_LISTENING_SOCK_FOR_CONNECTIONS;
        }

        log_write("[INFO] HTTPS server started on port %d", port_https);
    }

    /* Wakeup channel via UDP loopback para fecha o servidor corretamente */
    listener = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr = {
        .sin_family = AF_INET, .sin_addr.s_addr = htonl(INADDR_LOOPBACK), .sin_port = 0
    };
    bind(listener, (struct sockaddr *)&addr, sizeof(addr));
    socklen_t addrlen = sizeof(addr);
    getsockname(listener, (struct sockaddr *)&addr, &addrlen);
    writer = socket(AF_INET, SOCK_DGRAM, 0);
    connect(writer, (struct sockaddr *)&addr, addrlen);

    return STATUS_OK;
}

socket_t get_listener(void) {
    return listener;
}
socket_t get_writer(void) {
    return writer;
}
socket_t get_sock_http(void) {
    return sock_http;
}
socket_t get_sock_https(void) {
    return sock_https;
}
SSL_CTX *get_ssl_context(void) {
    return ssl_context;
}

int get_port_http(void) {
    return port_http;
}
int get_port_https(void) {
    return port_https;
}

void sock_setup_cleanup(void) {
    if(ssl_context) SSL_CTX_free(ssl_context);
    if(listener != INVALID_SOCK) CLOSE_SOCK(listener);
    if(writer != INVALID_SOCK) CLOSE_SOCK(writer);
    if(sock_http != INVALID_SOCK) CLOSE_SOCK(sock_http);
    if(sock_https != INVALID_SOCK) CLOSE_SOCK(sock_https);
    ssl_context = NULL;
    listener = INVALID_SOCK;
    writer = INVALID_SOCK;
    sock_http = INVALID_SOCK;
    sock_https = INVALID_SOCK;

#ifdef _WIN32
    if(wsa_initialized) WSACleanup();
    wsa_initialized = 0;
#endif
}
