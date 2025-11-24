#include "serial.h"
#include "log.h"
#include <stdio.h>

void lerSerialArduino() {
    FILE* arduino = fopen("COM3", "r");
    if (!arduino) { printf("Erro ao abrir porta serial.\n"); return; }

    char buffer[100];
    if (fgets(buffer, sizeof(buffer), arduino)) {
        printf("Recebido: %s\n", buffer);
        registrarLog(buffer);
    }
    fclose(arduino);
}