#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "code.h"
#include "db.h"
#include "helpers.h"
#include "ir_node.h"
#include "variable.h"

std::vector<std::shared_ptr<Code>> parseNodeInstructions(IRNode node, db_s* db) {
    std::vector<std::shared_ptr<Code>> instructions;

    while (node) {
        const std::string instruction = node.instruction();
        std::cout << "Parsing Instruction: " << instruction << std::endl;
        std::shared_ptr<Code> code;
        if (instruction == "VARIABLE_DECLARATION") {
            code = std::make_shared<GenVariableDeclaration>(db);
        } else if (instruction == "ASSIGN") {
            code = std::make_shared<GenAssign>(db);
        } else if (instruction == "ASSIGN_ARRAY") {
            code = std::make_shared<GenAssignArray>(db);
        } else if (instruction == "PRINT_STATEMENT") {
            code = std::make_shared<GenPrintStatement>(db);
        }

        else if (instruction == "FOR_LOOP") {
            code = std::make_shared<GenForLoop>(db);
        } else if (instruction == "WHILE_LOOP") {
            code = std::make_shared<GenWhileLoop>(db);
        }

        else if (instruction == "LESS_THAN") {
            code = std::make_shared<GenLessThan>(db);
        } else if (instruction == "LESS_THAN_EQUAL") {
            code = std::make_shared<GenLessThanEqual>(db);
        } else if (instruction == "EQUAL") {
            code = std::make_shared<GenEqual>(db);
        } else if (instruction == "NOT_EQUAL") {
            code = std::make_shared<GenNotEqual>(db);
        } else if (instruction == "GREATER_THAN") {
            code = std::make_shared<GenGreaterThan>(db);
        } else if (instruction == "GREATER_THAN_EQUAL") {
            code = std::make_shared<GenGreaterThanEqual>(db);

        } else if (instruction == "IF_ELSE_CONDITION") {
            code = std::make_shared<GenIfElseCondition>(db);
        } else if (instruction == "IF") {
            code = std::make_shared<GenIf>(db);
        }

        else if (instruction == "OR") {
            code = std::make_shared<GenOr>(db);
        }

        else if (instruction == "INTEGER_MODULUS") {
            code = std::make_shared<GenIntegerModulus>(db);
        } else {
        }

        if (code) {
            instructions.push_back(code);

            std::vector<std::shared_ptr<Code>> generatedInstructions = code->generate(node);

            appendVectors(instructions, generatedInstructions);
        }

        node = node.next();
    }
    return instructions;
}