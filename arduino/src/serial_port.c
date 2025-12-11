#include "serial_port.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
    HANDLE hSerial = INVALID_HANDLE_VALUE;

    int setupSerial(const char* porta) {
        hSerial = CreateFileA(porta, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        
        if (hSerial == INVALID_HANDLE_VALUE) { 
            printf("Erro Windows: Porta %s nao encontrada.\n", porta); 
            return -1; 
        }
        ;
        DCB dcb = {0}; 
        dcb.DCBlength = sizeof(dcb); 
        GetCommState(hSerial, &dcb);
        
        dcb.BaudRate = CBR_9600; 
        dcb.ByteSize = 8; 
        dcb.StopBits = ONESTOPBIT; 
        dcb.Parity = NOPARITY;
        SetCommState(hSerial, &dcb);
        
        COMMTIMEOUTS timeouts = {0}; 
        timeouts.ReadIntervalTimeout = 20; 
        timeouts.ReadTotalTimeoutConstant = 10; 
        timeouts.ReadTotalTimeoutMultiplier = 10;
        SetCommTimeouts(hSerial, &timeouts);
        
        printf("Conectado (Windows): %s\n", porta);
        return 0;
    }

    void enviarComando(char cmd) { 
        if (hSerial == INVALID_HANDLE_VALUE) {
            return;
        }
        
        DWORD escritos; 
        WriteFile(hSerial, &cmd, 1, &escritos, NULL); 
    }

    void enviarStringSerial(const char* str) {
        if (hSerial == INVALID_HANDLE_VALUE) {
            return;
        }
        
        DWORD escritos; 
        WriteFile(hSerial, str, strlen(str), &escritos, NULL);
    }

    int lerSerial(char* buffer, int max) { 
        if (hSerial == INVALID_HANDLE_VALUE) {
            return 0; 
        }
        
        DWORD lidos = 0; 
        if (ReadFile(hSerial, buffer, max, &lidos, NULL)) {
            return (int)lidos; 
        }
        
        return 0; 
    }

    void fecharSerial() { 
        if (hSerial != INVALID_HANDLE_VALUE) {
            CloseHandle(hSerial); 
        }
    }

    void abrirPastaLocal() { 
        system("explorer ."); 
    }

#else
    #include <fcntl.h>
    #include <errno.h>
    #include <termios.h>
    #include <unistd.h>
    int serial_fd = -1;

    int setupSerial(const char* porta) {
        serial_fd = open(porta, O_RDWR | O_NOCTTY | O_NDELAY);
        
        if (serial_fd == -1) { 
            printf("Erro Linux: Porta %s nao encontrada.\n", porta); 
            return -1; 
        }
        
        struct termios options; 
        tcgetattr(serial_fd, &options);
        
        cfsetispeed(&options, B9600); 
        cfsetospeed(&options, B9600);
        
        options.c_cflag &= ~PARENB; 
        options.c_cflag &= ~CSTOPB; 
        options.c_cflag &= ~CSIZE; 
        options.c_cflag |= CS8;
        
        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); 
        options.c_oflag &= ~OPOST;
        
        tcsetattr(serial_fd, TCSANOW, &options); 
        fcntl(serial_fd, F_SETFL, FNDELAY);
        
        printf("Conectado (Linux): %s\n", porta);
        return 0;
    }

    void enviarComando(char cmd) { 
        if (serial_fd != -1) {
            write(serial_fd, &cmd, 1); 
        }
    }

    void enviarStringSerial(const char* str) {
        if (serial_fd != -1) {
            write(serial_fd, str, strlen(str));
        }
    }

    int lerSerial(char* buffer, int max) { 
        if (serial_fd == -1) {
            return 0; 
        }
        return read(serial_fd, buffer, max); 
    }

    void fecharSerial() { 
        if (serial_fd != -1) {
            close(serial_fd); 
        }
    }

    void abrirPastaLocal() { 
        system("xdg-open ."); 
    }
#endif