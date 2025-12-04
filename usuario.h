#ifndef USUARIO_H
#define USUARIO_H

typedef struct {
    char nome[50];
    char senha[20];
    char nivel[20];
} Usuario;

extern Usuario usuarios[50];
extern int totalUsuarios;

void enviarCadastroUsuario(const char *nome, const char *senha, const char *nivel);
void listarUsuarios();
void alterarSenha(const char *nome, const char *novaSenha);
void salvarUsuariosEmArquivo();
void carregarUsuariosDoArquivo();
int deletarUsuario(const char *nome);

#endif
