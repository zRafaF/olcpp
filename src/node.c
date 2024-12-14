#include "node.h"

Node *createNode(const char *instruction, Node *operand, Node *next) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->instruction = strdup(instruction);
    node->operands->nodes = operand;
    node->operands->size = 1;
    node->next = next;
    return node;
}

Node *createNode(const char *instruction, OperandsArray *operands, Node *next) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->instruction = strdup(instruction);
    node->operands = operands;
    node->next = next;
    return node;
}

Node *appendOperand(Node *node, Node *operand) {
    if (node == NULL)
        return NULL;

    if (node->operands == NULL) {
        node->operands = (OperandsArray *)malloc(sizeof(OperandsArray));
        node->operands->nodes = (Node **)malloc(sizeof(Node *));
        node->operands->size = 0;
    }

    node->operands->nodes = (Node **)realloc(node->operands->nodes, (node->operands->size + 1) * sizeof(Node *));
    node->operands->nodes[node->operands->size] = operand;
    node->operands->size++;
    return node;
}

void generateIR(Node *root, FILE *output) {
    if (root == NULL)
        return;

    fprintf(output, "{\"instruction\":\"%s\"", root->instruction);

    if (root->operands->nodes) {
        fprintf(output, ",\"operands\": [");
        for (size_t i = 0; i < root->operands->size; i++) {
            generateIR(root->operands->nodes[i], output);
            if (i < root->operands->size - 1)
                fprintf(output, ",");
        }
        fprintf(output, "]");
    }

    if (root->next) {
        fprintf(output, ",\"next\": ");
        generateIR(root->next, output);
    }

    fprintf(output, "}");
}

void freeNode(Node *node) {
    if (node == NULL)
        return;
    free(node->instruction);
    if (node->operands) {
        for (size_t i = 0; i < node->operands->size; i++)
            freeNode(node->operands->nodes[i]);
        free(node->operands->nodes);
        free(node->operands);
    }
    freeNode(node->next);
    free(node);
}
