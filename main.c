#include <stdio.h>
#include "usuario.h"
#include "log.h"
#include "serial.h"

int main() {
    int opcao;
    do {
        printf("\n=== MENU ADMIN ===\n");
        printf("1. Cadastrar usuario\n");
        printf("2. Listar usuarios\n");
        printf("3. Ler logs do Arduino\n");
        printf("0. Sair\n");
        scanf("%d", &opcao);

        if (opcao == 1) cadastrarUsuario();
        else if (opcao == 2) listarUsuarios();
        else if (opcao == 3) lerSerialArduino();
    } while (opcao != 0);

    return 0;
}