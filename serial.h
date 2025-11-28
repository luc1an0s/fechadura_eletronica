#ifndef SERIAL_H
#define SERIAL_H

int abrirSerial(const char *porta);  
void enviarSerial(const char *prefixo, const char *mensagem);
int receberSerial(char *buffer, int tamanho);
void fecharSerial();

#endif
