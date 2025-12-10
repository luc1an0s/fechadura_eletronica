#include "usuario.h"
#include "serial_port.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Includes para o delay (usleep/Sleep)
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

static Usuario lista[MAX_USUARIOS];
static int total = 0;

// --- Função de Comparação para Ordenar (Maior ID primeiro) ---
int compararUsuariosDesc(const void* a, const void* b) {
    Usuario* u1 = (Usuario*)a;
    Usuario* u2 = (Usuario*)b;
    return (u2->id - u1->id); // Decrescente
}

void ordenarLista(void) {
    if (total > 1) {
        qsort(lista, total, sizeof(Usuario), compararUsuariosDesc);
    }
}

// --- Parser de Texto ---
static void extrairCampo(char **cursor, char *destino, int tamanhoMax) {
    if (*cursor == NULL || **cursor == '\0') {
        destino[0] = '\0';
        return;
    }
    char *fim = strchr(*cursor, ';');
    if (fim != NULL) {
        int len = fim - *cursor;
        if (len >= tamanhoMax) len = tamanhoMax - 1;
        strncpy(destino, *cursor, len);
        destino[len] = '\0';
        *cursor = fim + 1;
    } else {
        strncpy(destino, *cursor, tamanhoMax - 1);
        destino[tamanhoMax - 1] = '\0';
        *cursor = NULL;
    }
}

void salvarDados(void) {
    ordenarLista(); 
    FILE *f = fopen("usuarios.txt", "w");
    if (f) {
        for (int i = 0; i < total; i++) {
            fprintf(f, "%d;%s;%s;%d;%s;%s;%s\n",
                    lista[i].id, lista[i].nome, lista[i].senha, lista[i].nivel,
                    lista[i].uid, lista[i].perguntaSecret, lista[i].respostaSecret);
        }
        fclose(f);
    }
}

void carregarDados(void) {
    FILE *f = fopen("usuarios.txt", "r");
    if (!f) return;

    total = 0;
    char linha[512];
    while (fgets(linha, sizeof(linha), f) && total < MAX_USUARIOS) {
        linha[strcspn(linha, "\n")] = 0;
        if (strlen(linha) == 0) continue;

        Usuario u;
        char *cursor = linha;
        char bufferTemp[100];

        extrairCampo(&cursor, bufferTemp, sizeof(bufferTemp));
        u.id = atoi(bufferTemp);
        extrairCampo(&cursor, u.nome, sizeof(u.nome));
        extrairCampo(&cursor, u.senha, sizeof(u.senha));
        extrairCampo(&cursor, bufferTemp, sizeof(bufferTemp));
        u.nivel = atoi(bufferTemp);
        extrairCampo(&cursor, u.uid, sizeof(u.uid));
        extrairCampo(&cursor, u.perguntaSecret, sizeof(u.perguntaSecret));
        extrairCampo(&cursor, u.respostaSecret, sizeof(u.respostaSecret));

        if (u.id > 0 && strlen(u.nome) > 0) {
            lista[total] = u;
            total++;
        }
    }
    fclose(f);
    ordenarLista();
}

void inicializarSistema(void) {
    carregarDados();
    if (total == 0) {
        cadastrarUsuario(999, "Admin Master", "admin", 2, "", "Codigo Mestre?", "0000");
    }
}

int cadastrarUsuario(int id, const char* nome, const char* senha, int nivel, const char* uid, const char* perg, const char* resp) {
    if (total >= MAX_USUARIOS) return 0;
    
    for (int i = 0; i < total; i++) {
        if (lista[i].id == id) return -1; 
        if (strcmp(lista[i].senha, senha) == 0) return -2; 
    }

    lista[total].id = id;
    lista[total].nivel = nivel;
    strcpy(lista[total].nome, nome);
    strcpy(lista[total].senha, senha);
    strcpy(lista[total].uid, uid);
    strcpy(lista[total].perguntaSecret, perg);
    strcpy(lista[total].respostaSecret, resp);

    // Envia para o Arduino
    char comandoArduino[256];
    const char* nivelStr = (nivel == 2) ? "ADMIN" : "COMUM";

    if (strlen(uid) > 0) {
        sprintf(comandoArduino, "CADASTRO_RFID:%d;%s;%s;%s;%s\n", id, nome, senha, nivelStr, uid);
    } else {
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
    if (!f) return;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(f, "[%02d/%02d %02d:%02d] %s\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_hour, tm.tm_min, evento);
    fclose(f);
}

void lerLog(char* buffer, int tamanhoMax) {
    FILE *f = fopen("acessos.log", "r");
    if (!f) { strcpy(buffer, "Sem registros."); return; }
    int lidos = fread(buffer, 1, tamanhoMax - 1, f);
    buffer[lidos] = '\0';
    fclose(f);
}

int getQuantidadeUsuarios(void) { return total; }

Usuario* getUsuarioPorIndice(int indice) {
    if (indice >= 0 && indice < total) return &lista[indice];
    return NULL;
}

int sugerirNovoId(void) {
    int maxId = 0;
    for (int i = 0; i < total; i++) {
        if (lista[i].id > maxId && lista[i].id != 999) {
            maxId = lista[i].id;
        }
    }
    return maxId + 1;
}

// --- FUNÇÃO RESTAURADA ---
// Envia todos os usuários para o Arduino (útil ao iniciar o sistema)
void sincronizarUsuariosComArduino(void) {
    if (total == 0) return;

    printf(">>> Sincronizando %d usuarios com o Arduino...\n", total);

    for (int i = 0; i < total; i++) {
        char comandoArduino[256];
        const char* nivelStr = (lista[i].nivel == 2) ? "ADMIN" : "COMUM";

        if (strlen(lista[i].uid) > 0) {
            sprintf(comandoArduino, "CADASTRO_RFID:%d;%s;%s;%s;%s\n", 
                    lista[i].id, lista[i].nome, lista[i].senha, nivelStr, lista[i].uid);
        } else {
            sprintf(comandoArduino, "CADASTRO:%d;%s;%s;%s\n", 
                    lista[i].id, lista[i].nome, lista[i].senha, nivelStr);
        }

        for (int k = 0; comandoArduino[k] != '\0'; k++) {
            enviarComando(comandoArduino[k]);
        }

        printf("Enviado ID %d...\n", lista[i].id);

        // Pequeno delay para não travar o buffer do Arduino
        #ifdef _WIN32
            Sleep(100); 
        #else
            usleep(100000); // 100ms
        #endif
    }
    printf(">>> Sincronizacao concluida!\n");
}