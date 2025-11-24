#ifndef USUARIO_H
#define USUARIO_H

#define MAX_USUARIOS 10
#define MAX_SENHA 20

typedef struct {
    char nome[50];
    char senha[MAX_SENHA];
} Usuario;

extern Usuario usuarios[MAX_USUARIOS];
extern int totalUsuarios;

void cadastrarUsuario(const char *nome, const char *senha);
int validarUsuario(const char *senha);

#endif