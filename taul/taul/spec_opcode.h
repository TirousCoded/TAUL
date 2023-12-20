

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

        // within lprs, string/charset match against the target character sequence
        
        // within pprs, string/charset match against single non-failure tokens w/ matched strings which 
        // conform to their rule:
        //      - for string, this means the token's matched string must equal its string
        //      - for charset, this means the token's matched string must be a single character
        //        which is present in the charset's string

        string,         // lexer/parser
        charset,        // lexer/parser

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

        // TODO: when we add parser stuff, we'll need semantics to make it illegal for a localend
        //       in an embedded lexer subexpr within a parser expr to be for a parser constraint

        localend,       // lexer/parser

        // lexer subexprs accept only a single nested expr, so a sequence is expected to be generated
        // by the compiler in order to enable TAUL syntax to treat them as allowing a sequence of subexprs

        //lsubexpr,       // parser-only

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

