# Documentação do Parser e Analisador Léxico

Este documento descreve a implementação de um analisador léxico e parser, utilizando **Flex** e **Bison**. Ele abrange as etapas de análise léxica, análise sintática e geração de Representação Intermediária (IR), destacando as principais decisões de design e funcionamento.

---

## Metodologia Utilizada

O projeto segue as seguintes etapas principais:

1. **Análise Léxica (Flex)**:
   O analisador léxico identifica tokens na entrada, como identificadores, palavras-chave, números, operadores e símbolos personalizados. Esses tokens são então passados para o parser.

2. **Análise Sintática (Bison)**:
   O parser utiliza as regras gramaticais da linguagem para validar e estruturar a entrada, construindo uma Árvore de Sintaxe Abstrata (AST).

3. **Geração de Representação Intermediária (IR)**:
   A AST é convertida em uma Representação Intermediária (IR) no formato JSON. Esse formato facilita análises posteriores e a geração de código final.

4. **Analisador Semântico**:
   O analisador semântico verifica a consistência e correção do código, identificando erros como variáveis não declaradas, tipos incompatíveis e operações inválidas.

5. **Geração de Código**:
   Com a AST e a IR geradas, é possível gerar código final que nesse caso é para o interpretador raposeitor.

---

## Estrutura do Código

### 1. **Analisador Léxico (Flex)**

O arquivo Flex define padrões para identificar tokens. Cada token reconhecido é associado a uma ação, como retornar um símbolo para o parser.

#### Tokens Personalizados:

A linguagem utiliza emojis para delimitar estruturas como início e fim de programa (`👍️`, `👎️`) e operadores (`➕`, `➖`, `🔀`, etc.).

#### Exemplo de Padrão para Tokens:

```c
[0-9]+ {
    yylval.str = strdup(yytext);
    return INTEGER;
}

👉([^👈]+)👈 {
    char *str = strdup(yytext);
    str += 4;
    str[strlen(str) - 4] = '\0';
    yylval.str = str;
    return STRING;
}
```

### 2. **Parser (Bison)**

O parser define as regras gramaticais da linguagem e constrói a AST com base nos tokens identificados.

#### Estrutura Padrão:

1. **Declarações Iniciais:**

   - Tokens e tipos de dados são declarados com `%token` e `%type`.
   - A precedência de operadores é definida com `%left`, `%right` ou `%nonassoc`.

   Exemplo:

   ```bison
   %token <str> INTEGER IDENTIFIER STRING
   %left INTEGER_ADDITION INTEGER_SUBTRACTION
   ```

2. **Regras Gramaticais:**
   As regras descrevem a sintaxe da linguagem, utilizando `$$` para o nó atual e `$1`, `$2`, etc., para os valores dos elementos da regra.

   Exemplo:

   ```c
   statement:
       IDENTIFIER ASSIGN expr {
           $$ = createNode("ASSIGN", createNode($1, $3, NULL), NULL);
       }
   ;
   ```

3. **Construção da AST:**
   A AST é construída com a função `createNode`. Cada nó contém:
   - Uma instrução (como `ASSIGN`, `IF_CONDITION`);
   - Ponteiros para subnós;
   - Informações adicionais, como valores ou identificadores.

### 3. **Representação Intermediária (IR)**

A IR é gerada percorrendo a AST e convertendo-a em um formato `.json`. Isso é feito com a função `generateIR`, definida no arquivo `node.c`:

```c
void generateIR(Node *root, FILE *output) {
    if (root == NULL) return;

    fprintf(output, "{\"instruction\":\"%s\"", root->instruction);

    if (root->value) {
        fprintf(output, ",\"value\":");
        generateIR(root->value, output);
    }

    if (root->next) {
        fprintf(output, ",\"next\": ");
        generateIR(root->next, output);
    }

    fprintf(output, "}");
}
```

---

## Componentes do Projeto

### 1. Estruturas de Dados

A AST é construída usando a estrutura `Node`, definida em `node.h`:

```c
typedef struct Node {
    char *instruction;
    struct Node *value;
    struct Node *next;
} Node;
```

### 2. Regras de Linguagem

#### Principais Regras Gramaticais:

- **Estrutura Geral do Programa:**

```c
program:
    PROGRAM_BEGIN e_o_l statement_list optional_end_of_lines PROGRAM_END {
        root = $3;
    }
;
```

- **Declaração de Variáveis:**

```c
var_declaration:
    VARIABLE_DECLARATION IDENTIFIER var_declaration_types ASSIGN var_declaration_expressions {
        $$ = createNode("VARIABLE_DECLARATION", createNode($2, createNode($3, $5, NULL), NULL), NULL);
    }
;
```

- **Estruturas de Controle:**

```c
if:
    IF_START condition e_o_l statement_list optional_end_of_lines IF_END {
        $$ = createNode("IF_CONDITION", createNode("IF", $2, $4), NULL);
    }
;
```

- **Expressões Aritméticas:**

```c
expr:
    expr INTEGER_ADDITION expr {
        $$ = createNode("ADD", $1, $3);
    }
    | INTEGER {
        $$ = createNode("CONSTANT", createNode($1, NULL, NULL), NULL);
    }
;
```

---

## Tratamento de Erros

- O analisador léxico identifica caracteres inválidos e exibe mensagens de erro.
- O parser utiliza a função `yyerror` para reportar erros de sintaxe, indicando a linha e o token problemático.

Exemplo de implementação:

```c
void yyerror(const char *s) {
    fprintf(stderr, "Erro na linha %d: %s em '%s'\n", yylineno, s, yytext);
}
```

---
