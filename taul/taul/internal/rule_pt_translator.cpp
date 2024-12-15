

#include "rule_pt_translator.h"

#include "../string_and_charset.h"


using namespace taul::string_literals;


#define _DUMP_LOG 0
#define _DUMP_OUTPUT_LOG 0


void taul::internal::rule_pt_translator::assert_in_composite_expr() const noexcept {
    TAUL_ASSERT(!composite_expr_stk.empty());
}

void taul::internal::rule_pt_translator::assert_not_in_composite_expr() const noexcept {
    TAUL_ASSERT(composite_expr_stk.empty());
}

void taul::internal::rule_pt_translator::assert_in_subrule() const noexcept {
    TAUL_ASSERT(!subrule_stk.empty());
}

void taul::internal::rule_pt_translator::assert_not_in_subrule() const noexcept {
    TAUL_ASSERT(subrule_stk.empty());
}

taul::str taul::internal::rule_pt_translator::fetch_name_str_lxr(symbol_id x) const noexcept {
    return lexer_id_alloc.output_debug.dbgsyms.at(x).name;
}

taul::str taul::internal::rule_pt_translator::fetch_name_str_psr(symbol_id x) const noexcept {
    return parser_id_alloc.output_debug.dbgsyms.at(x).name;
}

taul::symbol_id taul::internal::rule_pt_translator::fetch_symbol_lxr(std::string_view name) const noexcept {
    TAUL_ASSERT(main_nonterminal_name_map_lxr.contains(name));
    return main_nonterminal_name_map_lxr.at(name);
}

taul::symbol_id taul::internal::rule_pt_translator::fetch_symbol_psr(std::string_view name) const noexcept {
    TAUL_ASSERT(main_nonterminal_name_map_psr.contains(name));
    return main_nonterminal_name_map_psr.at(name);
}

void taul::internal::rule_pt_translator::create_main_symbol_lxr(str name) {
    TAUL_ASSERT(lexer_id_alloc.is_defining_main());
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> create_main_symbol_lxr(\"{}\")", name);
#endif
    main_nonterminal_name_map_lxr[name] = lexer_id_alloc.define(deref_assert(pos_ptr), name);
}

void taul::internal::rule_pt_translator::create_main_symbol_psr(str name) {
    TAUL_ASSERT(parser_id_alloc.is_defining_main());
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> create_main_symbol_psr(\"{}\")", name);
#endif
    main_nonterminal_name_map_psr[name] = parser_id_alloc.define(deref_assert(pos_ptr), name);
}

taul::symbol_id taul::internal::rule_pt_translator::create_helper_symbol(str name) {
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> create_helper_symbol(\"{}\")", name);
#endif
    if (is_in_lpr_not_ppr()) {
        if (lexer_id_alloc.is_defining_main()) lexer_id_alloc.done_defining_main();
        return lexer_id_alloc.define(deref_assert(pos_ptr), name);
    }
    else {
        if (parser_id_alloc.is_defining_main()) parser_id_alloc.done_defining_main();
        return parser_id_alloc.define(deref_assert(pos_ptr), name);
    }
}

void taul::internal::rule_pt_translator::begin_composite_expr() {
    composite_expr_stk.push_back(composite_expr_frame{ .subrules = 0 });
    assert_in_composite_expr();
}

void taul::internal::rule_pt_translator::end_composite_expr() {
    assert_in_composite_expr();
    for (size_t i = 0; i < composite_expr_stk.back().subrules; i++) end_subrule();
    composite_expr_stk.pop_back();
}

void taul::internal::rule_pt_translator::begin_subrule(symbol_id x, mode m) {
    assert_in_composite_expr();
    TAUL_ASSERT(subrule_stk.empty() || !subrule_stk.back().is_set_like());
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> begin_subrule({}, {})", x, fmt_mode(m));
#endif
    begin_subrule_uncounted(x, m);
    composite_expr_stk.back().subrules++; // inform top composite expr about subrule
}

void taul::internal::rule_pt_translator::begin_subrule_uncounted(symbol_id x, mode m) {
    assert_in_composite_expr();
    TAUL_ASSERT(subrule_stk.empty() || !subrule_stk.back().is_set_like());
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> begin_subrule_uncounted({}, {})", x, fmt_mode(m));
#endif
    if (!subrule_stk.empty()) {
        next_nonterminal(x); // if not toplevel, put ref to x before start new rule
    }
    else { // if toplevel, update in_lpr_not_ppr as needed
        if (m == mode::lpr) in_lpr_not_ppr = true;
        if (m == mode::ppr) in_lpr_not_ppr = false;
    }
    if (in_lpr_not_ppr) { // (can't use is_in_lpr_not_ppr until we push frame)
        lexer_pt.add_rule(x);
    }
    else {
        parser_pt.add_rule(x);
    }
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> added rule for {}", x);
#endif
    const size_t rule =
        in_lpr_not_ppr // (can't use is_in_lpr_not_ppr until we push frame)
        ? lexer_pt.rules.size() - 1
        : parser_pt.rules.size() - 1;
    subrule_stk.push_back(
        subrule_frame{
            .nonterminal = x,
            .rule = rule,
            .m = m,
        });
    if (subrule_stk.back().is_optional()) next_alternative(); // ensure has empty alt if expected
    assert_in_subrule();
}

void taul::internal::rule_pt_translator::end_subrule() {
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> end_subrule");
#endif
    if (subrule_stk.back().is_set_like()) {
        // having populated stk.back().set, now check if stk.back().is_inverted() == true,
        // and if so, invert stk.back().set
        if (is_in_lpr_not_ppr()) {
            if (subrule_stk.back().is_inverted()) {
                subrule_stk.back().set_lxr =
                    subrule_stk.back().set_lxr
                    .inverse()
                    .remove_id(symbol_traits<glyph>::end_of_input_id)
                    .remove_epsilon();
                TAUL_ASSERT(!subrule_stk.back().set_lxr.includes_id(symbol_traits<glyph>::end_of_input_id)); // don't forget!
                TAUL_ASSERT(!subrule_stk.back().set_lxr.includes_epsilon()); // don't forget!
            }
        }
        else {
            if (subrule_stk.back().is_inverted()) {
                subrule_stk.back().set_psr =
                    subrule_stk.back().set_psr
                    .inverse()
                    .remove_id(symbol_traits<token>::end_of_input_id)
                    .remove_epsilon();
                TAUL_ASSERT(!subrule_stk.back().set_psr.includes_id(symbol_traits<token>::end_of_input_id)); // don't forget!
                TAUL_ASSERT(!subrule_stk.back().set_psr.includes_epsilon()); // don't forget!
            }
        }
        // if set-like, add set alternatives now
        bool not_first = false;
        if (is_in_lpr_not_ppr()) {
            for (const auto& I : subrule_stk.back().set_lxr.ranges()) {
                if (not_first) next_alternative_nocheck(); // important to use ***_nocheck overload here
                lexer_pt.add_terminal(subrule_stk.back().rule, I.low, I.high, subrule_stk.back().is_assertion());
                not_first = true;
            }
        }
        else {
            for (const auto& I : subrule_stk.back().set_psr.ranges()) {
                if (not_first) next_alternative_nocheck(); // important to use ***_nocheck overload here
                parser_pt.add_terminal(subrule_stk.back().rule, I.low, I.high, subrule_stk.back().is_assertion());
                not_first = true;
            }
        }
    }
    // add proceeding recurse non-terminal if required
    if (subrule_stk.back().is_recursive()) next_nonterminal(subrule_stk.back().nonterminal);
    // pop frame, exiting its scope
    subrule_stk.pop_back();
}

void taul::internal::rule_pt_translator::next_alternative() {
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_alternative");
#endif
    if (!subrule_stk.back().is_set_like()) next_alternative_nocheck(); // perform checks, then ***_nocheck overload does the actual work
    assert_in_subrule();
}

void taul::internal::rule_pt_translator::next_alternative_nocheck() {
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_alternative_nocheck");
#endif
    // for the next alternative, we change out the rule for a new one
    if (is_in_lpr_not_ppr()) {
        lexer_pt.add_rule(subrule_stk.back().nonterminal);
    }
    else {
        parser_pt.add_rule(subrule_stk.back().nonterminal);
    }
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> added rule for {}", stk.back().nonterminal);
#endif
    // bind the new rule via its index
    const size_t rule =
        is_in_lpr_not_ppr()
        ? lexer_pt.rules.size() - 1
        : parser_pt.rules.size() - 1;
    subrule_stk.back().rule = rule;
    assert_in_subrule();
}

void taul::internal::rule_pt_translator::next_terminal(symbol_id low, symbol_id high, bool assertion) {
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_terminal({}, {}, {})", low, high, assertion);
#endif
    if (!subrule_stk.back().is_set_like()) {
        if (is_in_lpr_not_ppr()) {
            lexer_pt.add_terminal(subrule_stk.back().rule, low, high, assertion);
        }
        else {
            parser_pt.add_terminal(subrule_stk.back().rule, low, high, assertion);
        }
    }
    else { // the function's assertion param shouldn't matter below as the context guarantees assertion usage anyway
        // inversion will be handled in end_subrule
        if (is_in_lpr_not_ppr()) {
            subrule_stk.back().set_lxr.add_id_range(low, high);
        }
        else {
            subrule_stk.back().set_psr.add_id_range(low, high);
        }
    }
}

void taul::internal::rule_pt_translator::next_terminal(symbol_id x, bool assertion) {
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_terminal({}, {})", x, assertion);
#endif
    next_terminal(x, x, assertion);
}

void taul::internal::rule_pt_translator::next_terminal_set(const glyph_set& set, bool assertion) {
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_terminal_set({}, {})", set, assertion);
#endif
    TAUL_ASSERT(is_in_lpr_not_ppr());
    if (!subrule_stk.back().is_set_like()) {
        begin_subrule_uncounted(create_helper_symbol(fetch_name_str_lxr(subrule_stk.back().nonterminal)), mode::sequence);
        bool not_first = false;
        for (const auto& I : set.ranges()) {
            if (not_first) next_alternative();
            next_terminal(I.low, I.high, assertion);
            not_first = true;
        }
        end_subrule();
    }
    else {
        subrule_stk.back().set_lxr.add_set(set);
    }
}

void taul::internal::rule_pt_translator::next_terminal_set(std::u32string_view charset_str, bool assertion) {
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_terminal_set({}, {})", taul::fmt_taul_charset(charset_str), assertion);
#endif
    TAUL_ASSERT(is_in_lpr_not_ppr());
    glyph_set set{};
    TAUL_ASSERT(charset_str.length() % 2 == 0);
    for (size_t i = 0; i < charset_str.length(); i += 2) {
        const unicode_t low = charset_str[i];
        const unicode_t high = charset_str[i + 1];
        set.add_range(low, high);
    }
    next_terminal_set(set, assertion);
}

void taul::internal::rule_pt_translator::next_nonterminal_with_preced(symbol_id nonterminal, preced_t preced_val) {
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_nonterminal_with_preced({}, {})", nonterminal, preced_val);
#endif
    if (is_in_lpr_not_ppr())    lexer_pt.add_nonterminal(subrule_stk.back().rule, nonterminal, preced_val);
    else                        parser_pt.add_nonterminal(subrule_stk.back().rule, nonterminal, preced_val);
}

void taul::internal::rule_pt_translator::next_nonterminal(symbol_id nonterminal) {
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_nonterminal({})", nonterminal);
#endif
    next_nonterminal_with_preced(nonterminal, signal_preced_val);
}

void taul::internal::rule_pt_translator::next_preced_pred(preced_t preced_max, preced_t preced_val) {
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_preced_pred({}, {})", size_t(preced_max), size_t(preced_val));
#endif
    if (is_in_lpr_not_ppr())    lexer_pt.add_preced_pred(subrule_stk.back().rule, preced_max, preced_val);
    else                        parser_pt.add_preced_pred(subrule_stk.back().rule, preced_max, preced_val);
}

void taul::internal::rule_pt_translator::next_pylon() {
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_pylon");
#endif
    if (is_in_lpr_not_ppr())    lexer_pt.add_pylon(subrule_stk.back().rule);
    else                        parser_pt.add_pylon(subrule_stk.back().rule);
}

void taul::internal::rule_pt_translator::on_startup() {
    if (cancelled) return;
    assert_not_in_composite_expr();
    assert_not_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_startup");
#endif
}

void taul::internal::rule_pt_translator::on_shutdown() {
    if (cancelled) return;
    assert_not_in_composite_expr();
    assert_not_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_shutdown");
#endif
    // if no helpers were added, then we need to make sure to call
    // done_defining_main here, before anything else
    if (lexer_id_alloc.is_defining_main()) lexer_id_alloc.done_defining_main();
    if (parser_id_alloc.is_defining_main()) parser_id_alloc.done_defining_main();
    lexer_pt.build_mappings(lexer_ptbd);
    parser_pt.build_mappings(parser_ptbd);
#if _DUMP_LOG || _DUMP_OUTPUT_LOG
    TAUL_LOG(make_stderr_logger(), "(for LPRs)");
    TAUL_LOG(make_stderr_logger(), "{}", lexer_ptbd.fmt(lexer_pt.grouper));
    TAUL_LOG(make_stderr_logger(), "{}", lexer_pt.fmt());
    TAUL_LOG(make_stderr_logger(), "(for PPRs)");
    TAUL_LOG(make_stderr_logger(), "{}", parser_ptbd.fmt(parser_pt.grouper));
    TAUL_LOG(make_stderr_logger(), "{}", parser_pt.fmt());
#endif
}

void taul::internal::rule_pt_translator::on_close() {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_close");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    if (sequence_counter >= 1) {
        sequence_counter--;
        return;
    }
    end_composite_expr();
}

void taul::internal::rule_pt_translator::on_alternative() {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_alternative");
#endif
    next_alternative();
}

void taul::internal::rule_pt_translator::on_lpr_decl(std::string_view name) {
    if (cancelled) return;
    assert_not_in_composite_expr();
    assert_not_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_lpr_decl");
#endif
    create_main_symbol_lxr(str(name));
}

void taul::internal::rule_pt_translator::on_ppr_decl(std::string_view name) {
    if (cancelled) return;
    assert_not_in_composite_expr();
    assert_not_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_ppr_decl");
#endif
    create_main_symbol_psr(str(name));
}

void taul::internal::rule_pt_translator::on_lpr(std::string_view name, qualifier) {
    if (cancelled) return;
    assert_not_in_composite_expr();
    assert_not_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_lpr");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    begin_composite_expr();
    begin_subrule(fetch_symbol_lxr(name), mode::lpr);
}

void taul::internal::rule_pt_translator::on_ppr(std::string_view name, qualifier) {
    if (cancelled) return;
    assert_not_in_composite_expr();
    assert_not_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_ppr");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    begin_composite_expr();
    begin_subrule(fetch_symbol_psr(name), mode::ppr);
}

void taul::internal::rule_pt_translator::on_end() {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_end");
#endif
    next_terminal(is_in_lpr_not_ppr() ? end_cp_id : end_lpr_id, true);
}

void taul::internal::rule_pt_translator::on_any() {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_any");
#endif
    const auto first = is_in_lpr_not_ppr() ? symbol_traits<glyph>::first_id : symbol_traits<token>::first_id;
    const auto last = is_in_lpr_not_ppr() ? symbol_traits<glyph>::last_id : symbol_traits<token>::last_id;
    // the '- 1' below removes end_cp_id/end_lpr_id
    next_terminal(first, last - 1);
}

void taul::internal::rule_pt_translator::on_string(std::u32string_view s) {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_string");
#endif
    TAUL_ASSERT(is_in_lpr_not_ppr());
    for (const auto& I : s) next_terminal(cp_id(I));
}

void taul::internal::rule_pt_translator::on_charset(std::u32string_view s) {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_charset");
#endif
    TAUL_ASSERT(is_in_lpr_not_ppr());
    next_terminal_set(s); // the s arg passed in is the product of parse_taul_charset (at on_charset call site)
}

void taul::internal::rule_pt_translator::on_token() {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_token");
#endif
    TAUL_ASSERT(!is_in_lpr_not_ppr());
    next_terminal(symbol_traits<token>::first_id, symbol_traits<token>::last_id - special_lpr_ids);
}

void taul::internal::rule_pt_translator::on_failure() {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_failure");
#endif
    TAUL_ASSERT(!is_in_lpr_not_ppr());
    next_terminal(failure_lpr_id);
}

void taul::internal::rule_pt_translator::on_name(std::string_view name, preced_t preced_val) {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_name");
#endif
    if (is_in_lpr_not_ppr()) {
        TAUL_ASSERT(preced_val == no_preced_val);
        next_nonterminal_with_preced(fetch_symbol_lxr(name), preced_val);
    }
    else {
        // name exprs in PPRs can ref both LPRs and PPRs
        if (is_lpr_name(name)) next_terminal(fetch_symbol_lxr(name)); // remember, gotta push this as terminal!
        if (is_ppr_name(name)) next_nonterminal_with_preced(fetch_symbol_psr(name), preced_val);
    }
}

void taul::internal::rule_pt_translator::on_sequence() {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_sequence");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    if (subrule_stk.back().is_set_like()) {
        sequence_counter++;
        return;
    }
    begin_composite_expr();
    begin_subrule(create_helper_symbol("sequence expr"_str), mode::sequence);
}

void taul::internal::rule_pt_translator::on_lookahead() {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_lookahead");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    begin_composite_expr();
    begin_subrule(create_helper_symbol("lookahead expr"_str), mode::lookahead);
}

void taul::internal::rule_pt_translator::on_lookahead_not() {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_lookahead_not");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    begin_composite_expr();
    begin_subrule(create_helper_symbol("lookahead-not expr"_str), mode::lookahead_not);
}

void taul::internal::rule_pt_translator::on_not() {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_not");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    begin_composite_expr();
    begin_subrule(create_helper_symbol("not expr"_str), mode::not0);
}

void taul::internal::rule_pt_translator::on_optional() {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_optional");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    begin_composite_expr();
    begin_subrule(create_helper_symbol("optional expr"_str), mode::optional);
}

void taul::internal::rule_pt_translator::on_kleene_star() {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_kleene_star");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    begin_composite_expr();
    begin_subrule(create_helper_symbol("kleene-star expr"_str), mode::kleene_star);
}

void taul::internal::rule_pt_translator::on_kleene_plus() {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_kleene_plus");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    begin_composite_expr();
    // kleene-plus is a bit more complicated than the others, and requires
    // us to define a total of three helper subrules to define 'VV*'
    const auto a = create_helper_symbol("kleene-plus expr"_str); // the sequence for toplevel 'VV*'
    const auto b = create_helper_symbol("kleene-plus expr"_str); // the kleene-star for 'V*'
    const auto c = create_helper_symbol("kleene-plus expr"_str); // the open subrule for 'V'
    begin_subrule(a, mode::sequence);
    next_nonterminal(c);
    begin_subrule(b, mode::kleene_star);
    begin_subrule(c, mode::sequence);
}

void taul::internal::rule_pt_translator::on_preced_pred(preced_t preced_max, preced_t preced_val) {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_preced_pred({}, {})", size_t(preced_max), size_t(preced_val));
#endif
    next_preced_pred(preced_max, preced_val);
}

void taul::internal::rule_pt_translator::on_pylon() {
    if (cancelled) return;
    assert_in_composite_expr();
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_pylon");
#endif
    next_pylon();
}

