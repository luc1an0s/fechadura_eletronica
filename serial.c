#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "serial.h"

static HANDLE hSerial = INVALID_HANDLE_VALUE;

int abrirSerial(const char *porta) {
    // GENERIC_READ é adicionado para permitir a leitura
    hSerial = CreateFileA(porta, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
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
    // Configura o timeout para leitura para ser não-bloqueante (ou quase)
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);

    return 1; 
}

void enviarSerial(const char *prefixo, const char *mensagem) {
    if (hSerial == INVALID_HANDLE_VALUE) return;

    char buffer[128];
    // Adiciona o prefixo e a nova linha para o protocolo
    snprintf(buffer, sizeof(buffer), "%s%s\n", prefixo, mensagem);

    DWORD bytesEscritos;
    WriteFile(hSerial, buffer, strlen(buffer), &bytesEscritos, NULL);

    printf("Mensagem enviada: %s", buffer);
}

int receberSerial(char *buffer, int tamanho) {
    if (hSerial == INVALID_HANDLE_VALUE) return 0;

    DWORD bytesLidos = 0;
    // Tenta ler o máximo de bytes possível até o tamanho do buffer
    ReadFile(hSerial, buffer, tamanho - 1, &bytesLidos, NULL);

    // Garante que a string seja terminada em nulo
    buffer[bytesLidos] = '\0';

    return bytesLidos;
}

void fecharSerial() {
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }
}
