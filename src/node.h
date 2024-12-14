#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura de um nó da árvore
typedef struct Node {
    char *instruction;
    struct Node *value;
    struct Node *next;
} Node;

Node *createNode(const char *instruction, Node *value, Node *next);
void generateIR(Node *root, FILE *output);
void freeNode(Node *node);

#endif  // NODE_H