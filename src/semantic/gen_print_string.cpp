#include "gen_print_string.h"

#include <iostream>

GenPrintString::GenPrintString(std::string message) : message(std::move(message)) {}

void GenPrintString::generate(std::ostream& out) const {
    out << "PRINT: " << message << "\n";
}