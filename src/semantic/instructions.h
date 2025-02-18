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
    Code() {
    }

    std::vector<std::shared_ptr<Code>> children;

    virtual void generate(json_object_s* value);
};

class GenVariableDeclaration : public Code {
   private:
    VariableTypeE type;

    std::string name;

    std::string valueString;
    int valueInt;

   public:
    GenVariableDeclaration() {
    }

    void generate(json_object_s* object) override {
        auto val = getValue(object);

        name = getInstruction(val);
        type = getInstruction(getValue(val)) == "INTEGER_TYPE" ? INT : STRING;

        std::cout << "mov %r0";
    }
};
