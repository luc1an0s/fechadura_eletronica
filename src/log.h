#ifndef LOG_H
#define LOG_H

void registrarLog(const char *mensagem);
char** carregarLogs(int *totalLinhas);
void liberarLogs(char **logs, int totalLinhas);

#endif
