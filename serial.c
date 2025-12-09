#include <windows.h>
#include <stdio.h>
#include "serial.h"

static HANDLE hSerial;

int abrirSerial(const char *porta) {
    hSerial = CreateFileA(
        porta,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hSerial == INVALID_HANDLE_VALUE)
        return 0;

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    GetCommState(hSerial, &dcbSerialParams);

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;

    SetCommState(hSerial, &dcbSerialParams);

    return 1;
}

void fecharSerial() {
    CloseHandle(hSerial);
}

void enviarSerial(const char *mensagem) {
    DWORD bytesEnviados;
    WriteFile(hSerial, mensagem, strlen(mensagem), &bytesEnviados, NULL);
    WriteFile(hSerial, "\n", 1, &bytesEnviados, NULL);
}

int lerSerial(char *buffer, int tamanho) {
    DWORD bytesLidos = 0;
    if (!ReadFile(hSerial, buffer, tamanho - 1, &bytesLidos, NULL))
        return 0;

    buffer[bytesLidos] = '\0';
    return bytesLidos;
}
