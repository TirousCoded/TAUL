

#pragma once


#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <format>


namespace taul {


    enum class spec_opcode : std::uint8_t {

        grammar_bias,   // grammar bias defaults to first-longest if missing
        close,
        lpr_decl,
        ppr_decl,
        lpr,
        ppr,

        begin,          // lexer/parser
        end,            // lexer/parser
        any,            // lexer/parser

        // within lprs, string matches against the target character sequence
        
        // within pprs, string matches a single non-failure token w/ a matched string
        // which matches the string instruction's oprand exactly

        string,         // lexer/parser

        // charset matches a single character which is present in its oprand string

        charset,        // lexer-only

        // range matches a single character that's within a character range

        // the character range is defined by a pair of character values, w/ the range 
        // being defined as including both characters, and all characters in between

        // range does not require the first character value to be the lower of the two,
        // w/ it working the same regardless of what order the range is specified in

        range,          // lexer-only

        // token matches any single non-failure token

        token,          // parser-only

        // failure matches any single failure token

        failure,        // parser-only

        // within lprs, name can refer only to lprs

        // within pprs, name can refer both to lprs and pprs

        name,           // lexer/parser

        sequence,       // lexer/parser
        set,            // lexer/parser

        // TODO: in docs, mention max == 0 disables max

        modifier,       // lexer/parser

        assertion,      // lexer/parser
        constraint,     // lexer/parser
        junction,       // the *junction* between 'constrained' and 'constraining' exprs; this is not an expr

        // TODO: if we ever add an 'lsubexpr' (aka. 'lexer subexpression') expr, we'll need to account
        //       for having a localend within a nested lsubexpr expr be able to detect when the available
        //       constraint expr is the one in the outside parser expr, w/ the localend being deemed
        //       illegal in such circumstances

        localend,       // lexer/parser

        num,            // this is not a valid spec opcode
    };

    constexpr std::size_t spec_opcodes = (std::size_t)spec_opcode::num;

    std::string fmt_spec_opcode(spec_opcode x);
}


template<>
struct std::formatter<taul::spec_opcode> final : std::formatter<std::string> {
    auto format(taul::spec_opcode x, format_context& ctx) const {
        return formatter<string>::format(taul::fmt_spec_opcode(x), ctx);
    }
};

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::spec_opcode& x) {
        return stream << taul::fmt_spec_opcode(x);
    }
}

