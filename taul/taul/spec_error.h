

#pragma once


#include <string>
#include <format>

#include "error_counter.h"


namespace taul {


    enum class spec_error : std::uint8_t {
        syntax_error,           // used by the frontend to indicate a syntax error
        scope_not_closed,       // spec ends w/ still open composite expr scopes
        stray_close,            // usage of close instruction w/ no associated scope to close
        rule_name_conflict,     // lpr/ppr name already taken
        rule_never_declared,    // expected lpr/ppr but never declared
        rule_never_defined,     // lpr/ppr declared but never defined
        rule_already_defined,   // attempted to define already defined lpr/ppr
        illegal_in_lpr_scope,   // illegal instruction use in lpr expr scope
        illegal_in_ppr_scope,   // illegal instruction use in ppr expr scope
        illegal_in_no_scope,    // illegal instruction use outside expr scope
        num,                    // this is not a valid spec error
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

