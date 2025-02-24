#pragma once

#include <iostream>
#include <string>

#include "db.h"
#include "variable.h"

void semanticError(const std::string& errorMessage) {
    std::cerr << "\033[31m" << "Error: " << errorMessage << "\033[0m" << std::endl;
    std::exit(EXIT_FAILURE);
}

bool isVariableTypeValid(variable_type_e type, std::string value) {
    if (type == INT) {
        try {
            std::stoi(value);
        } catch (const std::invalid_argument& e) {
            semanticError("Invalid value for integer variable");
            return false;
        }
    } else if (type == BOOL) {
        if (value != "TRUE" && value != "FALSE") {
            semanticError("Invalid value for boolean variable");
            return false;
        }
    }
    return true;
}

void checkVariableExists(const std::string& name, db_s* dataBase) {
    if (dataBase->variablesMap.find(name) == dataBase->variablesMap.end()) {
        semanticError("Undefined variable [" + name + "]");
    }
}

void checkValueType(variable_type_e type, const std::string& value) {
    if (!isVariableTypeValid(type, value)) {
        semanticError("Invalid value for variable type");
    }
}