#pragma once

#include <assert.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "ir_helpers.h"
#include "variable.h"

// enum
enum InstructionsE {
    VARIABLE_DECLARATION,
};

class Code {
   private:
   public:
    std::vector<std::shared_ptr<Code>> children;
    std::string output;  // Output assembly code

    virtual void generate(json_object_element_s* element,
                          std::map<std::string, variable_s>* variablesMap,
                          std::map<std::string, variable_s>* temporaryMap) {
        assert(element != nullptr);
        assert(variablesMap != nullptr);
        assert(temporaryMap != nullptr);
    }

    virtual ~Code() = default;  // Virtual destructor
};

class GenVariableDeclaration : public Code {
   private:
    variable_type_e type;

    std::string name;

    std::string valueString;
    int valueInt;

   public:
    void generate(json_object_element_s* element,
                  std::map<std::string, variable_s>* variablesMap,
                  std::map<std::string, variable_s>* temporaryMap) override {
        assert(element != nullptr);
        assert(variablesMap != nullptr);
        assert(temporaryMap != nullptr);

        const std::string variableName = getInstruction(element);
        std::cout << "Variable Name: " << variableName << std::endl;

        const std::string variableType = getInstruction(getValue(element));
        std::cout << "Variable Type: " << variableType << std::endl;

        variable_s variable(INT, 1, 0);

        variablesMap->insert(std::pair<std::string, variable_s>(variableName, variable));

        name = variableName;
        type = variableType == "int" ? INT : STRING;

        std::cout << "mov %r0";
    }
};