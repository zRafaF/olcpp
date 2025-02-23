#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "instructions.h"
#include "ir_node.h"
#include "variable.h"

class Generator {
   private:
    std::map<std::string, variable_s> variablesMap;
    std::map<std::string, variable_s> temporaryMap;

    std::vector<std::shared_ptr<Code>> instructions;
    std::vector<std::string> raposeitorOutput;

   public:
    Generator();

    std::vector<std::shared_ptr<Code>> parseInstructions(json_value_s* root);
    void printInstructions();
};

Generator::Generator(/* args */) {
    std::cout << "Generator created" << std::endl;
}

std::vector<std::shared_ptr<Code>> Generator::parseInstructions(json_value_s* root) {
    IRNode node(root);
    while (node) {
        const std::string instruction = node.instruction();
        std::cout << "Parsing Instruction: " << instruction << std::endl;

        if (instruction == "VARIABLE_DECLARATION") {
            IRNode valueNode = node.value();
            std::shared_ptr<Code> code = std::make_shared<GenVariableDeclaration>();
            instructions.push_back(code);
            code->generate(valueNode, &variablesMap, &temporaryMap);
        }

        node = node.next();
    }
    return instructions;
}

void Generator::printInstructions() {
    std::cout << "Variable Map" << std::endl;
    for (auto& variable : variablesMap) {
        std::cout << variable.first << std::endl;
        std::cout << variable.second << std::endl;
    }

    for (auto& instruction : instructions) {
        std::cout << "Instruction: " << instruction->output << std::endl;
    }
}