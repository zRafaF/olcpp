# Guia de Sintaxe e Exemplos OLC++

Este guia apresenta a sintaxe da linguagem e exemplos práticos para utilização das principais funcionalidades, incluindo declaração de variáveis, controle de fluxo, laços e operações aritméticas.

---

## 1. Declaração e Manipulação de Variáveis Inteiras

### Sintaxe:

```plaintext
🪣 IDENTIFICADOR 💯 🟰 EXPRESSÃO
```

### Exemplo:

```plaintext
👍️
🪣 x 💯 🟰 10
👎️
```

Neste exemplo, uma variável inteira `x` é declarada e inicializada com o valor `10`.

---

## 2. Declaração e Manipulação de Vetores de Inteiros

### Sintaxe:

```plaintext
🪣 IDENTIFICADOR 🔢 🟰 🫸VALOR0🚬VALOR1🚬...🫷
```

### Exemplo:

```plaintext
👍️
🪣 vetor 🔢 🟰 🫸1🚬2🚬3🚬4🚬5🫷
👎️
```

Declaração de um vetor chamado `vetor` contendo os valores inteiros `1, 2, 3, 4, 5`.

---

## 3. Leitura e Impressão de Valores

### Leitura de Valores:

```plaintext
🪣 IDENTIFICADOR 🔤 🟰 ✍
```

### Impressão de Valores:

```plaintext
💅 IDENTIFICADOR
```

### Exemplo:

```plaintext
👍️
🪣 x 🔤 🟰 ✍
💅 x
👎️
```

Este exemplo lê um valor do usuário e o imprime na saída.

---

## 4. Impressão de Textos/Strings

### Sintaxe:

```plaintext
💅 👉TEXTO👈
```

### Exemplo:

```plaintext
👍️
💅 👉Hello, world!👈
👎️
```

Imprime a mensagem `Hello, world!` para o usuário.

---

## 5. Seleções Condicionais

### Seleção Parcial (if):

```plaintext
🤔 🫸CONDIÇÃO🫷
    CÓDIGO_IF
😬
```

### Exemplo:

```plaintext
👍️
🤔 🫸x 🔁 0🫷
    💅 👉x é zero👈
😬
👎️
```

### Seleção Completa (if/else):

```plaintext
🤔 🫸CONDIÇÃO🫷
    CÓDIGO_IF
😬😒
    CÓDIGO_ELSE
😬
```

### Exemplo:

```plaintext
👍️
🤔 🫸x 🔼 10🫷
    💅 👉Maior que 10👈
😬😒
    💅 👉Menor ou igual a 10👈
😬
👎️
```

---

## 6. Laços

### Laço com Condição (while):

```plaintext
🐳 🫸(CONDIÇÃO)🫷
    CÓDIGO
🐋
```

### Exemplo:

```plaintext
👍️
🐳🫸i🔼5🫷
    i 🟰 i ➖ 100
🐋
👎️
```

### Laço Simplificado (for):

```plaintext
🐣 🫸(CONDIÇÃO 🚭 INCREMENTO)🫷
    CÓDIGO
🐥
```

### Exemplo:

```plaintext
👍️
🐣🫸i🔽45🚭i 🟰 i ➕ 1🫷
    💅 🫸i🫷
🐥
👎️
```

---

## 7. Aninhamento de Laços e Seleções

### Exemplo:

```plaintext
👍️
🪣 i 💯 🟰 1


🐣🫸i🔽45🚭i 🟰 i ➕ 1🫷
    💅🫸i🫷

    🤔🫸(i♻2)🔁0🫷
        💅🫸👉i e par👈🫷


    😬😒🤔🫸i🔁25🫷
        💅🫸👉i e igual a 25👈🫷

    😬
🐥

👎️
```

Este exemplo demonstra o uso de seleções dentro de um laço.

---

## 8. Expressões

### Expressões Aritméticas:

```plaintext
x 🟰 a ➕ b
x 🟰 a ➖ b
x 🟰 a ✖ b
x 🟰 a ➗ b
x 🟰 a ♻ b
```

### Expressões de Comparação:

```plaintext
x 🔼 y
x 🔽 y
x ⏫ y
x ⏬ y
x 🔁 y
x 🔀 y
```

### Expressões Lógicas:

```plaintext
❗x
x 🅰 y
x 🅾 y
```

### Uso de Parênteses:

```plaintext
x 🟰 (a ➕ b) ✖ (c ➖ d)
```

---

## 9. Comentários de Linha

### Sintaxe:

```plaintext
👅 COMENTÁRIO
```

### Exemplo:

```plaintext
👍️
👅 Este é um comentário explicativo
👎️
```

## 10. Acessando Elementos de Vetores

### Sintaxe:

```plaintext
IDENTIFICADOR 🔎 INDEX
```

### Exemplo:

```plaintext
👍

️🪣 minha_array 🔢 🟰 🫸0🚬1🚬2🚬3🫷

🪣 x 💯 🟰 minha_array 🔎 2

👎️
```

Neste exemplo, o valor `2` do vetor `minha_array` é atribuído à variável `x`.
