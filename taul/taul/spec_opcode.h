

#pragma once


#include <cstddef>
#include <cstdint>
#include <string>
#include <format>


namespace taul {


    // the '0' below are used to let us use identifiers which are otherwise keywords

    enum class spec_opcode : std::uint8_t {
        grammar_bias,   // grammar bias defaults to first-longest if missing
        close,
        lpr_decl,
        ppr_decl,
        lpr,
        ppr,

        // lexer simple exprs

        char0,

        // parser simple exprs

        //

        // shared simple exprs

        //

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

