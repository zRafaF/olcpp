#pragma once

#include <assert.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "checkers.h"
#include "ir_node.h"
#include "variable.h"

class Code {
   protected:
    std::map<std::string, variable_s>* variablesMap;
    std::map<std::string, variable_s>* temporaryMap;
    std::vector<std::shared_ptr<Code>> children;
    std::string output;

    void checkVariableExists(const std::string& name) {
        if (variablesMap->find(name) == variablesMap->end()) {
            semanticError("Undefined variable [" + name + "]");
        }
    }

    void checkValueType(variable_type_e type, const std::string& value) {
        if (!isVariableTypeValid(type, value)) {
            semanticError("Invalid value for variable type");
        }
    }

    std::string generateConstantAssignment(variable_s variable, const std::string& value) {
        std::string out;
        if (variable.type == INT) {
            out = "mov %r" + std::to_string(variable.offset) + " " + value;
        } else if (variable.type == STRING) {
            for (char letter : value) {
                out += "mov %r" + std::to_string(variable.offset) + " " + std::to_string(static_cast<int>(letter)) + "\n";
                variable.offset++;
            }
            out += "mov %r" + std::to_string(variable.offset) + " 0";
        } else if (variable.type == BOOL) {
            out = "mov %r" + std::to_string(variable.offset) + " " + (value == "TRUE" ? "1" : "0");
        }
        return out;
    }

    std::string generateVariableAssignment(variable_s target, const variable_s& source) {
        std::string out;

        if (target.type != source.type) {
            semanticError("Type mismatch of types " + std::to_string(target.type) + " and " + std::to_string(source.type));
        }

        if (target.type == INT || target.type == BOOL) {
            out = "mov %r" + std::to_string(target.offset) + " %r" + std::to_string(source.offset);
        } else if (target.type == STRING) {
            for (unsigned i = 0; i < source.size; i++) {
                out += "mov %r" + std::to_string(target.offset) + " %r" + std::to_string(source.offset + i) + "\n";
                target.offset++;
            }
            out += "mov %r" + std::to_string(target.offset) + " 0";
        }
        return out;
    }

   public:
    Code(std::map<std::string, variable_s>* vars,
         std::map<std::string, variable_s>* temps)
        : variablesMap(vars), temporaryMap(temps) {
        assert(variablesMap && temporaryMap);
    }

    virtual void generate(IRNode element) {
        assert(element);
    }

    virtual ~Code() = default;
    std::string getOutput() const { return output; }
    std::vector<std::shared_ptr<Code>> getChildren() const { return children; }
};

class GenVariableDeclaration : public Code {
   public:
    GenVariableDeclaration(std::map<std::string, variable_s>* vars,
                           std::map<std::string, variable_s>* temps)
        : Code(vars, temps) {}

    void generate(IRNode element) override {
        const std::string name = element.instruction();
        const variable_type_e type = parseVariableType(element.value().instruction());
        const std::string valueType = element.value().value().instruction();
        const std::string value = element.value().value().value().instruction();

        // Calculate offset
        unsigned offset = 0;
        for (const auto& pair : *variablesMap) {
            offset = pair.second.offset + pair.second.size;
        }

        const variable_s newVar(type, getVariableTypeSize(type), offset);

        if (variablesMap->count(name)) {
            semanticError("Duplicate variable [" + name + "]");
        }
        variablesMap->emplace(name, newVar);

        if (valueType == "CONSTANT") {
            checkValueType(type, value);
            output = generateConstantAssignment(newVar, value);
        } else if (valueType == "VARIABLE") {
            checkVariableExists(value);
            output = generateVariableAssignment(newVar, variablesMap->at(value));
        } else {
            semanticError("Invalid value type");
        }
    }
};

class GenAssign : public Code {
   public:
    GenAssign(std::map<std::string, variable_s>* vars,
              std::map<std::string, variable_s>* temps)
        : Code(vars, temps) {}

    void generate(IRNode element) override {
        const std::string varName = element.instruction();
        checkVariableExists(varName);

        const std::string valueType = element.value().instruction();
        const std::string value = element.value().value().instruction();
        variable_s target = variablesMap->at(varName);

        if (valueType == "CONSTANT") {
            checkValueType(target.type, value);
            output = generateConstantAssignment(target, value);
        } else if (valueType == "VARIABLE") {
            checkVariableExists(value);
            const variable_s source = variablesMap->at(value);
            if (target.type != source.type) {
                semanticError("Type mismatch");
            }
            output = generateVariableAssignment(target, source);
        } else {
            semanticError("Invalid value type");
        }
    }
};

class GenPrintStatement : public Code {
   public:
    GenPrintStatement(std::map<std::string, variable_s>* vars,
                      std::map<std::string, variable_s>* temps)
        : Code(vars, temps) {}

    void generate(IRNode element) override {
        std::string valueType = element.instruction();
        std::string value = element.value().instruction();

        replaceNewlines(value);

        if (valueType == "CONSTANT") {
            output = "printf \"" + value + "\"";
        } else if (valueType == "VARIABLE") {
            checkVariableExists(value);
            const variable_s source = variablesMap->at(value);
            if (source.type == INT) {
                output = "printv %r" + std::to_string(source.offset);
            } else if (source.type == STRING) {
                for (unsigned i = 0; i < source.size; i++) {
                    output += "printv %r" + std::to_string(source.offset + i) + "\n";
                }
            }
        } else {
            semanticError("Invalid value type");
        }
    }

   private:
    void replaceNewlines(std::string& str) {
        size_t pos = 0;
        while ((pos = str.find('\n', pos)) != std::string::npos) {
            str.replace(pos, 1, "\\n");
            pos += 2;
        }
    }
};