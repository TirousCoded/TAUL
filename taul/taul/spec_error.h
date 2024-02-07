

#pragma once


#include <ostream>
#include <string>
#include <format>

#include "error_counter.h"


namespace taul {


    enum class spec_error : std::uint8_t {

        // these errors arise during the creation of specs

        source_code_not_found,              // indicates that compilation could not begin due to source code being unavailable
        syntax_error,                       // indicates a syntax error during compilation

        // these errors arise during the actual loading of specs

        scope_not_closed,                   // spec ends w/ still open composite expr scopes
        stray_close,                        // usage of close instruction w/ no associated scope to close
        rule_name_conflict,                 // lpr/ppr name already taken
        rule_never_declared,                // expected lpr/ppr but never declared
        rule_never_defined,                 // lpr/ppr declared but never defined
        rule_already_defined,               // attempted to define already defined lpr/ppr
        rule_not_found,                     // name specified name of non-existent lpr/ppr
        rule_may_not_be_ppr,                // name specified name of ppr, but only lpr may be specified
        illegal_in_lpr_scope,               // illegal instruction use in lpr expr scope
        illegal_in_ppr_scope,               // illegal instruction use in ppr expr scope
        illegal_in_no_scope,                // illegal instruction use outside expr scope
        illegal_subexpr_count,              // illegal number of expr subexprs
        illegal_qualifier,                  // illegal LPR/PPR qualifier
        illegal_in_single_terminal_scope,   // illegal subexpr in single-terminal scope
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

