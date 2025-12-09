#ifndef USUARIO_H
#define USUARIO_H

#include <windows.h>

#define MAX_USUARIOS 10

typedef struct {
    char nome[50];
    char senha[50];
    char nivel[20];
    char uid[50];
} Usuario;

extern Usuario usuarios[MAX_USUARIOS];
extern int totalUsuarios;

void carregarUsuariosDoArquivo();
void salvarUsuariosEmArquivo();
void listarUsuarios();
int alterarSenha(const char *nome, const char *novaSenha);
int cadastrarUsuarioGUI(const char *nome, const char *senha, const char *nivel);
int cadastrarUsuarioRFIDGUI(const char *nome, const char *senha, const char *nivel, const char *uid);
void enviarCadastroUsuario(const char *nome, const char *senha, const char *nivel);
void enviarCadastroUsuarioRFID(const char *nome, const char *senha, const char *nivel, const char *uid);
int removerUsuario(const char *nome);
int verificarSenhaEmUso(const char *senha);
int verificarSenhaEmUsoExceto(const char *senha, const char *nomeExceto);
int verificarUIDEmUso(const char *uid);

#endif
