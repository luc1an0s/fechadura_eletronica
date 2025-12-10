#include "tela_login.h"
#include "usuario.h"
#include "serial_port.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void DesenharTelaLogin(AppData* app, EstadoTela* telaAtual) {
    int wBox = 340; 
    int hBox = 400;
    int pX = (GetScreenWidth() - wBox) / 2; 
    int pY = (GetScreenHeight() - hBox) / 2;

    DrawRectangle(pX + 10, pY + 10, wBox, hBox, (Color){0,0,0,100});
    DrawRectangle(pX, pY, wBox, hBox, colCard);
    DrawRectangleLinesEx((Rectangle){pX, pY, wBox, hBox}, 2, colAzulNeon);

    DrawText("LOGIN", pX + 20, pY + 20, 24, colAzulNeon);
    DrawRectangle(pX + 20, pY + 50, 300, 2, colAzulNeon);

    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(colTextoBranco));
    
    GuiLabel((Rectangle){pX + 30, pY + 80, 180, 20}, "ID de Usuario:");
    if (GuiTextBox((Rectangle){pX + 30, pY + 105, 280, 35}, app->txtId, 10, app->editId)) {
        if (app->editId) {
            app->editId = false; 
        } else {
            app->editId = true;
        }
    }

    GuiLabel((Rectangle){pX + 30, pY + 155, 180, 20}, "Senha de Acesso:");
    
    strcpy(app->senhaVisual, app->senhaReal);
    if (!app->showLoginPass) {
        for (int i = 0; i < strlen(app->senhaVisual); i++) {
            app->senhaVisual[i] = '*';
        }
    }

    if (GuiTextBox((Rectangle){pX + 30, pY + 180, 240, 35}, app->senhaVisual, 19, app->editSenha)) {
        if (app->editSenha) {
            app->editSenha = false; 
        } else {
            app->editSenha = true;
        }
    }
    
    GuiCheckBox((Rectangle){pX + 275, pY + 180, 35, 35}, "", &app->showLoginPass);

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(colAzulNeon));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    
    if (GuiButton((Rectangle){pX + 30, pY + 250, 280, 45}, "ENTRAR")) {
        int niv = verificarLogin(atoi(app->txtId), app->senhaReal);
        if (niv != -1) {
            char l[64]; 
            sprintf(l, "Acesso LIBERADO ID %s", app->txtId); 
            registrarLog(l);
            enviarComando('A');
            
            if (niv == 2) {
                *telaAtual = TELA_ADMIN; 
            } else {
                strcpy(app->msgLogin, "Acesso Permitido!");
            }
        } else {
            char l[64]; 
            sprintf(l, "Acesso NEGADO ID %s", app->txtId); 
            registrarLog(l);
            enviarComando('N'); 
            strcpy(app->msgLogin, "Credenciais Invalidas!");
        }
        strcpy(app->senhaReal, "");
    }

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x00000000);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(colTextoCinza));
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0x00000000);
    
    if (GuiButton((Rectangle){pX + 80, pY + 310, 180, 30}, "Esqueci a senha")) {
        *telaAtual = TELA_RECUPERACAO; 
        app->passoRec = 0; 
        strcpy(app->recId, "");
    }

    Color corMsg;
    if (strcmp(app->msgLogin, "Acesso Permitido!") == 0) {
        corMsg = GREEN; 
    } else {
        corMsg = RED;
    }
    DrawText(app->msgLogin, pX + 30, pY + 360, 20, corMsg);
}