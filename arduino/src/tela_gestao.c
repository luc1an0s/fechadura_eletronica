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

    // --- Fundo e Moldura Principal ---
    DrawRectangle(pX, pY, wGest, hGest, colCard);
    DrawRectangleLinesEx((Rectangle){pX, pY, wGest, hGest}, 2, colAzulNeon);
    
    DrawText("GESTAO DE USUARIOS", pX + 20, pY + 20, 24, colAzulNeon);
    DrawRectangle(pX + 20, pY + 50, wGest - 40, 2, colAzulNeon);

    // Botão Voltar
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x00000000);
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(colTextoCinza));
    if (GuiButton((Rectangle){pX + wGest - 100, pY + 15, 80, 30}, "VOLTAR")) {
        *telaAtual = TELA_ADMIN;
    }

    // --- ÁREA DE SCROLL PRINCIPAL (DA PÁGINA) ---
    Rectangle viewMain = {pX + 20, pY + 70, wGest - 40, hGest - 90};
    
    int alturaFormulario = 600; 
    int alturaListaBox = 350; 
    int alturaTotalConteudo = alturaFormulario + alturaListaBox + 50;
    
    Rectangle contentMain = {0, 0, viewMain.width - 20, (float)alturaTotalConteudo};

    // Fundo do conteúdo
    DrawRectangleRec(viewMain, BLACK);
    
    // Scroll Principal
    int prevBg = GuiGetStyle(DEFAULT, BACKGROUND_COLOR);
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(BLACK));
    GuiScrollPanel(viewMain, NULL, contentMain, &app->scrollMain, &viewMain);
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, prevBg);

    BeginScissorMode(viewMain.x, viewMain.y, viewMain.width, viewMain.height);

        int scrollY = (int)app->scrollMain.y;
        int baseX = viewMain.x + 10;
        int currentY = viewMain.y + 10 + scrollY;
        int widthInput = viewMain.width - 40; 

        // ==========================================
        //       FORMULÁRIO DE CADASTRO
        // ==========================================
        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
        DrawText("Novo Cadastro", baseX, currentY, 20, WHITE);
        currentY += 40;

        GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(colAzulNeon));
        GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, ColorToInt(colAzulNeon));

        // Nome
        GuiLabel((Rectangle){baseX, currentY, 100, 20}, "Nome Completo:");
        if (GuiTextBox((Rectangle){baseX, currentY + 20, widthInput, 30}, app->admNom, 49, app->eNom)) {
            app->eNom = !app->eNom;
        }
        currentY += 60;

        // ID e Nível
        GuiLabel((Rectangle){baseX, currentY, 50, 20}, "ID:");
        if (GuiTextBox((Rectangle){baseX, currentY + 20, 150, 30}, app->admId, 9, app->eId)) {
            app->eId = !app->eId;
        }

        int xNivel = baseX + 200;
        GuiLabel((Rectangle){xNivel, currentY, 200, 20}, "Nivel de Acesso:");
        
        // Botões de Nível (Comum / Admin)
        Rectangle btnComum = {xNivel, currentY + 20, 100, 30};
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(GREEN));
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
        if (app->novoNivelSelecionado == 0) GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(RED));
        else GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(GREEN));

        if (GuiButton(btnComum, "COMUM")) app->novoNivelSelecionado = 0;
        if (app->novoNivelSelecionado == 0) DrawRectangleLinesEx(btnComum, 2, RED);

        Rectangle btnAdmin = {xNivel + 110, currentY + 20, 100, 30};
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(YELLOW));
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
        if (app->novoNivelSelecionado == 1) GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(RED));
        else GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(YELLOW));

        if (GuiButton(btnAdmin, "ADMIN")) app->novoNivelSelecionado = 1;
        if (app->novoNivelSelecionado == 1) DrawRectangleLinesEx(btnAdmin, 2, RED);

        currentY += 60;

        // Senhas
        GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(colAzulNeon));
        GuiLabel((Rectangle){baseX, currentY, 100, 20}, "Senha:");
        strcpy(app->admS1Visual, app->admS1);
        if (!app->showAdmPass1) { for(int i=0; i<strlen(app->admS1Visual); i++) app->admS1Visual[i] = '*'; }
        
        if (GuiTextBox((Rectangle){baseX, currentY + 20, (widthInput/2) - 40, 30}, app->admS1Visual, 19, app->eS1)) app->eS1 = !app->eS1;
        GuiCheckBox((Rectangle){baseX + (widthInput/2) - 35, currentY + 20, 30, 30}, "", &app->showAdmPass1);
        if (app->eS1 && app->showAdmPass1) strcpy(app->admS1, app->admS1Visual); 

        int xConf = baseX + (widthInput/2) + 10;
        GuiLabel((Rectangle){xConf, currentY, 150, 20}, "Confirmar:");
        strcpy(app->admS2Visual, app->admS2);
        if (!app->showAdmPass2) { for(int i=0; i<strlen(app->admS2Visual); i++) app->admS2Visual[i] = '*'; }

        if (GuiTextBox((Rectangle){xConf, currentY + 20, (widthInput/2) - 50, 30}, app->admS2Visual, 19, app->eS2)) app->eS2 = !app->eS2;
        GuiCheckBox((Rectangle){xConf + (widthInput/2) - 45, currentY + 20, 30, 30}, "", &app->showAdmPass2);
        if (app->eS2 && app->showAdmPass2) strcpy(app->admS2, app->admS2Visual);

        currentY += 60;

        // Pergunta/Resposta
        GuiLabel((Rectangle){baseX, currentY, 400, 20}, "Pergunta de Seguranca:");
        if (GuiTextBox((Rectangle){baseX, currentY + 20, widthInput, 30}, app->admPg, 60, app->ePg)) app->ePg = !app->ePg;
        currentY += 60;

        GuiLabel((Rectangle){baseX, currentY, 400, 20}, "Resposta de Seguranca:");
        if (GuiTextBox((Rectangle){baseX, currentY + 20, widthInput, 30}, app->admRp, 60, app->eRp)) app->eRp = !app->eRp;
        currentY += 60;

        // Botão Cadastrar
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(colAzulNeon));
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(colAzulNeon));
        
        if (GuiButton((Rectangle){baseX, currentY, widthInput, 40}, "CADASTRAR USUARIO")) {
            if (!strlen(app->admS1) || !strlen(app->admPg)) {
                strcpy(app->admMsg, "Preencha todos os campos!");
            } else if (strcmp(app->admS1, app->admS2)) {
                strcpy(app->admMsg, "Senhas nao conferem!");
            } else {
                int nivelFinal = (app->novoNivelSelecionado == 1) ? 2 : 1;
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
        DrawText(app->admMsg, baseX, currentY, 20, (strstr(app->admMsg, "Erro")) ? RED : GREEN);
        currentY += 40;

        DrawLine(baseX, currentY, baseX + widthInput, currentY, colAzulNeon);
        currentY += 20;

        // ==========================================
        //       LISTA DE USUÁRIOS
        // ==========================================
        DrawText("Usuarios Cadastrados", baseX, currentY, 20, WHITE);
        
        // --- CAIXA DE BUSCA (Apenas Números/ID) ---
        DrawText("Buscar ID:", baseX + 250, currentY + 2, 18, LIGHTGRAY);
        GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL, ColorToInt((Color){30, 30, 30, 255}));
        GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(GRAY));
        
        if (GuiTextBox((Rectangle){baseX + 350, currentY, 200, 25}, app->termoBusca, 64, app->editandoBusca)) {
            app->editandoBusca = !app->editandoBusca;
        }
        currentY += 40;

        // --- PREPARAÇÃO DA LISTA FILTRADA ---
        int totalUsuarios = getQuantidadeUsuarios();
        int itensFiltrados = 0;
        
        // Conta itens visíveis (Filtro simples via strstr para IDs)
        for (int i = 0; i < totalUsuarios; i++) {
            Usuario* u = getUsuarioPorIndice(i);
            if (u) {
                char idStr[16]; 
                sprintf(idStr, "%d", u->id);
                // Se a busca estiver vazia OU se o ID contém o número digitado
                if (strlen(app->termoBusca) == 0 || strstr(idStr, app->termoBusca) != NULL) {
                    itensFiltrados++;
                }
            }
        }

        // --- CONFIGURAÇÃO DO SCROLL INTERNO DA LISTA ---
        Rectangle viewList = {baseX, currentY, widthInput, 300}; 
        
        int alturaItem = 45;
        int alturaConteudoList = itensFiltrados * alturaItem;
        if (alturaConteudoList < viewList.height) alturaConteudoList = viewList.height;

        Rectangle contentList = {0, 0, viewList.width - 20, (float)alturaConteudoList};

        // Fundo da Lista (Fundo base escuro)
        DrawRectangleRec(viewList, (Color){15, 15, 20, 255});
        DrawRectangleLinesEx(viewList, 1, DARKGRAY);

        // Estilo Scrollbar
        int scrollWidthOrig = GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);
        GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, 14);
        GuiSetStyle(SCROLLBAR, BORDER_WIDTH, 0);
        
        GuiScrollPanel(viewList, NULL, contentList, &app->scrollUserList, &viewList);
        
        GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, scrollWidthOrig);

        // --- DESENHO DOS ITENS ---
        BeginScissorMode(viewList.x, viewList.y, viewList.width, viewList.height);
            
            int desenhados = 0;
            int yListaBase = viewList.y + (int)app->scrollUserList.y;

            if (itensFiltrados == 0) {
                // Fundo cinza cobrindo a área da lista quando não há resultados
                DrawRectangleRec(viewList, (Color){100, 100, 100, 255});
                DrawText("Nenhum usuario encontrado.", viewList.x + 20, viewList.y + 20, 20, WHITE);
            }

            for (int i = 0; i < totalUsuarios; i++) {
                Usuario* u = getUsuarioPorIndice(i);
                if (u) {
                    char idStr[16]; 
                    sprintf(idStr, "%d", u->id);
                    
                    // --- FILTRO SIMPLIFICADO (Só ID) ---
                    if (strlen(app->termoBusca) == 0 || strstr(idStr, app->termoBusca) != NULL) {
                        
                        int itemY = yListaBase + (desenhados * alturaItem);
                        
                        // Otimização de renderização
                        if (itemY + alturaItem > viewList.y && itemY < viewList.y + viewList.height) {
                            
                            // Fundo zebrado (Cinza Médio e Cinza Escuro)
                            Color corFundo = (desenhados % 2 == 0) ? (Color){100, 100, 100, 255} : (Color){60, 60, 60, 255};
                            DrawRectangle(viewList.x, itemY, viewList.width - 15, alturaItem, corFundo);
                            
                            const char* labelNivel = (u->nivel == 2) ? "[ADMIN]" : "[COMUM]";
                            Color corNivel = (u->nivel == 2) ? YELLOW : GREEN;

                            // Textos agora em BRANCO (WHITE)
                            DrawText(TextFormat("ID: %d", u->id), viewList.x + 10, itemY + 12, 20, WHITE);
                            DrawText(u->nome, viewList.x + 100, itemY + 12, 20, WHITE);
                            DrawText(labelNivel, viewList.x + viewList.width - 180, itemY + 12, 18, corNivel);

                            if (u->id != 999) {
                                Rectangle btnDel = {viewList.x + viewList.width - 80, itemY + 8, 50, 28};
                                // Cores do botão mantidas
                                GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(MAROON));
                                GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
                                
                                Vector2 mouse = GetMousePosition();
                                bool mouseNaAreaLista = CheckCollisionPointRec(mouse, viewList);

                                if (mouseNaAreaLista && GuiButton(btnDel, "X")) {
                                    deletarUsuario(u->id);
                                }
                            }
                        }
                        desenhados++;
                    }
                }
            }

        EndScissorMode();

    EndScissorMode(); 
}