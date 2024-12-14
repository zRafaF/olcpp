#include "node.hpp"

SharedNode createNode(std::string instruction, SharedNode operand, SharedNode next) {
    SharedNode node;
    node.node = std::make_shared<Node>();
    node.node->instruction = instruction;
    node.node->operands.push_back(operand);
    node.node->next = next;
    return node;
}
SharedNode createNode(std::string instruction, std::vector<SharedNode> operands, SharedNode next) {
    SharedNode node;
    node.node = std::make_shared<Node>();
    node.node->instruction = instruction;
    node.node->operands = operands;
    node.node->next = next;
    return node;
}

void generateIR(SharedNode root, FILE *output) {
    if (root.node == nullptr)
        return;

    std::vector<int> v;
    printf("%d\n", v.size());

    fprintf(output, "{\"instruction\":\"%s\"", root.node->instruction);

    if (root.node->operands.size() > 0) {
        fprintf(output, ",\"operands\": [");

        for (SharedNode operand : root.node->operands) {
            generateIR(operand, output);
            fprintf(output, ",");
        }

        fprintf(output, "]");
    }

    if (root.node->next.node != nullptr) {
        fprintf(output, ",\"next\": ");
        generateIR(root.node->next, output);
    }

    fprintf(output, "}");
}

// void freeNode(Node *node) {
//     if (node == NULL)
//         return;
//     free(node->instruction);
//     freeNode(node->value);
//     freeNode(node->next);
//     free(node);
// }
