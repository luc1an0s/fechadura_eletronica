#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

void setupSerial(const char* porta);
void enviarComando(char cmd);
void enviarStringSerial(const char* str); // <--- NOVA
int lerSerial(char* buffer, int max);
void fecharSerial();
void abrirPastaLocal();

#endif