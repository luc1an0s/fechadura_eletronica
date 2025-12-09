// sistema de logs super sofisticado (arquivo txt basico)
#include "log.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

// registra uma mensagem no arquivo de log com timestamp bonitinho
void registrarLog(const char *mensagem) {
    FILE *f = fopen("log.txt", "a"); // append mode pra nao apagar tudo
    if (!f) return; // nao abriu? foda-se entao

    // pega a hora atual do sistema
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char buffer[64];
    strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", tm_info); // formata bonito: YYYY-MM-DD HH:MM:SS

    fprintf(f, "[%s] %s\n", buffer, mensagem); // escreve com timestamp
    fclose(f); // fecha pq sim
}

// carrega todos os logs do arquivo pra exibir na GUI
// retorna array dinamico de strings (sim, malloc na veia)
char** carregarLogs(int *totalLinhas) {
    FILE *f = fopen("log.txt", "r");
    if (!f) {
        *totalLinhas = 0;
        return NULL; // arquivo nao existe = sem logs
    }

    // primeiro conta quantas linhas tem (pra saber quanto alocar)
    int linhas = 0;
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), f)) {
        linhas++; // vai contando linha por linha
    }
    rewind(f); // volta pro inicio do arquivo

    if (linhas == 0) {
        fclose(f);
        *totalLinhas = 0;
        return NULL; // arquivo vazio
    }

    // aloca array de ponteiros (cada um vai apontar pra uma linha)
    char **logs = (char**)malloc(sizeof(char*) * linhas);
    int i = 0;
    while (fgets(buffer, sizeof(buffer), f) && i < linhas) {
        // tira o \n do final (se tiver)
        if (buffer[strlen(buffer)-1] == '\n') {
            buffer[strlen(buffer)-1] = '\0';
        }
        // aloca memoria pra essa linha especifica
        logs[i] = (char*)malloc(strlen(buffer) + 1);
        strcpy(logs[i], buffer); // copia a linha
        i++;
    }
    fclose(f);
    *totalLinhas = i; // retorna quantas linhas leu
    return logs; // retorna o array
}

// libera a memoria alocada pelos logs (pra nao vazar memoria que nem peneira)
void liberarLogs(char **logs, int totalLinhas) {
    for (int i = 0; i < totalLinhas; i++) {
        free(logs[i]); // libera cada linha
    }
    free(logs); // libera o array de ponteiros
}
