#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "instructions.h"
#include "ir_helpers.h"
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
    json_object_s* object = json_value_as_object(root);
    json_object_element_s* current = object->start;

    while (current != nullptr) {
        if (strcmp(current->name->string, "instruction") == 0) {
            const std::string instruction = getInstruction(current);
            std::cout << "Parsing Instruction: " << instruction << std::endl;

            // Gets the IR value
            json_object_element_s* value = getValue(current);
            value = json_value_as_object(value->value)->start;
            if (!value) {
                break;
            }

            if (instruction == "VARIABLE_DECLARATION") {
                std::shared_ptr<Code> code = std::make_shared<GenVariableDeclaration>();
                instructions.push_back(code);
                code->generate(value, &variablesMap, &temporaryMap);
            }
        }

        while (current->next && strcmp(current->next->name->string, "next") != 0) {
            current = current->next;
            std::cout << "Next1" << std::endl;
        }
        if (!current->next) {
            break;
        }
        current = json_value_as_object(current->next->value)->start;
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