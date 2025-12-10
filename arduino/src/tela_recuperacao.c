#include "tela_recuperacao.h"
#include "usuario.h"
#include <stdlib.h>
#include <string.h>

void DesenharTelaRecuperacao(AppData* app, EstadoTela* telaAtual) {
    int wBox = 500; 
    int hBox = 400;
    int pX = (GetScreenWidth() - wBox) / 2; 
    int pY = (GetScreenHeight() - hBox) / 2;

    DrawRectangle(pX, pY, wBox, hBox, colCard); 
    DrawRectangleLinesEx((Rectangle){pX, pY, wBox, hBox}, 2, colAzulNeon);
    DrawText("RECUPERAR SENHA", pX + 20, pY + 20, 24, colAzulNeon); 
    DrawRectangle(pX + 20, pY + 50, 460, 2, colAzulNeon);
    
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(colTextoBranco));

    if (app->passoRec == 0) {
        GuiLabel((Rectangle){pX + 50, pY + 70, 400, 20}, "Digite seu ID:");
        if (GuiTextBox((Rectangle){pX + 50, pY + 95, 400, 35}, app->recId, 9, app->eRecId)) { 
            if (app->eRecId) {
                app->eRecId = false; 
            } else {
                app->eRecId = true; 
            }
        }
        
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(colAzulNeon)); 
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
        
        if (GuiButton((Rectangle){pX + 50, pY + 150, 400, 40}, "BUSCAR PERGUNTA")) {
            if (buscarPergunta(atoi(app->recId), app->recPergunta)) {
                app->passoRec = 1; 
            } else {
                strcpy(app->recPergunta, "ID NAO ENCONTRADO");
            }
        }
        DrawText(app->recPergunta, pX + 50, pY + 210, 20, RED);
    } 
    else if (app->passoRec == 1) {
        GuiLabel((Rectangle){pX + 50, pY + 70, 400, 20}, "Pergunta de Seguranca:"); 
        DrawText(app->recPergunta, pX + 50, pY + 95, 20, colAzulNeon);
        
        GuiLabel((Rectangle){pX + 50, pY + 130, 400, 20}, "Sua Resposta:");
        if (GuiTextBox((Rectangle){pX + 50, pY + 155, 400, 35}, app->recResp, 60, app->eRecResp)) { 
            if (app->eRecResp) {
                app->eRecResp = false; 
            } else {
                app->eRecResp = true; 
            }
        }
        
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(colAzulNeon)); 
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
        
        if (GuiButton((Rectangle){pX + 50, pY + 210, 400, 40}, "VERIFICAR")) {
            if (verificarRecuperacao(atoi(app->recId), app->recResp)) {
                app->passoRec = 2; 
            } else {
                DrawText("Resposta Incorreta!", pX + 50, pY + 260, 20, RED);
            }
        }
    } 
    else if (app->passoRec == 2) {
        GuiLabel((Rectangle){pX + 50, pY + 70, 400, 20}, "Nova Senha:");
        if (GuiTextBox((Rectangle){pX + 50, pY + 95, 400, 35}, app->recNova, 19, app->eRecNova)) { 
            if (app->eRecNova) {
                app->eRecNova = false; 
            } else {
                app->eRecNova = true; 
            }
        }
        
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(colAzulNeon)); 
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
        
        if (GuiButton((Rectangle){pX + 50, pY + 150, 400, 40}, "SALVAR NOVA SENHA")) {
            alterarSenha(atoi(app->recId), app->recNova); 
            *telaAtual = TELA_LOGIN; 
            strcpy(app->msgLogin, "Senha Alterada com Sucesso!");
        }
    }

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x00000000); 
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(colTextoCinza));
    
    if (GuiButton((Rectangle){pX + 200, pY + 350, 100, 30}, "Voltar")) {
        *telaAtual = TELA_LOGIN;
    }
}