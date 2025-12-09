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
    printf(BLUE "+-----------------------------+\n" RESET);
    printf(BLUE "|        PAINEL ADMIN         |\n" RESET);
    printf(BLUE "+-----------------------------+\n" RESET);
    printf(CYAN "1 - Ver usuarios cadastrados\n");
    printf("2 - Alterar senha de usuario\n");
    printf("3 - Cadastrar novo usuario\n");
    printf("4 - Sair\n" RESET);
    printf(YELLOW "Escolha: " RESET);
}

int main() {
    setlocale(LC_ALL, "Portuguese_Brazil.1252");

    char nome[50], senha[50];

    limparTela();
    printf(BLUE "Login admin\n" RESET);

    printf("Usuario: ");
    lerLinha(nome, sizeof(nome));

    printf("Senha: ");
    lerLinha(senha, sizeof(senha));

    if (strcmp(nome, "admin") != 0 || strcmp(senha, "1234") != 0) {
        printf(RED "Acesso negado!\n" RESET);
        registrarLog("Tentativa de login admin falhou");
        printf("Pressione ENTER para sair...");
        getchar();
        return 1;
    }

    printf(GREEN "Login admin bem-sucedido!\n" RESET);
    registrarLog("Login admin bem-sucedido");

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
                char nomeNovo[50], senhaNovo[50], nivel[20], opc;
                char resposta[10];

                printf("Nome: ");
                lerLinha(nomeNovo, 50);

                printf("Senha: ");
                lerLinha(senhaNovo, 50);

                printf("Nivel (usuario/admin): ");
                lerLinha(nivel, 20);

                printf("Usuário tem PIN RFID? (S/N): ");
                lerLinha(resposta, 10);

                if (toupper(resposta[0]) == 'S') {
                    printf("Aproxime o cartão ao leitor...\n");

                    enviarSerial("LER_RFID");

                    char buffer[100];
                    int lidos = lerSerial(buffer, 100);

                    if (lidos > 0 && strstr(buffer, "RFID:")) {
                        char uid[50];
                        strcpy(uid, buffer + 5);

                        enviarCadastroUsuarioRFID(nomeNovo, senhaNovo, nivel, uid);

                        strcpy(usuarios[totalUsuarios].nome, nomeNovo);
                        strcpy(usuarios[totalUsuarios].senha, senhaNovo);
                        strcpy(usuarios[totalUsuarios].nivel, nivel);
                        strcpy(usuarios[totalUsuarios].uid, uid);
                        totalUsuarios++;

                        salvarUsuariosEmArquivo();
                        registrarLog("Usuario + RFID cadastrado");

                        printf(GREEN "Usuário cadastrado com RFID!\n" RESET);
                    }

                } else {
                    enviarCadastroUsuario(nomeNovo, senhaNovo, nivel);

                    strcpy(usuarios[totalUsuarios].nome, nomeNovo);
                    strcpy(usuarios[totalUsuarios].senha, senhaNovo);
                    strcpy(usuarios[totalUsuarios].nivel, nivel);
                    strcpy(usuarios[totalUsuarios].uid, "");
                    totalUsuarios++;

                    salvarUsuariosEmArquivo();
                    registrarLog("Usuario cadastrado sem RFID");

                    printf(GREEN "Usuário cadastrado com sucesso!\n" RESET);
                }

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
    printf("Pressione ENTER para sair...");
    getchar();

    return 0;
}
