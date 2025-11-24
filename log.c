#include <stdio.h>
#include <time.h>
#include "log.h"

void registrarLog(const char *mensagem) {
    FILE *arquivo = fopen("log.txt", "a");
    if (arquivo == NULL) return;

    time_t agora = time(NULL);
    struct tm *t = localtime(&agora);

    fprintf(arquivo, "[%02d/%02d/%04d %02d:%02d:%02d] %s\n",
            t->tm_mday, t->tm_mon+1, t->tm_year+1900,
            t->tm_hour, t->tm_min, t->tm_sec,
            mensagem);

    fclose(arquivo);
}
