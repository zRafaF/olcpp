#pragma once

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

    virtual void generate(json_object_s* value,
                          std::map<std::string, variable_s>* variablesMap,
                          std::map<std::string, variable_s>* temporaryMap) {
        // Default implementation
        if (value && variablesMap && temporaryMap) {
            std::cout << "Generating code for JSON object\n";
        }
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
    void generate(json_object_s* value,
                  std::map<std::string, variable_s>* variablesMap,
                  std::map<std::string, variable_s>* temporaryMap) override {
        auto val = getValue(value);

        const std::string variableName = json_value_as_string(value->start->value)->string;
        const std::string variableType = json_value_as_string(val->start->value)->string;

        variable_s variable;
        variable.type = variableType == "INTEGER_TYPE" ? INT : STRING;
        variable.size = 1;
        variable.offset = 0;

        variablesMap->insert(std::pair<std::string, variable_s>(variableName, variable));

        name = getInstruction(val);
        type = getInstruction(getValue(val)) == "INTEGER_TYPE" ? INT : STRING;

        std::cout << "mov %r0";
    }
};