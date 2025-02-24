#pragma once

#include <map>
#include <string>
#include <vector>

#include "variable.h"

struct db_s {
    std::map<std::string, variable_s> variablesMap;
    std::vector<variable_s> temporaryArray;
    unsigned int labelCounter = 0;
};
