// ir_helpers.h
#pragma once

#include <cstring>
#include <string>

#include "json.h"

class IRNode {
    json_value_s* m_value;

    json_object_element_s* find_element(const char* name) const {
        if (!m_value) return nullptr;
        json_object_s* obj = json_value_as_object(m_value);
        if (!obj) return nullptr;

        for (json_object_element_s* elem = obj->start; elem != nullptr; elem = elem->next) {
            if (strcmp(elem->name->string, name) == 0) {
                return elem;
            }
        }
        return nullptr;
    }

   public:
    IRNode(json_value_s* value = nullptr) : m_value(value) {}

    explicit operator bool() const { return m_value != nullptr; }

    std::string instruction() const {
        if (auto elem = find_element("instruction")) {
            if (auto str = json_value_as_string(elem->value)) {
                return str->string;
            }
        }
        return "";
    }

    IRNode value() const {
        if (auto elem = find_element("value")) {
            return IRNode(elem->value);
        }
        return nullptr;
    }

    IRNode next() const {
        if (auto elem = find_element("next")) {
            return IRNode(elem->value);
        }
        return nullptr;
    }

    json_value_s* raw_value() const { return m_value; }
};