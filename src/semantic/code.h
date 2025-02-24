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
#include "helpers.h"
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
    Code(db_s* db, std::string _output) : dataBase(db), output(_output) {
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
    GenVariableDeclaration(db_s* db) : Code(db) {}
    GenVariableDeclaration(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        const std::string name = element.value().instruction();
        const variable_type_e type = parseVariableType(element.value().value().instruction());
        const std::string valueType = element.value().value().value().instruction();
        const std::string value = element.value().value().value().value().instruction();

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

class GenIntegerAddition : public Code {
   private:
    std::variant<variable_s, int> left;
    std::variant<variable_s, int> right;

    std::string getAccessString(std::variant<variable_s, int> val) {
        if (std::holds_alternative<variable_s>(val)) {
            return "%r" + std::to_string(std::get<variable_s>(val).offset);
        } else {
            return std::to_string(std::get<int>(val));
        }
    }

   public:
    GenIntegerAddition(db_s* db) : Code(db) {}
    GenIntegerAddition(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        if (element.value().instruction() == "VARIABLE") {
            checkVariableExists(element.value().value().instruction());
            left = dataBase->variablesMap.at(element.value().value().instruction());
            std::cout << "LEFT: " << dataBase->variablesMap.at(element.value().value().instruction()) << std::endl;

        } else if (element.value().instruction() == "CONSTANT") {
            left = std::stoi(element.value().value().instruction());
        } else {
            semanticError("Invalid left operand");
        }

        IRNode rightNode = element.next();

        if (rightNode.instruction() == "VARIABLE") {
            checkVariableExists(rightNode.value().instruction());
            right = dataBase->variablesMap.at(rightNode.value().instruction());
        } else if (rightNode.instruction() == "CONSTANT") {
            right = std::stoi(rightNode.value().instruction());
        } else {
            semanticError("Invalid right operand");
        }

        size_t arraySize = dataBase->temporaryArray.size();

        dataBase->temporaryArray.push_back(variable_s(INT, 1, arraySize));

        output = "add %t" + std::to_string(arraySize) + ", " + getAccessString(left) + ", " + getAccessString(right);

        return {};
    }
};

class GenAssign : public Code {
   public:
    GenAssign(db_s* db) : Code(db) {}
    GenAssign(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        std::vector<std::shared_ptr<Code>> ret;

        const std::string varName = element.value().instruction();
        checkVariableExists(varName);

        const std::string valueType = element.value().value().instruction();
        const std::string value = element.value().value().value().instruction();
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
        } else if (valueType == "INTEGER_ADDITION") {
            if (target.type != INT)
                semanticError("Tried to assign an integer addition to a non integer variable");

            ret.push_back(std::make_shared<GenIntegerAddition>(dataBase));
            ret.back()->generate(element.value().value());
            ret.push_back(std::make_shared<Code>(dataBase, "mov %r" + std::to_string(target.offset) + ", %t" + std::to_string(dataBase->temporaryArray.size() - 1)));

            dataBase->temporaryArray.pop_back();

        }

        else {
            semanticError("Invalid variable assignment value type");
        }

        return ret;
    }
};

class GenPrintStatement : public Code {
   public:
    GenPrintStatement(db_s* db) : Code(db) {}
    GenPrintStatement(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        std::string valueType = element.value().instruction();
        std::string value = element.value().value().instruction();

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

class GenLessThan : public Code {
   private:
    std::variant<variable_s, int> left;
    std::variant<variable_s, int> right;

    std::string getAccessString(std::variant<variable_s, int> val) {
        if (std::holds_alternative<variable_s>(val)) {
            return "%r" + std::to_string(std::get<variable_s>(val).offset);
        } else {
            return std::to_string(std::get<int>(val));
        }
    }

   public:
    GenLessThan(db_s* db) : Code(db) {}
    GenLessThan(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        if (element.value().instruction() == "VARIABLE") {
            checkVariableExists(element.value().value().instruction());
            left = dataBase->variablesMap.at(element.value().value().instruction());
        } else if (element.value().instruction() == "CONSTANT") {
            left = std::stoi(element.value().value().instruction());
        } else {
            semanticError("Invalid left operand");
        }

        IRNode rightNode = element.next();

        if (rightNode.instruction() == "VARIABLE") {
            checkVariableExists(rightNode.value().instruction());
            right = dataBase->variablesMap.at(rightNode.value().instruction());
        } else if (rightNode.instruction() == "CONSTANT") {
            right = std::stoi(rightNode.value().instruction());
        } else {
            semanticError("Invalid right operand");
        }

        size_t arraySize = dataBase->temporaryArray.size();

        dataBase->temporaryArray.push_back(variable_s(BOOL, 1, arraySize));

        output = "less %t" + std::to_string(arraySize) + ", " + getAccessString(left) + ", " + getAccessString(right);

        return {};
    }
};

std::vector<std::shared_ptr<Code>> parseNodeInstructions(IRNode node, db_s* db);

class GenForLoop : public Code {
   public:
    GenForLoop(db_s* db) : Code(db) {}
    GenForLoop(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        std::vector<std::shared_ptr<Code>> instructions;

        if (element.value().instruction() != "FOR")
            semanticError("Invalid for loop");

        std::string startLabelStr = "begin_for_" + std::to_string(dataBase->labelCounter);
        dataBase->labelCounter++;
        std::string endLabelStr = "end_for_" + std::to_string(dataBase->labelCounter);
        dataBase->labelCounter++;

        // Begin label
        instructions.push_back(std::make_shared<Code>(dataBase, "label " + startLabelStr));

        // condition
        IRNode condition = element.value().value();
        appendVectors(instructions, parseNodeInstructions(condition, dataBase));

        // Jump out if condition is no longer met
        instructions.push_back(std::make_shared<Code>(dataBase, "jf %t" + std::to_string(dataBase->temporaryArray.size() - 1) + ", " + endLabelStr));

        // For assignment
        IRNode assignmentNode = element.value().next();

        if (assignmentNode.instruction() != "FOR_ASSIGN")
            semanticError("Invalid for assignment");

        appendVectors(instructions, parseNodeInstructions(assignmentNode.value(), dataBase));

        // Parse the for loop

        // End label
        instructions.push_back(std::make_shared<Code>(dataBase, "label " + endLabelStr));

        dataBase->temporaryArray.pop_back();

        return instructions;
    }
};