#include <stdio.h>
#include <string.h>
#include "usuario.h"
#include "serial.h"

Usuario usuarios[MAX_USUARIOS];
int totalUsuarios = 0;

void cadastrarUsuario(const char *nome, const char *senha) {
    if (totalUsuarios < MAX_USUARIOS) {
        strcpy(usuarios[totalUsuarios].nome, nome);
        strcpy(usuarios[totalUsuarios].senha, senha);
        totalUsuarios++;

        enviarSerial("CADASTRO:", senha);
    } else {
        printf("Limite de usuarios atingido!\n");
    }
}