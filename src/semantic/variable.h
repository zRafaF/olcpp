#pragma once
#include <iostream>

enum variable_type_e {
    INT,
    STRING,
    BOOL
};

struct variable_s {
    variable_type_e type;
    unsigned int size;    // Size of the variable in registers
    unsigned int offset;  // Offset from the base pointer

    variable_s(variable_type_e type, unsigned int size, unsigned int offset) : type(type), size(size), offset(offset) {};
    variable_s() : type(INT), size(1), offset(0) {};

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
    } else if (type == "BOOL_TYPE") {
        return BOOL;
    }
    return INT;
}

unsigned int getVariableTypeSize(variable_type_e type) {
    if (type == STRING) {
        return 1024;
    }
    return 1;
}
