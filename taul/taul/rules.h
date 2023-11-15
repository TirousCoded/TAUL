

#pragma once


#include <stddef.h>
#include <string>


namespace taul {


    // in taul::grammar, lexer/parser production rules will be referred
    // to as 'LPRs', and 'PPRs', respectively

    struct lexer_rule final {
        std::string name;
        std::size_t index = -1;
    };

    struct parser_rule final {
        std::string name;
        std::size_t index = -1;
    };
}

