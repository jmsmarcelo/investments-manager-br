#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <stddef.h>
#include "../include/server_def.h"

/**
 * Formata o horário atual em UTC.
 * 
 * @param out       Buffer de saída para a string formatada.
 * @param out_size  Tamanho máximo do buffer de saída.
 * @param format    Formato de data/hora (`strftime`).
 *                  Se `NULL`, usa o formato no estilo do PostgreSQL:
 *                  'YYYY-MM-DD HH:MM:SS+00' (`TMESTAMP WITH TIME ZONE`).
 * 
 * @return Nada. O resultado é escrito em `out`.
 */
void formatted_gmtime(char *out, size_t out_size, const char *format);

int fd_max(socket_t fd, ...);

#endif /* TIME_UTILS_H */