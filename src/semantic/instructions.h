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

    std::string generateOutputConst(variable_s variable) {
        if (type == INT) {
            return "mov %r" + std::to_string(variable.offset) + " " + value;
        } else {
            std::string out = "";
            for (auto letter : value) {
                out += "mov %r" + std::to_string(variable.offset) + " " + std::to_string((int)letter) + "\n";
                variable.offset++;
            }
            out += "mov %r" + std::to_string(variable.offset) + " 0";
            return out;
        }
    }

    std::string generateOutputVar(variable_s variable, std::map<std::string, variable_s>* variablesMap) {
        variable_s accessVariable = variablesMap->at(value);

        if (variable.type == INT) {
            return "mov %r" + std::to_string(variable.offset) + " %r" + std::to_string(accessVariable.offset);
        } else if (variable.type == STRING) {
            std::string out = "";

            for (unsigned int i = 0; i < accessVariable.size; i++) {
                out += "mov %r" + std::to_string(variable.offset) + " %r" + std::to_string(accessVariable.offset + i) + "\n";
                variable.offset++;
            }
            out += "mov %r" + std::to_string(variable.offset) + " 0";
            return out;
        } else {
            semanticError("Tipo de variável inválido");
        }
        return "";
    }

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

        unsigned int size = type == INT ? 1 : 1024;
        unsigned int offset = 0;
        for (auto& variable : *variablesMap) {
            offset = variable.second.offset + variable.second.size;
        }

        variable_s variable = variable_s(type, size, offset);

        if (variablesMap->find(name) != variablesMap->end()) {
            semanticError("Variável [" + name + "] já foi declarada");
        }
        variablesMap->insert({name, variable});

        if (valueType == "CONSTANT") {
            if (!isVariableTypeValid(type, value)) {
                semanticError("Valor inválido para o tipo de variável");
            }

            output = generateOutputConst(variable);

        } else if (valueType == "VARIABLE") {
            if (variablesMap->find(value) == variablesMap->end()) {
                semanticError("Tentou acessar variável não declarada [" + value + "]");
            }
            output = generateOutputVar(variable, variablesMap);
        } else {
            semanticError("Tipo de valor inválido");
        }
    }
};

class GenAssign : public Code {
   public:
    void generate(IRNode element,
                  std::map<std::string, variable_s>* variablesMap,
                  std::map<std::string, variable_s>* temporaryMap) override {
        assert(element);
        assert(variablesMap != nullptr);
        assert(temporaryMap != nullptr);

        std::string variableName = element.instruction();
        std::string valueType = element.value().instruction();
        std::string value = element.value().value().instruction();

        if (variablesMap->find(variableName) == variablesMap->end()) {
            semanticError("Tentou acessar variável não declarada [" + variableName + "]");
        }

        variable_s variable = variablesMap->at(variableName);

        variablesMap->erase(variableName);
        variablesMap->insert({variableName, variable});

        if (valueType == "CONSTANT") {
            if (!isVariableTypeValid(variable.type, value)) {
                semanticError("Valor inválido para o tipo de variável [" + variableName + "]");
            }

            if (variable.type == INT) {
                output = "mov %r" + std::to_string(variable.offset) + " " + value;
            } else if (variable.type == STRING) {
                std::string out = "";
                for (auto letter : value) {
                    out += "mov %r" + std::to_string(variable.offset) + " " + std::to_string((int)letter) + "\n";
                    variable.offset++;
                }
                out += "mov %r" + std::to_string(variable.offset) + " 0";
                output = out;
            }
        } else if (valueType == "VARIABLE") {
            if (variablesMap->find(value) == variablesMap->end()) {
                semanticError("Tentou acessar variável não declarada [" + value + "]");
            }

            variable_s accessVariable = variablesMap->at(value);
            if (variable.type != accessVariable.type) {
                semanticError("Tipos de variáveis incompatíveis");
            }

            if (variable.type == INT) {
                output = "mov %r" + std::to_string(variable.offset) + " %r" + std::to_string(accessVariable.offset);
            } else if (variable.type == STRING) {
                std::string out = "";
                for (unsigned int i = 0; i < accessVariable.size; i++) {
                    out += "mov %r" + std::to_string(variable.offset) + " %r" + std::to_string(accessVariable.offset + i) + "\n";
                    variable.offset++;
                }
                out += "mov %r" + std::to_string(variable.offset) + " 0";
                output = out;
            }

        } else {
            semanticError("Tipo de valor inválido");
        }
    }
};