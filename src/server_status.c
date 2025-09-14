#include "../include/server_status.h"

GENERATE_ARRAY(status_msgs, SERVER_STATUS)

const char *status_msg(status_t code) {
    if (code >= 0 && code < SERVER_STATUS_MAX) {
        return status_msgs[code];
    }
    return "Unknown status";
}
