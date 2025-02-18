#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "instructions.h"
#include "ir_helpers.h"

class Generator {
   private:
    std::map<std::string, std::string> variablesString;
    std::map<std::string, int> variablesInt;

    std::vector<std::shared_ptr<Code>> instructions;

    std::vector<std::string> raposeitorOutput;

    std::shared_ptr<Code> parse(json_object_s* object);

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

    const json_object_element_s* current = object->start;
    std::cout << "Parsing instructions" << std::endl;
    while (current != nullptr) {
        std::cout << current->name->string << std::endl;

        // instructions.push_back(parse(json_value_as_object(current->value)));

        if (current->next == nullptr) {
            break;
        }
        std::cout << "2" << std::endl;

        current = current->next;
    }

    return instructions;
}

std::shared_ptr<Code> Generator::parse(json_object_s* object) {
    std::shared_ptr<Code> code;

    std::cout << getInstruction(object) << std::endl;

    // if (getInstruction(object) == "VARIABLE_DECLARATION") {
    //     code = std::make_shared<GenVariableDeclaration>();
    // }

    // code->generate(object);

    return code;
}

void Generator::printInstructions() {
    for (auto& instruction : instructions) {
        instruction->generate(nullptr);
    }
}