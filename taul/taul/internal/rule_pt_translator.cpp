

#include "rule_pt_translator.h"

#include "../string_and_charset.h"


using namespace taul::string_literals;


#define _DUMP_LOG 0
#define _DUMP_OUTPUT_LOG 0


void taul::internal::rule_pt_translator::assert_in_subrule() const noexcept {
    TAUL_ASSERT(!stk.empty());
}

void taul::internal::rule_pt_translator::assert_not_in_subrule() const noexcept {
    TAUL_ASSERT(stk.empty());
}

taul::str taul::internal::rule_pt_translator::fetch_name_str_lxr(symbol_id x) const noexcept {
    return lexer_id_alloc.output_debug.dbgsyms.at(x).name;
}

taul::str taul::internal::rule_pt_translator::fetch_name_str_psr(symbol_id x) const noexcept {
    return parser_id_alloc.output_debug.dbgsyms.at(x).name;
}

taul::str taul::internal::rule_pt_translator::fetch_name_str_lxr(std::string_view name) const noexcept {
    return fetch_name_str_lxr(fetch_symbol_lxr(name));
}

taul::str taul::internal::rule_pt_translator::fetch_name_str_psr(std::string_view name) const noexcept {
    return fetch_name_str_psr(fetch_symbol_psr(name));
}

taul::symbol_id taul::internal::rule_pt_translator::fetch_symbol_lxr(std::string_view name) const noexcept {
    TAUL_ASSERT(main_nonterminal_name_map_lxr.contains(name));
    return main_nonterminal_name_map_lxr.at(name);
}

taul::symbol_id taul::internal::rule_pt_translator::fetch_symbol_psr(std::string_view name) const noexcept {
    TAUL_ASSERT(main_nonterminal_name_map_psr.contains(name));
    return main_nonterminal_name_map_psr.at(name);
}

taul::symbol_id taul::internal::rule_pt_translator::add_main_symbol_lxr(str name) {
    TAUL_ASSERT(lexer_id_alloc.is_defining_main());
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> add_main_symbol_lxr(\"{}\")", name);
#endif
    TAUL_DEREF_SAFE(instruction_ptr) {
        main_nonterminal_name_map_lxr[name] = lexer_id_alloc.define(*instruction_ptr, name);
    }
    return main_nonterminal_name_map_lxr.at(name);
}

taul::symbol_id taul::internal::rule_pt_translator::add_main_symbol_psr(str name) {
    TAUL_ASSERT(parser_id_alloc.is_defining_main());
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> add_main_symbol_psr(\"{}\")", name);
#endif
    TAUL_DEREF_SAFE(instruction_ptr) {
        main_nonterminal_name_map_psr[name] = parser_id_alloc.define(*instruction_ptr, name);
    }
    return main_nonterminal_name_map_psr.at(name);
}

taul::symbol_id taul::internal::rule_pt_translator::add_helper_symbol_lxr(str name) {
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> add_helper_symbol_lxr(\"{}\")", name);
#endif
    symbol_id result{};
    TAUL_DEREF_SAFE(instruction_ptr) {
        if (lexer_id_alloc.is_defining_main()) lexer_id_alloc.done_defining_main();
        result = lexer_id_alloc.define(*instruction_ptr, name);
    }
    return result;
}

taul::symbol_id taul::internal::rule_pt_translator::add_helper_symbol_psr(str name) {
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> add_helper_symbol_psr(\"{}\")", name);
#endif
    symbol_id result{};
    TAUL_DEREF_SAFE(instruction_ptr) {
        if (parser_id_alloc.is_defining_main()) parser_id_alloc.done_defining_main();
        result = parser_id_alloc.define(*instruction_ptr, name);
    }
    return result;
}

void taul::internal::rule_pt_translator::begin_subrule(symbol_id x, mode m, size_t subrules) {
    TAUL_ASSERT(stk.empty() || !stk.back().is_set_like());
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> begin_subrule({}, {})", x, fmt_mode(m), subrules);
#endif
    TAUL_ASSERT(subrules >= 1); // illegal for subrules == 0
    if (!stk.empty()) {
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
    if (in_lpr_not_ppr) { // (can't use is_in_lpr_not_ppr until we push frame)
        stk.push_back(
            frame{
                .nonterminal = x,
                .rule = lexer_pt.rules.size() - 1,
                .m = m,
                .subrules = subrules,
            });
    }
    else {
        stk.push_back(
            frame{
                .nonterminal = x,
                .rule = parser_pt.rules.size() - 1,
                .m = m,
                .subrules = subrules,
            });
    }
    if (stk.back().is_optional()) next_alternative(); // ensure has empty alt if expected
    assert_in_subrule();
}

void taul::internal::rule_pt_translator::end_subrule() {
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> end_subrule");
#endif
    const size_t subrules = stk.back().subrules; // copy so we still have it after popping its frame
    if (stk.back().is_set_like()) {
        // if inverted, invert stk.back().set now
        if (is_in_lpr_not_ppr()) {
            if (stk.back().is_inverted()) {
                stk.back().set_lxr =
                    stk.back().set_lxr
                    .inverse()
                    .remove_id(symbol_traits<glyph>::end_of_input_id)
                    .remove_epsilon();
                TAUL_ASSERT(!stk.back().set_lxr.includes_id(symbol_traits<glyph>::end_of_input_id)); // don't forget!
                TAUL_ASSERT(!stk.back().set_lxr.includes_epsilon()); // don't forget!
            }
        }
        else {
            if (stk.back().is_inverted()) {
                stk.back().set_psr =
                    stk.back().set_psr
                    .inverse()
                    .remove_id(symbol_traits<token>::end_of_input_id)
                    .remove_epsilon();
                TAUL_ASSERT(!stk.back().set_psr.includes_id(symbol_traits<token>::end_of_input_id)); // don't forget!
                TAUL_ASSERT(!stk.back().set_psr.includes_epsilon()); // don't forget!
            }
        }
        // if set-like, add set alternatives now
        bool not_first = false;
        if (is_in_lpr_not_ppr()) {
            for (const auto& I : stk.back().set_lxr.ranges()) {
                if (not_first) next_alternative_nocheck(); // important to use variant here
                lexer_pt.add_terminal(stk.back().rule, I.low, I.high, stk.back().is_assertion());
                not_first = true;
            }
        }
        else {
            for (const auto& I : stk.back().set_psr.ranges()) {
                if (not_first) next_alternative_nocheck(); // important to use variant here
                parser_pt.add_terminal(stk.back().rule, I.low, I.high, stk.back().is_assertion());
                not_first = true;
            }
        }
    }
    // add proceeding recurse non-terminal if required
    if (stk.back().is_recursive()) next_nonterminal(stk.back().nonterminal);
    // pop frame, exiting its scope
    stk.pop_back();
    // call end_subrule to cleanup any other subrules this subrule is to be cleaned up *in bulk* w/
    for (size_t i = 0; i < subrules - 1; i++) end_subrule();
}

void taul::internal::rule_pt_translator::next_alternative() {
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_alternative");
#endif
    if (!stk.back().is_set_like()) next_alternative_nocheck();
    assert_in_subrule();
}

void taul::internal::rule_pt_translator::next_alternative_nocheck() {
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_alternative_nocheck");
#endif
    // for the next alternative, we change out the rule for a new one
    if (is_in_lpr_not_ppr()) {
        lexer_pt.add_rule(stk.back().nonterminal);
    }
    else {
        parser_pt.add_rule(stk.back().nonterminal);
    }
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> added rule for {}", stk.back().nonterminal);
#endif
    // bind the new rule via its index
    if (is_in_lpr_not_ppr()) {
        stk.back().rule = lexer_pt.rules.size() - 1;
    }
    else {
        stk.back().rule = parser_pt.rules.size() - 1;
    }
    assert_in_subrule();
}

void taul::internal::rule_pt_translator::next_terminal(symbol_id low, symbol_id high, bool assertion) {
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_terminal({}, {}, {})", low, high, assertion);
#endif
    if (!stk.back().is_set_like()) {
        if (is_in_lpr_not_ppr()) {
            lexer_pt.add_terminal(stk.back().rule, low, high, assertion);
        }
        else {
            parser_pt.add_terminal(stk.back().rule, low, high, assertion);
        }
    }
    else { // assertion arg shouldn't matter below as the context guarantees assertion usage anyway
        // inversion will be handled in end_subrule
        if (is_in_lpr_not_ppr()) {
            stk.back().set_lxr.add_id_range(low, high);
        }
        else {
            stk.back().set_psr.add_id_range(low, high);
        }
    }
}

void taul::internal::rule_pt_translator::next_terminal(symbol_id x, bool assertion) {
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_terminal({}, {})", x, assertion);
#endif
    next_terminal(x, x, assertion);
}

void taul::internal::rule_pt_translator::next_terminal_set(const glyph_set& set, bool assertion) {
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_terminal_set({}, {})", set, assertion);
#endif
    TAUL_ASSERT(is_in_lpr_not_ppr());
    if (!stk.back().is_set_like()) {
        begin_subrule(add_helper_symbol_lxr(fetch_name_str_lxr(stk.back().nonterminal)), mode::sequence);
        bool not_first = false;
        for (const auto& I : set.ranges()) {
            if (not_first) next_alternative();
            next_terminal(I.low, I.high, assertion);
            not_first = true;
        }
        end_subrule();
    }
    else {
        stk.back().set_lxr.add_set(set);
    }
}

void taul::internal::rule_pt_translator::next_terminal_set(std::string_view charset_str, bool assertion) {
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_terminal_set({}, {})", (std::string)charset_str, assertion);
#endif
    TAUL_ASSERT(is_in_lpr_not_ppr());
    glyph_set set{};
    for (std::size_t i = 0; i < charset_str.length(); i += 2) {
        set.add_range(charset_str[i], charset_str[i + 1]);
    }
    next_terminal_set(set, assertion);
}

void taul::internal::rule_pt_translator::next_nonterminal(symbol_id nonterminal) {
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> next_nonterminal({})", nonterminal);
#endif
    if (is_in_lpr_not_ppr()) {
        lexer_pt.add_nonterminal(stk.back().rule, nonterminal);
    }
    else {
        parser_pt.add_nonterminal(stk.back().rule, nonterminal);
    }
}

void taul::internal::rule_pt_translator::on_startup() {
    if (cancelled) return;
    assert_not_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_startup");
#endif
}

void taul::internal::rule_pt_translator::on_shutdown() {
    if (cancelled) return;
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
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_close");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    if (sequence_counter >= 1) {
        sequence_counter--;
        return;
    }
    end_subrule();
}

void taul::internal::rule_pt_translator::on_alternative() {
    if (cancelled) return;
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_alternative");
#endif
    next_alternative();
}

void taul::internal::rule_pt_translator::on_lpr_decl(std::string_view name) {
    if (cancelled) return;
    assert_not_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_lpr_decl");
#endif
    add_main_symbol_lxr(str(name));
}

void taul::internal::rule_pt_translator::on_ppr_decl(std::string_view name) {
    if (cancelled) return;
    assert_not_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_ppr_decl");
#endif
    add_main_symbol_psr(str(name));
}

void taul::internal::rule_pt_translator::on_lpr(std::string_view name, qualifier) {
    if (cancelled) return;
    assert_not_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_lpr");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    begin_subrule(fetch_symbol_lxr(name), mode::lpr);
}

void taul::internal::rule_pt_translator::on_ppr(std::string_view name, qualifier) {
    if (cancelled) return;
    assert_not_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_ppr");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    begin_subrule(fetch_symbol_psr(name), mode::ppr);
}

void taul::internal::rule_pt_translator::on_end() {
    if (cancelled) return;
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_end");
#endif
    if (is_in_lpr_not_ppr()) {
        next_terminal(end_cp_id, true);
    }
    else {
        next_terminal(end_lpr_id, true);
    }
}

void taul::internal::rule_pt_translator::on_any() {
    if (cancelled) return;
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_any");
#endif
    if (is_in_lpr_not_ppr()) {
        next_terminal(symbol_traits<glyph>::first_id, symbol_traits<glyph>::last_id - 1); // the '- 1' removes end_cp_id
    }
    else {
        next_terminal(symbol_traits<token>::first_id, symbol_traits<token>::last_id - 1); // the '- 1' removes end_lpr_id
    }
}

void taul::internal::rule_pt_translator::on_string(std::string_view s) {
    if (cancelled) return;
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_string");
#endif
    TAUL_ASSERT(is_in_lpr_not_ppr());
    for (const auto& I : s) next_terminal(cp_id(I));
}

void taul::internal::rule_pt_translator::on_charset(std::string_view s) {
    if (cancelled) return;
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_charset");
#endif
    TAUL_ASSERT(is_in_lpr_not_ppr());
    next_terminal_set(s); // the s passed in is the product of parse_taul_charset
}

void taul::internal::rule_pt_translator::on_token() {
    if (cancelled) return;
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_token");
#endif
    TAUL_ASSERT(!is_in_lpr_not_ppr());
    next_terminal(symbol_traits<token>::first_id, symbol_traits<token>::last_id - sentinel_lpr_ids);
}

void taul::internal::rule_pt_translator::on_failure() {
    if (cancelled) return;
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_failure");
#endif
    TAUL_ASSERT(!is_in_lpr_not_ppr());
    next_terminal(failure_lpr_id);
}

void taul::internal::rule_pt_translator::on_name(std::string_view name) {
    if (cancelled) return;
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_name");
#endif
    if (is_in_lpr_not_ppr()) {
        next_nonterminal(fetch_symbol_lxr(name));
    }
    else {
        // name exprs in PPRs can ref both LPRs and PPRs
        if (is_lpr_name(name)) next_terminal(fetch_symbol_lxr(name)); // remember, gotta push this as terminal!
        if (is_ppr_name(name)) next_nonterminal(fetch_symbol_psr(name));
    }
}

void taul::internal::rule_pt_translator::on_sequence() {
    if (cancelled) return;
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_sequence");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    if (stk.back().is_set_like()) {
        sequence_counter++;
        return;
    }
    if (is_in_lpr_not_ppr()) {
        begin_subrule(add_helper_symbol_lxr("sequence expr"_str), mode::sequence);
    }
    else {
        begin_subrule(add_helper_symbol_psr("sequence expr"_str), mode::sequence);
    }
}

void taul::internal::rule_pt_translator::on_lookahead() {
    if (cancelled) return;
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_lookahead");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    if (is_in_lpr_not_ppr()) {
        begin_subrule(add_helper_symbol_lxr("lookahead expr"_str), mode::lookahead);
    }
    else {
        begin_subrule(add_helper_symbol_psr("lookahead expr"_str), mode::lookahead);
    }
}

void taul::internal::rule_pt_translator::on_lookahead_not() {
    if (cancelled) return;
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_lookahead_not");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    if (is_in_lpr_not_ppr()) {
        begin_subrule(add_helper_symbol_lxr("lookahead-not expr"_str), mode::lookahead_not);
    }
    else {
        begin_subrule(add_helper_symbol_psr("lookahead-not expr"_str), mode::lookahead_not);
    }
}

void taul::internal::rule_pt_translator::on_not() {
    if (cancelled) return;
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_not");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    if (is_in_lpr_not_ppr()) {
        begin_subrule(add_helper_symbol_lxr("not expr"_str), mode::not0);
    }
    else {
        begin_subrule(add_helper_symbol_psr("not expr"_str), mode::not0);
    }
}

void taul::internal::rule_pt_translator::on_optional() {
    if (cancelled) return;
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_optional");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    if (is_in_lpr_not_ppr()) {
        begin_subrule(add_helper_symbol_lxr("optional expr"_str), mode::optional);
    }
    else {
        begin_subrule(add_helper_symbol_psr("optional expr"_str), mode::optional);
    }
}

void taul::internal::rule_pt_translator::on_kleene_star() {
    if (cancelled) return;
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_kleene_star");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    if (is_in_lpr_not_ppr()) {
        begin_subrule(add_helper_symbol_lxr("kleene-star expr"_str), mode::kleene_star);
    }
    else {
        begin_subrule(add_helper_symbol_psr("kleene-star expr"_str), mode::kleene_star);
    }
}

void taul::internal::rule_pt_translator::on_kleene_plus() {
    if (cancelled) return;
    assert_in_subrule();
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> on_kleene_plus");
    TAUL_LOG(make_stderr_logger(), "->     stack depth == {}", stk.size());
#endif
    // kleene-plus is a bit more complicated than the others, and requires
    // us to define a total of three helper subrules to define 'VV*'
    if (is_in_lpr_not_ppr()) {
        auto a = add_helper_symbol_lxr("kleene-plus expr"_str); // the sequence for toplevel 'VV*'
        auto b = add_helper_symbol_lxr("kleene-plus expr"_str); // the kleene-star for 'V*'
        auto c = add_helper_symbol_lxr("kleene-plus expr"_str); // the open subrule for 'V'
        begin_subrule(a, mode::sequence);
        next_nonterminal(c);
        begin_subrule(b, mode::kleene_star);
        begin_subrule(c, mode::sequence, 3);
    }
    else {
        auto a = add_helper_symbol_psr("kleene-plus expr"_str); // the sequence for toplevel 'VV*'
        auto b = add_helper_symbol_psr("kleene-plus expr"_str); // the kleene-star for 'V*'
        auto c = add_helper_symbol_psr("kleene-plus expr"_str); // the open subrule for 'V'
        begin_subrule(a, mode::sequence);
        next_nonterminal(c);
        begin_subrule(b, mode::kleene_star);
        begin_subrule(c, mode::sequence, 3);
    }
}

