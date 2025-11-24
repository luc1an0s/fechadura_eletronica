#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <locale.h>
#include "usuario.h"
#include "serial.h"
#include "log.h"

#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[1;36m"

void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void mostrarMenu() {
    printf(BLUE "+-----------------------------+\n" RESET);
    printf(BLUE "|        PAINEL ADMIN         |\n" RESET);
    printf(BLUE "+-----------------------------+\n" RESET);
    printf(CYAN "1 - Ver usuários cadastrados\n");
    printf(CYAN "2 - Alterar senha de usuário\n");
    printf(CYAN "3 - Cadastrar novo usuário\n");
    printf(CYAN "4 - Sair\n" RESET);
    printf(YELLOW "Escolha: " RESET);
}

void lerLinha(char *buffer, int tamanho) {
    if (fgets(buffer, tamanho, stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
    }
}

int main() {
    setlocale(LC_ALL, ""); 

    char nome[50], senha[50];

    limparTela();
    printf(BLUE "Login admin\n" RESET);
    printf("Usuario: ");
    lerLinha(nome, 50);
    printf("Senha: ");
    lerLinha(senha, 50);

    if (strcmp(nome, "admin") != 0 || strcmp(senha, "1234") != 0) {
        printf(RED "Acesso negado!\n" RESET);
        registrarLog("Tentativa de login admin falhou");
        return 1;
    }

    printf(GREEN "Login admin bem-sucedido!\n" RESET);
    registrarLog("Login admin bem-sucedido");

    if (!abrirSerial("\\\\.\\COM3")) {
        printf(RED "Erro ao abrir porta COM3\n" RESET);
        return 1;
    }

    carregarUsuariosDoArquivo();

    int opcao;
    do {
        mostrarMenu();
        char opcaoStr[10];
        lerLinha(opcaoStr, 10);
        opcao = atoi(opcaoStr);

        switch(opcao) {
            case 1:
                limparTela();
                printf(BLUE "--- Usuários cadastrados ---\n" RESET);
                listarUsuarios();
                break;

            case 2: {
                char nomeAlt[50], novaSenha[50];
                printf("Usuario: ");
                lerLinha(nomeAlt, 50);
                printf("Nova senha: ");
                lerLinha(novaSenha, 50);
                alterarSenha(nomeAlt, novaSenha);
                salvarUsuariosEmArquivo();
                registrarLog("Senha de usuario alterada");
                printf(GREEN "Senha alterada com sucesso!\n" RESET);
                break;
            }

            case 3: {
                char nomeNovo[50], senhaNovo[50], nivel[20];
                printf("Nome: ");
                lerLinha(nomeNovo, 50);
                printf("Senha: ");
                lerLinha(senhaNovo, 50);
                printf("Nivel (usuario/admin): ");
                lerLinha(nivel, 20);
                enviarCadastroUsuario(nomeNovo, senhaNovo, nivel);
                salvarUsuariosEmArquivo();
                registrarLog("Usuario cadastrado via admin");
                printf(GREEN "Usuário cadastrado com sucesso!\n" RESET);
                break;
            }

            case 4:
                printf(BLUE "Encerrando painel admin...\n" RESET);
                break;

            default:
                printf(RED "Opção inválida! Tente novamente.\n" RESET);
                break;
        }

        printf("\n");
    } while (opcao != 4);

    fecharSerial();
    return 0;
}
