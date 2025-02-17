#pragma once

#include <string>

class Code {
   public:
    virtual ~Code() = default;
    virtual void generate(std::ostream& out) const = 0;
};
