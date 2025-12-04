#include <stdio.h>
#include <string.h>
#include "usuario.h"
#include "serial.h"

Usuario usuarios[50];
int totalUsuarios = 0;

void enviarCadastroUsuario(const char *nome, const char *senha, const char *nivel) {
    char comando[128];
    int encontrado = 0;


    for (int i = 0; i < totalUsuarios; i++) {
        if (strcmp(usuarios[i].nome, nome) == 0) {
            strcpy(usuarios[i].senha, senha);
            strcpy(usuarios[i].nivel, nivel);
            encontrado = 1;
            break;
        }
    }


    if (!encontrado && totalUsuarios < 50) {
        strcpy(usuarios[totalUsuarios].nome, nome);
        strcpy(usuarios[totalUsuarios].senha, senha);
        strcpy(usuarios[totalUsuarios].nivel, nivel);
        totalUsuarios++;
    }


    snprintf(comando, sizeof(comando), "%s;%s;%s", nome, senha, nivel);
    enviarSerial("CADASTRO:", comando);
}

void listarUsuarios() {
    printf("\n--- Usuarios cadastrados ---\n");
    for (int i = 0; i < totalUsuarios; i++) {
        printf("Nome: %s | Nivel: %s\n",
               usuarios[i].nome,
               usuarios[i].nivel);
    }
}

void alterarSenha(const char *nome, const char *novaSenha) {
    char comando[128];

    for (int i = 0; i < totalUsuarios; i++) {
        if (strcmp(usuarios[i].nome, nome) == 0) {

            strcpy(usuarios[i].senha, novaSenha);

            snprintf(comando, sizeof(comando), "%s;%s;%s",
                     usuarios[i].nome,
                     usuarios[i].senha,
                     usuarios[i].nivel);

            enviarSerial("CADASTRO:", comando);
            return;
        }
    }

    printf("Usuario %s nao encontrado!\n", nome);
}



void salvarUsuariosEmArquivo() {
    FILE *f = fopen("usuarios.txt", "w");
    if (!f) return;

    for (int i = 0; i < totalUsuarios; i++) {
        fprintf(f, "%s;%s;%s\n",
                usuarios[i].nome,
                usuarios[i].senha,
                usuarios[i].nivel);
    }

    fclose(f);
}

void carregarUsuariosDoArquivo() {
    FILE *f = fopen("usuarios.txt", "r");
    if (!f) return;

    char linha[128];

    while (fgets(linha, sizeof(linha), f)) {
        char nome[50], senha[20], nivel[20];

        sscanf(linha, "%49[^;];%19[^;];%19[^\n]", nome, senha, nivel);

        strcpy(usuarios[totalUsuarios].nome, nome);
        strcpy(usuarios[totalUsuarios].senha, senha);
        strcpy(usuarios[totalUsuarios].nivel, nivel);
        totalUsuarios++;
    }

    fclose(f);
}

int deletarUsuario(const char *nome) {
    int posicao = -1;

    //Procura onde está o usuário
    for (int i = 0; i < totalUsuarios; i++) {
        if (strcmp(usuarios[i].nome, nome) == 0) {
            posicao = i;
            break;
        }
    }

    //Se não achou, retorna 0 (errro)
    if (posicao == -1) return 0;

    //Remove e puxa todo mundo para trás (Shift)
    for (int i = posicao; i < totalUsuarios - 1; i++) {
        usuarios[i] = usuarios[i + 1]; //Copia o próximo para a posição atual
    }

    //Diminui o total
    totalUsuarios--;

    //Salva a nova lista no arquivo
    salvarUsuariosEmArquivo();

    //Envia comando para o Arduino
    char comando[128];
    snprintf(comando, sizeof(comando), "%s", nome);
    enviarSerial("DELETAR:", comando);

    return 1;// Sucesso
}
