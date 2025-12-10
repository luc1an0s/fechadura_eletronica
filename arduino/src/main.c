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

    #ifdef _WIN32
        setupSerial("COM3");
    #else
        setupSerial("/dev/ttyACM0");
    #endif 

    SetTargetFPS(60);
    
    EstadoTela tela = TELA_LOGIN;
    AppData app = {0}; 
    app.novoNivelSelecionado = 0;
    
    strcpy(app.recPergunta, "...");
    strcpy(app.msgLogin, "Sistema Pronto.");

    ConfigurarEstilos();

    while (!WindowShouldClose()) {
        char bufferLido[32];
        int n = lerSerial(bufferLido, sizeof(bufferLido)-1);
        
        if (n > 0) {
            for (int i = 0; i < n; i++) {
                char c = bufferLido[i];
                if (tela == TELA_LOGIN) {
                    if (c >= '0' && c <= '9' && strlen(app.senhaReal) < 19) {
                        int len = strlen(app.senhaReal); 
                        app.senhaReal[len] = c; 
                        app.senhaReal[len+1] = 0;
                    } 
                    else if (c == '*') {
                        strcpy(app.senhaReal, ""); 
                    }
                    else if (c == '#') { 
                        int niv = verificarLogin(atoi(app.txtId), app.senhaReal);
                        if (niv != -1) {
                            char l[64]; 
                            sprintf(l, "Acesso LIBERADO ID %s", app.txtId); 
                            registrarLog(l); 
                            enviarComando('A');
                            
                            if (niv == 2) {
                                tela = TELA_ADMIN; 
                            } else {
                                strcpy(app.msgLogin, "Acesso Permitido!");
                            }
                        } else {
                            char l[64]; 
                            sprintf(l, "Acesso NEGADO ID %s", app.txtId); 
                            registrarLog(l); 
                            enviarComando('N');
                            strcpy(app.msgLogin, "Credenciais Invalidas!");
                        }
                        strcpy(app.senhaReal, "");
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
