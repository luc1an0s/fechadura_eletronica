#include <stdio.h>
#include <string.h>
#include "usuario.h"
#include "serial.h"
#include "log.h"

void mostrarMenu() {
    printf("\033[1;34m+-----------------------------+\n");
    printf("|        PAINEL ADMIN         |\n");
    printf("+-----------------------------+\033[0m\n");
    printf("1 - Ver usuarios cadastrados\n");
    printf("2 - Alterar senha de usuario\n");
    printf("3 - Cadastrar novo usuario\n");
    printf("4 - Sair\n");
    printf("Escolha: ");
}

int main() {
    char nome[50], senha[20];

    printf("Login admin\nUsuario: ");
    scanf("%s", nome);
    printf("Senha: ");
    scanf("%s", senha);

    
    if (strcmp(nome, "admin") != 0 || strcmp(senha, "1234") != 0) {
        printf("\033[1;31mAcesso negado!\033[0m\n");
        registrarLog("Tentativa de login admin falhou");
        return 1;
    }

    printf("\033[1;32mLogin admin bem-sucedido!\033[0m\n");
    registrarLog("Login admin bem-sucedido");

    
    if (!abrirSerial("\\\\.\\COM3")) {
        printf("Erro ao abrir porta COM3\n");
        return 1;
    }

    
    carregarUsuariosDoArquivo();

    int opcao;
    do {
        mostrarMenu();
        scanf("%d", &opcao);

        if (opcao == 1) {
            listarUsuarios();
        } 
        else if (opcao == 2) {
            char nomeAlt[50], novaSenha[20];

            printf("Usuario: ");
            scanf("%s", nomeAlt);
            printf("Nova senha: ");
            scanf("%s", novaSenha);

            alterarSenha(nomeAlt, novaSenha);
            salvarUsuariosEmArquivo();
            registrarLog("Senha de usuario alterada");
        }
        else if (opcao == 3) {
            char nomeNovo[50], senhaNovo[20], nivelNovo[20];

            printf("Nome: ");
            scanf("%s", nomeNovo);
            printf("Senha: ");
            scanf("%s", senhaNovo);
            printf("Nivel (usuario/admin): ");
            scanf("%s", nivelNovo);

            enviarCadastroUsuario(nomeNovo, senhaNovo, nivelNovo);
            salvarUsuariosEmArquivo();
            registrarLog("Usuario cadastrado via admin");
        }

    } while (opcao != 4);

    fecharSerial();
    printf("Encerrando painel admin...\n");

    return 0;
}
