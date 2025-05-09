#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gravacomp.h"

/*
 * Arquivo de teste para a biblioteca gravacomp
 * Testa diferentes estruturas com combinações de campos: int, string e unsigned int
 * Cada teste pode ser ativado individualmente comentando/descomentando as linhas
 */

// Struct com dois campos de string e um campo de cada tipo numérico
struct teste1 {
    int  i1;
    char s1[5];
    char s2[3];
    unsigned int u1;
} Teste1[2] = {
    { -1, "abc", "f", 258 },
    {  1, "ABCD", "kk", 65535 }
};

// Struct com strings maiores e sem padding entre campos
struct teste2 {
    int  i1;
    char s1[8];
    char s2[3];
    unsigned int u1;
} Teste2[2] = {
    { 0, "abc",  "ax",     0 },
    { 1, "ABCD", "av", 65535 }
};

// Struct complexa com várias strings, int e unsigned
struct teste3 {
    char s3[5];
    int  i1;
    char s1[12];
    char s2[6];
    unsigned int u1;
} Teste3[3] = {
    { "123x", 35,   "abcddddqw", "ax",   258 },
    { "1224", 1,    "ABzxcCD",   "av", 65535 },
    { "foda", 423,  "ABzxcCD",   "axsv", 7003 }
};

// Struct com apenas inteiros
struct teste4 {
    int  i1;
    int  i2;
    int  i3;
    int  i4;
} Teste4[2] = {
    { -354, 234, 111, -200 },
    {    1,  34, -127,   24 }
};

// Struct com int e duas strings pequenas
struct teste5 {
    int  i1;
    char s1[5];
    char s2[3];
} Teste5[2] = {
    { -23, "acb", "ab" },
    {  42, "aaa", "bb" }
};

int main(void) {
    FILE *arquivo;

    // Abre o arquivo binário para escrita
    arquivo = fopen("arquivo.bin", "wb");
    if (!arquivo) {
        perror("Erro ao abrir arquivo para escrita");
        return 1;
    }

    // === TESTES ===

    // Teste 1: int, string de 5, string de 3, unsigned
    // gravacomp(2, &Teste1, "is05s03u", arquivo); // PASSA

    // Teste 2: int, string de 8, unsigned
    // gravacomp(2, &Teste2, "is08u", arquivo); // PASSA

    // Teste 2 (com string extra): int, string de 8, string de 3, unsigned
    // gravacomp(2, &Teste2, "is08s03u", arquivo); // PASSA

    // Teste 3: string de 5, int, string de 12, string de 6, unsigned
    // gravacomp(3, &Teste3, "s05is12s06u", arquivo); // PASSA 

    // Teste 4: somente inteiros
    // gravacomp(2, &Teste4, "iiii", arquivo); // ATIVO

    // Teste 5: int, string de 5, string de 3
    gravacomp(2, &Teste5, "is05s03", arquivo); // PASSA

    fclose(arquivo);

    // Abre o arquivo binário para leitura
    arquivo = fopen("arquivo.bin", "rb");
    if (!arquivo) {
        perror("Erro ao abrir arquivo para leitura");
        return 1;
    }

    // Exibe o conteúdo gravado usando mostracomp
    mostracomp(arquivo);

    fclose(arquivo);

    return 0;
}
