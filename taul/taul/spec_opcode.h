

#pragma once


#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <format>


namespace taul {


    enum class spec_opcode : std::uint8_t {

        // composite exprs may encapsulate the following special scopes:
        //      - single-terminal scope
        //      - no-alternation scope
        //      - single-subexpr scope
        //      - no-end-subexpr scope


        // TODO: single-terminal, no-alternation and single-subexpr scope non-propagation 
        //       has not really been unit tested yet

        
        // within single-terminal scope, the exprs therein are required to describe
        // a pattern that, no matter what, always matches exactly one terminal,
        // no more, no less

        // single-terminal scope allows only the following exprs:
        //      - end
        //      - any
        //      - string            <- only 1 char long, only within lprs
        //      - charset
        //      - token
        //      - failure
        //      - name              <- only if ref to lpr, only within pprs
        //      - sequence
        
        // composite exprs w/ single-terminal scope are implicitly also defined as
        // encapsulating single-subexpr scope

        // single-terminal scope does not propagate down to composite subexprs

        // the following composite exprs encapsulate single-terminal scope:
        //      - sequence          <- if nested within outer single-terminal scope
        //      - lookahead
        //      - lookahead-not
        //      - not


        // within no-alternation scope, the alternative instruction is forbidden,
        // and thus the composite expr may encapsulate no more than one alternative

        // no-alternation scope does not propagate down to composite subexprs

        // the following composite exprs encaspulate no-alternation scope:
        //      - optional
        //      - kleene-star
        //      - kleene-plus

        // take note that in our system, all composite exprs encapsulate at least
        // one 'alternative', w/ no-alternative scope simply forbidding the creation
        // of additional alternatives via the 'alternative' instruction


        // within single-subexpr scope, each alternative of the expr is required
        // to carry exactly one subexpr, no more, no less

        // single-subexpr scope does not propagate down to composite subexprs
        // (w/ the exception of propagation by proxy of single-terminal scope)

        // the following composite exprs encapsulate single-subexpr scope:
        //      - optional
        //      - kleene-star
        //      - kleene-plus
        //      - all single-terminal scope encapsulating composite exprs


        // within no-end-subexpr scope, the end expr may not be used, pretty simple

        // no-end-subexpr scope does not propagate down to composite subexprs

        // the following composite exprs encapsulate no-end-subexpr scope:
        //      - sequence          <- if nested within outer no-end-subexpr scope
        //      - lookahead
        //      - lookahead-not
        //      - not


        // close indicates end of composite expr

        close,          // not an expr

        // alternative indicates start of next alternative within composite expr

        // within composite exprs, alternatives w/ common prefixes may result in the
        // grammar becoming ambiguous

        alternative,    // not an expr

        lpr_decl,       // top-level
        ppr_decl,       // top-level
        lpr,            // top-level
        ppr,            // top-level

        // end matches the end-of-input terminal, w/out consuming it

        end,            // lexer/parser

        // any matches any terminal, excluding the end-of-input terminal

        any,            // lexer/parser

        // TODO: maybe reimpl string use in PPRs, later

        // within lprs, string matches against the target character sequence
        
        // string exprs use taul::parse_taul_string to allow for use of escape sequence 
        // syntax in their oprands

        string,         // lexer-only

        // charset matches a single character which is present in its oprand string

        // charset exprs use taul::parse_taul_charset to allow for use of escape sequence 
        // and char range syntax in their oprands

        // having multiple of the same character in a charset oprand string does not result
        // in the grammar becoming ambiguous, as the duplicates are simply removed

        charset,        // lexer-only

        // TODO: do our tests cover w/ end-of-input terminal?

        // token matches any single non-failure token, nor end-of-input terminal

        token,          // parser-only

        // failure matches any single failure token

        failure,        // parser-only

        // within lprs, name can refer only to lprs

        // within pprs, name can refer both to lprs and pprs
        
        name,           // lexer/parser

        sequence,       // lexer/parser

        // lookahead asserts that the next terminal must be one in its set,
        // w/out consuming input

        // lookahead-not asserts that the next terminal must be one NOT in 
        // its set, nor an end-of-input terminal, w/out consuming input

        // not asserts that the next terminal must be one NOT in its set,
        // nor an end-of-input terminal, w/ consuming input

        // lookahead and lookahead-not are *transparent*, in that they do
        // not contribute to any kind of parse tree or other output

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

