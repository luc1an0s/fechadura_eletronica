#include <stdio.h>
#include <string.h>
#include "serial.h"

static FILE *arduino = NULL;

void abrirSerial(const char *porta) {
    arduino = fopen("COM3:", "w");
    if (arduino == NULL) {
        printf("Erro ao abrir porta %s\n", porta);
    }
}

void enviarSerial(const char *prefixo, const char *mensagem) {
    if (arduino != NULL) {
        fprintf(arduino, "%s%s\n", prefixo, mensagem);
        fflush(arduino);
    }
}

void fecharSerial() {
    if (arduino != NULL) {
        fclose(arduino);
        arduino = NULL;
    }
}
