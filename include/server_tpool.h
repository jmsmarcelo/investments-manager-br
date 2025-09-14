#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "server_status.h"

status_t tpool_init(void);
status_t tpool_put(socket_t sock, int is_https);
status_t tpool_destroy(void);

#endif /* THREADPOOL_H */