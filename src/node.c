#include "node.h"

Node *createNode(const char *instruction, Node *operand, Node *next) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed in createNode\n");
        exit(EXIT_FAILURE);
    }

    node->instruction = strdup(instruction);
    if (node->instruction == NULL) {
        fprintf(stderr, "Memory allocation failed for instruction\n");
        free(node);
        exit(EXIT_FAILURE);
    }

    if (operand) {
        node->operands = (Node **)malloc(sizeof(Node *));
        if (node->operands == NULL) {
            fprintf(stderr, "Memory allocation failed for operands\n");
            free(node->instruction);
            free(node);
            exit(EXIT_FAILURE);
        }
        node->operands[0] = operand;
        node->operandsSize = 1;
    } else {
        node->operands = NULL;
        node->operandsSize = 0;
    }

    node->next = next;
    return node;
}

Node *createNodeWithOperands(const char *instruction, Node **operands, size_t _operandsSize, Node *next) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed in createNodeWithOperands\n");
        exit(EXIT_FAILURE);
    }

    node->instruction = strdup(instruction);
    if (node->instruction == NULL) {
        fprintf(stderr, "Memory allocation failed for instruction\n");
        free(node);
        exit(EXIT_FAILURE);
    }

    node->operands = operands;
    node->operandsSize = _operandsSize;
    node->next = next;
    return node;
}
Node *createHelperNode(const char *instruction, const char *operandValue) {
    Node *operandNode = createNode(strdup(operandValue), NULL, NULL);
    return createNode(instruction, operandNode, NULL);
}

Node *appendOperand(Node *node, Node *operand) {
    if (node == NULL) {
        fprintf(stderr, "Cannot append operand to a NULL node\n");
        return NULL;
    }

    if (node->operands == NULL) {
        node->operands = (Node **)malloc(sizeof(Node *));
        if (node->operands == NULL) {
            fprintf(stderr, "Memory allocation failed in appendOperand\n");
            exit(EXIT_FAILURE);
        }
        node->operands[0] = operand;
        node->operandsSize = 1;
        return node;
    }

    Node **newOperands = (Node **)realloc(node->operands, (node->operandsSize + 1) * sizeof(Node *));
    if (newOperands == NULL) {
        fprintf(stderr, "Memory allocation failed in appendOperand\n");
        exit(EXIT_FAILURE);
    }

    node->operands = newOperands;
    node->operands[node->operandsSize] = operand;
    node->operandsSize++;
    return node;
}

void generateIR(Node *root, FILE *output) {
    if (root == NULL)
        return;

    fprintf(output, "{\"instruction\":\"%s\"", root->instruction);

    if (root->operands && root->operandsSize > 0) {
        fprintf(output, ",\"operands\": [");
        for (size_t i = 0; i < root->operandsSize; i++) {
            generateIR(root->operands[i], output);
            if (i < root->operandsSize - 1)
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
        for (size_t i = 0; i < node->operandsSize; i++)
            freeNode(node->operands[i]);
        free(node->operands);
    }
    freeNode(node->next);
    free(node);
}
