

#include "spec_load_interpreter.h"

#include "../string_and_charset.h"


taul::internal::spec_load_interpreter::spec_load_interpreter() 
    : spec_interpreter(), 
    rule_pt_trans(instruction),
    gb(rule_pt_trans) {}

void taul::internal::spec_load_interpreter::check_err_scope_not_closed() {
    for (auto it = ess.rbegin(); it != ess.rend(); it++) {
        raise(
            spec_error::scope_not_closed, 
            "{} does not have corresponding {}!", 
            it->opcode, spec_opcode::close);
    }
}

void taul::internal::spec_load_interpreter::check_err_stray_close() {
    if (!in_composite_expr()) {
        raise(
            spec_error::stray_close, 
            "{} with no corresponding composite expr scope to close!", 
            spec_opcode::close);
    }
}

void taul::internal::spec_load_interpreter::check_err_rule_name_conflict(std::string_view name) {
    if (has_lpr_or_ppr_decl(name)) {
        raise(
            spec_error::rule_name_conflict, 
            "name {} already used by another lexer/parser rule!", 
            std::string(name));
    }
}

void taul::internal::spec_load_interpreter::check_err_rule_never_declared(std::string_view name, bool lpr) {
    if (lpr) {
        if (!has_lpr_decl(name)) {
            raise(
                spec_error::rule_never_declared, 
                "lexer rule {} never declared!", 
                std::string(name));
        }
    }
    else {
        if (!has_ppr_decl(name)) {
            raise(
                spec_error::rule_never_declared, 
                "parser rule {} never declared!", 
                std::string(name));
        }
    }
}

void taul::internal::spec_load_interpreter::check_err_rule_never_defined() {
    for (const auto& I : lprs) {
        if (!has_lpr_def(I)) {
            raise(
                spec_error::rule_never_defined, 
                "lexer rule {} declared but never defined!", 
                I);
        }
    }
    for (const auto& I : pprs) {
        if (!has_ppr_def(I)) {
            raise(
                spec_error::rule_never_defined, 
                "parser rule {} declared but never defined!", 
                I);
        }
    }
}

void taul::internal::spec_load_interpreter::check_err_rule_already_defined(std::string_view name, bool lpr) {
    if (lpr) {
        if (has_lpr_def(name)) {
            raise(
                spec_error::rule_already_defined, 
                "lexer rule {} already defined!", 
                std::string(name));
        }
    }
    else {
        if (has_ppr_def(name)) {
            raise(
                spec_error::rule_already_defined, 
                "parser rule {} already defined!", 
                std::string(name));
        }
    }
}

void taul::internal::spec_load_interpreter::check_err_rule_not_found(std::string_view name) {
    if (!has_lpr_decl(name) && !has_ppr_decl(name)) {
        raise(
            spec_error::rule_not_found, 
            "no lexer/parser rule found with name {}!", 
            std::string(name));
    }
}

void taul::internal::spec_load_interpreter::check_err_rule_may_not_be_ppr(std::string_view name) {
    // if an lpr (illegally) also exists w/ name, we'll not raise an error as that
    // lpr will be considered the one *selected* for this association
    if (!has_lpr_decl(name) && has_ppr_decl(name)) {
        raise(
            spec_error::rule_may_not_be_ppr, 
            "name {} may not specify parser rule!", 
            std::string(name));
    }
}

void taul::internal::spec_load_interpreter::check_err_illegal_rule_declare(spec_opcode opcode) {
    if (!legal_to_decl()) {
        raise(
            spec_error::illegal_rule_declare,
            "{} illegal once rule defining begins!",
            opcode);
    }
}

void taul::internal::spec_load_interpreter::check_err_illegal_in_lpr_scope(spec_opcode opcode) {
    if (in_lpr()) {
        raise(
            spec_error::illegal_in_lpr_scope, 
            "{} illegal in lexer expr scope!", 
            opcode);
    }
}

void taul::internal::spec_load_interpreter::check_err_illegal_in_ppr_scope(spec_opcode opcode) {
    if (in_ppr()) {
        raise(
            spec_error::illegal_in_ppr_scope, 
            "{} illegal in parser expr scope!", 
            opcode);
    }
}

void taul::internal::spec_load_interpreter::check_err_illegal_in_no_scope(spec_opcode opcode) {
    if (!in_lpr_or_ppr()) {
        raise(
            spec_error::illegal_in_no_scope, 
            "{} illegal outside expr scope!", 
            opcode);
    }
}

void taul::internal::spec_load_interpreter::check_err_illegal_qualifier(std::string_view name, taul::qualifier qualifier) {
    if (qualifier != qualifier::none) {
        raise(
            spec_error::illegal_qualifier, 
            "parser rule {} may not have {} qualifier!", 
            std::string(name), 
            qualifier);
    }
}

void taul::internal::spec_load_interpreter::check_err_illegal_in_single_terminal_scope(spec_opcode opcode, std::string_view s) {
    if (in_single_terminal_scope()) {
        TAUL_ASSERT(in_single_subexpr_scope());
        if (opcode == spec_opcode::string && in_lpr() && s.length() == 1) return; // okay
        else if (opcode == spec_opcode::name && in_ppr() && has_lpr_decl(s)) return; // okay
        else if (opcode == spec_opcode::string && in_lpr() && s.length() > 1) {
            raise(
                spec_error::illegal_in_single_terminal_scope,
                "length of {} in single-terminal scope in lexer expr must only 1 char!",
                opcode);
        }
        else if (opcode == spec_opcode::name && in_lpr()) {
            raise(
                spec_error::illegal_in_single_terminal_scope,
                "illegal {} in single-terminal scope in lexer expr!",
                opcode);
        }
        else if (opcode == spec_opcode::name && in_ppr() && !has_lpr_decl(s)) {
            raise(
                spec_error::illegal_in_single_terminal_scope,
                "{} in single-terminal scope in parser expr must reference lpr!",
                opcode);
        }
        else {
            TAUL_ASSERT(s.empty());
            raise(
                spec_error::illegal_in_single_terminal_scope,
                "{} illegal in single-terminal scope!",
                opcode);
        }
    }
}

void taul::internal::spec_load_interpreter::check_err_illegal_in_no_alternation_scope(spec_opcode opcode) {
    if (in_no_alternation_scope()) {
        TAUL_ASSERT(!ess.empty());
        if (ess.back().alts >= 1) {
            raise(
                spec_error::illegal_in_no_alternation_scope,
                "{} illegal in no-alternation scope due to having {} alts!",
                opcode, ess.back().alts);
        }
    }
}

void taul::internal::spec_load_interpreter::check_err_illegal_in_single_subexpr_scope(spec_opcode opcode) {
    if (in_single_subexpr_scope()) {
        TAUL_ASSERT(!ess.empty());
        if (ess.back().subexprs != 1) {
            raise(
                spec_error::illegal_in_single_subexpr_scope,
                "{} illegal in single-subexpr scope due to having alt with {} subexprs!",
                opcode, ess.back().subexprs);
        }
    }
}

void taul::internal::spec_load_interpreter::check_err_illegal_in_no_end_subexpr_scope(spec_opcode opcode) {
    TAUL_ASSERT(opcode == spec_opcode::end);
    if (in_no_end_subexpr_scope()) {
        raise(
            spec_error::illegal_in_no_end_subexpr_scope,
            "{} illegal in no-end-subexpr scope!",
            opcode);
    }
}

void taul::internal::spec_load_interpreter::check_err_illegal_ambiguity() {
    for (const auto& I : rule_pt_trans.lexer_ptbd.collisions) {
        raise_at(
            rule_pt_trans.lexer_id_alloc.output_debug.query(I.nonterminal).value().inst,
            spec_error::illegal_ambiguity,
            "{} is ambiguous over the inputs {}!",
            rule_pt_trans.fetch_name_str_lxr(I.nonterminal),
            rule_pt_trans.lexer_pt.grouper.get_symbol_range(I.terminal_group));
    }
    for (const auto& I : rule_pt_trans.parser_ptbd.collisions) {
        raise_at(
            rule_pt_trans.parser_id_alloc.output_debug.query(I.nonterminal).value().inst,
            spec_error::illegal_ambiguity,
            "{} is ambiguous over the inputs {}!",
            rule_pt_trans.fetch_name_str_psr(I.nonterminal),
            rule_pt_trans.parser_pt.grouper.get_symbol_range(I.terminal_group));
    }
    check_err_illegal_ambiguity_due_to_trivial_left_recursion();
}

void taul::internal::spec_load_interpreter::check_err_illegal_ambiguity_due_to_trivial_left_recursion() {
    // the below loops expected grammar builder to have been able to build mappings, 
    // meaning we need to abort this method if it was cancelled and never did
    if (gb.cancelled) return;
    for (size_t i = 0; i < rule_pt_trans.lexer_pt.rules.size(); i++) {
        const auto& first_set_w = rule_pt_trans.lexer_ptbd.first_sets_w[i];
        const auto nonterminal = rule_pt_trans.lexer_pt.rules[i].id;
        if (first_set_w.empty() && !first_set_w.includes_epsilon()) {
            raise(
                spec_error::illegal_ambiguity,
                "{} is ambiguous due to left-recursion!", 
                nonterminal);
        }
    }
    for (size_t i = 0; i < rule_pt_trans.parser_pt.rules.size(); i++) {
        const auto& first_set_w = rule_pt_trans.parser_ptbd.first_sets_w[i];
        const auto nonterminal = rule_pt_trans.parser_pt.rules[i].id;
        if (first_set_w.empty() && !first_set_w.includes_epsilon()) {
            raise(
                spec_error::illegal_ambiguity,
                "{} is ambiguous due to left-recursion!",
                nonterminal);
        }
    }
}

void taul::internal::spec_load_interpreter::check_for_internal_errors() {
    // these are internal errors reporting that the system is broken
    // these are not represented in our unit tests
    if (rule_pt_trans.lexer_ptbd.nonterminal_id_is_terminal_id) {
        raise(
            spec_error::internal_error,
            "internal error! (rule_pt_trans.lexer_ptbd.nonterminal_id_is_terminal_id == true)");
    }
    if (rule_pt_trans.lexer_ptbd.terminal_ids_not_in_legal_range) {
        raise(
            spec_error::internal_error,
            "internal error! (rule_pt_trans.lexer_ptbd.terminal_ids_not_in_legal_range == true)");
    }
    if (rule_pt_trans.lexer_ptbd.nonterminal_not_in_rules_vector) {
        raise(
            spec_error::internal_error,
            "internal error! (rule_pt_trans.lexer_ptbd.nonterminal_not_in_rules_vector == true)");
    }
    if (rule_pt_trans.parser_ptbd.nonterminal_id_is_terminal_id) {
        raise(
            spec_error::internal_error,
            "internal error! (rule_pt_trans.parser_ptbd.nonterminal_id_is_terminal_id == true)");
    }
    if (rule_pt_trans.parser_ptbd.terminal_ids_not_in_legal_range) {
        raise(
            spec_error::internal_error,
            "internal error! (rule_pt_trans.parser_ptbd.terminal_ids_not_in_legal_range == true)");
    }
    if (rule_pt_trans.parser_ptbd.nonterminal_not_in_rules_vector) {
        raise(
            spec_error::internal_error,
            "internal error! (rule_pt_trans.parser_ptbd.nonterminal_not_in_rules_vector == true)");
    }
}

void taul::internal::spec_load_interpreter::on_startup() {
    TAUL_ASSERT(ec);
    success = true; // <- deem eval successful until proven otherwise
    instruction = 0;
    rule_pt_trans.on_startup();
}

void taul::internal::spec_load_interpreter::on_shutdown() {
    check_err_scope_not_closed();
    check_err_rule_never_defined();
    rule_pt_trans.on_shutdown();
    check_err_illegal_ambiguity();
    check_for_internal_errors();
    instruction++;
}

void taul::internal::spec_load_interpreter::on_close() {
    check_err_stray_close();
    if (in_composite_expr()) {
        check_err_illegal_in_single_subexpr_scope(ess.back().opcode);
        pop_ess();
    }
    rule_pt_trans.on_close();
    instruction++;
}

void taul::internal::spec_load_interpreter::on_alternative() {
    check_err_illegal_in_no_scope(spec_opcode::alternative);
    if (in_composite_expr()) {
        check_err_illegal_in_single_subexpr_scope(ess.back().opcode);
        next_alternative_for_top_ess();
        check_err_illegal_in_no_alternation_scope(ess.back().opcode);
    }
    rule_pt_trans.on_alternative();
    instruction++;
}

void taul::internal::spec_load_interpreter::on_lpr_decl(std::string_view name) {
    check_err_rule_name_conflict(name);
    check_err_illegal_rule_declare(spec_opcode::lpr_decl);
    check_err_illegal_in_lpr_scope(spec_opcode::lpr_decl);
    check_err_illegal_in_ppr_scope(spec_opcode::lpr_decl);
    count_subexpr_for_top_ess();
    add_lpr_decl(name);
    rule_pt_trans.on_lpr_decl(name);
    gb.add_lpr_decl(str(name));
    instruction++;
}

void taul::internal::spec_load_interpreter::on_ppr_decl(std::string_view name) {
    check_err_rule_name_conflict(name);
    check_err_illegal_rule_declare(spec_opcode::ppr_decl);
    check_err_illegal_in_lpr_scope(spec_opcode::ppr_decl);
    check_err_illegal_in_ppr_scope(spec_opcode::ppr_decl);
    count_subexpr_for_top_ess();
    add_ppr_decl(name);
    rule_pt_trans.on_ppr_decl(name);
    gb.add_ppr_decl(str(name));
    instruction++;
}

void taul::internal::spec_load_interpreter::on_lpr(std::string_view name, qualifier qualifier) {
    check_err_rule_never_declared(name, true);
    check_err_rule_already_defined(name, true);
    check_err_illegal_in_lpr_scope(spec_opcode::lpr);
    check_err_illegal_in_ppr_scope(spec_opcode::lpr);
    count_subexpr_for_top_ess();
    add_lpr_def(name);
    push_ess(ess_frame{ spec_opcode::lpr });
    push_dss(dss_frame{ name, true });
    rule_pt_trans.on_lpr(name, qualifier);
    gb.add_lpr(str(name), qualifier);
    instruction++;
}

void taul::internal::spec_load_interpreter::on_ppr(std::string_view name, qualifier qualifier) {
    check_err_rule_never_declared(name, false);
    check_err_rule_already_defined(name, false);
    check_err_illegal_in_lpr_scope(spec_opcode::ppr);
    check_err_illegal_in_ppr_scope(spec_opcode::ppr);
    check_err_illegal_qualifier(name, qualifier);
    count_subexpr_for_top_ess();
    add_ppr_def(name);
    push_ess(ess_frame{ spec_opcode::ppr });
    push_dss(dss_frame{ name, false });
    rule_pt_trans.on_ppr(name, qualifier);
    gb.add_ppr(str(name), qualifier);
    instruction++;
}

void taul::internal::spec_load_interpreter::on_end() {
    check_err_illegal_in_no_scope(spec_opcode::end);
    check_err_illegal_in_no_end_subexpr_scope(spec_opcode::end);
    count_subexpr_for_top_ess();
    rule_pt_trans.on_end();
    instruction++;
}

void taul::internal::spec_load_interpreter::on_any() {
    check_err_illegal_in_no_scope(spec_opcode::any);
    count_subexpr_for_top_ess();
    rule_pt_trans.on_any();
    instruction++;
}

void taul::internal::spec_load_interpreter::on_string(std::string_view s) {
    const auto ss = parse_taul_string(s);
    check_err_illegal_in_no_scope(spec_opcode::string);
    check_err_illegal_in_ppr_scope(spec_opcode::string);
    check_err_illegal_in_single_terminal_scope(spec_opcode::string, ss);
    count_subexpr_for_top_ess();
    rule_pt_trans.on_string(ss);
    instruction++;
}

void taul::internal::spec_load_interpreter::on_charset(std::string_view s) {
    const auto ss = parse_taul_charset(s);
    check_err_illegal_in_no_scope(spec_opcode::charset);
    check_err_illegal_in_ppr_scope(spec_opcode::charset);
    count_subexpr_for_top_ess();
    rule_pt_trans.on_charset(ss);
    instruction++;
}

void taul::internal::spec_load_interpreter::on_token() {
    check_err_illegal_in_no_scope(spec_opcode::token);
    check_err_illegal_in_lpr_scope(spec_opcode::token);
    count_subexpr_for_top_ess();
    rule_pt_trans.on_token();
    instruction++;
}

void taul::internal::spec_load_interpreter::on_failure() {
    check_err_illegal_in_no_scope(spec_opcode::failure);
    check_err_illegal_in_lpr_scope(spec_opcode::failure);
    count_subexpr_for_top_ess();
    rule_pt_trans.on_failure();
    instruction++;
}

void taul::internal::spec_load_interpreter::on_name(std::string_view name) {
    check_err_illegal_in_no_scope(spec_opcode::name);
    if (in_lpr()) {
        check_err_rule_may_not_be_ppr(name);
    }
    check_err_rule_not_found(name);
    check_err_illegal_in_single_terminal_scope(spec_opcode::name, name);
    count_subexpr_for_top_ess();
    rule_pt_trans.on_name(name);
    instruction++;
}

void taul::internal::spec_load_interpreter::on_sequence() {
    check_err_illegal_in_no_scope(spec_opcode::sequence);
    //check_err_illegal_in_single_terminal_scope(spec_opcode::sequence);
    count_subexpr_for_top_ess();
    // query scopes before ess push
    const bool single_terminal_scope = in_single_terminal_scope();
    const bool no_end_subexpr_scope = in_no_end_subexpr_scope();
    push_ess(ess_frame{ spec_opcode::sequence });
    if (single_terminal_scope) mark_single_terminal_scope();
    if (no_end_subexpr_scope) mark_no_end_subexpr_scope();
    rule_pt_trans.on_sequence();
    instruction++;
}

void taul::internal::spec_load_interpreter::on_lookahead() {
    check_err_illegal_in_no_scope(spec_opcode::lookahead);
    check_err_illegal_in_single_terminal_scope(spec_opcode::lookahead);
    count_subexpr_for_top_ess();
    push_ess(ess_frame{ spec_opcode::lookahead });
    mark_single_terminal_scope();
    mark_no_end_subexpr_scope();
    rule_pt_trans.on_lookahead();
    instruction++;
}

void taul::internal::spec_load_interpreter::on_lookahead_not() {
    check_err_illegal_in_no_scope(spec_opcode::lookahead_not);
    check_err_illegal_in_single_terminal_scope(spec_opcode::lookahead_not);
    count_subexpr_for_top_ess();
    push_ess(ess_frame{ spec_opcode::lookahead_not });
    mark_single_terminal_scope();
    mark_no_end_subexpr_scope();
    rule_pt_trans.on_lookahead_not();
    instruction++;
}

void taul::internal::spec_load_interpreter::on_not() {
    check_err_illegal_in_no_scope(spec_opcode::not0);
    check_err_illegal_in_single_terminal_scope(spec_opcode::not0);
    count_subexpr_for_top_ess();
    push_ess(ess_frame{ spec_opcode::not0 });
    mark_single_terminal_scope();
    mark_no_end_subexpr_scope();
    rule_pt_trans.on_not();
    instruction++;
}

void taul::internal::spec_load_interpreter::on_optional() {
    check_err_illegal_in_no_scope(spec_opcode::optional);
    check_err_illegal_in_single_terminal_scope(spec_opcode::optional);
    count_subexpr_for_top_ess();
    push_ess(ess_frame{ spec_opcode::optional });
    mark_no_alternation_scope();
    mark_single_subexpr_scope();
    rule_pt_trans.on_optional();
    instruction++;
}

void taul::internal::spec_load_interpreter::on_kleene_star() {
    check_err_illegal_in_no_scope(spec_opcode::kleene_star);
    check_err_illegal_in_single_terminal_scope(spec_opcode::kleene_star);
    count_subexpr_for_top_ess();
    push_ess(ess_frame{ spec_opcode::kleene_star });
    mark_no_alternation_scope();
    mark_single_subexpr_scope();
    rule_pt_trans.on_kleene_star();
    instruction++;
}

void taul::internal::spec_load_interpreter::on_kleene_plus() {
    check_err_illegal_in_no_scope(spec_opcode::kleene_plus);
    check_err_illegal_in_single_terminal_scope(spec_opcode::kleene_plus);
    count_subexpr_for_top_ess();
    push_ess(ess_frame{ spec_opcode::kleene_plus });
    mark_no_alternation_scope();
    mark_single_subexpr_scope();
    rule_pt_trans.on_kleene_plus();
    instruction++;
}

