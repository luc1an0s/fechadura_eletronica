#include "tela_gestao.h"
#include "usuario.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void DesenharTelaGestao(AppData* app, EstadoTela* telaAtual) {
    int wGest = 900; 
    int hGest = 650;
    int pX = (GetScreenWidth() - wGest) / 2; 
    int pY = (GetScreenHeight() - hGest) / 2;

    DrawRectangle(pX, pY, wGest, hGest, colCard);
    DrawRectangleLinesEx((Rectangle){pX, pY, wGest, hGest}, 2, colAzulNeon);
    
    DrawText("GESTAO DE USUARIOS", pX + 20, pY + 20, 24, colAzulNeon);
    DrawRectangle(pX + 20, pY + 50, wGest - 40, 2, colAzulNeon);

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x00000000);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(colTextoCinza));
    if (GuiButton((Rectangle){pX + wGest - 100, pY + 15, 80, 30}, "VOLTAR")) {
        *telaAtual = TELA_ADMIN;
    }

    Rectangle viewMain = {pX + 20, pY + 70, wGest - 40, hGest - 90};
    
    int totalUsuarios = getQuantidadeUsuarios();
    int alturaItemLista = 50;
    int alturaListaTotal = (totalUsuarios * alturaItemLista) + 50; // +50 para margem/cabeçalho da lista
    int alturaFormulario = 580; // Altura fixa da parte de cima
    
    int alturaTotalConteudo = alturaFormulario + alturaListaTotal;
    
    // Garante que o scroll tenha pelo menos o tamanho da tela
    if (alturaTotalConteudo < viewMain.height) alturaTotalConteudo = viewMain.height;

    Rectangle contentMain = {0, 0, viewMain.width - 20, (float)alturaTotalConteudo};

    DrawRectangleRec(viewMain, BLACK);
    int prevBg = GuiGetStyle(DEFAULT, BACKGROUND_COLOR);
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(BLACK));
    GuiScrollPanel(viewMain, NULL, contentMain, &app->scrollMain, &viewMain);
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, prevBg);

    BeginScissorMode(viewMain.x, viewMain.y, viewMain.width, viewMain.height);

        int scrollY = (int)app->scrollMain.y;
        int baseX = viewMain.x + 10;
        int currentY = viewMain.y + 10 + scrollY;
        int widthInput = viewMain.width - 40; 

        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
        DrawText("Novo Cadastro", baseX, currentY, 20, WHITE);
        currentY += 40;

        GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(colAzulNeon));
        GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, ColorToInt(colAzulNeon));

        GuiLabel((Rectangle){baseX, currentY, 100, 20}, "Nome Completo:");
        if (GuiTextBox((Rectangle){baseX, currentY + 20, widthInput, 30}, app->admNom, 49, app->eNom)) {
            if (app->eNom) app->eNom = false; else app->eNom = true;
        }
        currentY += 60;

        GuiLabel((Rectangle){baseX, currentY, 50, 20}, "ID:");
        if (GuiTextBox((Rectangle){baseX, currentY + 20, 150, 30}, app->admId, 9, app->eId)) {
            if (app->eId) app->eId = false; else app->eId = true;
        }

        int xNivel = baseX + 200;
        GuiLabel((Rectangle){xNivel, currentY, 200, 20}, "Nivel de Acesso:");
        
        // --- BOTÃO COMUM (VERDE) ---
        Rectangle btnComum = {xNivel, currentY + 20, 100, 30};
        
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(GREEN));
        GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(GREEN));
        GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(DARKGREEN));
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
        GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(WHITE));
        GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(WHITE));
        
        if (app->novoNivelSelecionado == 0) {
            GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(RED));
            GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(RED));
        } else {
            GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(GREEN));
            GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(GREEN));
        }

        if (GuiButton(btnComum, "COMUM")) {
            app->novoNivelSelecionado = 0;
        }
        
        if (app->novoNivelSelecionado == 0) {
            DrawRectangleLinesEx(btnComum, 2, RED);
        }

        // --- BOTÃO ADMIN (AMARELO) ---
        Rectangle btnAdmin = {xNivel + 110, currentY + 20, 100, 30};

        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(YELLOW));
        GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(YELLOW));
        GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(ORANGE));
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
        GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(BLACK)); 
        GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(BLACK));
        
        if (app->novoNivelSelecionado == 1) {
            GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(RED));
            GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(RED));
        } else {
            GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(YELLOW));
            GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(YELLOW));
        }

        if (GuiButton(btnAdmin, "ADMIN")) {
            app->novoNivelSelecionado = 1;
        }

        if (app->novoNivelSelecionado == 1) {
            DrawRectangleLinesEx(btnAdmin, 2, RED);
        }

        currentY += 60;

        // Reset Styles
        GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(colAzulNeon));
        GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, ColorToInt(colAzulNeon));
        
        GuiLabel((Rectangle){baseX, currentY, 100, 20}, "Senha:");
        strcpy(app->admS1Visual, app->admS1);
        if (!app->showAdmPass1) { 
            for(int i=0; i<strlen(app->admS1Visual); i++) {
                app->admS1Visual[i] = '*';
            }
        }
        
        if (GuiTextBox((Rectangle){baseX, currentY + 20, (widthInput/2) - 40, 30}, app->admS1Visual, 19, app->eS1)) {
            if (app->eS1) app->eS1 = false; else app->eS1 = true;
        }
        GuiCheckBox((Rectangle){baseX + (widthInput/2) - 35, currentY + 20, 30, 30}, "", &app->showAdmPass1);

        int xConf = baseX + (widthInput/2) + 10;
        GuiLabel((Rectangle){xConf, currentY, 150, 20}, "Confirmar:");
        strcpy(app->admS2Visual, app->admS2);
        if (!app->showAdmPass2) { 
            for(int i=0; i<strlen(app->admS2Visual); i++) {
                app->admS2Visual[i] = '*'; 
            }
        }

        if (GuiTextBox((Rectangle){xConf, currentY + 20, (widthInput/2) - 50, 30}, app->admS2Visual, 19, app->eS2)) {
            if (app->eS2) app->eS2 = false; else app->eS2 = true;
        }
        GuiCheckBox((Rectangle){xConf + (widthInput/2) - 45, currentY + 20, 30, 30}, "", &app->showAdmPass2);
        currentY += 60;

        GuiLabel((Rectangle){baseX, currentY, 400, 20}, "Pergunta de Seguranca:");
        if (GuiTextBox((Rectangle){baseX, currentY + 20, widthInput, 30}, app->admPg, 60, app->ePg)) {
            if (app->ePg) app->ePg = false; else app->ePg = true;
        }
        currentY += 60;

        GuiLabel((Rectangle){baseX, currentY, 400, 20}, "Resposta de Seguranca:");
        if (GuiTextBox((Rectangle){baseX, currentY + 20, widthInput, 30}, app->admRp, 60, app->eRp)) {
            if (app->eRp) app->eRp = false; else app->eRp = true;
        }
        currentY += 60;

        // Botão Cadastrar (Azul Neon)
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(colAzulNeon));
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(colAzulNeon));
        GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(colInputBgFoc));
        GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(BLACK));
        GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(WHITE));
        
        if (GuiButton((Rectangle){baseX, currentY, widthInput, 40}, "CADASTRAR USUARIO")) {
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

                if (cadastrarUsuario(atoi(app->admId), app->admNom, app->admS1, nivelFinal, "", app->admPg, app->admRp)) {
                    strcpy(app->admMsg, "Usuario Cadastrado!");
                    strcpy(app->admNom, ""); strcpy(app->admId, ""); strcpy(app->admS1, ""); strcpy(app->admS2, "");
                    strcpy(app->admPg, ""); strcpy(app->admRp, "");
                } else {
                    strcpy(app->admMsg, "Erro: ID ja existe!");
                }
            }
        }
        
        currentY += 50;
        Color corMsg; 
        if (strcmp(app->admMsg, "Usuario Cadastrado!") == 0) {
            corMsg = GREEN; 
        } else {
            corMsg = RED;
        }
        DrawText(app->admMsg, baseX, currentY, 20, corMsg);
        currentY += 40;

        DrawLine(baseX, currentY, baseX + widthInput, currentY, colAzulNeon);
        currentY += 20;

        // --- LISTA DE USUÁRIOS (AGORA NO MESMO FLUXO) ---
        DrawText("Usuarios Cadastrados", baseX, currentY, 20, WHITE);
        currentY += 40;

        for (int i = 0; i < totalUsuarios; i++) {
            Usuario* u = getUsuarioPorIndice(i);
            
            if (u != NULL) {
                // Desenha cada linha da lista (Fundo Zebrado)
                if (i % 2 == 0) {
                    DrawRectangle(baseX, currentY, widthInput, alturaItemLista, (Color){20, 20, 20, 255});
                } else {
                    DrawRectangle(baseX, currentY, widthInput, alturaItemLista, (Color){30, 30, 30, 255});
                }
                
                const char* labelNivel;
                Color corNivel;
                if (u->nivel == 2) { 
                    labelNivel = "[ADMIN]"; 
                    corNivel = YELLOW; 
                } else { 
                    labelNivel = "[COMUM]"; 
                    corNivel = GREEN; 
                }
                
                DrawText(TextFormat("ID: %d", u->id), baseX + 10, currentY + 15, 20, WHITE);
                DrawText(u->nome, baseX + 100, currentY + 15, 20, WHITE);
                DrawText(labelNivel, baseX + widthInput - 200, currentY + 15, 18, corNivel);

                // Botão Excluir
                if (u->id != 999) {
                    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(RED));
                    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
                    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(MAROON));
                    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(RED)); 
                    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(BLACK));
                    
                    Rectangle btnDel = {baseX + widthInput - 90, currentY + 10, 60, 25};
                    
                    Vector2 mouse = GetMousePosition();
                    // Importante: Checa colisão com a View Principal, senão clica "através" do header
                    if (CheckCollisionPointRec(mouse, viewMain)) {
                        if (GuiButton(btnDel, "X")) {
                            deletarUsuario(u->id);
                            break; // Sai do loop para recarregar no próximo frame
                        }
                    } else {
                        // Desenha estático se estiver fora da área visível (scrollado pra cima)
                        DrawRectangleRec(btnDel, RED);
                        DrawText("X", btnDel.x + 25, btnDel.y + 2, 20, WHITE);
                    }
                }
                
                // Avança Y para o próximo item
                currentY += alturaItemLista;
            }
        }

    EndScissorMode();
}