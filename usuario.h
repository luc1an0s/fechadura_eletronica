#ifndef USUARIO_H
#define USUARIO_H

typedef struct {
    char nome[50];
    char senha[20];
} Usuario;

void cadastrarUsuario();
void listarUsuarios();

#endif