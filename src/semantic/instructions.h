#pragma once

#include <assert.h>

#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "checkers.h"
#include "ir_node.h"
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

    virtual void generate(IRNode element,
                          std::map<std::string, variable_s>* variablesMap,
                          std::map<std::string, variable_s>* temporaryMap) {
        assert(element);
        assert(variablesMap != nullptr);
        assert(temporaryMap != nullptr);
    }

    virtual ~Code() = default;  // Virtual destructor
};

class GenVariableDeclaration : public Code {
   private:
    variable_type_e type;

    std::string name;

    std::string value;

   public:
    void generate(IRNode element,
                  std::map<std::string, variable_s>* variablesMap,
                  std::map<std::string, variable_s>* temporaryMap) override {
        assert(element);
        assert(variablesMap != nullptr);
        assert(temporaryMap != nullptr);

        name = element.instruction();
        type = parseVariableType(element.value().instruction());

        std::string valueType = element.value().value().instruction();
        value = element.value().value().value().instruction();

        if (valueType == "CONSTANT") {
            if (!isVariableTypeValid(type, value)) {
                semanticError("Valor inválido para o tipo de variável");
            }
        } else if (valueType == "VARIABLE") {
            if (variablesMap->find(element.value().value().instruction()) == variablesMap->end()) {
                semanticError("Tentou acessar variável não declarada");
            }
        } else {
            semanticError("Tipo de valor inválido");
        }

        isVariableTypeValid(type, value);
    }
};