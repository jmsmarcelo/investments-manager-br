#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include "../include/server_utils.h"

void formatted_gmtime(char *out, size_t out_size, const char *format) {
    if(!format) format = "%Y-%m-%d %H:%M:%S+00";
    time_t now = time(NULL);
    struct tm tm_buf;

#ifdef _WIN32
    gmtime_s(&tm_buf, &now);
#else
    gmtime_r(&now, &tm_buf);
#endif

    strftime(out, out_size, format, &tm_buf);
}

int fd_max(socket_t fd, ...) {
    socket_t current;
    int maxfd = fd;

    va_list args;
    va_start(args, fd);
    while((current = va_arg(args, socket_t)) != INVALID_SOCK) {
        if(current > maxfd) maxfd = current;
    }
    va_end(args);

    return maxfd;
}