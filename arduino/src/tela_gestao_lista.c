#include "tela_gestao_lista.h"
#include "usuario.h"
#include <stdio.h>

void DesenharListaGestao(AppData* app, int x, int* y, int width, Rectangle viewMainArea) {
    int currentY = *y;

    // Linha divisória e Título
    DrawLine(x, currentY, x + width, currentY, colAzulNeon);
    currentY += 20;
    DrawText("Usuarios Cadastrados", x, currentY, 20, WHITE);
    currentY += 30;

    // Definição da View (Janela visível da lista)
    Rectangle viewList = {x, currentY, width, 300}; 
    
    int totalUsuarios = getQuantidadeUsuarios();
    int alturaItem = 45;
    int alturaTotalLista = totalUsuarios * alturaItem;
    
    float contentH;
    if (alturaTotalLista > viewList.height) {
        contentH = (float)alturaTotalLista;
    } else {
        contentH = viewList.height;
    }

    Rectangle contentList = {0, 0, viewList.width - 20, contentH};

    // --- CUSTOMIZAÇÃO DA SCROLLBAR (Estilo Minimalista da Imagem) ---
    // Salva os estilos originais para restaurar depois
    int prevScrollWidth = GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);
    int prevBorderWidth = GuiGetStyle(LISTVIEW, BORDER_WIDTH);
    int prevScrollPadding = GuiGetStyle(SCROLLBAR, SCROLL_PADDING);
    int prevSliderPadding = GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING);

    // Aplica o estilo "Flat/Cinza" da imagem
    GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, 12);           // Largura da barra
    GuiSetStyle(LISTVIEW, BORDER_WIDTH, 0);               // Sem borda na lista
    GuiSetStyle(SCROLLBAR, BORDER_WIDTH, 0);              // Sem borda na barra
    GuiSetStyle(SCROLLBAR, SCROLL_PADDING, 0);            // Sem padding externo
    GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING, 0);     // Sem padding interno

    // Cores específicas para parecer com a imagem
    GuiSetStyle(SCROLLBAR, BASE_COLOR_NORMAL, ColorToInt((Color){30, 30, 30, 255})); // Fundo do trilho (cinza escuro)
    GuiSetStyle(SCROLLBAR, BASE_COLOR_PRESSED, ColorToInt((Color){100, 100, 100, 255})); // Cor da barra (cinza médio)
    GuiSetStyle(SCROLLBAR, BASE_COLOR_FOCUSED, ColorToInt((Color){120, 120, 120, 255})); // Cor ao passar o mouse
    GuiSetStyle(SCROLLBAR, TEXT_COLOR_NORMAL, ColorToInt((Color){100, 100, 100, 255}));  // Cor padrão
    
    // Desenha Fundo da Lista (Preto/Cinza muito escuro para contraste)
    DrawRectangleRec(viewList, (Color){10, 10, 10, 255});
    
    // Desenha o Painel de Scroll
    GuiScrollPanel(viewList, NULL, contentList, &app->scrollUserList, &viewList);
    
    // Restaura os estilos originais para não afetar o resto do sistema
    GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, prevScrollWidth);
    GuiSetStyle(LISTVIEW, BORDER_WIDTH, prevBorderWidth);
    GuiSetStyle(SCROLLBAR, BORDER_WIDTH, 1); // Reset padrão
    GuiSetStyle(SCROLLBAR, SCROLL_PADDING, prevScrollPadding);
    GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING, prevSliderPadding);
    // ----------------------------------------------------------------

    // Scissor Mode para a lista interna
    BeginScissorMode(viewList.x, viewList.y, viewList.width, viewList.height);
        
        for (int i = 0; i < totalUsuarios; i++) {
            Usuario* u = getUsuarioPorIndice(i);
            if (u != NULL) {
                int listY = viewList.y + (int)app->scrollUserList.y + (i * alturaItem);
                
                const char* labelNivel;
                Color corNivel;
                if (u->nivel == 2) { 
                    labelNivel = "[ADMIN]"; 
                    corNivel = YELLOW; 
                } else { 
                    labelNivel = "[COMUM]"; 
                    corNivel = GREEN; 
                }
                
                // Texto
                DrawText(TextFormat("ID: %d", u->id), viewList.x + 10, listY + 12, 20, WHITE);
                DrawText(u->nome, viewList.x + 100, listY + 12, 20, WHITE);
                DrawText(labelNivel, viewList.x + width - 200, listY + 12, 18, corNivel);

                // Botão Excluir
                if (u->id != 999) {
                    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(RED));
                    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
                    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(MAROON));
                    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(RED)); 
                    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(BLACK));
                    
                    Rectangle btnDel = {viewList.x + width - 90, listY + 8, 60, 25};
                    
                    // Colisão Dupla
                    Vector2 mouse = GetMousePosition();
                    if (CheckCollisionPointRec(mouse, viewList) && CheckCollisionPointRec(mouse, viewMainArea)) {
                        if (GuiButton(btnDel, "X")) {
                            deletarUsuario(u->id);
                            break; 
                        }
                    } else {
                        DrawRectangleRec(btnDel, RED);
                        DrawText("X", btnDel.x + 25, btnDel.y + 2, 20, WHITE);
                    }
                }
                // Linha divisória suave entre itens
                DrawLine(viewList.x, listY + alturaItem, viewList.x + viewList.width - 15, listY + alturaItem, (Color){40, 40, 40, 255});
            }
        }
    EndScissorMode();

    *y = currentY + 310; // Avança o Y principal
}