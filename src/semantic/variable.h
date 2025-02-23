#pragma once
#include <iostream>

enum variable_type_e {
    INT,
    STRING,
};

struct variable_s {
    variable_type_e type;
    unsigned int size;    // Size of the variable in registers
    unsigned int offset;  // Offset from the base pointer

    variable_s(variable_type_e type, unsigned int size, unsigned int offset) : type(type), size(size), offset(offset) {};

    // Overload << operator for std::ostream
    friend std::ostream& operator<<(std::ostream& os, const variable_s& var) {
        os << "Variable: { type: " << var.type << ", size: " << var.size << ", offset: " << var.offset << " }";
        return os;
    }
};

variable_type_e parseVariableType(const std::string type) {
    if (type == "INTEGER_TYPE") {
        return INT;
    } else if (type == "STRING_TYPE") {
        return STRING;
    }
    return INT;
}