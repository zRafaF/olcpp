#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "ir_helpers.h"

// enum
enum InstructionsE {
    VARIABLE_DECLARATION,
};

enum VariableTypeE {
    INT,
    STRING,
};

class Code {
   private:
   public:
    std::vector<std::shared_ptr<Code>> children;

    virtual void generate(json_object_s* value) {
        // Default implementation
        if (value) {
            std::cout << "Generating code for JSON object\n";
        }
    }

    virtual ~Code() = default;  // Virtual destructor
};

class GenVariableDeclaration : public Code {
   private:
    VariableTypeE type;

    std::string name;

    std::string valueString;
    int valueInt;

   public:
    void generate(json_object_s* object) override {
        auto val = getValue(object);
        std::cout << "ab:" << json_value_as_string(object->start->value)->string << std::endl;
        std::cout << "Val: " << json_value_as_string(val->start->value)->string << std::endl;

        name = getInstruction(val);
        type = getInstruction(getValue(val)) == "INTEGER_TYPE" ? INT : STRING;

        std::cout << "mov %r0";
    }
};