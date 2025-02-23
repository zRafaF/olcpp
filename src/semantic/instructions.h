#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "code.h"
#include "db.h"
#include "ir_node.h"
#include "variable.h"

std::vector<std::shared_ptr<Code>> parseNodeInstructions(IRNode node, db_s* db) {
    std::vector<std::shared_ptr<Code>> instructions;

    while (node) {
        const std::string instruction = node.instruction();
        std::cout << "Parsing Instruction: " << instruction << std::endl;
        IRNode valueNode = node.value();
        std::shared_ptr<Code> code;
        if (instruction == "VARIABLE_DECLARATION") {
            code = std::make_shared<GenVariableDeclaration>(db);
            instructions.push_back(code);
        } else if (instruction == "ASSIGN") {
            code = std::make_shared<GenAssign>(db);
            instructions.push_back(code);
        } else if (instruction == "PRINT_STATEMENT") {
            code = std::make_shared<GenPrintStatement>(db);
            instructions.push_back(code);
        } else if (instruction == "FOR_LOOP") {
            code = std::make_shared<GenForLoop>(db);
            instructions.push_back(code);
        }

        if (code) {
            std::vector<std::shared_ptr<Code>> generatedInstructions = code->generate(valueNode);

            instructions.insert(instructions.end(), generatedInstructions.begin(), generatedInstructions.end());
        }

        node = node.next();
    }
    return instructions;
}