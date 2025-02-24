#pragma once

#include <variant>

#include "checkers.h"
#include "db.h"
#include "ir_node.h"
#include "variable.h"

struct condition_s {
    std::variant<std::monostate, variable_s, int> left;
    std::variant<std::monostate, variable_s, int> right;
};

condition_s getConditions(db_s* dataBase, IRNode element) {
    condition_s returnCondition;

    if (element.value().instruction() == "VARIABLE") {
        checkVariableExists(element.value().value().instruction(), dataBase);
        returnCondition.left = dataBase->variablesMap.at(element.value().value().instruction());
    } else if (element.value().instruction() == "CONSTANT") {
        returnCondition.left = std::stoi(element.value().value().instruction());
    } else {
        returnCondition.left = std::monostate();
    }

    IRNode rightNode = element.next();

    if (rightNode.instruction() == "VARIABLE") {
        checkVariableExists(rightNode.value().instruction(), dataBase);
        returnCondition.right = dataBase->variablesMap.at(rightNode.value().instruction());
    } else if (rightNode.instruction() == "CONSTANT") {
        returnCondition.right = std::stoi(rightNode.value().instruction());
    } else {
        returnCondition.right = std::monostate();
    }

    return returnCondition;
}

std::string getAccessString(std::variant<std::monostate, variable_s, int> val) {
    if (std::holds_alternative<variable_s>(val)) {
        return "%r" + std::to_string(std::get<variable_s>(val).offset);
    } else {
        return std::to_string(std::get<int>(val));
    }
}
