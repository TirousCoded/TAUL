

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

        begin,          // lexer-only   TODO: add parser
        end,            // lexer-only   TODO: add parser
        any,            // lexer-only   TODO: add parser
        string,         // lexer-only   TODO: add parser
        charset,        // lexer-only   TODO: add parser
        sequence,       // lexer-only   TODO: add parser
        set,            // lexer-only   TODO: add parser
        modifier,       // lexer-only   TODO: add parser TODO: in docs, mention max==0 disables max
        assertion,      // lexer-only   TODO: add parser
        constraint,     // lexer-only   TODO: add parser
        junction,       // the *junction* between 'constrained' and 'constraining' exprs; this is not an expr

        // TODO: when we add parser stuff, we'll need semantics to make it illegal for a localend
        //       in an embedded lexer expr within a parser expr to be for a parser constraint

        localend,       // lexer-only   TODO: add parser

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

