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


void lerLinha(char *buffer, int tamanho) {
    if (fgets(buffer, tamanho, stdin))
        buffer[strcspn(buffer, "\n")] = 0;
}

void mostrarMenu() {
    printf(BLUE "\n" RESET);
    printf(BLUE "+===========================================+\n" RESET);
    printf(BLUE "|              PAINEL ADMIN                 |\n" RESET);
    printf(BLUE "+===========================================+\n" RESET);
    printf(CYAN "| 1 | Ver usuarios cadastrados              |\n");
    printf("| 2 | Alterar senha de usuario              |\n");
    printf("| 3 | Cadastrar novo usuario                |\n");
    printf("| 4 | " RED "Excluir usuario" CYAN "                       |\n");
    printf("| 5 | Sair                                  |\n" RESET);
    printf(BLUE "+===========================================+\n" RESET);
    printf(YELLOW " Escolha uma opcao: " RESET);
}

int main() {
    setlocale(LC_ALL, "Portuguese_Brazil.1252");

    char nome[50], senha[50];

    limparTela();
    //Cabeçalho de Login com visual melhorado :D
    printf(BLUE "+-------------------------------------------+\n" RESET);
    printf(BLUE "|               LOGIN ADMIN                 |\n" RESET);
    printf(BLUE "+-------------------------------------------+\n" RESET);

    printf(" Usuario: ");
    lerLinha(nome, sizeof(nome));

    printf(" Senha: ");
    lerLinha(senha, sizeof(senha));

    if (strcmp(nome, "admin") != 0 || strcmp(senha, "1234") != 0) {
        printf(RED "\n [X] Acesso negado!\n" RESET);
        registrarLog("Tentativa de login admin falhou");
        printf(" Pressione ENTER para sair...");
        getchar();
        return 1;
    }

    printf(GREEN "\n [V] Login admin bem-sucedido!\n" RESET);
    registrarLog("Login admin bem-sucedido");

    printf(" Carregando sistema...\n");

    if (!abrirSerial("\\\\.\\COM5")) {
        printf(RED "Erro ao abrir porta COM\n" RESET);
        printf("Pressione ENTER para sair...");
        getchar();
        return 1;
    }

    carregarUsuariosDoArquivo();

    int opcao = 0;
    char opcaoStr[10];

    do {
        mostrarMenu();
        lerLinha(opcaoStr, sizeof(opcaoStr));
        opcao = atoi(opcaoStr);

        switch (opcao) {
            case 1:
                limparTela();
                // Cabeçalho para a listagem
                printf(CYAN "+-------------------------------------------+\n");
                printf("|           USUARIOS CADASTRADOS            |\n");
                printf("+-------------------------------------------+\n" RESET);
                listarUsuarios();
                printf(CYAN "+-------------------------------------------+\n" RESET);
                break;

            case 2: {
                char nomeAlt[50], novaSenha[50];
                printf(YELLOW "\n--- Alterar Senha ---\n" RESET);
                printf("Usuario: ");
                lerLinha(nomeAlt, 50);

                printf("Nova senha: ");
                lerLinha(novaSenha, 50);

                alterarSenha(nomeAlt, novaSenha);
                salvarUsuariosEmArquivo();
                registrarLog("Senha de usuario alterada");
                printf(GREEN " [V] Senha alterada com sucesso!\n" RESET);
                break;
            }

            case 3: {
                char nomeNovo[50], senhaNovo[50], nivel[20];
                printf(YELLOW "\n--- Novo Cadastro ---\n" RESET);
                printf("Nome: ");
                lerLinha(nomeNovo, 50);

                printf("Senha: ");
                lerLinha(senhaNovo, 50);

                printf("Nivel (usuario/admin): ");
                lerLinha(nivel, 20);

                enviarCadastroUsuario(nomeNovo, senhaNovo, nivel);
                salvarUsuariosEmArquivo();
                registrarLog("Usuario cadastrado via admin");
                printf(GREEN " [V] Usuario cadastrado com sucesso!\n" RESET);
                break;
            }

            // --- NOVA OPÇÃO DE DELETAR ---
            case 4: {
                char nomeDel[50];
                char confirmacao[10];

                printf(RED "\n--- EXCLUIR USUARIO ---\n" RESET);
                printf("Nome do usuario para excluir: ");
                lerLinha(nomeDel, 50);

                printf(RED "Tem certeza? (s/n): " RESET);
                lerLinha(confirmacao, 10);

                if (confirmacao[0] == 's' || confirmacao[0] == 'S') {
                    if (deletarUsuario(nomeDel)) {
                        registrarLog("Usuario excluido pelo admin");
                        printf(GREEN " [V] Usuario '%s' excluido com sucesso!\n" RESET, nomeDel);
                    } else {
                        printf(RED " [X] Erro: Usuario nao encontrado.\n" RESET);
                    }
                } else {
                    printf(YELLOW " Operacao cancelada.\n" RESET);
                }
                break;
            }

            case 5: //O SAIR AGORA É O 5
                printf(BLUE " Encerrando painel admin...\n" RESET);
                break;

            default:
                printf(RED " Opcao invalida! Tente novamente.\n" RESET);
                break;
        }

    } while (opcao != 5);

    fecharSerial();
    printf("Pressione ENTER para sair...");
    getchar();
    return 0;
}
