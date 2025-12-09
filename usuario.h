#ifndef USUARIO_H
#define USUARIO_H

void carregarUsuariosDoArquivo();
void salvarUsuariosEmArquivo();
void listarUsuarios();
void alterarSenha(const char *nome, const char *novaSenha);
void enviarCadastroUsuario(const char *nome, const char *senha, const char *nivel);
void enviarCadastroUsuarioRFID(const char *nome, const char *senha, const char *nivel, const char *uid);

#endif
