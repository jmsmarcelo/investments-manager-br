#ifndef SERVER_SETUP_H
#define SERVER_SETUP_H

#include <openssl/ssl.h>
#include "server_status.h"
#include "server_def.h"

status_t sock_setup(void);
void sock_setup_cleanup(void);

socket_t get_listener(void);
socket_t get_writer(void);
socket_t get_sock_http(void);
socket_t get_sock_https(void);

SSL_CTX *get_ssl_context(void);

int get_port_http(void);
int get_port_https(void);

#endif /* SERVER_SETUP_H */
