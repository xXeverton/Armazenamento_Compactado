/* Nome_do_Aluno1 Matricula Turma */
/* Nome_do_Aluno2 Matricula Turma */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gravacomp.h"

// Funções auxiliares

// Conta quantos campos existem no descritor
int conta_campos(char *descritor) {
    int campos = 0;
    int i = 0;
    while (descritor[i] != '\0') {
        if (descritor[i] == 'i' || descritor[i] == 'u') {
            campos++;
            i += 1;
        } else if (descritor[i] == 's') {
            campos++;
            i += 3; // s + dois dígitos
        }
    }
    return campos;
}

// Calcula quantos bytes são necessários para um unsigned int
static int bytes_necessarios_unsigned(unsigned int valor) {
    if (valor <= 0xFF) return 1;
    if (valor <= 0xFFFF) return 2;
    if (valor <= 0xFFFFFF) return 3;
    return 4;
}

// Calcula quantos bytes são necessários para um signed int
static int bytes_necessarios_signed(int valor) {
    if (valor >= -128 && valor <= 127) return 1;
    if (valor >= -32768 && valor <= 32767) return 2;
    if (valor >= -8388608 && valor <= 8388607) return 3;
    return 4;
}

// Implementação da função gravacomp
int gravacomp(int nstructs, void* valores, char* descritor, FILE* arquivo) {
    unsigned char *ptr = (unsigned char*)valores; // ponteiro para percorrer structs
    int i;

    if (fputc((unsigned char)nstructs, arquivo) == EOF) {
        return -1; // erro ao gravar
    }

    // Para cada struct
    for (i = 0; i < nstructs; i++) {
        int pos_descritor = 0;
        int n_campos = conta_campos(descritor);
        int campo_atual = 0;

        // Para cada campo
        while (descritor[pos_descritor] != '\0') {
            char tipo = descritor[pos_descritor];

            if (tipo == 'i') {
                int valor;
                memcpy(&valor, ptr, sizeof(int));

                int num_bytes = bytes_necessarios_signed(valor);

                unsigned char header = 0;
                if (campo_atual == n_campos - 1) header |= 0x80; // cont = 1 se último
                header |= (1 << 5); // tipo = 01 (signed int)
                header |= num_bytes; // tamanho

                if (fputc(header, arquivo) == EOF) return -1;

                // Correção: grava big endian
                for (int k = 0; k < num_bytes; k++) {
                    unsigned char byte = (valor >> (8 * (num_bytes - k - 1))) & 0xFF;
                    if (fputc(byte, arquivo) == EOF) return -1;
                }

                ptr += sizeof(int);
                pos_descritor += 1;
                campo_atual += 1;
            }
            
            else if (tipo == 'u') {
                unsigned int valor;
                memcpy(&valor, ptr, sizeof(unsigned int));

                int num_bytes = bytes_necessarios_unsigned(valor);

                unsigned char header = 0;
                if (campo_atual == n_campos - 1) header |= 0x80; // cont = 1 se último
                header |= (0 << 5); // tipo = 00 (unsigned int)
                header |= num_bytes; // tamanho

                if (fputc(header, arquivo) == EOF) return -1;

                // Correção: grava big endian
                for (int k = 0; k < num_bytes; k++) {
                    unsigned char byte = (valor >> (8 * (num_bytes - k - 1))) & 0xFF;
                    if (fputc(byte, arquivo) == EOF) return -1;
                }

                ptr += sizeof(unsigned int);
                pos_descritor += 1;
                campo_atual += 1;
            }
            
            else if (tipo == 's') {
                int tamanho_str = (descritor[pos_descritor+1] - '0') * 10 + (descritor[pos_descritor+2] - '0');
                char *str = (char*)ptr;

                int len = strlen(str);
                if (len > 63) len = 63; // Proteção extra

                unsigned char header = 0;
                if (campo_atual == n_campos - 1) header |= 0x80; // cont = 1 se último
                header |= (1 << 6); // tipo = 1 para string
                header |= len; // tamanho da string

                if (fputc(header, arquivo) == EOF) return -1;

                if (fwrite(str, 1, len, arquivo) != len) return -1;

                ptr += tamanho_str;
                pos_descritor += 3;
                campo_atual += 1;
            }
        }
    }

    return 0;
}

// Implementação da função mostracomp
void mostracomp(FILE *arquivo) {
    int nstructs;
    int i;
    unsigned char header;

    nstructs = fgetc(arquivo);
    printf("Estruturas: %d\n\n", nstructs);

    for (i = 0; i < nstructs; i++) {
        int ultimo = 0;

        while (!ultimo) {
            header = fgetc(arquivo);
            ultimo = (header & 0x80) != 0; // bit 7

            if ((header & 0x40) != 0) {
                // String
                int tamanho = header & 0x3F;
                char buffer[64];
                fread(buffer, 1, tamanho, arquivo);
                buffer[tamanho] = '\0';
                printf("(str) %s\n", buffer);
            }
            else {
                // Inteiro
                int tipo = (header >> 5) & 0x03; // bits 6-5
                int tamanho = header & 0x1F;     // bits 4-0
                unsigned char bytes[4] = {0, 0, 0, 0};

                fread(bytes + (4 - tamanho), 1, tamanho, arquivo); // alinhar para big endian

                unsigned int valor_unsigned = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
                int valor_signed = (int)valor_unsigned;

                if (tipo == 0) {
                    printf("(uns) %u (%08x)\n", valor_unsigned, valor_unsigned);
                } else if (tipo == 1) {
                    printf("(int) %d (%08x)\n", valor_signed, valor_unsigned);
                }
            }
        }

        printf("\n"); // linha em branco entre structs
    }
}
