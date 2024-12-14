#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura de um nó da árvore
typedef struct Node {
    char *instruction;
    struct Node **operands;
    size_t operandsSize;
    struct Node *next;
} Node;

Node *createNode(const char *instruction, Node *operand, Node *next);
Node *createNodeWithOperands(const char *instruction, Node **operands, size_t _operandsSize, Node *next);

Node *createHelperNode(const char *instruction, const char *operandValue);

Node *appendOperand(Node *node, Node *operand);

void generateIR(Node *root, FILE *output);
void freeNode(Node *node);

#endif  // NODE_H
