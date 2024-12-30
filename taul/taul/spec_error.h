

#pragma once


#include <ostream>
#include <string>
#include <format>

#include "error_counter.h"


namespace taul {


    enum class spec_error : std::uint8_t {

        // these errors arise during the creation of specs

        // general errors

        internal_error,                     // indicates that compilation failed due to internal compiler error
        illegal_string_literal,             // illegal string literal
        illegal_charset_literal,            // illegal charset literal

        // syntax compilation errors

        source_code_not_found,              // indicates that compilation could not begin due to source code being unavailable
        syntax_error,                       // indicates a syntax error during compilation
        illegal_multiple_qualifiers,        // illegal for a rule to have multiple qualifiers

        // loading errors

        scope_not_closed,                   // spec ends w/ still open composite expr scopes
        stray_close,                        // usage of close instruction w/ no associated scope to close
        rule_name_conflict,                 // LPR/PPR name already taken
        rule_never_declared,                // expected LPR/PPR but never declared
        rule_never_defined,                 // LPR/PPR declared but never defined
        rule_already_defined,               // attempted to define already defined LPR/PPR
        rule_not_found,                     // name specified name of non-existent LPR/PPR
        rule_may_not_be_ppr,                // name specified name of PPR, but only LPR may be specified
        illegal_rule_declare,               // illegal declaration of an LPR/PPR after rule defining has already started
        illegal_in_lpr_scope,               // illegal instruction use in LPR expr scope
        illegal_in_ppr_scope,               // illegal instruction use in PPR expr scope
        illegal_in_no_scope,                // illegal instruction use outside expr scope
        illegal_qualifier,                  // illegal LPR/PPR qualifier
        illegal_in_single_terminal_scope,   // illegal instruction use in single-terminal scope
        illegal_in_no_alternation_scope,    // illegal instruction use in no-alternation scope
        illegal_in_single_subexpr_scope,    // illegal instruction use in single-subexpr scope
        illegal_in_no_end_subexpr_scope,    // illegal instruction use in no-end-subexpr scope
        illegal_ambiguity,                  // illegal grammar ambiguity
        illegal_right_assoc,                // illegal right associativity specifier

        num,                                // this is not a valid spec error
    };

    constexpr std::size_t spec_errors = (std::size_t)spec_error::num;

    std::string fmt_spec_error(spec_error x);


    using spec_error_counter = error_counter<spec_error>;
}


template<>
struct std::formatter<taul::spec_error> final : std::formatter<std::string> {
    auto format(taul::spec_error x, format_context& ctx) const {
        return formatter<string>::format(taul::fmt_spec_error(x), ctx);
    }
};

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::spec_error& x) {
        return stream << taul::fmt_spec_error(x);
    }
}

