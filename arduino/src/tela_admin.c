#include "tela_admin.h"
#include "usuario.h"
#include "serial_port.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void DesenharTelaAdmin(AppData* app, EstadoTela* telaAtual) {
    int wAdm = 860; 
    int hAdm = 610;
    int pX = (GetScreenWidth() - wAdm) / 2; 
    int pY = (GetScreenHeight() - hAdm) / 2;

    // Fundo e Moldura
    DrawRectangle(pX, pY, wAdm, hAdm, colCard);
    DrawRectangleLinesEx((Rectangle){pX, pY, wAdm, hAdm}, 2, colAzulNeon);
    DrawText("PAINEL DO ADMINISTRADOR", pX + 20, pY + 20, 24, colAzulNeon);
    DrawRectangle(pX + 20, pY + 50, wAdm - 40, 2, colAzulNeon);

    // Botão Sair
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x00000000);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(RED));
    
    if (GuiButton((Rectangle){pX + wAdm - 100, pY + 15, 80, 30}, "SAIR")) {
        *telaAtual = TELA_LOGIN;
    }

    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(colTextoBranco));
    DrawText("Cadastro Rapido (Simples)", pX + 40, pY + 80, 20, WHITE);

    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(colAzulNeon));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, ColorToInt(colAzulNeon));

    // Nome
    GuiLabel((Rectangle){pX + 40, pY + 110, 80, 20}, "Nome:");
    if (GuiTextBox((Rectangle){pX + 40, pY + 130, 350, 30}, app->admNom, 49, app->eNom)) { 
        if (app->eNom) app->eNom = false; else app->eNom = true; 
    }

    // ID
    GuiLabel((Rectangle){pX + 40, pY + 170, 80, 20}, "ID:");
    if (GuiTextBox((Rectangle){pX + 40, pY + 190, 100, 30}, app->admId, 9, app->eId)) { 
        if (app->eId) app->eId = false; else app->eId = true; 
    }

    // Senha
    GuiLabel((Rectangle){pX + 40, pY + 230, 80, 20}, "Senha");
    strcpy(app->admS1Visual, app->admS1);
    if (!app->showAdmPass1) {
        for (int i = 0; i < strlen(app->admS1Visual); i++) {
            app->admS1Visual[i] = '*';
        }
    }
    
    if (GuiTextBox((Rectangle){pX + 40, pY + 250, 115, 30}, app->admS1Visual, 19, app->eS1)) { 
        if (app->eS1) app->eS1 = false; else app->eS1 = true; 
    }
    GuiCheckBox((Rectangle){pX + 160, pY + 250, 30, 30}, "", &app->showAdmPass1);
    
    if (app->eS1 && app->showAdmPass1) strcpy(app->admS1, app->admS1Visual);
    else if (app->eS1 && !app->showAdmPass1) { 
        if(strlen(app->admS1Visual) > 0 && app->admS1Visual[0] != '*') strcpy(app->admS1, app->admS1Visual); 
    }

    // Confirmar Senha
    GuiLabel((Rectangle){pX + 210, pY + 230, 120, 20}, "Confirmar");
    strcpy(app->admS2Visual, app->admS2);
    if (!app->showAdmPass2) {
        for (int i = 0; i < strlen(app->admS2Visual); i++) {
            app->admS2Visual[i] = '*';
        }
    }

    if (GuiTextBox((Rectangle){pX + 210, pY + 250, 145, 30}, app->admS2Visual, 19, app->eS2)) { 
        if (app->eS2) app->eS2 = false; else app->eS2 = true; 
    }
    GuiCheckBox((Rectangle){pX + 360, pY + 250, 30, 30}, "", &app->showAdmPass2);
    
    if (app->eS2 && app->showAdmPass2) strcpy(app->admS2, app->admS2Visual);

    // Reverte estilo padrão para os outros componentes
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(GREEN));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, ColorToInt(BLACK));

    // Botão CADASTRAR USUARIO 
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(colAzulNeon));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(colAzulNeon));
    
    if (GuiButton((Rectangle){pX + 40, pY + 310, 350, 40}, "CADASTRAR USUARIO")) {
        if (!strlen(app->admS1)) {
            strcpy(app->admMsg, "Senha vazia!");
        } else if (strcmp(app->admS1, app->admS2)) {
            strcpy(app->admMsg, "Senhas nao conferem!");
        } else {
            if (cadastrarUsuario(atoi(app->admId), app->admNom, app->admS1, 1, "", "Indefinido", "Indefinido")) {
                strcpy(app->admMsg, "Usuario Cadastrado!");
                // Limpa campos
                strcpy(app->admNom, ""); strcpy(app->admId, ""); 
                strcpy(app->admS1, ""); strcpy(app->admS2, "");
            } else {
                strcpy(app->admMsg, "Erro: ID ja existe!");
            }
        }
    }

    // Feedback visual
    Color corAdmMsg; 
    if (strcmp(app->admMsg, "Usuario Cadastrado!") == 0) corAdmMsg = GREEN; else corAdmMsg = RED;
    DrawText(app->admMsg, pX + 40, pY + 360, 20, corAdmMsg);

    // Botão GESTÃO DE USUÁRIOS
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(colInputBgNorm)); 
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));          
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(WHITE));        
    
    if (GuiButton((Rectangle){pX + 40, pY + 540, 350, 30}, "GESTAO DE USUARIOS")) {
        *telaAtual = TELA_GESTAO;
    }

    // --- COLUNA DA DIREITA: LOGS ---
    DrawLine(pX + 420, pY + 80, pX + 420, pY + 580, colAzulNeon);
    DrawText("Logs de Acesso", pX + 440, pY + 80, 20, WHITE);
    
    Rectangle viewLog = {pX + 440, pY + 110, 400, 380};
    Vector2 textSize = MeasureTextEx(GetFontDefault(), app->bufferLog, 18, 1);
    
    float contentHeight; 
    if (textSize.y > viewLog.height) {
        contentHeight = textSize.y; 
    } else {
        contentHeight = viewLog.height;
    }
    Rectangle contentLog = {0, 0, viewLog.width - 20, contentHeight};

    // Fundo Preto para os logs
    DrawRectangleRec(viewLog, BLACK);
    
    // Scroll do Log
    int prevBg = GuiGetStyle(DEFAULT, BACKGROUND_COLOR); 
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(BLACK));
    GuiScrollPanel(viewLog, NULL, contentLog, &app->scrollLog, &viewLog);
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, prevBg);

    BeginScissorMode(viewLog.x, viewLog.y, viewLog.width, viewLog.height);
        // Texto Verde Matrix para os logs
        DrawText(app->bufferLog, viewLog.x + app->scrollLog.x + 5, viewLog.y + app->scrollLog.y + 5, 18, GREEN);
    EndScissorMode();

    // Botões de Log
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(colAzulNeon)); 
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(colAzulNeon));
    
    if (GuiButton((Rectangle){pX + 440, pY + 530, 190, 30}, "ATUALIZAR")) {
        lerLog(app->bufferLog, sizeof(app->bufferLog));
    }
    
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(DARKGRAY)); 
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    
    if (GuiButton((Rectangle){pX + 650, pY + 530, 190, 30}, "ABRIR PASTA")) {
        abrirPastaLocal();
    }
}