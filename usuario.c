#include "usuario.h"
#include <stdio.h>
#include <stdlib.h>

void cadastrarUsuario() {
    Usuario u;
    printf("Nome: ");
    scanf("%s", u.nome);
    printf("Senha: ");
    scanf("%s", u.senha);

    FILE *f = fopen("usuarios.dat", "ab");
    fwrite(&u, sizeof(Usuario), 1, f);
    fclose(f);
    printf("Usuario cadastrado!\n");
}

void listarUsuarios() {
    Usuario u;
    FILE *f = fopen("usuarios.dat", "rb");
    if (!f) { printf("Nenhum usuario.\n"); return; }
    while (fread(&u, sizeof(Usuario), 1, f)) {
        printf("Nome: %s | Senha: %s\n", u.nome, u.senha);
    }
    fclose(f);
}