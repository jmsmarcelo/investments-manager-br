#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include "server_def.h"

int send_all(client_t *client, const char *content, size_t content_size);

#endif