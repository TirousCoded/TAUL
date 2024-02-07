

#pragma once


#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <format>


namespace taul {


    enum class spec_opcode : std::uint8_t {

        close,
        lpr_decl,
        ppr_decl,
        lpr,
        ppr,

        end,            // lexer/parser
        any,            // lexer/parser

        // within lprs, string matches against the target character sequence
        
        // within pprs, string matches a single non-failure token w/ a matched string
        // which matches the string instruction's oprand exactly

        // string exprs use taul::parse_taul_string to allow for use of escape sequence 
        // syntax in their oprands

        string,         // lexer/parser

        // charset matches a single character which is present in its oprand string

        // charset exprs use taul::parse_taul_charset to allow for use of escape sequence 
        // and char range syntax in their oprands

        charset,        // lexer-only

        // token matches any single non-failure token

        token,          // parser-only

        // failure matches any single failure token

        failure,        // parser-only

        // within lprs, name can refer only to lprs

        // within pprs, name can refer both to lprs and pprs

        name,           // lexer/parser

        sequence,       // lexer/parser
        set,            // lexer/parser

        // lookahead, lookahead-not, and not exprs encapsulate 'single 
        // terminal scope' within their scopes, which is a scope which 
        // only allows a single subexpr matching exactly one terminal

        // terminal-only scope allows only the following exprs:
        //      - end
        //      - any
        //      - string        <- may only be 1 char long within lprs
        //      - charset
        //      - token
        //      - failure
        //      - set           <- containing only allowed subexprs

        lookahead,      // lexer/parser
        lookahead_not,  // lexer/parser
        not0,           // lexer/parser

        optional,       // lexer/parser
        kleene_star,    // lexer/parser
        kleene_plus,    // lexer/parser

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

