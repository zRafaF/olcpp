#pragma once

#include <iostream>
#include <string>

#include "variable.h"

bool isVariableTypeValid(variable_type_e type, std::string value) {
    std::cout << (type == INT ? "int" : "string") << std::endl;

    std::cout << "Value: " << value << std::endl;

    if (type == INT) {
        try {
            std::stoi(value);
        } catch (const std::invalid_argument& e) {
            throw std::invalid_argument("Invalid value for integer variable");
            return false;
        }
    }
    return true;
}

void semanticError(const std::string& errorMessage) {
    std::cerr << "\033[31m" << "Error: " << errorMessage << "\033[0m" << std::endl;
    std::exit(EXIT_FAILURE);
}
