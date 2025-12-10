#include "estiloTela.h"

// A implementação do Raygui fica AQUI
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// Definição das Cores Globais
Color colFundo       = (Color){ 10, 15, 25, 255 }; // Azul muito escuro
Color colCard        = (Color){ 5, 5, 5, 255 };    // Preto quase puro
Color colInputBgNorm = (Color){ 20, 25, 35, 255 }; // Fundo input normal
Color colInputBgFoc  = (Color){ 130, 255, 255, 255 }; // Fundo input focado (ciano)
Color colAzulNeon    = (Color){ 30, 144, 255, 255 };
Color colTextoBranco = WHITE;
Color colTextoCinza  = (Color){ 150, 150, 150, 255 };

void ConfigurarEstilos(void) {
    GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
    GuiSetStyle(DEFAULT, BORDER_WIDTH, 1);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(colTextoBranco));
    
    // Estilo dos Inputs
    GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL, ColorToInt(colInputBgNorm));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(GREEN));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL, ColorToInt(colAzulNeon));
    GuiSetStyle(TEXTBOX, BASE_COLOR_FOCUSED, ColorToInt(colInputBgFoc));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, ColorToInt(BLACK));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED, ColorToInt(WHITE));

    // Estilo dos Checkbox
    GuiSetStyle(CHECKBOX, BASE_COLOR_NORMAL, ColorToInt(colInputBgNorm));
    GuiSetStyle(CHECKBOX, BORDER_COLOR_NORMAL, ColorToInt(colAzulNeon));
    GuiSetStyle(CHECKBOX, TEXT_COLOR_NORMAL, ColorToInt(colTextoBranco));
    GuiSetStyle(CHECKBOX, BASE_COLOR_PRESSED, ColorToInt(colAzulNeon));
    GuiSetStyle(CHECKBOX, BORDER_COLOR_PRESSED, ColorToInt(colAzulNeon));
}