#include <unistd.h>
#include "raylib.h"
#include "estiloTela.h"      // Módulo comum
#include "tela_login.h"      // Módulos das telas
#include "tela_admin.h"
#include "tela_gestao.h"
#include "tela_recuperacao.h"
#include "serial_port.h"
#include "usuario.h"
#include <locale.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    setlocale(LC_ALL, "Portuguese");
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(900, 650, "Sistema de Controle de Acesso");
    SetWindowMinSize(900, 650); 
    inicializarSistema();

    int statusSerial = -1;

    #ifdef _WIN32
        statusSerial = setupSerial("COM3");
    #else 
        statusSerial = setupSerial("/dev/ttyACM0");
    #endif 
    if (statusSerial != 0) {
        printf("Erro ao abrir serial. O Arduino esta conectado?\n");
    } else {
        printf("Serial conectada com sucesso.\n");
        // Espera o Arduino reiniciar (quando abre a serial, o Uno reseta)
        usleep(2); 
    }

    // 2. Carrega os dados do arquivo para a memória do C
    inicializarSistema(); 

    // 3. envia tudo para o Arduino
    if (statusSerial == 0) {
        sincronizarUsuariosComArduino();
    }
    SetTargetFPS(60);
    
    EstadoTela tela = TELA_LOGIN;
    AppData app = {0}; 
    app.novoNivelSelecionado = 0;
    
    strcpy(app.recPergunta, "...");

    strcpy(app.msgLogin, "Sistema Pronto.");
    printf("Limpando buffer inicial...\n");
        char lixo[256];
        int bytesLixo = 0;
        do {
            bytesLixo = lerSerial(lixo, sizeof(lixo));
            if (bytesLixo > 0) {
                printf("Ignorando lixo serial: %.*s\n", bytesLixo, lixo);
            }
            usleep(10000); // 10ms pausa
        } while (bytesLixo > 0);
    ConfigurarEstilos();

    app.editId = true;       // Começa editando o ID
    app.editSenha = false;

    while (!WindowShouldClose()) {
        char bufferLido[32];
        int n = lerSerial(bufferLido, sizeof(bufferLido)-1);
        
        if (n > 0) {
            for (int i = 0; i < n; i++) {
                char c = bufferLido[i];
                
                if (tela == TELA_LOGIN) {
                    
                    // --- CASO 1: CANCELAR / LIMPAR (*) ---
                    if (c == '*') {
                        strcpy(app.txtId, "");
                        strcpy(app.senhaReal, "");
                        app.editId = true;      // Volta o foco para o ID
                        app.editSenha = false;
                        strcpy(app.msgLogin, "Limpo.");
                    }
                    
                    // --- CASO 2: ENTER / AVANÇAR (#) ---
                    else if (c == '#') {
                        if (app.editId) {
                            // Se estava no ID, muda para a SENHA
                            app.editId = false;
                            app.editSenha = true;
                        } 
                        else {
                            // Se já estava na SENHA, tenta fazer LOGIN
                            int niv = verificarLogin(atoi(app.txtId), app.senhaReal);
                            
                            if (niv != -1) {
                                // LOGIN SUCESSO
                                char l[64]; 
                                sprintf(l, "Acesso LIBERADO ID %s", app.txtId); 
                                registrarLog(l); 
                                enviarComando('A');
                                
                                if (niv == 2) tela = TELA_ADMIN; 
                                else strcpy(app.msgLogin, "Acesso Permitido!");
                            } else {
                                // LOGIN FALHA
                                char l[64]; 
                                sprintf(l, "Acesso NEGADO ID %s", app.txtId); 
                                registrarLog(l); 
                                enviarComando('N');
                                strcpy(app.msgLogin, "Acesso Negado!");
                            }
                            
                            // Limpa campos após tentativa
                            strcpy(app.senhaReal, "");
                            strcpy(app.txtId, "");
                            app.editId = true;      // Reseta para o começo
                            app.editSenha = false;
                        }
                    }
                   // --- CASO 3: NÚMEROS (0-9) ---
                    else if (c >= '0' && c <= '9') {
                        if (app.editId) {
                            // Só escreve no ID se estiver focado nele
                            int len = strlen(app.txtId); 
                            if (len < 9) {
                                app.txtId[len] = c; 
                                app.txtId[len+1] = 0;
                            }
                        } 
                        else if (app.editSenha) { 
                            // Só escreve na Senha se já tiver passado pelo ID (#)
                            int len = strlen(app.senhaReal); 
                            if (len < 19) {
                                app.senhaReal[len] = c; 
                                app.senhaReal[len+1] = 0;
                            }
                        }
                    }
                }
            }
        }

        // Lógica de Teclado PC
        if (app.editSenha || app.eS1 || app.eS2) {
            int k = GetCharPressed();
            char* target = NULL;
            
            if (app.editSenha) {
                target = app.senhaReal;
            } else if (app.eS1) {
                target = app.admS1;
            } else if (app.eS2) {
                target = app.admS2;
            }

            if (target) {
                while (k > 0) {
                    if (k >= 32 && k <= 125 && strlen(target) < 19) {
                        int l = strlen(target); 
                        target[l] = k; 
                        target[l+1] = 0;
                    }
                    k = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE) && strlen(target) > 0) {
                    target[strlen(target)-1] = 0;
                }
            }
        }

        BeginDrawing();
        ClearBackground(colFundo);

        if (tela == TELA_LOGIN) {
            DesenharTelaLogin(&app, &tela);
        } 
        else if (tela == TELA_ADMIN) {
            DesenharTelaAdmin(&app, &tela);
        }
        else if (tela == TELA_GESTAO) {
            DesenharTelaGestao(&app, &tela);
        } 
        else if (tela == TELA_RECUPERACAO) {
            DesenharTelaRecuperacao(&app, &tela);
        }

        EndDrawing();
    }

    fecharSerial();
    CloseWindow();
    
    return 0;
}
