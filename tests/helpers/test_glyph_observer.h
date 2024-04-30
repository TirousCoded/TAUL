

#pragma once


#include <string>
#include <format>

#include <taul/symbols.h>
#include <taul/symbol_observer.h>


class test_glyph_observer final : public taul::glyph_observer {
public:

    std::string output;


    inline void observe(taul::glyph x) {
        output += std::format("\n{}", x);
    }
};

