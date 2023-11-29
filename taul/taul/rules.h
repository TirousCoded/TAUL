

#pragma once


#include <stddef.h>
#include <string>

#include "qualifier.h"
#include "lexer.h"


namespace taul {


    // in taul::grammar, lexer/parser production rules will be referred
    // to as 'LPRs', and 'PPRs', respectively

    struct lexer_rule final {
        std::string     name;
        std::size_t     index       = -1;
        taul::qualifier qualifer    = taul::qualifier::none;
        taul::lexer     fnobj;                                  // the lexer function object of the lexer rule


        // TODO: the behaviour of fnobj is tested in grammar_usage_tests.cpp, but this
        //       method is otherwise not *directly* been unit tested yet

        constexpr const taul::lexer& lexer() const noexcept { return fnobj; }
    };

    struct parser_rule final {
        std::string name;
        std::size_t index = -1;
    };
}

