#include <stdio.h>
#include <windows.h>
#include "log.h"

int main() {
    HANDLE serial = CreateFile("COM3", GENERIC_READ, 0, NULL,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (serial == INVALID_HANDLE_VALUE) {
        printf("Erro ao abrir porta serial\n");
        return 1;
    }

    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);
    GetCommState(serial, &dcb);
    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity   = NOPARITY;
    SetCommState(serial, &dcb);

    char buffer[256];
    DWORD bytesRead;

    printf("Lendo logs do Arduino...\n");

    while (1) {
        if (ReadFile(serial, buffer, sizeof(buffer)-1, &bytesRead, NULL)) {
            buffer[bytesRead] = '\0';

            if (bytesRead > 0) {
                printf("Recebido: %s\n", buffer);
                registrarLog(buffer);
            }
        }
    }

    CloseHandle(serial);
    return 0;
}