#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gravacomp.h"

// ------------------------------
// Funções Auxiliares Gerais (usadas por gravacomp e mostracomp)
// ------------------------------

// Converte string numérica para inteiro (ex: "05" -> 5)
int string_para_inteiro(char *s) {
    int resultado = 0;
    for (; *s; s++)
        resultado = resultado * 10 + (*s - '0');
    return resultado;
}

// Verifica se é o último campo da estrutura a partir do header
int campo_final(unsigned char cabecalho) {
    return (cabecalho & 0x80) == 0x80;  // Bit 7 indica último campo
}

// Retorna o tipo do campo a partir do header
char tipo_campo(unsigned char cabecalho) {
    if ((cabecalho & 0x40) == 0x40) return 's'; // Bit 6: string
    if ((cabecalho & 0x20) == 0x20) return 'i'; // Bit 5: int
    return 'u'; // Caso nenhum dos bits acima: unsigned
}

// Extrai o tamanho em bytes do header (bits menos significativos)
unsigned char tamanho_em_bytes(unsigned char cabecalho, char tipo) {
    return cabecalho & (tipo == 's' ? 0x3F : 0x1F); // Máscara depende do tipo
}

// Verifica se o byte mais significativo indica número negativo (bit 7 = 1)
int eh_negativo(unsigned char byte_mais_significativo) {
    return (byte_mais_significativo & 0x80) == 0x80;
}

// ------------------------------
// Funções Auxiliares usadas apenas em gravacomp
// ------------------------------

// Calcula quantos bytes são necessários para representar um int com sinal
unsigned char tamanho_signed(int numero) {
    if (numero == 0) return 1;
    if (numero >= 128 && numero <= 255) return 2;
    if (numero >= 32768 && numero <= 65535) return 3;
    if (numero >= 8388608) return 4;

    int i = 31;
    while (i-- && (numero & (1 << i)) != (1 << i));
    if (i < 7) return 1;
    if (i < 15) return 2;
    if (i < 23) return 3;
    return 4;
}

// Calcula quantos bytes são necessários para representar um unsigned int
unsigned char tamanho_unsigned(unsigned int numero) {
    if (numero == 0) return 1;
    int bytes = 0, i = 0;
    for (int j = 0; j < 4; j++) {
        if ((numero >> i) != 0) bytes++;
        i += 8;
    }
    return bytes;
}

// Cria o cabeçalho para um int ou unsigned int
unsigned char montar_header_int(unsigned char eh_ultimo, unsigned char tamanho, int eh_signed) {
    unsigned char header = tamanho;
    if (eh_ultimo) header |= (1 << 7); // Define bit de último campo
    if (eh_signed) header |= (1 << 5); // Define bit de tipo signed
    return header;
}

// Cria o cabeçalho para uma string
unsigned char montar_header_string(unsigned char eh_ultimo, unsigned char tamanho) {
    unsigned char header = tamanho;
    if (eh_ultimo) header |= (1 << 7); // Define bit de último campo
    header |= (1 << 6); // Define bit de string
    return header;
}

// Calcula padding para alinhamento de int/unsigned int (4 bytes)
unsigned char calcular_padding(int posicao) {
    int soma = 0;
    while (posicao % 4 != 0) {
        posicao++;
        soma++;
    }
    return soma;
}

// Retorna a diferença de endereço (ajuda no cálculo de alinhamento)
int posicao_byte(unsigned char *inicio, unsigned char *atual) {
    return atual - inicio;
}

// ------------------------------
// Função gravacomp
// ------------------------------

int gravacomp(int nstructs, void *valores, char *descritor, FILE *arquivo) {
    unsigned char byte_auxiliar;
    unsigned char *ponteiro = (unsigned char *)valores; // Ponteiro para percorrer os bytes da struct
    unsigned char *inicio = ponteiro; // Salva o início da struct
    unsigned int valor_unsigned;
    int valor_int;
    char tamanho_str_str[3];
    int tamanho_str;
    unsigned char header;
    unsigned char eh_ultimo = 0;
    unsigned char tamanho;
    int posicao;

    // Grava o número de structs no arquivo (1 byte)
    if (fwrite(&nstructs, sizeof(unsigned char), 1, arquivo) != 1) return -1;

    // Loop para cada struct
    while (nstructs--) {
        for (int i = 0; i < strlen(descritor); i++) {
            // Verifica se é o último campo da struct
            if (i == strlen(descritor) - 1 || (descritor[i] == 's' && strlen(descritor) - i == 3)) {
                eh_ultimo = 1;
            }

            switch (descritor[i]) {
                case 's': // Campo tipo string
                    // Lê o tamanho da string a partir do descritor (ex: s05)
                    tamanho_str_str[0] = descritor[i + 1];
                    tamanho_str_str[1] = descritor[i + 2];
                    tamanho_str_str[2] = '\0';
                    tamanho_str = string_para_inteiro(tamanho_str_str);

                    // Calcula tamanho real da string e monta o header
                    tamanho = strlen((char *)ponteiro);
                    header = montar_header_string(eh_ultimo, tamanho);

                    // Grava o header e a string no arquivo
                    if (fwrite(&header, 1, 1, arquivo) != 1) return -1;
                    if (fwrite(ponteiro, 1, tamanho, arquivo) != tamanho) return -1;

                    ponteiro += tamanho_str; // Avança no ponteiro o espaço da string
                    i += 2; // Avança no descritor
                    break;

                case 'i': // Campo tipo int
                    posicao = posicao_byte(inicio, ponteiro);
                    ponteiro += calcular_padding(posicao); // Alinha ponteiro

                    valor_int = *((int *)ponteiro);
                    tamanho = tamanho_signed(valor_int);
                    header = montar_header_int(eh_ultimo, tamanho, 1);
                    if (fwrite(&header, 1, 1, arquivo) != 1) return -1;

                    // Grava os bytes do int em Big Endian
                    for (int b = tamanho; b > 0; b--) {
                        byte_auxiliar = (valor_int >> (8 * (b - 1))) & 0xFF;
                        if (fwrite(&byte_auxiliar, 1, 1, arquivo) != 1) return -1;
                    }
                    ponteiro += sizeof(int);
                    break;

                case 'u': // Campo tipo unsigned int
                    posicao = posicao_byte(inicio, ponteiro);
                    ponteiro += calcular_padding(posicao);

                    valor_unsigned = *((unsigned int *)ponteiro);
                    tamanho = tamanho_unsigned(valor_unsigned);
                    header = montar_header_int(eh_ultimo, tamanho, 0);
                    if (fwrite(&header, 1, 1, arquivo) != 1) return -1;

                    for (int b = tamanho; b > 0; b--) {
                        byte_auxiliar = (valor_unsigned >> (8 * (b - 1))) & 0xFF;
                        if (fwrite(&byte_auxiliar, 1, 1, arquivo) != 1) return -1;
                    }
                    ponteiro += sizeof(unsigned int);
                    break;
            }
            eh_ultimo = 0; // Reset para o próximo campo
        }
    }
    return 0;
}

// ------------------------------
// Função mostracomp
// ------------------------------

void mostracomp(FILE *arquivo) {
    char texto[64]; // Buffer para leitura de strings
    int nstructs;
    unsigned char cabecalho;
    char tipo;
    unsigned char num_bytes;
    unsigned char bytes[4];
    int numero = 0;

    // Lê e imprime o número de structs
    nstructs = fgetc(arquivo);
    printf("Estruturas: %d\n\n", nstructs);

    while (nstructs--) {
        int ultimo = 0;
        while (!ultimo) {
            cabecalho = fgetc(arquivo); // Lê o header
            ultimo = campo_final(cabecalho);
            tipo = tipo_campo(cabecalho);
            num_bytes = tamanho_em_bytes(cabecalho, tipo);

            numero = 0;

            switch (tipo) {
                case 's':
                    fread(texto, 1, num_bytes, arquivo); // Lê string
                    texto[num_bytes] = '\0';
                    printf("(str) %s\n", texto);
                    break;

                case 'i':
                    fread(bytes, 1, num_bytes, arquivo); // Lê bytes
                    for (int j = 0; j < num_bytes; j++) {
                        numero = (numero << 8) | bytes[j]; // Reconstrói int
                    }
                    if (eh_negativo(bytes[0])) {
                        numero |= (-1 << (8 * num_bytes)); // Ajusta sinal
                    }
                    printf("(int) %d (%08x)\n", numero, numero);
                    break;

                case 'u':
                    fread(bytes, 1, num_bytes, arquivo);
                    for (int j = 0; j < num_bytes; j++) {
                        numero = (numero << 8) | bytes[j]; // Reconstrói unsigned
                    }
                    printf("(uns) %u (%08x)\n", numero, numero);
                    break;
            }
        }
        printf("\n");
    }
}
