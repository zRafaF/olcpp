#ifndef NODE_HPP
#define NODE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <memory>
#include <string>
#include <vector>

struct SharedNode {
    std::shared_ptr<Node> node;
};

// Estrutura de um nó da árvore
typedef struct Node {
    std::string instruction;
    std::vector<SharedNode> operands;
    SharedNode next;
} Node;

SharedNode createNode(std::string instruction, SharedNode operand, SharedNode next);
SharedNode createNode(std::string instruction, std::vector<SharedNode> operands, SharedNode next);
void generateIR(SharedNode root, FILE *output);
// void freeNode(Node *node);

#endif  // NODE_HPP
