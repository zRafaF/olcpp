#pragma once
#include <string>

#include "code.h"

class GenPrintString : public Code {
   public:
    explicit GenPrintString(std::string message);
    void generate(std::ostream& out) const override;

   private:
    std::string message;
};