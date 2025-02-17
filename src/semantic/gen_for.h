#pragma once

#include <memory>
#include <vector>

#include "code.h"

class GenCondition;  // Forward declaration

class GenFor : public Code {
   public:
    GenFor(std::unique_ptr<GenCondition> condition, std::vector<std::unique_ptr<Code>> code);
    void generate(std::ostream& out) const override;

   private:
    std::unique_ptr<GenCondition> condition;
    std::vector<std::unique_ptr<Code>> code;
};
