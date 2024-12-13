#include "node.h"

Node *createNode(const char *instruction, Node *value, Node *next) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->instruction = strdup(instruction);
    node->value = value;
    node->next = next;
    return node;
}

void generateIR(Node *root, FILE *output) {
    if (root == NULL)
        return;

    fprintf(output, "{\"instruction\":\"%s\"", root->instruction);

    if (root->value) {
        fprintf(output, ",\"operands\": [");
        generateIR(root->value, output);
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
    freeNode(node->value);
    freeNode(node->next);
    free(node);
}
