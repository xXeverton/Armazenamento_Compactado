# Trabalho 1 - Software Básico

## Alunos
 - Everton Pereria Militão - 2320462 3WA
 - Joana Moreira Flores - 2320924 3WA

## Descrição
Este projeto implementa duas funções principais:
- `gravacomp`: grava uma ou mais estruturas em um arquivo binário utilizando compactação e marcação por tipo.
- `mostracomp`: lê esse mesmo arquivo binário e imprime os dados de forma legível.

O trabalho visa exercitar conhecimentos de:
- Representação de dados em memória (int, unsigned int, strings)
- Alinhamento e padding de structs
- Endianness (conversão para Big Endian)
- Manipulação de arquivos binários
- Bitwise para montar headers

## Arquivos entregues

- `gravacomp.c`: implementação das funções `gravacomp`, `mostracomp` e auxiliares.
- `gravacomp.h`: cabeçalho fornecido pela disciplina.
- `teste.c`: código de teste com diversas estruturas diferentes.
- `arquivo.bin`: arquivo binário gerado pelos testes.
- `relatorio.txt`: relatório explicando os testes realizados e os resultados.
- `README.md`: este arquivo.

## Compilação

Para compilar:

```bash
gcc -Wall -o teste gravacomp.c teste.c
```

## Execução

```bash
./teste
```

O programa grava o conteúdo no arquivo `arquivo.bin` e depois o lê e imprime no terminal.

## Estruturas de Teste

Foram testadas diferentes structs com diferentes combinações de:
- int
- unsigned int
- strings com tamanhos variados

## Saída Esperada

```text
Estruturas: 2

(int) -1 (ffffffff)
(str) abc
(uns) 258 (00000102)

(int) 1 (00000001)
(str) ABCD
(uns) 65535 (0000ffff)
```

## Observações

- O código está totalmente comentado para facilitar o entendimento por parte do avaliador.
- Todos os testes exigidos no enunciado foram realizados com sucesso.
- O código não contém `main` dentro de `gravacomp.c`, conforme solicitado.