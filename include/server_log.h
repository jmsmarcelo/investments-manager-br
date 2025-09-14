#ifndef SERVER_LOG_H
#define SERVER_LOG_H

/**
 * Inicializa o sistema de log do servidor.
 * 
 * Deve ser chamado **uma vez** no início do programa
 * antes de qualquer chamada a `log_write()`.
 * Cria o arquivo de log (server.log)
 * e inicializa o mutex para acesso thread-safe.
 */
void log_init(void);

/**
 * @brief           Escreve uma mensagem formatada no log.
 * 
 * A mensagem será registrada no `server.log`
 * com timestamp em UTC no formato `YYYY-MM-DD HH:MM:SS+00`.
 * Cada escrita é protegida por mutex para ser segura em ambientes multithread.
 * 
 * @param format    String de formato no estilo `printf`.
 * @param ...       Argumentos variádicos correspondentes ao formato.
 * 
 * @note            O sistema de log deve ser inicializado antes de chamar esta função (`log_init()`)
 *                  e finalizado apenas ao término da aplicação (`log_cleanup()`).
 */
void log_write(const char *const format, ...);

/**
 * Finaliza o sistema de log.
 * 
 * Fecha o arquivo de log e libera recursos associados ao mutex.
 * Deve ser chamado, somente, no fim do programa,
 * após a última chamada a `log_write()`.
 */
void log_cleanup(void);

#endif