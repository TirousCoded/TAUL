

#pragma once


#include <stddef.h>
#include <stdexcept>
#include <string>

#include "qualifier.h"

#include "internal/migrated/lexer.h"
#include "internal/migrated/parser.h"


namespace taul {


    class lpr_association_error final : public std::logic_error {
    public:

        inline explicit lpr_association_error(const std::string& msg) : logic_error(msg) {}
        inline explicit lpr_association_error(const char* msg) : logic_error(msg) {}
    };

    class ppr_association_error final : public std::logic_error {
    public:

        inline explicit ppr_association_error(const std::string& msg) : logic_error(msg) {}
        inline explicit ppr_association_error(const char* msg) : logic_error(msg) {}
    };


    // in taul::grammar, lexer/parser production rules will be referred
    // to as 'LPRs', and 'PPRs', respectively

    struct lexer_rule final {
        std::string     name;
        std::size_t     index       = -1;
        taul::qualifier qualifer    = taul::qualifier::none;

        taul::internal::lexer fnobj; // internal, do not use
    };

    struct parser_rule final {
        std::string     name;
        std::size_t     index = -1;

        taul::internal::parser fnobj; // internal, do not use
    };
}

