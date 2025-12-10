#include "tela_gestao_form.h"
#include "usuario.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void DesenharFormularioGestao(AppData* app, int x, int* y, int width) {
    int curY = *y;
    
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    DrawText("Novo Cadastro", x, curY, 20, WHITE);
    curY += 40;

    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(colAzulNeon));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, ColorToInt(colAzulNeon));

    // Campo Nome
    GuiLabel((Rectangle){x, curY, 100, 20}, "Nome Completo:");
    if (GuiTextBox((Rectangle){x, curY + 20, width, 30}, app->admNom, 49, app->eNom)) {
        if (app->eNom) app->eNom = false; else app->eNom = true;
    }
    curY += 60;

    // Campo ID
    GuiLabel((Rectangle){x, curY, 50, 20}, "ID:");
    if (GuiTextBox((Rectangle){x, curY + 20, 150, 30}, app->admId, 9, app->eId)) {
        if (app->eId) app->eId = false; else app->eId = true;
    }

    int xNivel = x + 200;
    GuiLabel((Rectangle){xNivel, curY, 200, 20}, "Nivel de Acesso:");
    
    // --- BOTÃO COMUM (Verde) ---
    Rectangle btnComum = {xNivel, curY + 20, 100, 30};
    
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(GREEN));
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(GREEN));
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(DARKGREEN)); 
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));       
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(WHITE));      
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(WHITE));

    if (app->novoNivelSelecionado == 0) {
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(RED));
        GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(RED));
        GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt(RED));
    } else {
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(GREEN));
        GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(GREEN));
        GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt(GREEN));
    }

    if (GuiButton(btnComum, "COMUM")) {
        app->novoNivelSelecionado = 0;
    }
    
    if (app->novoNivelSelecionado == 0) {
        DrawRectangleLinesEx(btnComum, 2, RED);
    }

    // --- BOTÃO ADMIN (Amarelo) ---
    Rectangle btnAdmin = {xNivel + 110, curY + 20, 100, 30};

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(YELLOW));
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(YELLOW));
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(ORANGE));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(WHITE)); 
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(BLACK));

    if (app->novoNivelSelecionado == 1) {
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(RED));
        GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(RED));
        GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt(RED));
    } else {
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(YELLOW));
        GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(YELLOW));
        GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt(YELLOW));
    }

    if (GuiButton(btnAdmin, "ADMIN")) {
        app->novoNivelSelecionado = 1;
    }

    if (app->novoNivelSelecionado == 1) {
        DrawRectangleLinesEx(btnAdmin, 2, RED);
    }

    curY += 60;

    // Resetando estilo TextBox para o padrão
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(colAzulNeon));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, ColorToInt(colAzulNeon));
    
    // Senha
    GuiLabel((Rectangle){x, curY, 100, 20}, "Senha:");
    strcpy(app->admS1Visual, app->admS1);
    if (!app->showAdmPass1) { for(int i=0; i<strlen(app->admS1Visual); i++) app->admS1Visual[i] = '*'; }
    
    if (GuiTextBox((Rectangle){x, curY + 20, (width/2) - 40, 30}, app->admS1Visual, 19, app->eS1)) {
        if (app->eS1) app->eS1 = false; else app->eS1 = true;
    }
    GuiCheckBox((Rectangle){x + (width/2) - 35, curY + 20, 30, 30}, "", &app->showAdmPass1);
    
    if (app->eS1 && app->showAdmPass1) strcpy(app->admS1, app->admS1Visual);
    else if (app->eS1 && !app->showAdmPass1 && strlen(app->admS1Visual) > 0 && app->admS1Visual[0] != '*') strcpy(app->admS1, app->admS1Visual);

    // Confirmar
    int xConf = x + (width/2) + 10;
    GuiLabel((Rectangle){xConf, curY, 150, 20}, "Confirmar:");
    strcpy(app->admS2Visual, app->admS2);
    if (!app->showAdmPass2) { for(int i=0; i<strlen(app->admS2Visual); i++) app->admS2Visual[i] = '*'; }

    if (GuiTextBox((Rectangle){xConf, curY + 20, (width/2) - 50, 30}, app->admS2Visual, 19, app->eS2)) {
        if (app->eS2) app->eS2 = false; else app->eS2 = true;
    }
    GuiCheckBox((Rectangle){xConf + (width/2) - 45, curY + 20, 30, 30}, "", &app->showAdmPass2);
    
    if (app->eS2 && app->showAdmPass2) strcpy(app->admS2, app->admS2Visual);

    curY += 60;

    // Pergunta
    GuiLabel((Rectangle){x, curY, 400, 20}, "Pergunta de Seguranca:");
    if (GuiTextBox((Rectangle){x, curY + 20, width, 30}, app->admPg, 60, app->ePg)) {
        if (app->ePg) app->ePg = false; else app->ePg = true;
    }
    curY += 60;

    // Resposta
    GuiLabel((Rectangle){x, curY, 400, 20}, "Resposta de Seguranca:");
    if (GuiTextBox((Rectangle){x, curY + 20, width, 30}, app->admRp, 60, app->eRp)) {
        if (app->eRp) app->eRp = false; else app->eRp = true;
    }
    curY += 60;

    // --- CONFIGURAÇÃO DO BOTÃO CADASTRAR ---
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(colAzulNeon));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(colAzulNeon));
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(colInputBgFoc));
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(BLACK));
    
    if (GuiButton((Rectangle){x, curY, width, 40}, "CADASTRAR USUARIO")) {
        if (!strlen(app->admS1) || !strlen(app->admPg)) {
            strcpy(app->admMsg, "Preencha todos os campos!");
        } else if (strcmp(app->admS1, app->admS2)) {
            strcpy(app->admMsg, "Senhas nao conferem!");
        } else {
            int nivelFinal;
            if (app->novoNivelSelecionado == 1) {
                nivelFinal = 2; 
            } else {
                nivelFinal = 1;
            }

            // Nota: Passa string vazia "" no lugar do UID pois o Arduino não lê RFID no cadastro simples via tela
            if (cadastrarUsuario(atoi(app->admId), app->admNom, app->admS1, nivelFinal, "", app->admPg, app->admRp)) {
                strcpy(app->admMsg, "Usuario Cadastrado e Enviado!");
                // Limpa campos
                strcpy(app->admNom, ""); strcpy(app->admId, ""); strcpy(app->admS1, ""); strcpy(app->admS2, "");
                strcpy(app->admPg, ""); strcpy(app->admRp, "");
            } else {
                strcpy(app->admMsg, "Erro: ID ja existe!");
            }
        }
    }
    
    curY += 50;
    Color corMsg; 
    if (strcmp(app->admMsg, "Usuario Cadastrado e Enviado!") == 0) corMsg = GREEN; else corMsg = RED;
    DrawText(app->admMsg, x, curY, 20, corMsg);
    curY += 40;

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(colAzulNeon));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(colAzulNeon));
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(colInputBgFoc));
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(BLACK));
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(WHITE));
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(colAzulNeon)); 
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt(colAzulNeon));

    // Atualiza o ponteiro Y para que a próxima função saiba onde desenhar
    *y = curY;
}