

#include "spec_lowerer.h"


#define _DUMP_LOG 0
#define _DUMP_FINAL_LLSPEC 0

#if _DUMP_FINAL_LLSPEC
#include "../disassemble_spec.h"
#endif


taul::internal::spec_lowerer::spec_lowerer(source_pos& pos)
    : pos_ptr(&pos) {}

void taul::internal::spec_lowerer::cancel() {
    cancelled = true;
}

taul::internal::llspec_writer& taul::internal::spec_lowerer::target() {
    return deref_assert(target_ptr);
}

void taul::internal::spec_lowerer::bind_target(llspec_writer& new_target) {
    target_ptr = &new_target;
}

void taul::internal::spec_lowerer::reset_target() {
    bind_target(output);
}

void taul::internal::spec_lowerer::reset_depth() {
    depth = 0;
}

void taul::internal::spec_lowerer::begin_composite_expr() {
    depth++;
}

void taul::internal::spec_lowerer::end_composite_expr() {
    depth--;
}

bool taul::internal::spec_lowerer::at_rule_depth() {
    return depth == 1;
}

void taul::internal::spec_lowerer::reset_rule_subexprs() {
    rule_subexprs = 0;
}

void taul::internal::spec_lowerer::acknowledge_subexpr() {
    rule_subexprs++;
}

void taul::internal::spec_lowerer::bind_rule_name(std::string_view name) {
    rule_name = name;
}

bool taul::internal::spec_lowerer::is_self_ref(std::string_view name) {
    return name == rule_name;
}

void taul::internal::spec_lowerer::reset_in_preced_ppr_flag() {
    in_preced_ppr_flag = false;
}

void taul::internal::spec_lowerer::consider_next_alt_a_base_alt_until_discerned_otherwise() {
    in_base_alt = true;
}

bool taul::internal::spec_lowerer::in_recurse_alt() {
    return !in_base_alt;
}

void taul::internal::spec_lowerer::handle_begin_preced_ppr(std::string_view name, qualifier qualifier) {
    if (qualifier != precedence) return;
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> handle_begin_preced_ppr({}, {})", std::string(name), qualifier);
#endif
    TAUL_ASSERT(!in_preced_ppr_flag);
    TAUL_ASSERT(at_rule_depth());
    in_preced_ppr_flag = true;
    base_alts.clear();
    recurse_alts.clear();
    bind_target(temp);
    reset_current_preced();
    consider_next_alt_a_base_alt_until_discerned_otherwise();
}

void taul::internal::spec_lowerer::handle_end_preced_ppr() {
    if (!in_preced_ppr_flag) return;
    if (!at_rule_depth()) return; // we only care for PPR itself
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> handle_end_preced_ppr");
#endif
    push_temp_to_base_or_recurse_alts_vector(); // can't forget to push last alt
    in_preced_ppr_flag = false;
    reset_target(); // resume normal writing
    // w/ everything else done, perform final *rewrite*
    TAUL_ASSERT(base_alts.size() >= 1);
    auto write_alts =
        [&](const std::vector<llspec>& alts) {
        bool first = true;
        for (const auto& I : alts) {
            if (!first) target().pos(pos()).alternative();
            target().write_spec(I);
            first = false;
        }
        };
    if (recurse_alts.empty()) { // special case for when we have no recurse alts
        write_alts(base_alts);
    }
    else {
        target().pos(pos()).sequence();
        write_alts(base_alts);
        target().pos(pos()).close();
        target().pos(pos()).kleene_star();
        target().pos(pos()).sequence();
        write_alts(recurse_alts);
        target().pos(pos()).close();
        target().pos(pos()).close();
        target().pos(pos()).pylon(); // on_name writes the predicates
    }
}

void taul::internal::spec_lowerer::handle_preced_ppr_alt() {
    if (!in_preced_ppr_flag) return;
    if (!at_rule_depth()) return; // we only care for PPR itself
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "-> handle_preced_ppr_alt");
#endif
    push_temp_to_base_or_recurse_alts_vector();
    incr_current_preced();
    consider_next_alt_a_base_alt_until_discerned_otherwise();
}

void taul::internal::spec_lowerer::push_temp_to_base_or_recurse_alts_vector() {
    // if this is for recurse alt, and we have only one subexpr, then this is the special
    // case in which we just want to forgo acknowledging the alt altogether
    if (in_recurse_alt() && rule_subexprs == 1) {
#if _DUMP_LOG
        TAUL_LOG(make_stderr_logger(), "-> dumping temp");
#endif
        (void)temp.done(); // reset temp
        return;
    }
#if _DUMP_LOG
    if (in_base_alt)    TAUL_LOG(make_stderr_logger(), "-> pushing temp -> base_alts");
    else                TAUL_LOG(make_stderr_logger(), "-> pushing temp -> recurse_alts");
#endif
    // push finished llspec to base/recurse alts vector, resetting temp for next alt
    (in_base_alt ? base_alts : recurse_alts).push_back(temp.done());
}

void taul::internal::spec_lowerer::reset_next_is_prefix_ref_flag() {
    next_is_prefix_ref_flag = false;
}

void taul::internal::spec_lowerer::reset_current_preced() {
    current_preced = 0;
}

void taul::internal::spec_lowerer::incr_current_preced() {
    current_preced++;
}

void taul::internal::spec_lowerer::on_startup() {
    cancelled = false;
    reset_target();
    reset_depth();
    reset_rule_subexprs();
    reset_in_preced_ppr_flag();
    reset_next_is_prefix_ref_flag();
    reset_current_preced();
}

void taul::internal::spec_lowerer::on_shutdown() {
#if _DUMP_FINAL_LLSPEC
    // copy output so we don't call done on the original
    auto final_llspec = llspec_writer(output).done();
    std::cerr << std::format("dumping final llspec...\n{}\n", internal::disassemble_llspec(final_llspec));

#endif
    // do nothing
}

void taul::internal::spec_lowerer::on_close() {
    if (cancelled) return;
    handle_end_preced_ppr();
    end_composite_expr();
    target().pos(pos()).close();
}

void taul::internal::spec_lowerer::on_alternative() {
    if (cancelled) return;
    if (at_rule_depth()) reset_rule_subexprs();
    handle_preced_ppr_alt();
    const bool alt_of_preced_ppr_expr = in_preced_ppr_flag && at_rule_depth();
    if (!alt_of_preced_ppr_expr) target().pos(pos()).alternative(); // precedence PPR alts are written elsewhere
}

void taul::internal::spec_lowerer::on_lpr_decl(std::string_view name) {
    if (cancelled) return;
    target().pos(pos()).lpr_decl(name);
}

void taul::internal::spec_lowerer::on_ppr_decl(std::string_view name) {
    if (cancelled) return;
    target().pos(pos()).ppr_decl(name);
}

void taul::internal::spec_lowerer::on_lpr(std::string_view name, qualifier qualifier) {
    if (cancelled) return;
    reset_rule_subexprs();
    bind_rule_name(name);
    target().pos(pos()).lpr(name, qualifier);
    begin_composite_expr();
}

void taul::internal::spec_lowerer::on_ppr(std::string_view name, qualifier qualifier) {
    if (cancelled) return;
    reset_rule_subexprs();
    bind_rule_name(name);
    target().pos(pos()).ppr(name, qualifier);
    begin_composite_expr();
    handle_begin_preced_ppr(name, qualifier);
}

void taul::internal::spec_lowerer::on_end() {
    if (cancelled) return;
    acknowledge_subexpr();
    target().pos(pos()).end();
}

void taul::internal::spec_lowerer::on_any() {
    if (cancelled) return;
    acknowledge_subexpr();
    target().pos(pos()).any();
}

void taul::internal::spec_lowerer::on_string(std::string_view s) {
    if (cancelled) return;
    acknowledge_subexpr();
    target().pos(pos()).string(s);
}

void taul::internal::spec_lowerer::on_charset(std::string_view s) {
    if (cancelled) return;
    acknowledge_subexpr();
    target().pos(pos()).charset(s);
}

void taul::internal::spec_lowerer::on_token() {
    if (cancelled) return;
    acknowledge_subexpr();
    target().pos(pos()).token();
}

void taul::internal::spec_lowerer::on_failure() {
    if (cancelled) return;
    acknowledge_subexpr();
    target().pos(pos()).failure();
}

void taul::internal::spec_lowerer::on_name(std::string_view name, std::optional<spec_opcode> peek) {
    if (cancelled) return;
    acknowledge_subexpr();
    if (next_is_prefix_ref_flag) { // don't write this name expr if we're the prefix-ref
        target().pos(pos()).preced_pred(current_preced, signal_preced_val); // write preced_pred before anything else in alt
        in_base_alt = false; // we know we're in a recurse alt
    }
    else { // non-prefix-ref
        const bool self_ref =
            in_preced_ppr_flag &&
            is_self_ref(name);
        // if self_ref == true, suffix_ref == true means this name expr is a suffix-ref
        const bool suffix_ref =
            at_rule_depth() &&
            (peek == spec_opcode::close || peek == spec_opcode::alternative);
        const auto preced_val =
            self_ref
            ? (suffix_ref ? current_preced + 1 : 0)
            : no_preced_val;
        target().pos(pos()).name(name, preced_val);
    }
    reset_next_is_prefix_ref_flag();
}

void taul::internal::spec_lowerer::on_sequence() {
    if (cancelled) return;
    acknowledge_subexpr();
    target().pos(pos()).sequence();
    begin_composite_expr();
}

void taul::internal::spec_lowerer::on_lookahead() {
    if (cancelled) return;
    acknowledge_subexpr();
    target().pos(pos()).lookahead();
    begin_composite_expr();
}

void taul::internal::spec_lowerer::on_lookahead_not() {
    if (cancelled) return;
    acknowledge_subexpr();
    target().pos(pos()).lookahead_not();
    begin_composite_expr();
}

void taul::internal::spec_lowerer::on_not() {
    if (cancelled) return;
    acknowledge_subexpr();
    target().pos(pos()).not0();
    begin_composite_expr();
}

void taul::internal::spec_lowerer::on_optional() {
    if (cancelled) return;
    acknowledge_subexpr();
    target().pos(pos()).optional();
    begin_composite_expr();
}

void taul::internal::spec_lowerer::on_kleene_star() {
    if (cancelled) return;
    acknowledge_subexpr();
    target().pos(pos()).kleene_star();
    begin_composite_expr();
}

void taul::internal::spec_lowerer::on_kleene_plus() {
    if (cancelled) return;
    acknowledge_subexpr();
    target().pos(pos()).kleene_plus();
    begin_composite_expr();
}

void taul::internal::spec_lowerer::detect_prefix_ref() {
    next_is_prefix_ref_flag = true;
}

