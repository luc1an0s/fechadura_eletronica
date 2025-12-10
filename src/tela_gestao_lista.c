#include "tela_gestao_lista.h"
#include "usuario.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h> // Para verificar se o scroll está com valor inválido

// Função auxiliar para busca que ignora maiúsculas/minúsculas
int contemTexto(const char* texto, const char* busca) {
    if (!busca || strlen(busca) == 0) return 1; 
    
    char t[100], b[100];
    strncpy(t, texto, 99); t[99] = '\0';
    strncpy(b, busca, 99); b[99] = '\0';
    
    for(int i = 0; t[i]; i++) t[i] = tolower(t[i]);
    for(int i = 0; b[i]; i++) b[i] = tolower(b[i]);
    
    return strstr(t, b) != NULL;
}

void DesenharListaGestao(AppData* app, int x, int* y, int width, Rectangle viewMainArea) {
    int currentY = *y + 20; // Dá um espaço extra do formulário de cima

    // --- SEGURANÇA DO SCROLL ---
    // Se o scroll estiver com valor "Lixo" (NaN), reseta para 0
    if (isnan(app->scrollUserList.y)) app->scrollUserList.y = 0;

    // --- LINHA DO TÍTULO E PESQUISA ---
    DrawText("Lista de Usuarios", x, currentY + 5, 20, WHITE);
    
    // DEFINIÇÃO DA CAIXA DE BUSCA (Alinhada à Direita)
    int larguraBusca = 200;
    int xBusca = x + width - larguraBusca; // Cola na direita
    
    DrawText("Buscar ID/Nome:", xBusca - 110, currentY + 8, 10, LIGHTGRAY);

    // Estilo de Alto Contraste para a Busca
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL, ColorToInt((Color){50, 50, 50, 255})); 
    GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL, ColorToInt(WHITE));
    
    // CORREÇÃO: Usar TEXT_PADDING em vez de TEXT_INNER_PADDING
    GuiSetStyle(TEXTBOX, TEXT_PADDING, 5); 

    if (GuiTextBox((Rectangle){xBusca, currentY, larguraBusca, 25}, app->termoBusca, 64, app->editandoBusca)) {
        app->editandoBusca = !app->editandoBusca;
    }
    
    currentY += 40;

    // --- CÁLCULO DO CONTEÚDO ---
    int totalUsuarios = getQuantidadeUsuarios();
    int itensFiltrados = 0;
    
    // Conta itens visíveis
    for (int i = 0; i < totalUsuarios; i++) {
        Usuario* u = getUsuarioPorIndice(i);
        if (u) {
            char idStr[16];
            sprintf(idStr, "%d", u->id);
            if (contemTexto(idStr, app->termoBusca) || contemTexto(u->nome, app->termoBusca)) {
                itensFiltrados++;
            }
        }
    }

    // --- ÁREA DE LISTA FIXA (300px) ---
    Rectangle viewList = {x, currentY, width, 300}; 
    
    // Desenha o FUNDO da caixa (para você ver que ela existe mesmo vazia)
    DrawRectangleRec(viewList, (Color){20, 20, 20, 255});
    DrawRectangleLinesEx(viewList, 1, DARKGRAY); // Borda

    // Altura do conteúdo interno
    int alturaItem = 45;
    int alturaConteudo = itensFiltrados * alturaItem;
    
    // Se a lista for menor que a caixa, o conteúdo é do tamanho da caixa (para não bugar o fundo)
    if (alturaConteudo < viewList.height) alturaConteudo = viewList.height;

    Rectangle contentList = {0, 0, viewList.width - 20, (float)alturaConteudo};

    // --- CONFIGURAÇÃO DO SCROLLBAR INTERNO ---
    int prevScrollWidth = GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);
    int prevBorder = GuiGetStyle(LISTVIEW, BORDER_WIDTH);
    
    GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, 15); // Barra larga
    GuiSetStyle(LISTVIEW, BORDER_WIDTH, 0);
    GuiSetStyle(SCROLLBAR, BORDER_WIDTH, 0);
    GuiSetStyle(SCROLLBAR, BASE_COLOR_NORMAL, ColorToInt((Color){40, 40, 40, 255}));
    GuiSetStyle(SCROLLBAR, BASE_COLOR_PRESSED, ColorToInt(LIGHTGRAY));

    // Desenha o Painel de Rolagem
    // Nota: O Raygui desenha as barras automaticamente aqui se content > view
    GuiScrollPanel(viewList, NULL, contentList, &app->scrollUserList, &viewList);
    
    // Restaura estilos
    GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, prevScrollWidth);
    GuiSetStyle(LISTVIEW, BORDER_WIDTH, prevBorder);

    // --- DESENHO DOS ITENS (Dentro da Tesoura/Scissor) ---
    BeginScissorMode(viewList.x, viewList.y, viewList.width, viewList.height);
        
        // Se não tiver ninguém, avisa
        if (itensFiltrados == 0) {
            DrawText("Nenhum usuario encontrado.", viewList.x + 20, viewList.y + 20, 20, GRAY);
        }

        int desenhados = 0;
        for (int i = 0; i < totalUsuarios; i++) {
            Usuario* u = getUsuarioPorIndice(i);
            if (u != NULL) {
                char idStr[16];
                sprintf(idStr, "%d", u->id);
                
                if (contemTexto(idStr, app->termoBusca) || contemTexto(u->nome, app->termoBusca)) {
                    
                    int listY = viewList.y + (int)app->scrollUserList.y + (desenhados * alturaItem);
                    
                    // Desenha apenas se estiver visível (Otimização)
                    if (listY + alturaItem > viewList.y && listY < viewList.y + viewList.height) {
                        
                        // Fundo alternado para facilitar leitura
                        if (desenhados % 2 == 0) {
                            DrawRectangle(viewList.x, listY, viewList.width - 15, alturaItem, (Color){30, 30, 30, 255});
                        }

                        const char* labelNivel = (u->nivel == 2) ? "ADMIN" : "COMUM";
                        Color corNivel = (u->nivel == 2) ? YELLOW : GREEN;
                        
                        DrawText(TextFormat("#%d", u->id), viewList.x + 10, listY + 12, 20, GRAY);
                        DrawText(u->nome, viewList.x + 80, listY + 12, 20, WHITE);
                        DrawText(labelNivel, viewList.x + width - 160, listY + 12, 18, corNivel);

                        // Botão Excluir
                        if (u->id != 999) {
                            Rectangle btnDel = {viewList.x + width - 70, listY + 8, 40, 25};
                            GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(MAROON));
                            GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
                            
                            // Colisão precisa corrigida para Scroll
                            Vector2 mouse = GetMousePosition();
                            bool mouseNaLista = CheckCollisionPointRec(mouse, viewList);
                            
                            if (mouseNaLista && GuiButton(btnDel, "X")) {
                                deletarUsuario(u->id);
                            }
                        }
                    }
                    desenhados++;
                }
            }
        }
    EndScissorMode();

    *y = currentY + 310; // Empurra o resto da interface para baixo
}