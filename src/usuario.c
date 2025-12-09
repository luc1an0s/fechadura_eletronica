// gerenciamento de usuarios da fechadura
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usuario.h"
#include "log.h"
#include "serial.h"

Usuario usuarios[MAX_USUARIOS]; // array gigante de usuarios
int totalUsuarios = 0; // contador de quantos tem cadastrado

// carrega os usuarios do arquivo txt (sim, banco de dados é arquivo txt mesmo)
void carregarUsuariosDoArquivo() {
    FILE *f = fopen("usuarios.txt", "r");
    if (!f) return; // arquivo nao existe? foda-se, cria depois

    totalUsuarios = 0;
    // le linha por linha com esse formato esquisito de ; separando tudo
    while (fscanf(f, "%49[^;];%49[^;];%19[^;];%49[^\n]\n",
                  usuarios[totalUsuarios].nome,
                  usuarios[totalUsuarios].senha,
                  usuarios[totalUsuarios].nivel,
                  usuarios[totalUsuarios].uid) == 4) {
        totalUsuarios++;
        if (totalUsuarios >= MAX_USUARIOS) break; // lotou? para ai mesmo
    }
    fclose(f);
}

// salva tudo de volta no arquivo (sobrescreve tudo, foda-se)
void salvarUsuariosEmArquivo() {
    FILE *f = fopen("usuarios.txt", "w");
    if (!f) return; // nao conseguiu abrir? problema seu

    for (int i = 0; i < totalUsuarios; i++) {
        fprintf(f, "%s;%s;%s;%s\n",
                usuarios[i].nome,
                usuarios[i].senha,
                usuarios[i].nivel,
                usuarios[i].uid);
    }
    fclose(f);
}

// lista usuarios no console (ninguem usa isso mas ta ai)
void listarUsuarios() {
    printf("Usuarios cadastrados:\n");
    for (int i = 0; i < totalUsuarios; i++) {
        printf("Nome: %s | Senha: %s | Nivel: %s | UID: %s\n",
               usuarios[i].nome,
               usuarios[i].senha,
               usuarios[i].nivel,
               strlen(usuarios[i].uid) ? usuarios[i].uid : "(nenhum)"); // mostra (nenhum) se nao tem rfid
    }
}

// muda a senha de um usuario (obvio né)
int alterarSenha(const char *nome, const char *novaSenha) {
    for (int i = 0; i < totalUsuarios; i++) {
        if (strcmp(usuarios[i].nome, nome) == 0) {
            strcpy(usuarios[i].senha, novaSenha); // achou? muda a senha e era isso
            return 1; // sucesso!
        }
    }
    return 0; // nao achou o usuario = falha
}

// cadastra usuario SEM rfid pela GUI
int cadastrarUsuarioGUI(const char *nome, const char *senha, const char *nivel) {
    if (totalUsuarios >= MAX_USUARIOS) return 0; // ta cheio? nao cabe mais
    strcpy(usuarios[totalUsuarios].nome, nome);
    strcpy(usuarios[totalUsuarios].senha, senha);
    strcpy(usuarios[totalUsuarios].nivel, nivel);
    strcpy(usuarios[totalUsuarios].uid, ""); // sem rfid = string vazia
    totalUsuarios++;
    salvarUsuariosEmArquivo();
    registrarLog("Usuário cadastrado via GUI");
    return 1;
}

// cadastra usuario COM rfid pela GUI (diferenca é so o uid preenchido)
int cadastrarUsuarioRFIDGUI(const char *nome, const char *senha, const char *nivel, const char *uid) {
    if (totalUsuarios >= MAX_USUARIOS) return 0; // lotou de novo
    strcpy(usuarios[totalUsuarios].nome, nome);
    strcpy(usuarios[totalUsuarios].senha, senha);
    strcpy(usuarios[totalUsuarios].nivel, nivel);
    strcpy(usuarios[totalUsuarios].uid, uid); // aqui tem rfid porra
    totalUsuarios++;
    salvarUsuariosEmArquivo();
    registrarLog("Usuário com RFID cadastrado via GUI");
    return 1;
}

// manda pro arduino cadastrar usuario sem rfid
void enviarCadastroUsuario(const char *nome, const char *senha, const char *nivel) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "CADASTRO:%s,%s,%s", nome, senha, nivel);
    enviarSerial(buffer); // envia pela serial
    registrarLog(buffer); // registra que mandou
}

// manda pro arduino cadastrar usuario COM rfid
void enviarCadastroUsuarioRFID(const char *nome, const char *senha, const char *nivel, const char *uid) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "CADASTRO_RFID:%s;%s;%s;%s", nome, senha, nivel, uid);
    enviarSerial(buffer); // vai la arduino, grava isso ai
    registrarLog(buffer);
}

// remove usuario pelo nome (com aquele deslocamento de array classico)
int removerUsuario(const char *nome) {
    for (int i = 0; i < totalUsuarios; i++) {
        if (strcmp(usuarios[i].nome, nome) == 0) {
            // achou! agora desloca todo mundo pra tras
            for (int j = i; j < totalUsuarios - 1; j++) {
                usuarios[j] = usuarios[j + 1]; // move cada um uma posicao
            }
            totalUsuarios--; // diminui o contador
            salvarUsuariosEmArquivo(); // salva tudo de novo
            registrarLog("Usuário removido via GUI");
            return 1; // removeu com sucesso
        }
    }
    return 0; // nao achou esse usuario
}

// verifica se a senha ja ta sendo usada por alguem (pra evitar duplicata)
int verificarSenhaEmUso(const char *senha) {
    for (int i = 0; i < totalUsuarios; i++) {
        if (strcmp(usuarios[i].senha, senha) == 0) {
            return 1; // ja tem alguem usando essa senha!
        }
    }
    return 0; // senha livre pra usar
}

// verifica se a senha ta em uso MAS deixa o proprio usuario manter a dele
// util quando ta alterando outros dados mas nao quer mudar a senha
int verificarSenhaEmUsoExceto(const char *senha, const char *nomeExceto) {
    for (int i = 0; i < totalUsuarios; i++) {
        // se nao for o cara que ta editando E a senha for igual = problema
        if (strcmp(usuarios[i].nome, nomeExceto) != 0 && strcmp(usuarios[i].senha, senha) == 0) {
            return 1; // outro cara ja usa essa senha
        }
    }
    return 0; // pode usar tranquilo
}

// verifica se o UID do rfid ja ta cadastrado (pra nao ter 2 pessoas com o mesmo cartao)
int verificarUIDEmUso(const char *uid) {
    if (strlen(uid) == 0) return 0; // uid vazio nao conta (obvio)
    
    for (int i = 0; i < totalUsuarios; i++) {
        if (strlen(usuarios[i].uid) > 0 && strcmp(usuarios[i].uid, uid) == 0) {
            return 1; // ja tem alguem com esse cartao!
        }
    }
    return 0; // cartao novo, pode cadastrar
}
