# OLC++

Este repositório contém um compilador simples para a linguagem OLCPP, com ferramentas de análise léxica e sintática para o **Assembly Raposeitor**.

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

### Passo 2: Rodar o compilador

Após a compilação, o comando `make run` pode ser utilizado para rodar o compilador em um arquivo de código de exemplo. Para isso, use o comando:

```bash
make run PATH=examples/code1.olc
```

Você pode substituir `examples/code1.olc` por qualquer outro arquivo `.olc` presente no diretório `examples/`. O parâmetro `PATH` é utilizado para especificar o arquivo de entrada que será processado.

### Comandos Disponíveis

- **`make`**: Compila os arquivos necessários para gerar o compilador.
- **`make lexical`**: Gera o arquivo do analisador léxico (`lex.yy.c`).
- **`make parser`**: Gera os arquivos do analisador sintático (`parser.tab.c` e `parser.tab.h`).
- **`make clean`**: Remove os arquivos gerados nos diretórios `gen/` e `build/`.
- **`make run PATH=<arquivo.olc>`**: Compila o projeto e executa o compilador com o arquivo `.olc` especificado.
