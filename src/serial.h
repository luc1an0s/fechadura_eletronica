#ifndef SERIAL_H
#define SERIAL_H

#include <windows.h>

int abrirSerial(const char *porta);
void fecharSerial();
int enviarSerial(const char *mensagem);
int lerSerial(char *buffer, int tamanho);
void processarLogsArduino();
DWORD WINAPI threadLeituraSerial(LPVOID lpParam);
void iniciarThreadSerial();
void pararThreadSerial();
extern HANDLE hThreadSerial;
extern volatile int parar_thread_serial;

// Função para ler RFID de forma síncrona
char* lerRFIDSimples(int timeout_segundos);

#endif
