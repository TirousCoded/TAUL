

#include "load.h"

#include "asserts.h"
#include "string_and_charset.h"
#include "compile.h"


std::optional<taul::grammar> taul::load(
    const taul::spec& s, 
    const std::shared_ptr<logger>& lgr) {
    spec_error_counter ec{};
    return load(s, ec, lgr);
}

std::optional<taul::grammar> taul::load(
    const taul::spec& s, 
    taul::spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    internal::load_spec_interpreter interp{};
    interp.ec = &ec;
    interp.lgr = lgr;
    interp.interpret(s);
    if (interp.success) {
        auto data = std::make_shared<internal::grammar_data>(internal::grammar_data{
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
        data->build_gwls();
        // TODO: maybe put these into some 'finish_up_lprs' and 'finish_up_pprs' methods
        // for each lpr we gotta do two things right at the end:
        //      1) hook up the gramdat ptr of the toplevel_lexer_pat of the lpr
        //      2) hook up its fnobj
        for (auto& I : data->_lprs) {
            TAUL_ASSERT(interp.lexer_pat_map.contains(I.name));
            static_cast<internal::toplevel_lexer_pat*>(interp.lexer_pat_map.at(I.name).get())->gramdat = data.get();
            I.fnobj = taul::internal::lexer(internal::pat_lexer_fn, interp.lexer_pat_map.at(I.name));
        }
        // for each ppr we gotta do two things right at the end:
        //      1) hook up the gramdat ptr of the toplevel_parser_pat of the ppr
        //      2) hook up its fnobj
        for (auto& I : data->_pprs) {
            TAUL_ASSERT(interp.parser_pat_map.contains(I.name));
            static_cast<internal::toplevel_parser_pat*>(interp.parser_pat_map.at(I.name).get())->gramdat = data.get();
            I.fnobj = taul::internal::parser(internal::pat_parser_fn, interp.parser_pat_map.at(I.name));
        }
        return std::make_optional<taul::grammar>(internal::for_internal_use_tag{}, std::move(data));
    }
    else return std::nullopt;
}

std::optional<taul::grammar> taul::load(
    node_ctx& ctx, 
    const std::shared_ptr<source_code>& src, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    const auto compiled = compile(ctx, src, ec, lgr);
    return 
        compiled 
        ? load(*compiled, ec, lgr) 
        : std::nullopt;
}

std::optional<taul::grammar> taul::load(
    node_ctx& ctx, 
    const std::shared_ptr<source_code>& src, 
    const std::shared_ptr<logger>& lgr) {
    const auto compiled = compile(ctx, src, lgr);
    return
        compiled
        ? load(*compiled, lgr)
        : std::nullopt;
}

std::optional<taul::grammar> taul::load(
    const std::shared_ptr<source_code>& src, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    const auto compiled = compile(src, ec, lgr);
    return
        compiled
        ? load(*compiled, ec, lgr)
        : std::nullopt;
}

std::optional<taul::grammar> taul::load(
    const std::shared_ptr<source_code>& src, 
    const std::shared_ptr<logger>& lgr) {
    const auto compiled = compile(src, lgr);
    return
        compiled
        ? load(*compiled, lgr)
        : std::nullopt;
}

std::optional<taul::grammar> taul::load(
    node_ctx& ctx, 
    const std::string& src, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    const auto compiled = compile(ctx, src, ec, lgr);
    return
        compiled
        ? load(*compiled, ec, lgr)
        : std::nullopt;
}

std::optional<taul::grammar> taul::load(
    node_ctx& ctx, 
    const std::string& src, 
    const std::shared_ptr<logger>& lgr) {
    const auto compiled = compile(ctx, src, lgr);
    return
        compiled
        ? load(*compiled, lgr)
        : std::nullopt;
}

std::optional<taul::grammar> taul::load(
    const std::string& src, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    const auto compiled = compile(src, lgr);
    return
        compiled
        ? load(*compiled, lgr)
        : std::nullopt;
}

std::optional<taul::grammar> taul::load(
    const std::string& src, 
    const std::shared_ptr<logger>& lgr) {
    const auto compiled = compile(src, lgr);
    return
        compiled
        ? load(*compiled, lgr)
        : std::nullopt;
}

std::optional<taul::grammar> taul::load(
    node_ctx& ctx, 
    const std::filesystem::path& src_path, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    const auto compiled = compile(ctx, src_path, ec, lgr);
    return
        compiled
        ? load(*compiled, ec, lgr)
        : std::nullopt;
}

std::optional<taul::grammar> taul::load(
    node_ctx& ctx, 
    const std::filesystem::path& src_path, 
    const std::shared_ptr<logger>& lgr) {
    const auto compiled = compile(ctx, src_path, lgr);
    return
        compiled
        ? load(*compiled, lgr)
        : std::nullopt;
}

std::optional<taul::grammar> taul::load(
    const std::filesystem::path& src_path, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    const auto compiled = compile(src_path, lgr);
    return
        compiled
        ? load(*compiled, lgr)
        : std::nullopt;
}

std::optional<taul::grammar> taul::load(
    const std::filesystem::path& src_path, 
    const std::shared_ptr<logger>& lgr) {
    const auto compiled = compile(src_path, lgr);
    return
        compiled
        ? load(*compiled, lgr)
        : std::nullopt;
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

void taul::internal::load_spec_interpreter::check_rule_is_not_ppr(std::string_view name) {
    // if an lpr (illegally) also exists w/ name, we'll not raise an error as that
    // lpr will be considered the one *selected* for this association
    if (!has_lpr_decl(name) && has_ppr_decl(name)) {
        raise(spec_error::rule_may_not_be_ppr, "name {} may not specify parser rule!", std::string(name));
    }
}

void taul::internal::load_spec_interpreter::check_lpr_or_ppr_exists_with_name(std::string_view name) {
    if (!has_lpr_decl(name) && !has_ppr_decl(name)) {
        raise(spec_error::rule_not_found, "no lexer/parser rule found with name {}!", (std::string)name);
    }
}

void taul::internal::load_spec_interpreter::check_ppr_has_no_qualifier(std::string_view name, qualifier qualifier) {
    if (qualifier != qualifier::none) {
        raise(spec_error::illegal_qualifier, "parser rule {} may not have {} qualifier!", (std::string)name, qualifier);
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
    std::size_t subexprs{};
    if (in_lpr()) {
        TAUL_ASSERT(!lexer_pats.empty());
        subexprs = lexer_pats.back()->children.size();
    }
    if (in_ppr()) {
        TAUL_ASSERT(!parser_pats.empty());
        subexprs = parser_pats.back()->children.size();
    }
    if (subexprs != n) {
        raise(spec_error::illegal_subexpr_count, "{} has {} subexprs, but must have exactly {}!", opcode, subexprs, n);
    }
}

void taul::internal::load_spec_interpreter::check_not_in_single_terminal_scope(spec_opcode opcode) {
    if (in_single_terminal_scope()) {
        raise(spec_error::illegal_in_single_terminal_scope, "{} illegal in single-terminal scope!", opcode);
    }
}

void taul::internal::load_spec_interpreter::check_string_in_single_terminal_scope_in_lpr_has_only_one_char(std::string_view s) {
    if (in_single_terminal_scope() && in_lpr() && s.length() > 1) {
        raise(spec_error::illegal_in_single_terminal_scope, "length of {} in single-terminal scope in lexer expr may not exceed 1 char!", spec_opcode::string);
    }
}

void taul::internal::load_spec_interpreter::on_startup() {
    TAUL_ASSERT(ec);
    success = true; // <- deem eval successful until proven otherwise
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

void taul::internal::load_spec_interpreter::on_close() {
    check_scope_exists_to_close();
    check_subexpr_count_legal(spec_opcode::lookahead, 1);
    check_subexpr_count_legal(spec_opcode::lookahead_not, 1);
    check_subexpr_count_legal(spec_opcode::not0, 1);
    check_subexpr_count_legal(spec_opcode::optional, 1);
    check_subexpr_count_legal(spec_opcode::kleene_star, 1);
    check_subexpr_count_legal(spec_opcode::kleene_plus, 1);
    handle_single_terminal_scope_for_top_ess();
    handle_pop_lexer_pat_for_top_ess();
    handle_pop_parser_pat_for_top_ess();
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

void taul::internal::load_spec_interpreter::on_lpr(std::string_view name, qualifier qualifier) {
    check_lpr_was_declared(name);
    check_lpr_not_already_defined(name);
    check_not_in_lpr_nor_ppr_scope(spec_opcode::lpr);
    add_lpr_def(name, qualifier);
    ess_entry entry{ spec_opcode::lpr, ets_type::lexer, name, true, false };
    if (lprs.contains(name)) {
        entry.index = lprs.at(name).index;
    }
    push_expr(entry);
    push_lexer_pat<toplevel_lexer_pat>(entry.index);
}

void taul::internal::load_spec_interpreter::on_ppr(std::string_view name, qualifier qualifier) {
    check_ppr_was_declared(name);
    check_ppr_not_already_defined(name);
    check_not_in_lpr_nor_ppr_scope(spec_opcode::ppr);
    check_ppr_has_no_qualifier(name, qualifier);
    add_ppr_def(name);
    ess_entry entry{ spec_opcode::ppr, ets_type::parser, name, false, true };
    if (pprs.contains(name)) {
        entry.index = pprs.at(name).index;
    }
    push_expr(std::move(entry));
    push_parser_pat<toplevel_parser_pat>(entry.index);
}

void taul::internal::load_spec_interpreter::on_end() {
    check_in_lpr_or_ppr_scope(spec_opcode::end);
    if (in_lpr()) bind_lexer_pat<end_lexer_pat>();
    if (in_ppr()) bind_parser_pat<end_parser_pat>();
}

void taul::internal::load_spec_interpreter::on_any() {
    check_in_lpr_or_ppr_scope(spec_opcode::any);
    if (in_lpr()) bind_lexer_pat<any_lexer_pat>();
    if (in_ppr()) bind_parser_pat<any_parser_pat>();
}

void taul::internal::load_spec_interpreter::on_string(std::string_view s) {
    check_in_lpr_or_ppr_scope(spec_opcode::string);
    check_string_in_single_terminal_scope_in_lpr_has_only_one_char(s);
    const auto ss = parse_taul_string(s);
    if (in_lpr()) bind_lexer_pat<string_lexer_pat>(ss);
    if (in_ppr()) bind_parser_pat<string_parser_pat>(ss);
}

void taul::internal::load_spec_interpreter::on_charset(std::string_view s) {
    check_not_in_ppr_scope(spec_opcode::charset);
    check_in_lpr_or_ppr_scope(spec_opcode::charset);
    const auto ss0 = parse_taul_charset(s);
    const auto ss1 = optimize_charset_str(ss0);
    if (in_lpr()) bind_lexer_pat<charset_lexer_pat>(ss1);
    if (in_ppr()) (void)0;
}

void taul::internal::load_spec_interpreter::on_token() {
    check_not_in_lpr_scope(spec_opcode::token);
    check_in_lpr_or_ppr_scope(spec_opcode::token);
    if (in_lpr()) (void)0;
    if (in_ppr()) bind_parser_pat<token_parser_pat>();
}

void taul::internal::load_spec_interpreter::on_failure() {
    check_not_in_lpr_scope(spec_opcode::failure);
    check_in_lpr_or_ppr_scope(spec_opcode::failure);
    if (in_lpr()) (void)0;
    if (in_ppr()) bind_parser_pat<failure_parser_pat>();
}

void taul::internal::load_spec_interpreter::on_name(std::string_view name) {
    check_in_lpr_or_ppr_scope(spec_opcode::name);
    check_lpr_or_ppr_exists_with_name(name);
    check_not_in_single_terminal_scope(spec_opcode::name);
    if (in_lpr()) {
        check_rule_is_not_ppr(name);
        const std::size_t lprIndOfRef =
            lprs.contains(name)
            ? lprs.at(name).index
            : std::size_t(-1); // if name can't be found, pass *dummy* value
        bind_lexer_pat<name_lexer_pat>(lprIndOfRef);
    }
    if (in_ppr()) {
        bool lpr = lprs.contains(name);
        std::size_t ruleIndOfRef{};
        if (lpr) {
            ruleIndOfRef = lprs.at(name).index;
        }
        else {
            ruleIndOfRef =
                pprs.contains(name)
                ? pprs.at(name).index
                : std::size_t(-1); // if name can't be found, pass *dummy* value
        }
        bind_parser_pat<name_parser_pat>(lpr, ruleIndOfRef);
    }
}

void taul::internal::load_spec_interpreter::on_sequence() {
    check_in_lpr_or_ppr_scope(spec_opcode::sequence);
    check_not_in_single_terminal_scope(spec_opcode::sequence);
    push_expr(ess_entry{ spec_opcode::sequence, ets_type::none, "", in_lpr(), in_ppr(), false });
    if (in_lpr()) push_lexer_pat<sequence_lexer_pat>();
    if (in_ppr()) push_parser_pat<sequence_parser_pat>();
}

void taul::internal::load_spec_interpreter::on_set() {
    check_in_lpr_or_ppr_scope(spec_opcode::set);
    push_expr(ess_entry{ spec_opcode::set, ets_type::none, "", in_lpr(), in_ppr(), false });
    if (in_lpr()) push_lexer_pat<set_lexer_pat>();
    if (in_ppr()) push_parser_pat<set_parser_pat>();
}

void taul::internal::load_spec_interpreter::on_lookahead() {
    check_in_lpr_or_ppr_scope(spec_opcode::lookahead);
    check_not_in_single_terminal_scope(spec_opcode::lookahead);
    push_expr(ess_entry{ spec_opcode::lookahead, ets_type::none, "", in_lpr(), in_ppr(), true });
    push_single_terminal_scope();
    if (in_lpr()) push_lexer_pat<assertion_lexer_pat>(true);
    if (in_ppr()) push_parser_pat<assertion_parser_pat>(true);
}

void taul::internal::load_spec_interpreter::on_lookahead_not() {
    check_in_lpr_or_ppr_scope(spec_opcode::lookahead_not);
    check_not_in_single_terminal_scope(spec_opcode::lookahead_not);
    push_expr(ess_entry{ spec_opcode::lookahead_not, ets_type::none, "", in_lpr(), in_ppr(), true });
    push_single_terminal_scope();
    if (in_lpr()) push_lexer_pat<assertion_lexer_pat>(false);
    if (in_ppr()) push_parser_pat<assertion_parser_pat>(false);
}

void taul::internal::load_spec_interpreter::on_not() {
    check_in_lpr_or_ppr_scope(spec_opcode::not0);
    check_not_in_single_terminal_scope(spec_opcode::not0);
    push_expr(ess_entry{ spec_opcode::not0, ets_type::none, "", in_lpr(), in_ppr(), true });
    push_single_terminal_scope();
    if (in_lpr()) push_lexer_pat<not_lexer_pat>();
    if (in_ppr()) push_parser_pat<not_parser_pat>();
}

void taul::internal::load_spec_interpreter::on_optional() {
    check_in_lpr_or_ppr_scope(spec_opcode::optional);
    check_not_in_single_terminal_scope(spec_opcode::optional);
    push_expr(ess_entry{ spec_opcode::optional, ets_type::none, "", in_lpr(), in_ppr(), false });
    if (in_lpr()) push_lexer_pat<modifier_lexer_pat>(0, 1);
    if (in_ppr()) push_parser_pat<modifier_parser_pat>(0, 1);
}

void taul::internal::load_spec_interpreter::on_kleene_star() {
    check_in_lpr_or_ppr_scope(spec_opcode::kleene_star);
    check_not_in_single_terminal_scope(spec_opcode::kleene_star);
    push_expr(ess_entry{ spec_opcode::kleene_star, ets_type::none, "", in_lpr(), in_ppr(), false });
    if (in_lpr()) push_lexer_pat<modifier_lexer_pat>(0, 0);
    if (in_ppr()) push_parser_pat<modifier_parser_pat>(0, 0);
}

void taul::internal::load_spec_interpreter::on_kleene_plus() {
    check_in_lpr_or_ppr_scope(spec_opcode::kleene_plus);
    check_not_in_single_terminal_scope(spec_opcode::kleene_plus);
    push_expr(ess_entry{ spec_opcode::kleene_plus, ets_type::none, "", in_lpr(), in_ppr(), false });
    if (in_lpr()) push_lexer_pat<modifier_lexer_pat>(1, 0);
    if (in_ppr()) push_parser_pat<modifier_parser_pat>(1, 0);
}

