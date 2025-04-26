#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gravacomp.h" // incluir o seu header fornecido

// Definimos uma struct para testar
struct exemplo {
    int i;
    char nome[5];   // 5 bytes, incluindo \0
    unsigned int u;
};

int main() {
    struct exemplo dados[2] = {
        { -1, "abc", 258 },
        { 1, "ABCD", 65535 }
    };

    FILE *arquivo;

    // GRAVAR
    arquivo = fopen("saida.bin", "wb");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        return 1;
    }

    // descrição: "is05u" → int, string de tamanho 5, unsigned int
    if (gravacomp(2, dados, "is05u", arquivo) != 0) {
        printf("Erro ao gravar!\n");
        fclose(arquivo);
        return 1;
    }

    fclose(arquivo);

    // LER
    arquivo = fopen("saida.bin", "rb");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para leitura");
        return 1;
    }

    mostracomp(arquivo);

    fclose(arquivo);

    return 0;
}
