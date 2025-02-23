#pragma once

#include <cstring>
#include <string>

#include "json.h"

std::string getInstruction(json_object_element_s* element) {
    const json_object_element_s* current = element;
    while (current != nullptr) {
        if (strcmp(current->name->string, "instruction") == 0) {
            return json_value_as_string(current->value)->string;
        }
        current = current->next;
    }
    return "";  // Return an empty string if "instruction" is not found
}

json_object_element_s* getValue(json_object_element_s* element) {
    json_object_element_s* current = element;
    while (current != nullptr) {
        if (strcmp(current->name->string, "value") == 0) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

json_object_element_s* getNext(json_object_element_s* element) {
    json_object_element_s* current = element;
    while (current != nullptr) {
        if (strcmp(current->name->string, "next") == 0) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}
