#ifndef ESTILO_TELA_H
#define ESTILO_TELA_H

#include "raylib.h"
#include "raygui.h"
#include <stdbool.h>

typedef enum { 
    TELA_LOGIN, 
    TELA_ADMIN, 
    TELA_RECUPERACAO,
    TELA_GESTAO
} EstadoTela;

typedef struct {
    // Login
    char txtId[20];
    char senhaReal[20];
    char senhaVisual[20];
    bool editId;
    bool editSenha;
    char msgLogin[100];
    bool showLoginPass;

    // Admin / Gestão
    char admNom[50]; 
    char admId[10];
    char admS1[20]; 
    char admS2[20];
    char admS1Visual[20]; 
    char admS2Visual[20];
    char admPg[64]; 
    char admRp[64];
    bool eNom, eId, eS1, eS2, ePg, eRp;
    char admMsg[100];
    bool showAdmPass1, showAdmPass2;
    
    // Controle do nível (0=Comum, 1=Admin)
    int novoNivelSelecionado; 

    // Logs
    char bufferLog[8192];
    Vector2 scrollLog;

    // Gestão
    Vector2 scrollMain;      // Scroll da página inteira
    Vector2 scrollUserList;  // Scroll da lista de usuarios

    // Recuperação
    int passoRec;
    char recId[10];
    char recPergunta[64];
    char recResp[64];
    char recNova[20];
    bool eRecId, eRecResp, eRecNova;
} AppData;

extern Color colFundo;
extern Color colCard;
extern Color colInputBgNorm;
extern Color colInputBgFoc;
extern Color colAzulNeon;
extern Color colTextoBranco;
extern Color colTextoCinza;

void ConfigurarEstilos(void);

#endif