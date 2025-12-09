#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "usuario.h"
#include "serial.h"

#define ARQUIVO "usuarios.txt"

typedef struct {
    char nome[50];
    char senha[50];
    char nivel[20];
    char uid[40];
} Usuario;

Usuario usuarios[50];
int totalUsuarios = 0;

void carregarUsuariosDoArquivo() {
    FILE *f = fopen(ARQUIVO, "r");
    if (!f) return;

    totalUsuarios = 0;

    while (!feof(f)) {
        fscanf(f, "%49[^;];%49[^;];%19[^;];%39[^\n]\n",
               usuarios[totalUsuarios].nome,
               usuarios[totalUsuarios].senha,
               usuarios[totalUsuarios].nivel,
               usuarios[totalUsuarios].uid);

        totalUsuarios++;
    }

    fclose(f);
}

void salvarUsuariosEmArquivo() {
    FILE *f = fopen(ARQUIVO, "w");
    if (!f) return;

    for (int i = 0; i < totalUsuarios; i++) {
        fprintf(f, "%s;%s;%s;%s\n",
                usuarios[i].nome,
                usuarios[i].senha,
                usuarios[i].nivel,
                usuarios[i].uid);
    }

    fclose(f);
}

void listarUsuarios() {
    printf("\n--- USUÁRIOS CADASTRADOS ---\n");
    for (int i = 0; i < totalUsuarios; i++) {
        printf("Nome: %s | Nivel: %s | RFID: %s\n",
               usuarios[i].nome,
               usuarios[i].nivel,
               strlen(usuarios[i].uid) ? usuarios[i].uid : "(nenhum)");
    }
}

void alterarSenha(const char *nome, const char *novaSenha) {
    for (int i = 0; i < totalUsuarios; i++) {
        if (strcmp(usuarios[i].nome, nome) == 0) {
            strcpy(usuarios[i].senha, novaSenha);
            return;
        }
    }
}

void enviarCadastroUsuario(const char *nome, const char *senha, const char *nivel) {
    char comando[200];
    sprintf(comando, "CADASTRO:%s;%s;%s", nome, senha, nivel);
    enviarSerial(comando);
}

void enviarCadastroUsuarioRFID(const char *nome, const char *senha, const char *nivel, const char *uid) {
    char comando[250];
    sprintf(comando, "CADASTRO_RFID:%s;%s;%s;%s", nome, senha, nivel, uid);
    enviarSerial(comando);
}
