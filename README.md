# OLC++

Este repositório contém um compilador simples para a linguagem OLCPP, com ferramentas de análise léxica e sintática para o **Assembly Raposeitor**.

## Grupo

- Rafael F. Meneses
- Reinaldo K. Neto

## Pipeline

O compilador é dividido em etapas:

1. Análise Léxica
2. Análise Sintática
3. Geração de Código Intermediário
4. Analisador Semântico
5. Geração de Código

O código intermediário (IR) é representado através de uma árvore sintática abstrata (AST) em formato `.JSON`. A AST é gerada durante a análise sintática e é utilizada para a análise semântica e geração de código.

## Documentação

Para mais informações sobre a linguagem OLCPP, consulte o diretório `docs/`. Lá você encontrará a documentação da linguagem, incluindo a gramática, exemplos de código.

## Requerimentos

- [Unix]: Todo o código foi desenvolvido e testado em um ambiente Unix. Não garantimos que o código funcione em um ambiente Windows.
- [Flex](https://github.com/westes/flex): Estamos usando o Flex como nosso analisador léxico.
- [Bison](https://www.gnu.org/software/bison/): Estamos usando o Bison como nosso analisador sintático.
- [GCC](https://gcc.gnu.org/): Estamos usando o GCC como nosso compilador C.

### Opcional

- [Yash](https://marketplace.visualstudio.com/items?itemName=daohong-emilio.yash): É um highlighter para a linguagem lex/yacc.

## Instalando requerimentos

### Instalando Flex

```bash
sudo apt-get install flex
```

### Instalando Bison

```bash
sudo apt-get install bison
```

## Estrutura do Projeto

- `src/lexical.l`: Arquivo de definição do analisador léxico.
- `src/parser.y`: Arquivo de definição do analisador sintático.
- `src/node.c`: Arquivo com as operações de manipulação da AST.
- `examples/`: Diretório com exemplos de código para testar o compilador.
- `gen/`: Diretório gerado com os arquivos do analisador.
- `build/`: Diretório de saída com o compilador final.

## Como usar

### Passo 1: Executar o `make`

Antes de rodar o compilador, é necessário compilar os arquivos gerados pelo `flex` e `bison` com o comando `make`:

```bash
make
```

Este comando irá gerar o compilador em `build/parser`.

### Passo 2: Rodar o compilador em um arquivo de exemplo

Após a compilação, o comando `make run` pode ser utilizado para rodar o compilador em um arquivo de código de exemplo. Para isso, use o comando:

```bash
make run
```

Por padrão, o comando acima irá rodar o compilador no arquivo `examples/code1.olc`. Para rodar o compilador em outro arquivo, utilize o comando:

```bash
make run INPUT=<arquivo.olc> IR_OUTPUT=<arquivo.json>
```
