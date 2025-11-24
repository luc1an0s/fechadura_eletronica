#ifndef SERIAL_H
#define SERIAL_H

void abrirSerial(const char *porta);
void enviarSerial(const char *prefixo, const char *mensagem);
void fecharSerial();

#endif
