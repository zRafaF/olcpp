#pragma once

#include <assert.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "checkers.h"
#include "db.h"
#include "ir_node.h"
#include "variable.h"

class Code {
   protected:
    db_s* dataBase;

    std::vector<std::shared_ptr<Code>>
        children;
    std::string output;

    void checkVariableExists(const std::string& name) {
        if (dataBase->variablesMap.find(name) == dataBase->variablesMap.end()) {
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
    Code(db_s* db) : dataBase(db) {
        assert(dataBase);
    }

    virtual std::vector<std::shared_ptr<Code>> generate(IRNode element) {
        assert(element);

        return {};
    }

    virtual ~Code() = default;
    std::string getOutput() const { return output; }
    void setOutput(const std::string& out) { output = out; }
    std::vector<std::shared_ptr<Code>> getChildren() const { return children; }
};

class GenVariableDeclaration : public Code {
   public:
    GenVariableDeclaration(db_s* db)
        : Code(db) {}

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        const std::string name = element.instruction();
        const variable_type_e type = parseVariableType(element.value().instruction());
        const std::string valueType = element.value().value().instruction();
        const std::string value = element.value().value().value().instruction();

        // Calculate offset
        unsigned offset = 0;
        for (const auto& pair : dataBase->variablesMap) {
            offset = pair.second.offset + pair.second.size;
        }

        const variable_s newVar(type, getVariableTypeSize(type), offset);

        if (dataBase->variablesMap.count(name)) {
            semanticError("Duplicate variable [" + name + "]");
        }
        dataBase->variablesMap.emplace(name, newVar);

        if (valueType == "CONSTANT") {
            checkValueType(type, value);
            output = generateConstantAssignment(newVar, value);
        } else if (valueType == "VARIABLE") {
            checkVariableExists(value);
            output = generateVariableAssignment(newVar, dataBase->variablesMap.at(value));
        } else {
            semanticError("Invalid value type");
        }
        return {};
    }
};

class GenAssign : public Code {
   public:
    GenAssign(db_s* db)
        : Code(db) {}

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        const std::string varName = element.instruction();
        checkVariableExists(varName);

        const std::string valueType = element.value().instruction();
        const std::string value = element.value().value().instruction();
        variable_s target = dataBase->variablesMap.at(varName);

        if (valueType == "CONSTANT") {
            checkValueType(target.type, value);
            output = generateConstantAssignment(target, value);
        } else if (valueType == "VARIABLE") {
            checkVariableExists(value);
            const variable_s source = dataBase->variablesMap.at(value);
            if (target.type != source.type) {
                semanticError("Type mismatch");
            }
            output = generateVariableAssignment(target, source);
        } else {
            semanticError("Invalid value type");
        }

        return {};
    }
};

class GenPrintStatement : public Code {
   public:
    GenPrintStatement(db_s* db)
        : Code(db) {}

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        std::string valueType = element.instruction();
        std::string value = element.value().instruction();

        replaceNewlines(value);

        if (valueType == "CONSTANT") {
            output = "printf \"" + value + "\"";
        } else if (valueType == "VARIABLE") {
            checkVariableExists(value);
            const variable_s source = dataBase->variablesMap.at(value);
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

        return {};
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

class GenForLoop : public Code {
   public:
    GenForLoop(db_s* db)
        : Code(db) {}

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        std::vector<std::shared_ptr<Code>> instructions;

        IRNode forCondition = element.value();

        std::cout << forCondition.instruction() << std::endl;
        // if (forCondition.instruction() != "FOR") {
        //     semanticError("Invalid for loop");
        // }

        std::shared_ptr<Code> startLabel = std::make_shared<Code>(dataBase);
        // startLabel->se

        return instructions;
    }
};