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
    
    // Botões Nivel
    Rectangle btnComum = {xNivel, curY + 20, 100, 30};
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(GREEN));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    
    if (app->novoNivelSelecionado == 0) GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(RED));
    else GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(GREEN));

    if (GuiButton(btnComum, "COMUM")) app->novoNivelSelecionado = 0;
    if (app->novoNivelSelecionado == 0) DrawRectangleLinesEx(btnComum, 2, RED);

    Rectangle btnAdmin = {xNivel + 110, curY + 20, 100, 30};
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(YELLOW));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(BLACK));

    if (app->novoNivelSelecionado == 1) GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(RED));
    else GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(YELLOW));

    if (GuiButton(btnAdmin, "ADMIN")) app->novoNivelSelecionado = 1;
    if (app->novoNivelSelecionado == 1) DrawRectangleLinesEx(btnAdmin, 2, RED);

    curY += 60;

    // Resetando estilo
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(colAzulNeon));
    
    // Senha
    GuiLabel((Rectangle){x, curY, 100, 20}, "Senha:");
    strcpy(app->admS1Visual, app->admS1);
    if (!app->showAdmPass1) { for(int i=0; i<strlen(app->admS1Visual); i++) app->admS1Visual[i] = '*'; }
    
    if (GuiTextBox((Rectangle){x, curY + 20, (width/2) - 40, 30}, app->admS1Visual, 19, app->eS1)) {
        app->eS1 = !app->eS1;
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
        app->eS2 = !app->eS2;
    }
    GuiCheckBox((Rectangle){xConf + (width/2) - 45, curY + 20, 30, 30}, "", &app->showAdmPass2);
    if (app->eS2 && app->showAdmPass2) strcpy(app->admS2, app->admS2Visual);

    curY += 60;

    // Pergunta e Resposta
    GuiLabel((Rectangle){x, curY, 400, 20}, "Pergunta de Seguranca:");
    if (GuiTextBox((Rectangle){x, curY + 20, width, 30}, app->admPg, 60, app->ePg)) app->ePg = !app->ePg;
    curY += 60;

    GuiLabel((Rectangle){x, curY, 400, 20}, "Resposta de Seguranca:");
    if (GuiTextBox((Rectangle){x, curY + 20, width, 30}, app->admRp, 60, app->eRp)) app->eRp = !app->eRp;
    curY += 60;

    // Botão Cadastrar
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(colAzulNeon));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    
    if (GuiButton((Rectangle){x, curY, width, 40}, "CADASTRAR USUARIO")) {
        if (!strlen(app->admS1) || !strlen(app->admPg)) {
            strcpy(app->admMsg, "Preencha todos os campos!");
        } else if (strcmp(app->admS1, app->admS2)) {
            strcpy(app->admMsg, "Senhas nao conferem!");
        } else {
            int nivelFinal = (app->novoNivelSelecionado == 1) ? 2 : 1;
            
            // Tenta cadastrar e verifica o retorno
            int res = cadastrarUsuario(atoi(app->admId), app->admNom, app->admS1, nivelFinal, "", app->admPg, app->admRp);
            
            if (res == 1) {
                strcpy(app->admMsg, "Usuario Cadastrado e Enviado!");
                // Limpa tudo (incluindo ID para gerar novo na proxima vez)
                strcpy(app->admNom, ""); strcpy(app->admId, ""); strcpy(app->admS1, ""); strcpy(app->admS2, "");
                strcpy(app->admPg, ""); strcpy(app->admRp, "");
            } else if (res == -1) {
                strcpy(app->admMsg, "ERRO: ID ja existe! Tente outro.");
            } else if (res == -2) {
                strcpy(app->admMsg, "ERRO: Senha ja usada por outro!");
            } else {
                strcpy(app->admMsg, "Erro desconhecido.");
            }
        }
    }
    
    curY += 50;
    Color corMsg = (strstr(app->admMsg, "ERRO") != NULL) ? RED : GREEN;
    DrawText(app->admMsg, x, curY, 20, corMsg);
    curY += 40;

    // Reset styles
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(colAzulNeon));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(colAzulNeon));

    *y = curY;
}