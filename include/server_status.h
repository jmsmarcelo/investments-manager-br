#ifndef SERVER_STATUS_H
#define SERVER_STATUS_H

#include "server_def.h"

#define SERVER_STATUS(X) \
    X(STATUS_OK, "OK") \
    X(ERROR_INIT_WINSOCKET, "Error initializing WinSock") \
    X(ERROR_CREATING_SOCK, "Error creating sock") \
    X(ERROR_ASSOCIATING_SOCK_WITH_PORT, "Error associating sock with port") \
    X(ERROR_LISTENING_SOCK_FOR_CONNECTIONS, "Error listening sock for connections") \
    X(COULD_NOT_CREATE_SSL_CTX, "Could not create SSL context") \
    X(ERROR_LOADING_CERT, "Error loading certificate") \
    X(ERROR_LOADING_KEY, "Error loading private key") \
    X(CLIENT_ACCEPT_FAILURE, "Client accept failure") \
    X(FAILED_ALLOC, "Memory allocation failed") \
    X(PG_ERROR, "Failed to connect to PostgreSQL")

GENERATE_ENUM(status, SERVER_STATUS)
const char *status_msg(status_t code);

#endif /* SERVER_STATUS_H */