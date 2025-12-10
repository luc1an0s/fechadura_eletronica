#ifndef USUARIOS_H
#define USUARIOS_H

#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#define MAX_USUARIOS 50

typedef struct {
    int id;
    char nome[50];
    char senha[20];
    int nivel; // 1=Comum, 2=Admin
    char uid[50]; //RFID
    char perguntaSecret[64]; 
    char respostaSecret[64]; 
} Usuario;

void inicializarSistema(void);
//assinatura para receber UID
int cadastrarUsuario(int id, const char* nome, const char* senha, int nivel, const char* uid, const char* perg, const char* resp);
int deletarUsuario(int id);
int verificarLogin(int id, const char* senha);
int buscarPergunta(int id, char* bufferPergunta);
int verificarRecuperacao(int id, const char* respostaTentada);
void alterarSenha(int id, const char* novaSenha);
void registrarLog(const char* evento);
void lerLog(char* bufferDestino, int tamanhoMax);

int getQuantidadeUsuarios(void);
Usuario* getUsuarioPorIndice(int indice);

#endif