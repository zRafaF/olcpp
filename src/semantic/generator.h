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

    void printChildrenInstructionsRecursively(std::shared_ptr<Code> code) {
        if (code->getChildren().empty()) {
            return;
        }
        for (auto& child : code->getChildren()) {
            printChildrenInstructionsRecursively(child);
        }
        std::cout << code->getOutput() << std::endl;
    }

   public:
    Generator() {}

    std::vector<std::shared_ptr<Code>> parseInstructions(json_value_s* root);
    void printInstructions() {
        std::cout
            << std::endl
            << std::endl;

        std::cout << "Variable Map" << std::endl;
        for (auto& variable : variablesMap) {
            std::cout << variable.first << std::endl;
            std::cout << variable.second << std::endl;
        }

        for (auto& instruction : instructions) {
            std::cout << "Instruction: " << instruction->getOutput() << std::endl;
            printChildrenInstructionsRecursively(instruction);
        }
    }

    std::string getOutput() const {
        std::string output;
        for (const auto& instruction : instructions) {
            output += instruction->getOutput() + "\n";
        }
        return output;
    }
};

std::vector<std::shared_ptr<Code>> Generator::parseInstructions(json_value_s* root) {
    IRNode node(root);
    while (node) {
        const std::string instruction = node.instruction();
        std::cout << "Parsing Instruction: " << instruction << std::endl;
        IRNode valueNode = node.value();

        if (instruction == "VARIABLE_DECLARATION") {
            std::shared_ptr<Code> code = std::make_shared<GenVariableDeclaration>(&variablesMap, &temporaryMap);
            instructions.push_back(code);
            code->generate(valueNode);
        } else if (instruction == "ASSIGN") {
            std::shared_ptr<Code> code = std::make_shared<GenAssign>(&variablesMap, &temporaryMap);
            instructions.push_back(code);
            code->generate(valueNode);
        } else if (instruction == "PRINT_STATEMENT") {
            std::shared_ptr<Code> code = std::make_shared<GenPrintStatement>(&variablesMap, &temporaryMap);
            instructions.push_back(code);
            code->generate(valueNode);
        }

        node = node.next();
    }
    return instructions;
}
