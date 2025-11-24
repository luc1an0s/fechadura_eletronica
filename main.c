#include <stdio.h>
#include "usuario.h"
#include "serial.h"
#include "log.h"

int main() {
    if (!abrirSerial("\\\\.\\COM3")) {
        return 1;
    }

    int opcao;
    char nome[50], senha[20];

    do {
        printf("\n1 - Cadastrar usuario\n");
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
    return 0;
}