#include <openssl/ssl.h>
#include "../include/http_handler.h"

int send_all(client_t *client, const char *content, size_t content_len) {
    size_t total_sent = 0;
    int sent;
    while(total_sent < content_len) {
        sent =
            client->is_https
            ? SSL_write(client->ssl, content + total_sent, content_len - total_sent)
            : send(client->sock, content + total_sent, content_len - total_sent, 0);
        if(sent <= 0) break;
        total_sent += sent;
    }
    return total_sent;
}