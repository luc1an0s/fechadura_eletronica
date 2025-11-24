#include <stdio.h>
#include <time.h>
#include "log.h"

void registrarLog(const char *mensagem) {
    FILE *f = fopen("log.txt", "a");
    if (f != NULL) {
        time_t agora = time(NULL);
        fprintf(f, "%s - %s\n", ctime(&agora), mensagem);
        fclose(f);
    }
}