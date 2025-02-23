#pragma once

#include <map>
#include <string>

#include "variable.h"

struct db_s {
    std::map<std::string, variable_s> variablesMap;
    std::map<std::string, variable_s> temporaryMap;
    unsigned int labelCounter = 0;
};
