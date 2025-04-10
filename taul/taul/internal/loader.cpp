

#include "loader.h"

#include "../string_and_charset.h"


#define _DUMP_LOWERED_LLSPEC 0


#if _DUMP_LOWERED_LLSPEC
#include "../disassemble_spec.h"
#endif


std::optional<std::string_view> taul::internal::loader::lpr_name_by_id(symbol_id id) const noexcept {
    if (!is_lpr_id(id)) return std::nullopt;
    const size_t index = size_t(id - TAUL_FIRST_ID(lpr));
    return
        index < lpr_names.size()
        ? std::make_optional(lpr_names[index])
        : std::nullopt;
}

std::optional<std::string_view> taul::internal::loader::ppr_name_by_id(symbol_id id) const noexcept {
    if (!is_ppr_id(id)) return std::nullopt;
    const size_t index = size_t(id - TAUL_FIRST_ID(ppr));
    return
        index < ppr_names.size()
        ? std::make_optional(ppr_names[index])
        : std::nullopt;
}

std::string taul::internal::loader::fmt_symbol_id_ext(symbol_id x) const {
    const auto lpr_nm = lpr_name_by_id(x);
    const auto ppr_nm = ppr_name_by_id(x);
    if (lpr_nm)             return std::format("{} ({})", x, std::string(*lpr_nm));
    else if (ppr_nm)        return std::format("{} ({})", x, std::string(*ppr_nm));
    else                    return fmt_symbol_id(x);
}

taul::internal::loader::loader(std::shared_ptr<source_code> src, spec_error_counter& ec, std::shared_ptr<logger> lgr)
    : src(src),
    ec(&ec),
    lgr(lgr),
    rule_pt_trans(pos),
    gb(rule_pt_trans),
    sl(pos) {}

std::optional<taul::grammar> taul::internal::loader::load(const spec& s) {
    spec_stage stage1(*this);
    stage1.interpret(s);
    const auto result1 = stage1.result();
    if (!result1) return std::nullopt;
#if _DUMP_LOWERED_LLSPEC
    TAUL_LOG(make_stderr_logger(), "dumping llspec...\n{}", disassemble_llspec(result1.value()));
#endif
    llspec_stage stage2(*this);
    stage2.interpret(result1.value());
    return stage2.result();
}

taul::internal::loader::spec_stage::spec_stage(loader& owner)
    : spec_interpreter(),
    owner_ptr(&owner) {}

std::optional<taul::internal::llspec> taul::internal::loader::spec_stage::result() {
    return
        owner().success
        ? std::make_optional(owner().sl.output.done())
        : std::nullopt;
}

void taul::internal::loader::spec_stage::update_pos() {
    owner().pos = pos();
}

void taul::internal::loader::spec_stage::next_is_legal_right_assoc(std::optional<spec_opcode> peek) noexcept {
    next_right_assoc_is_legal_flag =
        peek == spec_opcode::right_assoc &&
        in_precedence_scope();
}

void taul::internal::loader::spec_stage::clear_next_right_assoc_is_legal_flag() noexcept {
    next_right_assoc_is_legal_flag = false;
}

void taul::internal::loader::spec_stage::check_err_scope_not_closed() {
    for (auto it = ess.rbegin(); it != ess.rend(); it++) {
        owner().raise(
            spec_error::scope_not_closed,
            "{} does not have corresponding {}!",
            it->opcode, spec_opcode::close);
    }
}

void taul::internal::loader::spec_stage::check_err_stray_close() {
    if (!in_composite_expr()) {
        owner().raise(
            spec_error::stray_close,
            "{} with no corresponding composite expr scope to close!",
            spec_opcode::close);
    }
}

void taul::internal::loader::spec_stage::check_err_rule_name_conflict(std::string_view name) {
    if (has_lpr_or_ppr_decl(name)) {
        owner().raise(
            spec_error::rule_name_conflict,
            "name {} already used by another lexer/parser rule!",
            std::string(name));
    }
}

void taul::internal::loader::spec_stage::check_err_rule_never_declared(std::string_view name, bool lpr) {
    if (lpr) {
        if (!has_lpr_decl(name)) {
            owner().raise(
                spec_error::rule_never_declared,
                "lexer rule {} never declared!",
                std::string(name));
        }
    }
    else {
        if (!has_ppr_decl(name)) {
            owner().raise(
                spec_error::rule_never_declared,
                "parser rule {} never declared!",
                std::string(name));
        }
    }
}

void taul::internal::loader::spec_stage::check_err_rule_never_defined() {
    for (const auto& I : lprs) {
        if (!has_lpr_def(I)) {
            owner().raise(
                spec_error::rule_never_defined,
                "lexer rule {} declared but never defined!",
                I);
        }
    }
    for (const auto& I : pprs) {
        if (!has_ppr_def(I)) {
            owner().raise(
                spec_error::rule_never_defined,
                "parser rule {} declared but never defined!",
                I);
        }
    }
}

void taul::internal::loader::spec_stage::check_err_rule_already_defined(std::string_view name, bool lpr) {
    if (lpr) {
        if (has_lpr_def(name)) {
            owner().raise(
                spec_error::rule_already_defined,
                "lexer rule {} already defined!",
                std::string(name));
        }
    }
    else {
        if (has_ppr_def(name)) {
            owner().raise(
                spec_error::rule_already_defined,
                "parser rule {} already defined!",
                std::string(name));
        }
    }
}

void taul::internal::loader::spec_stage::check_err_rule_not_found(std::string_view name) {
    if (!has_lpr_decl(name) && !has_ppr_decl(name)) {
        owner().raise(
            spec_error::rule_not_found,
            "no lexer/parser rule found with name {}!",
            std::string(name));
    }
}

void taul::internal::loader::spec_stage::check_err_rule_may_not_be_ppr(std::string_view name) {
    // if an lpr (illegally) also exists w/ name, we'll not raise an error as that
    // lpr will be considered the one *selected* for this association
    if (!has_lpr_decl(name) && has_ppr_decl(name)) {
        owner().raise(
            spec_error::rule_may_not_be_ppr,
            "name {} may not specify parser rule!",
            std::string(name));
    }
}

void taul::internal::loader::spec_stage::check_err_illegal_rule_declare(spec_opcode opcode) {
    if (!legal_to_decl()) {
        owner().raise(
            spec_error::illegal_rule_declare,
            "{} illegal once rule defining begins!",
            opcode);
    }
}

void taul::internal::loader::spec_stage::check_err_illegal_in_lpr_scope(spec_opcode opcode) {
    if (in_lpr()) {
        owner().raise(
            spec_error::illegal_in_lpr_scope,
            "{} illegal in lexer rule!",
            opcode);
    }
}

void taul::internal::loader::spec_stage::check_err_illegal_in_ppr_scope(spec_opcode opcode) {
    if (in_ppr()) {
        owner().raise(
            spec_error::illegal_in_ppr_scope,
            "{} illegal in parser rule!",
            opcode);
    }
}

void taul::internal::loader::spec_stage::check_err_illegal_in_no_scope(spec_opcode opcode) {
    if (!in_lpr_or_ppr()) {
        owner().raise(
            spec_error::illegal_in_no_scope,
            "{} illegal outside a production rule!",
            opcode);
    }
}

void taul::internal::loader::spec_stage::check_err_illegal_qualifier(std::string_view name, taul::qualifier qualifier, bool lpr_not_ppr) {
    if (lpr_not_ppr) {
        if (qualifier == qualifier::none) return;
        if (qualifier == qualifier::skip) return;
        if (qualifier == qualifier::support) return;
        owner().raise(
            spec_error::illegal_qualifier,
            "lexer rule {} may not have {} qualifier!",
            std::string(name),
            qualifier);
    }
    else {
        if (qualifier == qualifier::none) return;
        if (qualifier == qualifier::precedence) return;
        owner().raise(
            spec_error::illegal_qualifier,
            "parser rule {} may not have {} qualifier!",
            std::string(name),
            qualifier);
    }
}

void taul::internal::loader::spec_stage::check_err_illegal_in_single_terminal_scope(spec_opcode opcode, std::string_view s) {
    TAUL_ASSERT(opcode != spec_opcode::string); // <- use std::u32string version
    if (in_single_terminal_scope()) {
        TAUL_ASSERT(in_single_subexpr_scope());
        if (opcode == spec_opcode::name && in_ppr() && has_lpr_decl(s)) return; // okay
        else if (opcode == spec_opcode::name && in_lpr()) {
            owner().raise(
                spec_error::illegal_in_single_terminal_scope,
                "{} is illegal here; in lexer exprs, name exprs is illegal within lookahead/lookahead-not/not exprs!",
                opcode);
        }
        else if (opcode == spec_opcode::name && in_ppr() && !has_lpr_decl(s)) {
            owner().raise(
                spec_error::illegal_in_single_terminal_scope,
                "{} is illegal here; in parser, name exprs within lookahead/lookahead-not/not expr must reference lexer rule!",
                opcode);
        }
        else {
            TAUL_ASSERT(s.empty());
            owner().raise(
                spec_error::illegal_in_single_terminal_scope,
                "{} is illegal within lookahead/lookahead-not/not exprs!",
                opcode);
        }
    }
}

void taul::internal::loader::spec_stage::check_err_illegal_in_single_terminal_scope(spec_opcode opcode, std::u32string_view s) {
    TAUL_ASSERT(opcode == spec_opcode::string); // <- use std::string version
    if (in_single_terminal_scope()) {
        TAUL_ASSERT(in_single_subexpr_scope());
        if (opcode == spec_opcode::string && in_lpr() && s.length() == 1) return; // okay
        else if (opcode == spec_opcode::string && in_lpr() && s.length() > 1) {
            owner().raise(
                spec_error::illegal_in_single_terminal_scope,
                "{} with length {} is illegal here; only length 1 strings are allowed!",
                opcode, s.length());
        }
        else {
            TAUL_ASSERT(s.empty());
            owner().raise(
                spec_error::illegal_in_single_terminal_scope,
                "{} is illegal within lookahead/lookahead-not/not exprs!",
                opcode);
        }
    }
}

void taul::internal::loader::spec_stage::check_err_illegal_in_no_alternation_scope(spec_opcode opcode) {
    if (in_no_alternation_scope()) {
        TAUL_ASSERT(!ess.empty());
        if (ess.back().alts >= 1) {
            owner().raise(
                spec_error::illegal_in_no_alternation_scope,
                "{} with {} alts is illegal here; no alternation is allowed!",
                opcode, ess.back().alts);
        }
    }
}

void taul::internal::loader::spec_stage::check_err_illegal_in_single_subexpr_scope(spec_opcode opcode) {
    if (in_single_subexpr_scope()) {
        TAUL_ASSERT(!ess.empty());
        if (ess.back().subexprs != 1) {
            owner().raise(
                spec_error::illegal_in_single_subexpr_scope,
                "{} expr with {} subexprs is illegal here; only 1 subexpr is allowed!",
                opcode, ess.back().subexprs);
        }
    }
}

void taul::internal::loader::spec_stage::check_err_illegal_in_no_end_subexpr_scope(spec_opcode opcode) {
    TAUL_ASSERT(opcode == spec_opcode::end);
    if (in_no_end_subexpr_scope()) {
        owner().raise(
            spec_error::illegal_in_no_end_subexpr_scope,
            "{} expr is illegal here!",
            opcode);
    }
}

void taul::internal::loader::spec_stage::check_err_illegal_string_literal_due_to_illegal_codepoints(std::u32string_view s) {
    for (const auto& I : s) {
        if (!is_unicode(I)) {
            owner().raise(
                spec_error::illegal_string_literal,
                "string literal '{}' contains illegal codepoint {}!",
                fmt_taul_string(s), fmt_unicode(I));
        }
    }
}

void taul::internal::loader::spec_stage::check_err_illegal_string_literal_due_to_nonvisible_ascii(std::string_view unparsed_literal) {
    decoder<char> decoder(utf8, unparsed_literal);
    while (!decoder.done()) {
        const auto decoded = decoder.next();
        TAUL_ASSERT(decoded);
        const auto cp = decoded.value().cp;
        if (is_ascii(cp) && !is_visible_ascii(cp)) {
            owner().raise(
                spec_error::illegal_string_literal,
                "string literal '{}' contains unescaped non-visible ASCII {}!",
                fmt_taul_string(parse_taul_string(unparsed_literal)), // <- help end-user better visualize what's wrong
                fmt_unicode(cp));
        }
    }
}

void taul::internal::loader::spec_stage::check_err_illegal_charset_literal_due_to_illegal_codepoints(std::u32string_view s) {
    auto legal_codepoints =
        glyph_set()
        .add_all()
        .remove_epsilon()
        .remove_range(0xd800, 0xdfff)
        .remove_id_range(cp_id(0x0011'0000), TAUL_LAST_ID(cp));

    TAUL_ASSERT(s.length() % 2 == 0);
    glyph_set charset_codepoints;
    for (size_t i = 0; i < s.length(); i += 2) {
        charset_codepoints.add_range(s[i], s[i + 1]);
    }

    auto illegal_charset_codepoints = charset_codepoints - legal_codepoints;

    if (illegal_charset_codepoints.empty()) return;

    for (const auto& I : illegal_charset_codepoints.ranges()) {
        if (I.count() == 1) {
            owner().raise(
                spec_error::illegal_charset_literal,
                "charset literal [{}] contains illegal codepoint {}!",
                fmt_taul_charset(s),
                fmt_unicode(symbol_traits<glyph>::preferred(I.low).value()));
        }
        else {
            owner().raise(
                spec_error::illegal_charset_literal,
                "charset literal [{}] contains illegal codepoints [{}, {}]!",
                fmt_taul_charset(s),
                fmt_unicode(symbol_traits<glyph>::preferred(I.low).value()),
                fmt_unicode(symbol_traits<glyph>::preferred(I.high).value()));
        }
    }
}

void taul::internal::loader::spec_stage::check_err_illegal_charset_literal_due_to_nonvisible_ascii(std::string_view unparsed_literal) {
    // take note that we're iterating over unparsed_literal as just
    // a regular string, rather than considering details of it being
    // a charset w/ syntactic rules
    decoder<char> decoder(utf8, unparsed_literal);
    while (!decoder.done()) {
        const auto decoded = decoder.next();
        TAUL_ASSERT(decoded);
        const auto cp = decoded.value().cp;
        if (is_ascii(cp) && !is_visible_ascii(cp)) {
            owner().raise(
                spec_error::illegal_charset_literal,
                "charset literal [{}] contains unescaped non-visible ASCII {}!",
                // take note how we use the string versions here, rather than charset versions,
                // as we don't care about charset details much, just aesthetics of diagnostics
                fmt_taul_string(parse_taul_string(unparsed_literal)), // <- help end-user better visualize what's wrong
                fmt_unicode(cp));
        }
    }
}

void taul::internal::loader::spec_stage::check_err_illegal_ambiguity_due_to_only_having_recurse_alts(spec_opcode opcode) {
    if (in_precedence_scope()) {
        TAUL_ASSERT(ess.back().alts >= ess.back().recurse_alts);
        const auto base_alts = ess.back().alts - ess.back().recurse_alts;
        if (base_alts >= 1) return;
        owner().raise(
            spec_error::illegal_ambiguity,
            "{} contains only recurse alts!",
            opcode);
    }
}

void taul::internal::loader::spec_stage::check_err_illegal_right_assoc(spec_opcode opcode) {
    TAUL_ASSERT(opcode == spec_opcode::right_assoc);
    if (next_right_assoc_is_legal_flag) return;
    owner().raise(
        spec_error::illegal_right_assoc,
        "illegal right associativity specifier!");
}

void taul::internal::loader::spec_stage::on_startup() {
    TAUL_ASSERT(owner().ec);
    owner().success = true; // <- deem eval successful until proven otherwise
    owner().pos = 0;
    owner().sl.on_startup();
}

void taul::internal::loader::spec_stage::on_shutdown() {
    check_err_scope_not_closed();
    check_err_rule_never_defined();
    owner().sl.on_shutdown();
}

void taul::internal::loader::spec_stage::on_close() {
    update_pos();
    check_err_stray_close();
    if (in_composite_expr()) {
        check_err_illegal_in_single_subexpr_scope(ess.back().opcode);
        check_err_illegal_ambiguity_due_to_only_having_recurse_alts(ess.back().opcode);
        pop_ess();
    }
    owner().sl.on_close();
}

void taul::internal::loader::spec_stage::on_alternative() {
    update_pos();
    check_err_illegal_in_no_scope(spec_opcode::alternative);
    if (in_composite_expr()) {
        check_err_illegal_in_single_subexpr_scope(ess.back().opcode);
        next_alternative_for_top_ess();
        check_err_illegal_in_no_alternation_scope(ess.back().opcode);
    }
    next_is_legal_right_assoc(peek());
    owner().sl.on_alternative();
}

void taul::internal::loader::spec_stage::on_right_assoc() {
    update_pos();
    check_err_illegal_in_no_scope(spec_opcode::right_assoc);
    check_err_illegal_right_assoc(spec_opcode::right_assoc);
    clear_next_right_assoc_is_legal_flag();
    owner().sl.on_right_assoc();
}

void taul::internal::loader::spec_stage::on_lpr_decl(std::string_view name) {
    update_pos();
    check_err_rule_name_conflict(name);
    check_err_illegal_rule_declare(spec_opcode::lpr_decl);
    check_err_illegal_in_lpr_scope(spec_opcode::lpr_decl);
    check_err_illegal_in_ppr_scope(spec_opcode::lpr_decl);
    count_subexpr_for_top_ess();
    add_lpr_decl(name);
    owner().sl.on_lpr_decl(name);
}

void taul::internal::loader::spec_stage::on_ppr_decl(std::string_view name) {
    update_pos();
    check_err_rule_name_conflict(name);
    check_err_illegal_rule_declare(spec_opcode::ppr_decl);
    check_err_illegal_in_lpr_scope(spec_opcode::ppr_decl);
    check_err_illegal_in_ppr_scope(spec_opcode::ppr_decl);
    count_subexpr_for_top_ess();
    add_ppr_decl(name);
    owner().sl.on_ppr_decl(name);
}

void taul::internal::loader::spec_stage::on_lpr(std::string_view name, qualifier qualifier) {
    update_pos();
    check_err_rule_never_declared(name, true);
    check_err_rule_already_defined(name, true);
    check_err_illegal_in_lpr_scope(spec_opcode::lpr);
    check_err_illegal_in_ppr_scope(spec_opcode::lpr);
    check_err_illegal_qualifier(name, qualifier, true);
    count_subexpr_for_top_ess();
    add_lpr_def(name);
    push_ess(ess_frame{ spec_opcode::lpr, owner().pos });
    push_dss(dss_frame{ name, true });
    owner().sl.on_lpr(name, qualifier);
}

void taul::internal::loader::spec_stage::on_ppr(std::string_view name, qualifier qualifier) {
    update_pos();
    check_err_rule_never_declared(name, false);
    check_err_rule_already_defined(name, false);
    check_err_illegal_in_lpr_scope(spec_opcode::ppr);
    check_err_illegal_in_ppr_scope(spec_opcode::ppr);
    check_err_illegal_qualifier(name, qualifier, false);
    count_subexpr_for_top_ess();
    add_ppr_def(name);
    push_ess(ess_frame{ spec_opcode::ppr, owner().pos });
    push_dss(dss_frame{ name, false });
    if (qualifier == precedence) mark_precedence_scope();
    next_is_legal_right_assoc(peek());
    owner().sl.on_ppr(name, qualifier);
}

void taul::internal::loader::spec_stage::on_end() {
    update_pos();
    check_err_illegal_in_no_scope(spec_opcode::end);
    check_err_illegal_in_no_end_subexpr_scope(spec_opcode::end);
    count_subexpr_for_top_ess();
    owner().sl.on_end();
}

void taul::internal::loader::spec_stage::on_any() {
    update_pos();
    check_err_illegal_in_no_scope(spec_opcode::any);
    count_subexpr_for_top_ess();
    owner().sl.on_any();
}

void taul::internal::loader::spec_stage::on_string(std::string_view s) {
    update_pos();
    const auto ss = parse_taul_string(s);
    check_err_illegal_in_no_scope(spec_opcode::string);
    check_err_illegal_in_ppr_scope(spec_opcode::string);
    check_err_illegal_in_single_terminal_scope(spec_opcode::string, ss);
    check_err_illegal_string_literal_due_to_illegal_codepoints(ss);
    check_err_illegal_string_literal_due_to_nonvisible_ascii(s);
    count_subexpr_for_top_ess();
    owner().sl.on_string(s);
}

void taul::internal::loader::spec_stage::on_charset(std::string_view s) {
    const auto ss = parse_taul_charset(s);
    check_err_illegal_in_no_scope(spec_opcode::charset);
    check_err_illegal_in_ppr_scope(spec_opcode::charset);
    check_err_illegal_charset_literal_due_to_illegal_codepoints(ss);
    check_err_illegal_charset_literal_due_to_nonvisible_ascii(s);
    count_subexpr_for_top_ess();
    owner().sl.on_charset(s);
}

void taul::internal::loader::spec_stage::on_token() {
    update_pos();
    check_err_illegal_in_no_scope(spec_opcode::token);
    check_err_illegal_in_lpr_scope(spec_opcode::token);
    count_subexpr_for_top_ess();
    owner().sl.on_token();
}

void taul::internal::loader::spec_stage::on_failure() {
    update_pos();
    check_err_illegal_in_no_scope(spec_opcode::failure);
    check_err_illegal_in_lpr_scope(spec_opcode::failure);
    count_subexpr_for_top_ess();
    owner().sl.on_failure();
}

void taul::internal::loader::spec_stage::on_name(std::string_view name) {
    update_pos();
    check_err_illegal_in_no_scope(spec_opcode::name);
    if (in_lpr()) {
        check_err_rule_may_not_be_ppr(name);
    }
    check_err_rule_not_found(name);
    check_err_illegal_in_single_terminal_scope(spec_opcode::name, name);
    count_subexpr_for_top_ess();
    detect_prefix_ref(name);
    owner().sl.on_name(name, peek());
}

void taul::internal::loader::spec_stage::on_sequence() {
    update_pos();
    check_err_illegal_in_no_scope(spec_opcode::sequence);
    //check_err_illegal_in_single_terminal_scope(spec_opcode::sequence);
    count_subexpr_for_top_ess();
    // query scopes before ess push
    const bool single_terminal_scope = in_single_terminal_scope();
    const bool no_end_subexpr_scope = in_no_end_subexpr_scope();
    push_ess(ess_frame{ spec_opcode::sequence, owner().pos });
    if (single_terminal_scope) mark_single_terminal_scope();
    if (no_end_subexpr_scope) mark_no_end_subexpr_scope();
    owner().sl.on_sequence();
}

void taul::internal::loader::spec_stage::on_lookahead() {
    update_pos();
    check_err_illegal_in_no_scope(spec_opcode::lookahead);
    check_err_illegal_in_single_terminal_scope(spec_opcode::lookahead);
    count_subexpr_for_top_ess();
    push_ess(ess_frame{ spec_opcode::lookahead, owner().pos });
    mark_single_terminal_scope();
    mark_no_end_subexpr_scope();
    owner().sl.on_lookahead();
}

void taul::internal::loader::spec_stage::on_lookahead_not() {
    update_pos();
    check_err_illegal_in_no_scope(spec_opcode::lookahead_not);
    check_err_illegal_in_single_terminal_scope(spec_opcode::lookahead_not);
    count_subexpr_for_top_ess();
    push_ess(ess_frame{ spec_opcode::lookahead_not, owner().pos });
    mark_single_terminal_scope();
    mark_no_end_subexpr_scope();
    owner().sl.on_lookahead_not();
}

void taul::internal::loader::spec_stage::on_not() {
    update_pos();
    check_err_illegal_in_no_scope(spec_opcode::not0);
    check_err_illegal_in_single_terminal_scope(spec_opcode::not0);
    count_subexpr_for_top_ess();
    push_ess(ess_frame{ spec_opcode::not0, owner().pos });
    mark_single_terminal_scope();
    mark_no_end_subexpr_scope();
    owner().sl.on_not();
}

void taul::internal::loader::spec_stage::on_optional() {
    update_pos();
    check_err_illegal_in_no_scope(spec_opcode::optional);
    check_err_illegal_in_single_terminal_scope(spec_opcode::optional);
    count_subexpr_for_top_ess();
    push_ess(ess_frame{ spec_opcode::optional, owner().pos });
    mark_no_alternation_scope();
    mark_single_subexpr_scope();
    owner().sl.on_optional();
}

void taul::internal::loader::spec_stage::on_kleene_star() {
    update_pos();
    check_err_illegal_in_no_scope(spec_opcode::kleene_star);
    check_err_illegal_in_single_terminal_scope(spec_opcode::kleene_star);
    count_subexpr_for_top_ess();
    push_ess(ess_frame{ spec_opcode::kleene_star, owner().pos });
    mark_no_alternation_scope();
    mark_single_subexpr_scope();
    owner().sl.on_kleene_star();
}

void taul::internal::loader::spec_stage::on_kleene_plus() {
    update_pos();
    check_err_illegal_in_no_scope(spec_opcode::kleene_plus);
    check_err_illegal_in_single_terminal_scope(spec_opcode::kleene_plus);
    count_subexpr_for_top_ess();
    push_ess(ess_frame{ spec_opcode::kleene_plus, owner().pos });
    mark_no_alternation_scope();
    mark_single_subexpr_scope();
    owner().sl.on_kleene_plus();
}

taul::internal::loader::llspec_stage::llspec_stage(loader& owner)
    : llspec_interpreter(),
    owner_ptr(&owner) {}

std::optional<taul::grammar> taul::internal::loader::llspec_stage::result() {
    return owner().gb.get_result();
}

void taul::internal::loader::llspec_stage::update_pos() {
    owner().pos = pos();
}

void taul::internal::loader::llspec_stage::check_err_illegal_ambiguity() {
    for (const auto& I : owner().rule_pt_trans.lexer_ptbd.collisions) {
        const auto dbgsym_pos = owner().rule_pt_trans.lexer_id_alloc.output_debug.query(I.nonterminal).value().pos;
        const auto dbgsym_name = owner().rule_pt_trans.fetch_name_str_lxr(I.nonterminal);
        const auto inputs = owner().rule_pt_trans.lexer_pt.grouper.get_symbol_range(I.terminal_group);
        owner().raise_at(
            dbgsym_pos,
            spec_error::illegal_ambiguity,
            "{} is ambiguous over the inputs {}!",
            dbgsym_name,
            owner().fmt_symbol_range_ext(inputs));
    }
    for (const auto& I : owner().rule_pt_trans.parser_ptbd.collisions) {
        const auto dbgsym_pos = owner().rule_pt_trans.parser_id_alloc.output_debug.query(I.nonterminal).value().pos;
        const auto dbgsym_name = owner().rule_pt_trans.fetch_name_str_psr(I.nonterminal);
        const auto inputs = owner().rule_pt_trans.parser_pt.grouper.get_symbol_range(I.terminal_group);
        owner().raise_at(
            dbgsym_pos,
            spec_error::illegal_ambiguity,
            "{} is ambiguous over the inputs {}!",
            dbgsym_name,
            owner().fmt_symbol_range_ext(inputs));
    }
    check_err_illegal_ambiguity_due_to_trivial_left_recursion();
}

void taul::internal::loader::llspec_stage::check_err_illegal_ambiguity_due_to_trivial_left_recursion() {
    // the below loops expected grammar builder to have been able to build mappings, 
    // meaning we need to abort this method if it was cancelled and never did
    if (owner().gb.cancelled) return;
    for (size_t i = 0; i < owner().rule_pt_trans.lexer_pt.rules.size(); i++) {
        const auto& first_set_w = owner().rule_pt_trans.lexer_ptbd.first_sets_w[i];
        const auto nonterminal = owner().rule_pt_trans.lexer_pt.rules[i].id;
        if (first_set_w.empty() && !first_set_w.includes_epsilon()) {
            owner().raise(
                spec_error::illegal_ambiguity,
                "{} is ambiguous due to left-recursion!",
                owner().fmt_symbol_id_ext(nonterminal));
        }
    }
    for (size_t i = 0; i < owner().rule_pt_trans.parser_pt.rules.size(); i++) {
        const auto& first_set_w = owner().rule_pt_trans.parser_ptbd.first_sets_w[i];
        const auto nonterminal = owner().rule_pt_trans.parser_pt.rules[i].id;
        if (first_set_w.empty() && !first_set_w.includes_epsilon()) {
            owner().raise(
                spec_error::illegal_ambiguity,
                "{} is ambiguous due to left-recursion!",
                owner().fmt_symbol_id_ext(nonterminal));
        }
    }
}

void taul::internal::loader::llspec_stage::check_for_internal_errors() {
    // these are internal errors reporting that the system is broken
    // these are not represented in our unit tests
    if (owner().rule_pt_trans.lexer_ptbd.nonterminal_id_is_terminal_id) {
        owner().raise(
            spec_error::internal_error,
            "internal error! (rule_pt_trans.lexer_ptbd.nonterminal_id_is_terminal_id == true)");
    }
    if (owner().rule_pt_trans.lexer_ptbd.terminal_ids_not_in_legal_range) {
        owner().raise(
            spec_error::internal_error,
            "internal error! (rule_pt_trans.lexer_ptbd.terminal_ids_not_in_legal_range == true)");
    }
    if (owner().rule_pt_trans.lexer_ptbd.nonterminal_not_in_rules_vector) {
        owner().raise(
            spec_error::internal_error,
            "internal error! (rule_pt_trans.lexer_ptbd.nonterminal_not_in_rules_vector == true)");
    }
    if (owner().rule_pt_trans.parser_ptbd.nonterminal_id_is_terminal_id) {
        owner().raise(
            spec_error::internal_error,
            "internal error! (rule_pt_trans.parser_ptbd.nonterminal_id_is_terminal_id == true)");
    }
    if (owner().rule_pt_trans.parser_ptbd.terminal_ids_not_in_legal_range) {
        owner().raise(
            spec_error::internal_error,
            "internal error! (rule_pt_trans.parser_ptbd.terminal_ids_not_in_legal_range == true)");
    }
    if (owner().rule_pt_trans.parser_ptbd.nonterminal_not_in_rules_vector) {
        owner().raise(
            spec_error::internal_error,
            "internal error! (rule_pt_trans.parser_ptbd.nonterminal_not_in_rules_vector == true)");
    }
}

void taul::internal::loader::llspec_stage::on_startup() {
    TAUL_ASSERT(owner().ec);
    TAUL_ASSERT(owner().success);
    owner().rule_pt_trans.on_startup();
}

void taul::internal::loader::llspec_stage::on_shutdown() {
    owner().rule_pt_trans.on_shutdown();
    check_err_illegal_ambiguity();
    check_for_internal_errors();
}

void taul::internal::loader::llspec_stage::on_close() {
    update_pos();
    owner().rule_pt_trans.on_close();
}

void taul::internal::loader::llspec_stage::on_alternative() {
    update_pos();
    owner().rule_pt_trans.on_alternative();
}

void taul::internal::loader::llspec_stage::on_right_assoc() {
    update_pos();
    owner().rule_pt_trans.on_right_assoc();
}

void taul::internal::loader::llspec_stage::on_lpr_decl(std::string_view name) {
    update_pos();
    owner().rule_pt_trans.on_lpr_decl(name);
    owner().gb.add_lpr_decl(str(name));
}

void taul::internal::loader::llspec_stage::on_ppr_decl(std::string_view name) {
    update_pos();
    owner().rule_pt_trans.on_ppr_decl(name);
    owner().gb.add_ppr_decl(str(name));
}

void taul::internal::loader::llspec_stage::on_lpr(std::string_view name, qualifier qualifier) {
    update_pos();
    owner().rule_pt_trans.on_lpr(name, qualifier);
    owner().gb.add_lpr(str(name), qualifier);
}

void taul::internal::loader::llspec_stage::on_ppr(std::string_view name, qualifier qualifier) {
    update_pos();
    owner().rule_pt_trans.on_ppr(name, qualifier);
    owner().gb.add_ppr(str(name), qualifier);
}

void taul::internal::loader::llspec_stage::on_end() {
    update_pos();
    owner().rule_pt_trans.on_end();
}

void taul::internal::loader::llspec_stage::on_any() {
    update_pos();
    owner().rule_pt_trans.on_any();
}

void taul::internal::loader::llspec_stage::on_string(std::string_view s) {
    update_pos();
    const auto ss = parse_taul_string(s);
    owner().rule_pt_trans.on_string(ss);
}

void taul::internal::loader::llspec_stage::on_charset(std::string_view s) {
    update_pos();
    const auto ss = parse_taul_charset(s);
    owner().rule_pt_trans.on_charset(ss);
}

void taul::internal::loader::llspec_stage::on_token() {
    update_pos();
    owner().rule_pt_trans.on_token();
}

void taul::internal::loader::llspec_stage::on_failure() {
    update_pos();
    owner().rule_pt_trans.on_failure();
}

void taul::internal::loader::llspec_stage::on_name(std::string_view name, preced_t preced_val) {
    update_pos();
    owner().rule_pt_trans.on_name(name, preced_val);
}

void taul::internal::loader::llspec_stage::on_sequence() {
    update_pos();
    owner().rule_pt_trans.on_sequence();
}

void taul::internal::loader::llspec_stage::on_lookahead() {
    update_pos();
    owner().rule_pt_trans.on_lookahead();
}

void taul::internal::loader::llspec_stage::on_lookahead_not() {
    update_pos();
    owner().rule_pt_trans.on_lookahead_not();
}

void taul::internal::loader::llspec_stage::on_not() {
    update_pos();
    owner().rule_pt_trans.on_not();
}

void taul::internal::loader::llspec_stage::on_optional() {
    update_pos();
    owner().rule_pt_trans.on_optional();
}

void taul::internal::loader::llspec_stage::on_kleene_star() {
    update_pos();
    owner().rule_pt_trans.on_kleene_star();
}

void taul::internal::loader::llspec_stage::on_kleene_plus() {
    update_pos();
    owner().rule_pt_trans.on_kleene_plus();
}

void taul::internal::loader::llspec_stage::on_preced_pred(preced_t preced_max, preced_t preced_val) {
    update_pos();
    owner().rule_pt_trans.on_preced_pred(preced_max, preced_val);
}

void taul::internal::loader::llspec_stage::on_pylon() {
    update_pos();
    owner().rule_pt_trans.on_pylon();
}

