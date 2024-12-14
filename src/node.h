#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct OperandsArray {
    Node **nodes;
    size_t size;
} OperandsArray;

// Estrutura de um nó da árvore
typedef struct Node {
    char *instruction;
    struct OperandsArray *operands;
    struct Node *next;
} Node;

Node *createNode(const char *instruction, Node *value, Node *next);
Node *createNode(const char *instruction, OperandsArray *operands, Node *next);

Node *appendOperand(Node *node, Node *operand);

void generateIR(Node *root, FILE *output);
void freeNode(Node *node);

#endif  // NODE_H
