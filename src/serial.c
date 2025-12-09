// comunicacao serial com arduino pq sim né
#include "serial.h"
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "log.h"

static HANDLE hSerial = INVALID_HANDLE_VALUE; // handle global pra porta serial

// abre a porta COM do arduino (a minha é COM5 mas vai saber né)
int abrirSerial(const char *porta) {
    hSerial = CreateFile(porta, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) return 0; // nao abriu? problema do usuario

    DCB dcbSerial = {0}; // config da porta serial
    dcbSerial.DCBlength = sizeof(dcbSerial);
    if (!GetCommState(hSerial, &dcbSerial)) return 0; // deu ruim aqui também??

    // 9600 baud pq o arduino é meio lentinho
    dcbSerial.BaudRate = CBR_9600;
    dcbSerial.ByteSize = 8;
    dcbSerial.StopBits = ONESTOPBIT;
    dcbSerial.Parity   = NOPARITY; // sem paridade pq isso só complica

    if (!SetCommState(hSerial, &dcbSerial)) return 0; // vai funcionar ou nao vai???

    // timeouts pra nao travar tudo se o arduino bugou
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    SetCommTimeouts(hSerial, &timeouts);

    return 1; // sucesso! milagre
}

// fecha a porta serial quando acabar tudo
void fecharSerial() {
    if (hSerial != INVALID_HANDLE_VALUE) CloseHandle(hSerial); // bye bye arduino
}

// manda mensagem pro arduino tipo "faz ai"
int enviarSerial(const char *mensagem) {
    if (hSerial == INVALID_HANDLE_VALUE) return 0; // porta nem ta aberta irmao
    DWORD bytesEscritos;
    WriteFile(hSerial, mensagem, strlen(mensagem), &bytesEscritos, NULL);
    return bytesEscritos; // retorna quantos bytes conseguiu escrever (ou nao)
}

// le o que o arduino mandou de volta
int lerSerial(char *buffer, int tamanho) {
    if (hSerial == INVALID_HANDLE_VALUE) return 0; // porta fechada = nada pra ler
    DWORD bytesLidos = 0;
    ReadFile(hSerial, buffer, tamanho-1, &bytesLidos, NULL); // -1 pro \0 obvio
    buffer[bytesLidos] = '\0'; // termina a string porque C é assim
    return bytesLidos;
}

// variaveis globais pra thread de leitura serial
HANDLE hThreadSerial = NULL;
volatile int parar_thread_serial = 0;

// Função simples para ler RFID - espera até encontrar RFID: ou timeout
char* lerRFIDSimples(int timeout_segundos) {
    static char rfid_resultado[100];
    rfid_resultado[0] = '\0';
    
    DWORD tempo_inicio = GetTickCount();
    DWORD timeout_ms = timeout_segundos * 1000;
    char buffer[256];
    char linha[256] = "";
    int pos_linha = 0;
    
    while ((GetTickCount() - tempo_inicio) < timeout_ms) {
        int lidos = lerSerial(buffer, sizeof(buffer));
        
        if (lidos > 0) {
            // Processa cada byte recebido
            for (int i = 0; i < lidos; i++) {
                char c = buffer[i];
                
                if (c == '\n') {
                    // Fim de linha
                    linha[pos_linha] = '\0';
                    pos_linha = 0;
                    
                    // Remove \r se tiver
                    char *cr = strchr(linha, '\r');
                    if (cr) *cr = '\0';
                    
                    // Se é RFID, retorna
                    if (strstr(linha, "RFID:") == linha) {
                        strcpy(rfid_resultado, linha + 5);
                        return rfid_resultado;
                    }
                    
                    // Se é LOG, processa
                    if (strstr(linha, "LOG:") == linha) {
                        registrarLog(linha);
                    }
                } else if (c != '\r') {
                    if (pos_linha < sizeof(linha) - 1) {
                        linha[pos_linha++] = c;
                    }
                }
            }
        }
        
        Sleep(50); // dorme um pouco pra nao ficar 100% CPU
    }
    
    return NULL; // timeout
}

// essa funcao fica stalkeando o arduino esperando logs
// Agora SIMPLES - sem complicacoes de thread
DWORD WINAPI threadLeituraSerial(LPVOID lpParam) {
    return 0; // thread desativada pra nao conflitar
}

// inicia a thread de leitura serial
void iniciarThreadSerial() {
    // Thread desativada - nao necessaria
}

// para a thread de leitura serial
void pararThreadSerial() {
    // Thread desativada - nao necessaria
}

// retrocompatibilidade - agora processa logs de forma simples
void processarLogsArduino() {
    static char linha[256] = "";
    static int pos_linha = 0;
    char buffer[256];
    
    // Le o que tiver disponivel (nao bloqueia)
    int lidos = lerSerial(buffer, sizeof(buffer));
    
    if (lidos > 0) {
        // Processa cada byte recebido
        for (int i = 0; i < lidos; i++) {
            char c = buffer[i];
            
            if (c == '\n') {
                // Fim de linha
                linha[pos_linha] = '\0';
                pos_linha = 0;
                
                // Remove \r se tiver
                char *cr = strchr(linha, '\r');
                if (cr) *cr = '\0';
                
                // Se é LOG, processa
                if (strstr(linha, "LOG:") == linha) {
                    registrarLog(linha);
                }
                
                linha[0] = '\0';
            } else if (c != '\r') {
                if (pos_linha < sizeof(linha) - 1) {
                    linha[pos_linha++] = c;
                }
            }
        }
    }
}
