#include "usuario.h"
#include "serial_port.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definição das variáveis globais
static Usuario lista[MAX_USUARIOS];
static int total = 0;

// --- Função Auxiliar para ler campos separados por ponto e vírgula ---
// Resolve o problema de campos vazios (ex: sem RFID) que quebravam o sscanf
static void extrairCampo(char **cursor, char *destino, int tamanhoMax) {
    if (*cursor == NULL || **cursor == '\0') {
        destino[0] = '\0'; // Campo vazio
        return;
    }

    // Procura o próximo separador ';'
    char *fim = strchr(*cursor, ';');
    
    if (fim != NULL) {
        // Calcula o tamanho do texto atual
        int len = fim - *cursor;
        if (len >= tamanhoMax) len = tamanhoMax - 1;
        
        strncpy(destino, *cursor, len);
        destino[len] = '\0';
        
        *cursor = fim + 1; // Avança o cursor para depois do ;
    } else {
        // Se não achar ';', é o último campo da linha (ou linha sem ;)
        strncpy(destino, *cursor, tamanhoMax - 1);
        destino[tamanhoMax - 1] = '\0';
        *cursor = NULL; // Marca como finalizado
    }
}
// ---------------------------------------------------------------------

void salvarDados(void) {
    FILE *f = fopen("usuarios.txt", "w");
    if (f) {
        for (int i = 0; i < total; i++) {
            // Formato: id;nome;senha;nivel;uid;pergunta;resposta
            fprintf(f, "%d;%s;%s;%d;%s;%s;%s\n",
                    lista[i].id, lista[i].nome, lista[i].senha, lista[i].nivel,
                    lista[i].uid, lista[i].perguntaSecret, lista[i].respostaSecret);
        }
        fclose(f);
    }
}

void carregarDados(void) {
    FILE *f = fopen("usuarios.txt", "r");
    if (!f) {
        return;
    }

    total = 0;
    char linha[512];

    while (fgets(linha, sizeof(linha), f) && total < MAX_USUARIOS) {
        // Remove quebra de linha do final
        linha[strcspn(linha, "\n")] = 0;

        // Se a linha estiver vazia, ignora
        if (strlen(linha) == 0) continue;

        Usuario u;
        char *cursor = linha;
        char bufferTemp[100]; // Buffer auxiliar para números

        // 1. ID
        extrairCampo(&cursor, bufferTemp, sizeof(bufferTemp));
        u.id = atoi(bufferTemp);

        // 2. Nome
        extrairCampo(&cursor, u.nome, sizeof(u.nome));

        // 3. Senha
        extrairCampo(&cursor, u.senha, sizeof(u.senha));

        // 4. Nivel
        extrairCampo(&cursor, bufferTemp, sizeof(bufferTemp));
        u.nivel = atoi(bufferTemp);

        // 5. UID (RFID) - Pode estar vazio
        extrairCampo(&cursor, u.uid, sizeof(u.uid));

        // 6. Pergunta Secreta
        extrairCampo(&cursor, u.perguntaSecret, sizeof(u.perguntaSecret));

        // 7. Resposta Secreta
        extrairCampo(&cursor, u.respostaSecret, sizeof(u.respostaSecret));

        // Só adiciona se tiver pelo menos ID e Nome válidos
        if (u.id > 0 && strlen(u.nome) > 0) {
            lista[total] = u;
            total++;
        }
    }
    fclose(f);
}

void inicializarSistema(void) {
    carregarDados();
    // Se não tiver ninguém, cria o admin padrão
    if (total == 0) {
        cadastrarUsuario(999, "Admin Master", "admin", 2, "", "Codigo Mestre?", "0000");
    }
}

int cadastrarUsuario(int id, const char* nome, const char* senha, int nivel, const char* uid, const char* perg, const char* resp) {
    if (total >= MAX_USUARIOS) {
        return 0;
    }

    // Verifica ID duplicado
    for (int i = 0; i < total; i++) {
        if (lista[i].id == id) {
            return -1;
        }
    }

    lista[total].id = id;
    lista[total].nivel = nivel;
    strcpy(lista[total].nome, nome);
    strcpy(lista[total].senha, senha);
    strcpy(lista[total].uid, uid);
    strcpy(lista[total].perguntaSecret, perg);
    strcpy(lista[total].respostaSecret, resp);

    // --- ATUALIZAÇÃO: Enviando o ID para o Arduino ---
    char comandoArduino[256];
    const char* nivelStr = (nivel == 2) ? "ADMIN" : "COMUM";

    if (strlen(uid) > 0) {
        // Formato: CADASTRO_RFID:ID;NOME;SENHA;NIVEL;UID
        sprintf(comandoArduino, "CADASTRO_RFID:%d;%s;%s;%s;%s\n", id, nome, senha, nivelStr, uid);
    } else {
        // Formato: CADASTRO:ID;NOME;SENHA;NIVEL
        sprintf(comandoArduino, "CADASTRO:%d;%s;%s;%s\n", id, nome, senha, nivelStr);
    }

    for (int k = 0; comandoArduino[k] != '\0'; k++) {
        enviarComando(comandoArduino[k]);
    }

    total++;
    salvarDados();
    return 1;
}
int deletarUsuario(int id) {
    for (int i = 0; i < total; i++) {
        if (lista[i].id == id) {
            // Remove movendo os itens seguintes para trás
            for (int j = i; j < total - 1; j++) {
                lista[j] = lista[j + 1];
            }
            total--;
            salvarDados();
            return 1;
        }
    }
    return 0;
}

int verificarLogin(int id, const char* senha) {
    for (int i = 0; i < total; i++) {
        if (lista[i].id == id && strcmp(lista[i].senha, senha) == 0) {
            return lista[i].nivel;
        }
    }
    return -1;
}

int buscarPergunta(int id, char* bufferPergunta) {
    for (int i = 0; i < total; i++) {
        if (lista[i].id == id) {
            strcpy(bufferPergunta, lista[i].perguntaSecret);
            return 1;
        }
    }
    return 0;
}

int verificarRecuperacao(int id, const char* respostaTentada) {
    for (int i = 0; i < total; i++) {
        if (lista[i].id == id && strcmp(lista[i].respostaSecret, respostaTentada) == 0) {
            return 1;
        }
    }
    return 0;
}

void alterarSenha(int id, const char* novaSenha) {
    for (int i = 0; i < total; i++) {
        if (lista[i].id == id) {
            strcpy(lista[i].senha, novaSenha);
            salvarDados();
            registrarLog("Senha alterada via Recuperacao");
            return;
        }
    }
}

void registrarLog(const char* evento) {
    FILE *f = fopen("acessos.log", "a");
    if (!f) {
        return;
    }
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(f, "[%02d/%02d %02d:%02d] %s\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_hour, tm.tm_min, evento);
    fclose(f);
}

void lerLog(char* buffer, int tamanhoMax) {
    FILE *f = fopen("acessos.log", "r");
    if (!f) {
        strcpy(buffer, "Sem registros.");
        return;
    }
    int lidos = fread(buffer, 1, tamanhoMax - 1, f);
    buffer[lidos] = '\0';
    fclose(f);
}

int getQuantidadeUsuarios(void) {
    return total;
}

Usuario* getUsuarioPorIndice(int indice) {
    if (indice >= 0 && indice < total) {
        return &lista[indice];
    }
    return NULL;
}