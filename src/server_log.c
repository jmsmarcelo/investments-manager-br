#include <stdio.h>
#include <stdarg.h>
#include "../include/server_log.h"
#include "../include/server_def.h"
#include "../include/server_utils.h"

static mutex_t log_mutex;

static FILE *log_ptr = NULL;

void log_init(void) {
    mutex_init(&log_mutex);

    log_ptr = fopen("server.log", "a");
    if(!log_ptr) {
        fprintf(stderr, "Error opening server.log\n");
    }
}

void log_write(const char *const format, ...) {
    if(!log_ptr) return;

    va_list args;
    va_start(args, format);

    mutex_lock(&log_mutex);

    char timestamp[32];
    formatted_gmtime(timestamp, sizeof(timestamp), NULL);
    
    fprintf(log_ptr, "[%s] ", timestamp);
    vfprintf(log_ptr, format, args);
    fprintf(log_ptr, "\n");

    fflush(log_ptr);

    mutex_unlock(&log_mutex);

    va_end(args);
}

void log_cleanup(void) {
    if(log_ptr) {
        fclose(log_ptr);
        log_ptr = NULL;
    }

#ifdef _WIN32
#else
pthread_mutex_destroy(&log_mutex);
#endif
}