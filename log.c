#include "log.h"
#include <stdio.h>
#include <time.h>

void registrarLog(const char* mensagem) {
    FILE* f = fopen("logs.txt", "a");
    if (f) {
        time_t agora = time(NULL);
        fprintf(f, "%s - %s", mensagem, ctime(&agora));
        fclose(f);
    }
}
