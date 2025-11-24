#include <stdio.h>
#include "usuario.h"
#include "serial.h"
#include "log.h"

int main() {
    printf("Tentando abrir porta serial...\n");

    // tenta abrir COM3
    if (!abrirSerial("\\\\.\\COM3")) {
        printf("Falha ao abrir porta COM3\n");
        printf("Pressione ENTER para sair...\n");
        getchar(); 
        return 1;
    }

    printf("Porta COM3 aberta com sucesso!\n");

    int opcao;
    char nome[50], senha[20];

    do {
        printf("\nMenu:\n");
        printf("1 - Cadastrar usuario\n");
        printf("2 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        if (opcao == 1) {
            printf("Nome: ");
            scanf("%s", nome);
            printf("Senha: ");
            scanf("%s", senha);

            cadastrarUsuario(nome, senha);
            registrarLog("Usuario cadastrado");
        }
    } while (opcao != 2);

    fecharSerial();

    printf("Programa encerrado.\n");
    printf("Pressione ENTER para sair...\n");
    getchar(); 
    return 0;
}
