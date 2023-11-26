

#include "load.h"

#include "asserts.h"


std::optional<taul::grammar> taul::load(const taul::spec& s, const std::shared_ptr<logger>& lgr) {
    spec_error_counter ec{};
    return load(s, ec, lgr);
}

std::optional<taul::grammar> taul::load(const taul::spec& s, taul::spec_error_counter& ec, const std::shared_ptr<logger>& lgr) {
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
        // for each lpr we gotta do two things right at the end:
        //      1) hook up the gramdat ptr of the toplevel_lexer_pat of the lpr
        //      2) hook up its fnobj
        for (auto& I : data->_lprs) {
            TAUL_ASSERT(interp.lexer_pat_map.contains(I.name));
            static_cast<internal::toplevel_lexer_pat*>(interp.lexer_pat_map.at(I.name).get())->gramdat = data.get();
            I.fnobj = taul::lexer(internal::pat_lexer_fn, interp.lexer_pat_map.at(I.name));
        }
        return std::make_optional<taul::grammar>(internal::for_internal_use_tag{}, std::move(data));
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

void taul::internal::load_spec_interpreter::check_lpr_or_ppr_exists_with_name(std::string_view name) {
    if (!has_lpr_decl(name) && !has_ppr_decl(name)) {
        raise(spec_error::rule_not_found, "no lexer/parser rule found with name {}!", (std::string)name);
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

void taul::internal::load_spec_interpreter::check_subexpr_count_legal(spec_opcode opcode, std::size_t n) {
    if (!in_composite_expr()) return;
    if (ess.back().opcode != opcode) return;
    TAUL_ASSERT(!lexer_pats.empty());
    const auto subexprs = lexer_pats.back()->children.size();
    if (subexprs != n) {
        raise(spec_error::illegal_subexpr_count, "{} has {} subexprs, but must have exactly {}!", opcode, subexprs, n);
    }
}

void taul::internal::load_spec_interpreter::check_junction_in_constraint_scope() {
    if (!in_composite_expr() || ess.back().opcode != spec_opcode::constraint) {
        raise(spec_error::junction_not_in_constraint, "junction illegal due to not in constraint expr scope!");
    }
}

void taul::internal::load_spec_interpreter::check_junction_not_misplaced() {
    if (!in_composite_expr()) return;
    if (ess.back().opcode != spec_opcode::constraint) return;
    TAUL_ASSERT(!lexer_pats.empty());
    const auto subexprs = lexer_pats.back()->children.size();
    if (subexprs != 1) {
        raise(spec_error::junction_misplaced, "junction must come after exactly 1 constraint expr subexpr, but came after {}!", subexprs);
    }
}

void taul::internal::load_spec_interpreter::check_junction_not_already_established() {
    if (!in_composite_expr()) return;
    if (ess.back().opcode != spec_opcode::constraint) return;
    if (ess.back().has_les_entry) {
        raise(spec_error::junction_already_established, "junction already established!");
    }
}

void taul::internal::load_spec_interpreter::check_has_junction_if_constraint() {
    if (!in_composite_expr()) return;
    if (ess.back().opcode != spec_opcode::constraint) return;
    if (!ess.back().has_les_entry) {
        raise(spec_error::junction_missing, "constraint does not have a junction!");
    }
}

void taul::internal::load_spec_interpreter::check_localend_in_constraint_scope_and_after_junction() {
    if (les.empty()) {
        raise(spec_error::illegal_localend, "localend cannot be used outside the scope of constraining expr of a constraint expr!");
    }
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
    check_subexpr_count_legal(spec_opcode::modifier, 1);
    check_subexpr_count_legal(spec_opcode::assertion, 1);
    check_subexpr_count_legal(spec_opcode::constraint, 2);
    check_has_junction_if_constraint();
    handle_pop_lexer_pat_for_top_ess();
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
    ess_entry entry{ spec_opcode::lpr, ets_type::lexer, name, true };
    if (lprs.contains(name)) {
        entry.index = lprs.at(name).index;
    }
    push_expr(entry);
    push_lexer_pat<toplevel_lexer_pat>(entry.index);
}

void taul::internal::load_spec_interpreter::on_ppr(std::string_view name) {
    check_ppr_was_declared(name);
    check_ppr_not_already_defined(name);
    check_not_in_lpr_nor_ppr_scope(spec_opcode::ppr);
    add_ppr_def(name);
    ess_entry entry{ spec_opcode::ppr, ets_type::parser, name, false };
    if (pprs.contains(name)) {
        entry.index = pprs.at(name).index;
    }
    push_expr(std::move(entry));
}

void taul::internal::load_spec_interpreter::on_begin() {
    check_not_in_ppr_scope(spec_opcode::begin);
    check_in_lpr_or_ppr_scope(spec_opcode::begin);
    bind_lexer_pat<begin_lexer_pat>();
}

void taul::internal::load_spec_interpreter::on_end() {
    check_not_in_ppr_scope(spec_opcode::end);
    check_in_lpr_or_ppr_scope(spec_opcode::end);
    bind_lexer_pat<end_lexer_pat>();
}

void taul::internal::load_spec_interpreter::on_any() {
    check_not_in_ppr_scope(spec_opcode::any);
    check_in_lpr_or_ppr_scope(spec_opcode::any);
    bind_lexer_pat<any_lexer_pat>();
}

void taul::internal::load_spec_interpreter::on_string(std::string_view s) {
    check_not_in_ppr_scope(spec_opcode::string);
    check_in_lpr_or_ppr_scope(spec_opcode::string);
    bind_lexer_pat<string_lexer_pat>((std::string)s);
}

void taul::internal::load_spec_interpreter::on_charset(std::string_view s) {
    check_not_in_ppr_scope(spec_opcode::charset);
    check_in_lpr_or_ppr_scope(spec_opcode::charset);
    bind_lexer_pat<charset_lexer_pat>((std::string)s);
}

void taul::internal::load_spec_interpreter::on_name(std::string_view name) {
    check_not_in_ppr_scope(spec_opcode::name);
    check_in_lpr_or_ppr_scope(spec_opcode::name);
    check_lpr_or_ppr_exists_with_name(name);
    // if name can't be found, we'll pass this *dummy* value
    std::size_t lprIndOfRef = std::size_t(-1);
    if (lprs.contains(name)) {
        const auto& lpr = lprs.at(name);
        lprIndOfRef = lpr.index;
    }
    bind_lexer_pat<name_ofLPR_forLPR_lexer_pat>(lprIndOfRef);
}

void taul::internal::load_spec_interpreter::on_sequence() {
    check_not_in_ppr_scope(spec_opcode::sequence);
    check_in_lpr_or_ppr_scope(spec_opcode::sequence);
    push_expr(ess_entry{ spec_opcode::sequence, ets_type::none, "", true });
    push_lexer_pat<sequence_lexer_pat>();
}

void taul::internal::load_spec_interpreter::on_set(bias b) {
    check_not_in_ppr_scope(spec_opcode::set);
    check_in_lpr_or_ppr_scope(spec_opcode::set);
    push_expr(ess_entry{ spec_opcode::set, ets_type::none, "", true });
    push_lexer_pat<set_lexer_pat>(b);
}

void taul::internal::load_spec_interpreter::on_modifier(std::uint16_t min, std::uint16_t max) {
    check_not_in_ppr_scope(spec_opcode::modifier);
    check_in_lpr_or_ppr_scope(spec_opcode::modifier);
    push_expr(ess_entry{ spec_opcode::modifier, ets_type::none, "", true });
    push_lexer_pat<modifier_lexer_pat>(min, max);
}

void taul::internal::load_spec_interpreter::on_assertion(polarity p) {
    check_not_in_ppr_scope(spec_opcode::assertion);
    check_in_lpr_or_ppr_scope(spec_opcode::assertion);
    push_expr(ess_entry{ spec_opcode::assertion, ets_type::none, "", true });
    push_lexer_pat<assertion_lexer_pat>(p);
}

void taul::internal::load_spec_interpreter::on_constraint(polarity p) {
    check_not_in_ppr_scope(spec_opcode::constraint);
    check_in_lpr_or_ppr_scope(spec_opcode::constraint);
    push_expr(ess_entry{ spec_opcode::constraint, ets_type::none, "", true });
    push_lexer_pat<constraint_lexer_pat>(p);
}

void taul::internal::load_spec_interpreter::on_junction() {
    check_junction_in_constraint_scope();
    check_junction_not_misplaced();
    check_junction_not_already_established();
    mark_junction();
}

void taul::internal::load_spec_interpreter::on_localend() {
    check_localend_in_constraint_scope_and_after_junction();
    bind_lexer_pat<localend_lexer_pat>();
}

