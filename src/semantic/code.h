#pragma once

#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "checkers.h"
#include "condition.h"
#include "db.h"
#include "helpers.h"
#include "ir_node.h"
#include "variable.h"

class Code;
std::vector<std::shared_ptr<Code>> parseNodeInstructions(IRNode node, db_s* db);

class Code {
   protected:
    db_s* dataBase;

    std::vector<std::shared_ptr<Code>>
        children;
    std::string output;

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

        size_t offset = 0;

        for (auto ma : dataBase->variablesMap) {
            if (ma.second.offset >= offset) {
                offset = ma.second.offset + ma.second.size;
            }
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
            checkVariableExists(value, dataBase);
            output = generateVariableAssignment(newVar, dataBase->variablesMap.at(value));
        } else if (valueType == "INT_ARRAY_TYPE") {
            // TODO IMPLEMENT
        } else {
            semanticError("Invalid value type");
        }
        return {};
    }
};

class GenIntegerAddition : public Code {
   private:
    condition_s condition;

   public:
    GenIntegerAddition(db_s* db) : Code(db) {}
    GenIntegerAddition(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        condition = getConditions(dataBase, element);

        size_t arraySize = dataBase->temporaryArray.size();

        dataBase->temporaryArray.push_back(variable_s(INT, 1, arraySize));

        output = "add %t" + std::to_string(arraySize) + ", " + getAccessString(condition.left) + ", " + getAccessString(condition.right);

        return {};
    }
};

class GenIntegerModulus : public Code {
   private:
    condition_s condition;

   public:
    GenIntegerModulus(db_s* db) : Code(db) {}
    GenIntegerModulus(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        std::vector<std::shared_ptr<Code>> instructions;

        condition = getConditions(dataBase, element);

        size_t arraySize = dataBase->temporaryArray.size();

        dataBase->temporaryArray.push_back(variable_s(INT, 1, arraySize));

        instructions.push_back(std::make_shared<Code>(dataBase, "mod %t" + std::to_string(arraySize) + ", " + getAccessString(condition.left) + ", " + getAccessString(condition.right)));

        return instructions;
    }
};

class GenAssign : public Code {
   public:
    GenAssign(db_s* db) : Code(db) {}
    GenAssign(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        std::vector<std::shared_ptr<Code>> ret;

        const std::string varName = element.value().instruction();
        checkVariableExists(varName, dataBase);

        const std::string valueType = element.value().value().instruction();
        const std::string value = element.value().value().value().instruction();
        variable_s target = dataBase->variablesMap.at(varName);

        if (valueType == "CONSTANT") {
            checkValueType(target.type, value);
            output = generateConstantAssignment(target, value);
        } else if (valueType == "VARIABLE") {
            checkVariableExists(value, dataBase);
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
        } else if (valueType == "INPUT_STATEMENT") {
            output = "read %r" + std::to_string(target.offset);
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
            checkVariableExists(value, dataBase);
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
    condition_s condition;

   public:
    GenLessThan(db_s* db) : Code(db) {}
    GenLessThan(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        std::vector<std::shared_ptr<Code>> instructions;
        condition = getConditions(dataBase, element);

        std::string rdest;
        std::string op1;
        std::string op2;

        if (std::holds_alternative<variable_s>(condition.left) || std::holds_alternative<int>(condition.left)) {
            op1 = getAccessString(condition.left);
        } else {
            appendVectors(instructions, parseNodeInstructions(element.value(), dataBase));
            op1 = "%t" + std::to_string(dataBase->temporaryArray.size() - 1);
        }

        if (std::holds_alternative<variable_s>(condition.right) || std::holds_alternative<int>(condition.right)) {
            op2 = getAccessString(condition.right);
        } else {
            appendVectors(instructions, parseNodeInstructions(element.value().next(), dataBase));
            op2 = "%t" + std::to_string(dataBase->temporaryArray.size() - 1);
        }

        size_t arraySize = dataBase->temporaryArray.size();
        rdest = "%t" + std::to_string(arraySize);
        dataBase->temporaryArray.push_back(variable_s(BOOL, 1, arraySize));

        instructions.push_back(std::make_shared<Code>(dataBase, "less " + rdest + ", " + op1 + ", " + op2));

        return instructions;
    }
};

class GenLessThanEqual : public Code {
   private:
    condition_s condition;

   public:
    GenLessThanEqual(db_s* db) : Code(db) {}
    GenLessThanEqual(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        condition = getConditions(dataBase, element);

        size_t arraySize = dataBase->temporaryArray.size();
        dataBase->temporaryArray.push_back(variable_s(BOOL, 1, arraySize));

        output = "lesseq %t" + std::to_string(arraySize) + ", " + getAccessString(condition.left) + ", " + getAccessString(condition.right);

        return {};
    }
};

class GenEqual : public Code {
   private:
    condition_s condition;

   public:
    GenEqual(db_s* db) : Code(db) {}
    GenEqual(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        std::vector<std::shared_ptr<Code>> instructions;
        condition = getConditions(dataBase, element);

        std::string rdest;
        std::string op1;
        std::string op2;

        if (std::holds_alternative<variable_s>(condition.left) || std::holds_alternative<int>(condition.left)) {
            op1 = getAccessString(condition.left);
        } else {
            appendVectors(instructions, parseNodeInstructions(element.value(), dataBase));
            op1 = "%t" + std::to_string(dataBase->temporaryArray.size() - 1);
        }

        if (std::holds_alternative<variable_s>(condition.right) || std::holds_alternative<int>(condition.right)) {
            op2 = getAccessString(condition.right);
        } else {
            appendVectors(instructions, parseNodeInstructions(element.value().next(), dataBase));
            op2 = "%t" + std::to_string(dataBase->temporaryArray.size() - 1);
        }

        size_t arraySize = dataBase->temporaryArray.size();
        rdest = "%t" + std::to_string(arraySize);
        dataBase->temporaryArray.push_back(variable_s(BOOL, 1, arraySize));

        instructions.push_back(std::make_shared<Code>(dataBase, "equal " + rdest + ", " + op1 + ", " + op2));

        return instructions;
    }
};

class GenNotEqual : public Code {
   private:
    condition_s condition;

   public:
    GenNotEqual(db_s* db) : Code(db) {}
    GenNotEqual(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        condition = getConditions(dataBase, element);

        size_t arraySize = dataBase->temporaryArray.size();
        dataBase->temporaryArray.push_back(variable_s(BOOL, 1, arraySize));

        output = "diff %t" + std::to_string(arraySize) + ", " + getAccessString(condition.left) + ", " + getAccessString(condition.right);

        return {};
    }
};

class GenGreaterThan : public Code {
   private:
    condition_s condition;

   public:
    GenGreaterThan(db_s* db) : Code(db) {}
    GenGreaterThan(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        condition = getConditions(dataBase, element);

        size_t arraySize = dataBase->temporaryArray.size();
        dataBase->temporaryArray.push_back(variable_s(BOOL, 1, arraySize));

        output = "greater %t" + std::to_string(arraySize) + ", " + getAccessString(condition.left) + ", " + getAccessString(condition.right);

        return {};
    }
};

class GenGreaterThanEqual : public Code {
   private:
    condition_s condition;

   public:
    GenGreaterThanEqual(db_s* db) : Code(db) {}
    GenGreaterThanEqual(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        std::vector<std::shared_ptr<Code>> instructions;
        condition = getConditions(dataBase, element);

        std::string rdest;
        std::string op1;
        std::string op2;

        if (std::holds_alternative<variable_s>(condition.left) || std::holds_alternative<int>(condition.left)) {
            op1 = getAccessString(condition.left);
        } else {
            appendVectors(instructions, parseNodeInstructions(element.value(), dataBase));
            op1 = "%t" + std::to_string(dataBase->temporaryArray.size() - 1);
        }

        if (std::holds_alternative<variable_s>(condition.right) || std::holds_alternative<int>(condition.right)) {
            op2 = getAccessString(condition.right);
        } else {
            appendVectors(instructions, parseNodeInstructions(element.value().next(), dataBase));
            op2 = "%t" + std::to_string(dataBase->temporaryArray.size() - 1);
        }

        size_t arraySize = dataBase->temporaryArray.size();
        rdest = "%t" + std::to_string(arraySize);
        dataBase->temporaryArray.push_back(variable_s(BOOL, 1, arraySize));

        instructions.push_back(std::make_shared<Code>(dataBase, "greatereq " + rdest + ", " + op1 + ", " + op2));

        return instructions;
    }
};

class GenOr : public Code {
   private:
    condition_s condition;

   public:
    GenOr(db_s* db) : Code(db) {}
    GenOr(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        std::vector<std::shared_ptr<Code>> instructions;
        condition = getConditions(dataBase, element);

        std::string rdest;
        std::string op1;
        std::string op2;

        if (std::holds_alternative<variable_s>(condition.left) || std::holds_alternative<int>(condition.left)) {
            op1 = getAccessString(condition.left);
        } else {
            appendVectors(instructions, parseNodeInstructions(element.value(), dataBase));
            op1 = "%t" + std::to_string(dataBase->temporaryArray.size() - 1);
        }

        if (std::holds_alternative<variable_s>(condition.right) || std::holds_alternative<int>(condition.right)) {
            op2 = getAccessString(condition.right);
        } else {
            appendVectors(instructions, parseNodeInstructions(element.value().next(), dataBase));
            op2 = "%t" + std::to_string(dataBase->temporaryArray.size() - 1);
        }

        size_t arraySize = dataBase->temporaryArray.size();
        rdest = "%t" + std::to_string(arraySize);
        dataBase->temporaryArray.push_back(variable_s(BOOL, 1, arraySize));

        instructions.push_back(std::make_shared<Code>(dataBase, "or " + rdest + ", " + op1 + ", " + op2));

        return instructions;
    }
};

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
        appendVectors(instructions, parseNodeInstructions(assignmentNode.next(), dataBase));

        // Loop back
        instructions.push_back(std::make_shared<Code>(dataBase, "jump " + startLabelStr));

        // End label
        instructions.push_back(std::make_shared<Code>(dataBase, "label " + endLabelStr));

        // Clearing the stack
        dataBase->temporaryArray.pop_back();

        return instructions;
    }
};

class GenWhileLoop : public Code {
   public:
    GenWhileLoop(db_s* db) : Code(db) {}
    GenWhileLoop(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        std::vector<std::shared_ptr<Code>> instructions;

        std::string startLabelStr = "begin_while_" + std::to_string(dataBase->labelCounter);
        dataBase->labelCounter++;
        std::string endLabelStr = "end_while_" + std::to_string(dataBase->labelCounter);
        dataBase->labelCounter++;

        // Begin label
        instructions.push_back(std::make_shared<Code>(dataBase, "label " + startLabelStr));

        // condition
        IRNode condition = element.value().value();
        appendVectors(instructions, parseNodeInstructions(condition, dataBase));

        // Jump out if condition is no longer met
        instructions.push_back(std::make_shared<Code>(dataBase, "jf %t" + std::to_string(dataBase->temporaryArray.size() - 1) + ", " + endLabelStr));

        // Parse the while loop
        appendVectors(instructions, parseNodeInstructions(element.value().next(), dataBase));

        // Loop back
        instructions.push_back(std::make_shared<Code>(dataBase, "jump " + startLabelStr));

        // End label
        instructions.push_back(std::make_shared<Code>(dataBase, "label " + endLabelStr));

        // Clearing the stack
        dataBase->temporaryArray.pop_back();

        return instructions;
    }
};

class GenIfElseCondition : public Code {
   public:
    GenIfElseCondition(db_s* db) : Code(db) {}
    GenIfElseCondition(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        std::vector<std::shared_ptr<Code>> instructions;

        if (element.value().instruction() != "IF")
            semanticError("Invalid if condition");

        std::string startLabelStr = "begin_if_" + std::to_string(dataBase->labelCounter);
        dataBase->labelCounter++;
        std::string elseLabelStr = "else_if_" + std::to_string(dataBase->labelCounter);
        dataBase->labelCounter++;
        std::string endLabelStr = "end_if_" + std::to_string(dataBase->labelCounter);
        dataBase->labelCounter++;

        // Begin label
        instructions.push_back(std::make_shared<Code>(dataBase, "label " + startLabelStr));

        // condition
        IRNode condition = element.value().value().value();
        appendVectors(instructions, parseNodeInstructions(condition, dataBase));

        // Jump to else if condition is not met
        instructions.push_back(std::make_shared<Code>(dataBase, "jf %t" + std::to_string(dataBase->temporaryArray.size() - 1) + ", " + elseLabelStr));

        // Parse the if block

        IRNode ifBlock = element.value().value().next();

        appendVectors(instructions, parseNodeInstructions(ifBlock, dataBase));

        // Jump to end
        instructions.push_back(std::make_shared<Code>(dataBase, "jump " + endLabelStr));

        // Else label
        instructions.push_back(std::make_shared<Code>(dataBase, "label " + elseLabelStr));

        // Parse the else block
        IRNode elseBlock = element.value().next().value();

        appendVectors(instructions, parseNodeInstructions(elseBlock, dataBase));

        // End label
        instructions.push_back(std::make_shared<Code>(dataBase, "label " + endLabelStr));

        return instructions;
    }
};

class GenIf : public Code {
   public:
    GenIf(db_s* db) : Code(db) {}
    GenIf(db_s* db, std::string _output) : Code(db, _output) {};

    std::vector<std::shared_ptr<Code>> generate(IRNode element) override {
        std::vector<std::shared_ptr<Code>> instructions;

        std::string startLabelStr = "begin_if_" + std::to_string(dataBase->labelCounter);
        dataBase->labelCounter++;
        std::string endLabelStr = "end_if_" + std::to_string(dataBase->labelCounter);
        dataBase->labelCounter++;

        // Begin label
        instructions.push_back(std::make_shared<Code>(dataBase, "label " + startLabelStr));

        // condition
        IRNode condition = element.value().value();
        appendVectors(instructions, parseNodeInstructions(condition, dataBase));

        // Jump to end if condition is not met
        instructions.push_back(std::make_shared<Code>(dataBase, "jf %t" + std::to_string(dataBase->temporaryArray.size() - 1) + ", " + endLabelStr));

        // Parse the if block

        IRNode ifBlock = element.value().next();

        appendVectors(instructions, parseNodeInstructions(ifBlock, dataBase));

        // End label
        instructions.push_back(std::make_shared<Code>(dataBase, "label " + endLabelStr));

        return instructions;
    }
};