#include <signal.h>
#include <errno.h>
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

volatile sig_atomic_t running = 1;

void signal_handler(int s) {
    running = 0;
    const char c = 'x';
    send(get_writer(), &c, 1, 0);
}

status_t server_start(void) {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);
    signal(SIGQUIT, signal_handler);

    log_init();
    tpool_init();

    status_t status = sock_setup();
    if(status != STATUS_OK) {
        log_write("[ERROR] %s", status_msg(status));
        goto CLEANUP;
    }

    fd_set fds_read;
    int maxfd = fd_max(get_listener(), get_sock_http(), get_sock_https(), INVALID_SOCK);

    while(running) {
        FD_ZERO(&fds_read);

        FD_SET(get_listener(), &fds_read);
        if(get_port_http() > 0) FD_SET(get_sock_http(), &fds_read);
        if(get_port_https() > 0) FD_SET(get_sock_https(), &fds_read);

        if(select(maxfd + 1, &fds_read, NULL, NULL, NULL) < 0) {
            if(errno != EINTR) log_write("[INFO] Error select client");
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
    log_write("[INFO] Closing server...");
    sock_setup_cleanup();
    tpool_destroy();
    log_cleanup();
    
    return status;
}