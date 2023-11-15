

#include "load.h"

#include "asserts.h"


std::optional<taul::grammar> taul::load(const taul::spec& s, const taul::logger_ref& lgr) {
    spec_error_counter ec{};
    return load(s, ec, lgr);
}

std::optional<taul::grammar> taul::load(const taul::spec& s, taul::spec_error_counter& ec, const taul::logger_ref& lgr) {
    internal::load_spec_interpreter interp{};
    interp.ec = &ec;
    interp.lgr = lgr;
    interp.interpret(s);
    if (interp.success) {
        auto data = std::make_shared<internal::grammar_data>(internal::grammar_data{
            interp.last_bias,
            [&]() -> auto {
                std::vector<lexer_rule> result{};
                result.resize(interp.lpr_count);
                for (auto& I : interp.lprs) {
                    result[I.second.index] = std::move(I.second);
                }
                return result;
            }(),
            [&]() -> auto {
                std::vector<parser_rule> result{};
                result.resize(interp.ppr_count);
                for (auto& I : interp.pprs) {
                    result[I.second.index] = std::move(I.second);
                }
                return result;
            }(),
            });
        data->build_lookup();
        return taul::grammar(internal::for_internal_use_tag{}, std::move(data));
    }
    else {
        return std::nullopt;
    }
}

void taul::internal::load_spec_interpreter::check_not_in_lpr_scope(spec_opcode opcode) {
    if (in_lpr()) {
        raise(spec_error::illegal_in_lpr_scope, "{} illegal in lexer expr scope!", opcode);
    }
}

void taul::internal::load_spec_interpreter::check_not_in_ppr_scope(spec_opcode opcode) {
    if (in_ppr()) {
        raise(spec_error::illegal_in_ppr_scope, "{} illegal in parser expr scope!", opcode);
    }
}

void taul::internal::load_spec_interpreter::check_not_in_lpr_nor_ppr_scope(spec_opcode opcode) {
    check_not_in_lpr_scope(opcode);
    check_not_in_ppr_scope(opcode);
}

void taul::internal::load_spec_interpreter::check_in_lpr_or_ppr_scope(spec_opcode opcode) {
    if (!in_lpr_or_ppr()) {
        raise(spec_error::illegal_in_no_scope, "{} illegal outside expr scope!", opcode);
    }
}

void taul::internal::load_spec_interpreter::check_scope_exists_to_close() {
    if (!in_composite_expr()) {
        raise(spec_error::stray_close, "{} with no corresponding composite expr scope to close!", spec_opcode::close);
    }
}

void taul::internal::load_spec_interpreter::check_rule_name_available(std::string_view name) {
    if (has_lpr_or_ppr_decl(name)) {
        raise(spec_error::rule_name_conflict, "name {} already used by another lexer/parser rule!", std::string(name));
    }
}

void taul::internal::load_spec_interpreter::check_lpr_was_declared(std::string_view name) {
    if (!has_lpr_decl(name)) {
        raise(spec_error::rule_never_declared, "lexer rule {} never declared!", std::string(name));
    }
}

void taul::internal::load_spec_interpreter::check_ppr_was_declared(std::string_view name) {
    if (!has_ppr_decl(name)) {
        raise(spec_error::rule_never_declared, "parser rule {} never declared!", std::string(name));
    }
}

void taul::internal::load_spec_interpreter::check_lpr_not_already_defined(std::string_view name) {
    if (has_lpr_def(name)) {
        raise(spec_error::rule_already_defined, "lexer rule {} already defined!", std::string(name));
    }
}

void taul::internal::load_spec_interpreter::check_ppr_not_already_defined(std::string_view name) {
    if (has_ppr_def(name)) {
        raise(spec_error::rule_already_defined, "parser rule {} already defined!", std::string(name));
    }
}

void taul::internal::load_spec_interpreter::check_all_scopes_closed() {
    for (auto it = ess.rbegin(); it != ess.rend(); it++) {
        raise(spec_error::scope_not_closed, "{} does not have corresponding {}!", it->opcode, spec_opcode::close);
    }
}

void taul::internal::load_spec_interpreter::check_all_lprs_are_defined() {
    for (const auto& I : lprs) {
        if (!has_lpr_def(I.second.name)) {
            raise(spec_error::rule_never_defined, "lexer rule {} declared but never defined!", I.second.name);
        }
    }
}

void taul::internal::load_spec_interpreter::check_all_pprs_are_defined() {
    for (const auto& I : pprs) {
        if (!has_ppr_def(I.second.name)) {
            raise(spec_error::rule_never_defined, "parser rule {} declared but never defined!", I.second.name);
        }
    }
}

void taul::internal::load_spec_interpreter::check_all_lprs_and_pprs_are_defined() {
    check_all_lprs_are_defined();
    check_all_pprs_are_defined();
}

void taul::internal::load_spec_interpreter::on_startup() {
    TAUL_ASSERT(ec);
    success = true; // <- deem eval successful until proven otherwise
    last_bias = bias::first_longest; // <- expected default
    lpr_count = 0;
    ppr_count = 0;
    TAUL_ASSERT(lprs.empty());
    TAUL_ASSERT(pprs.empty());
    TAUL_ASSERT(ess.empty());
    TAUL_ASSERT(ets.empty());
}

void taul::internal::load_spec_interpreter::on_shutdown() {
    check_all_scopes_closed();
    check_all_lprs_and_pprs_are_defined();
}

void taul::internal::load_spec_interpreter::on_grammar_bias(bias b) {
    check_not_in_lpr_nor_ppr_scope(spec_opcode::grammar_bias);
    last_bias = b;
}

void taul::internal::load_spec_interpreter::on_close() {
    check_scope_exists_to_close();
    pop_expr();
}

void taul::internal::load_spec_interpreter::on_lpr_decl(std::string_view name) {
    check_rule_name_available(name);
    check_not_in_lpr_nor_ppr_scope(spec_opcode::lpr_decl);
    add_lpr_decl(name);
}

void taul::internal::load_spec_interpreter::on_ppr_decl(std::string_view name) {
    check_rule_name_available(name);
    check_not_in_lpr_nor_ppr_scope(spec_opcode::ppr_decl);
    add_ppr_decl(name);
}

void taul::internal::load_spec_interpreter::on_lpr(std::string_view name) {
    check_lpr_was_declared(name);
    check_lpr_not_already_defined(name);
    check_not_in_lpr_nor_ppr_scope(spec_opcode::lpr);
    add_lpr_def(name);
    ess_entry entry{ spec_opcode::lpr, ets_type::lexer, name, std::size_t(-1) };
    if (lprs.contains(name)) {
        entry.index = lprs.at(name).index;
    }
    push_expr(std::move(entry));
}

void taul::internal::load_spec_interpreter::on_ppr(std::string_view name) {
    check_ppr_was_declared(name);
    check_ppr_not_already_defined(name);
    check_not_in_lpr_nor_ppr_scope(spec_opcode::ppr);
    add_ppr_def(name);
    ess_entry entry{ spec_opcode::ppr, ets_type::parser, name, std::size_t(-1) };
    if (pprs.contains(name)) {
        entry.index = pprs.at(name).index;
    }
    push_expr(std::move(entry));
}

void taul::internal::load_spec_interpreter::on_char() {
    check_not_in_ppr_scope(spec_opcode::char0);
    check_in_lpr_or_ppr_scope(spec_opcode::char0);
}
