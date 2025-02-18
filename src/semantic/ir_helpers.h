#pragma once

#include <cstring>
#include <string>

#include "json.h"

std::string getInstruction(const json_object_s* object) {
    const json_object_element_s* current = object->start;
    while (current != nullptr) {
        if (strcmp(current->name->string, "instruction") == 0) {
            return json_value_as_string(current->value)->string;
        }
        current = current->next;
    }
    return "";  // Return an empty string if "instruction" is not found
}

const json_object_s* getValue(const json_object_s* object) {
    const json_object_element_s* current = object->start;
    while (current != nullptr) {
        if (strcmp(current->name->string, "value") == 0) {
            return json_value_as_object(current->value);
        }
        current = current->next;
    }
    return nullptr;
}

const json_object_s* getNext(const json_object_s* object) {
    const json_object_element_s* current = object->start;
    while (current != nullptr) {
        if (strcmp(current->name->string, "next") == 0) {
            return json_value_as_object(current->value);
        }
        current = current->next;
    }
    return nullptr;
}
