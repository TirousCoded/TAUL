

#pragma once


#include <string>
#include <format>

#include <taul/symbols.h>
#include <taul/symbol_observer.h>


class test_token_observer final : public taul::token_observer {
public:

    std::string output;


    inline void observe(taul::token x) {
        output += std::format("\n{}", x);
    }
};

