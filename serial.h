#ifndef SERIAL_H
#define SERIAL_H

int abrirSerial(const char *porta);
void fecharSerial();
void enviarSerial(const char *mensagem);
int lerSerial(char *buffer, int tamanho);

#endif
