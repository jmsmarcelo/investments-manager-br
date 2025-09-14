#include <signal.h>
#ifdef _WIN32
#include <WS2tcpip.h>
#else
#include <sys/select.h>
#include <unistd.h>
#endif
#include "../include/server.h"
#include "../include/server_setup.h"
#include "../include/server_log.h"
#include "../include/server_utils.h"
#include "../include/server_tpool.h"

status_t server_start(void) {
    log_init();
    tpool_init();
    status_t status = sock_setup();
    if(status != STATUS_OK) {
        log_write("[ERROR] %s", status_msg(status));
        goto CLEANUP;
    }

    fd_set fds_read;
    int maxfd = fd_max(get_sock_http(), get_sock_https(), INVALID_SOCK);

    while(1) {
        FD_ZERO(&fds_read);

        if(get_port_http() > 0) FD_SET(get_sock_http(), &fds_read);
        if(get_port_https() > 0) FD_SET(get_sock_https(), &fds_read);

        if(select(maxfd + 1, &fds_read, NULL, NULL, NULL) < 0) {
            log_write("[INFO] Error select client");
            continue;
        }

        if(FD_ISSET(get_sock_http(), &fds_read)) {
            status = tpool_put(get_sock_http(), 0);
        } else if(FD_ISSET(get_sock_https(), &fds_read)) {
            status = tpool_put(get_sock_https(), 1);
        } else {
            log_write("[INFO] Unknown client");
        }

        if(status != STATUS_OK) {
            log_write("[INFO] %s", status_msg(status));
        }
    }

CLEANUP:
    sock_setup_cleanup();
    tpool_destroy();
    log_cleanup();
    
    return status;
}