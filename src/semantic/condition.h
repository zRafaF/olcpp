#pragma once

#include <variant>

#include "checkers.h"
#include "db.h"
#include "ir_node.h"
#include "variable.h"

struct condition_s {
    std::variant<variable_s, int> left;
    std::variant<variable_s, int> right;
};

condition_s getConditions(db_s* dataBase, IRNode element) {
    condition_s returnCondition;

    if (element.value().instruction() == "VARIABLE") {
        checkVariableExists(element.value().value().instruction(), dataBase);
        returnCondition.left = dataBase->variablesMap.at(element.value().value().instruction());
    } else if (element.value().instruction() == "CONSTANT") {
        returnCondition.left = std::stoi(element.value().value().instruction());
    } else {
        semanticError("Invalid left operand");
    }

    IRNode rightNode = element.next();

    if (rightNode.instruction() == "VARIABLE") {
        checkVariableExists(rightNode.value().instruction(), dataBase);
        returnCondition.right = dataBase->variablesMap.at(rightNode.value().instruction());
    } else if (rightNode.instruction() == "CONSTANT") {
        returnCondition.right = std::stoi(rightNode.value().instruction());
    } else {
        semanticError("Invalid right operand");
    }

    return returnCondition;
}