#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include "server_def.h"

void handle_client(client_t *client);
void client_addr_info(client_t *client, struct sockaddr_storage *addr);

#endif