#pragma once

#include <iostream>
#include <string>

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
    }
    return true;
}
