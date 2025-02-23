#pragma once

enum variable_type_e {
    INT,
    STRING,
};

struct variable_s {
    variable_type_e type;
    unsigned int size;    // Size of the variable in registers
    unsigned int offset;  // Offset from the base pointer
};
