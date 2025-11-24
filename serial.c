#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "serial.h"

static HANDLE hSerial;

int abrirSerial(const char *porta) {
    hSerial = CreateFileA(porta, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Erro ao abrir %s\n", porta);
        return 0;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    GetCommState(hSerial, &dcbSerialParams);
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;
    SetCommState(hSerial, &dcbSerialParams);

    COMMTIMEOUTS timeouts = {0};
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);

    return 1;
}

void enviarSerial(const char *prefixo, const char *mensagem) {
    if (hSerial == INVALID_HANDLE_VALUE) return;

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%s%s\n", prefixo, mensagem);
    DWORD bytesEscritos;
    WriteFile(hSerial, buffer, strlen(buffer), &bytesEscritos, NULL);
}

void fecharSerial() {
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }
}
