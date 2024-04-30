

#include "grammar_builder.h"

#include "../asserts.h"


taul::internal::grammar_builder::grammar_builder(
    rule_pt_translator& rule_pt_trans) 
    : rule_pt_trans(&rule_pt_trans) {}

void taul::internal::grammar_builder::cancel() noexcept {
    TAUL_DEREF_SAFE(rule_pt_trans) rule_pt_trans->cancel();
    cancelled = true;
}

void taul::internal::grammar_builder::add_lpr_decl(str name) {
    if (cancelled) return;
    TAUL_ASSERT(!lpr_index_map.contains(name));
    lpr_index_map[name] = data._lprs.size();
    data._lprs.push_back(
        lexer_rule{
            .name = taul::str(name),
            .index = data._lprs.size(),
        });
}

void taul::internal::grammar_builder::add_ppr_decl(str name) {
    if (cancelled) return;
    TAUL_ASSERT(!ppr_index_map.contains(name));
    ppr_index_map[name] = data._pprs.size();
    data._pprs.push_back(
        parser_rule{
            .name = taul::str(name),
            .index = data._pprs.size(),
        });
}

void taul::internal::grammar_builder::add_lpr(str name, qualifier q) {
    if (cancelled) return;
    TAUL_ASSERT(lpr_index_map.contains(name));
    TAUL_ASSERT(data._lprs.size() > lpr_index_map.at(name));
    data._lprs[lpr_index_map.at(name)].qualifier = q;
}

void taul::internal::grammar_builder::add_ppr(str name, qualifier) {
    if (cancelled) return;
    TAUL_ASSERT(ppr_index_map.contains(name));
    TAUL_ASSERT(data._pprs.size() > ppr_index_map.at(name));
}

const taul::internal::parse_table_build_details<taul::glyph>& taul::internal::grammar_builder::get_lpr_diagnostics() const noexcept {
    TAUL_ASSERT(rule_pt_trans);
    return rule_pt_trans->lexer_ptbd;
}

const taul::internal::parse_table_build_details<taul::token>& taul::internal::grammar_builder::get_ppr_diagnostics() const noexcept {
    TAUL_ASSERT(rule_pt_trans);
    return rule_pt_trans->parser_ptbd;
}

std::optional<taul::grammar> taul::internal::grammar_builder::get_result() {
    if (cancelled) return std::nullopt;
    data.build_lookup();
    TAUL_DEREF_SAFE(rule_pt_trans) {
        data._lpr_id_allocs = std::move(rule_pt_trans->lexer_id_alloc.output);
        data._ppr_id_allocs = std::move(rule_pt_trans->parser_id_alloc.output);
        data._lpr_pt = std::move(rule_pt_trans->lexer_pt);
        data._ppr_pt = std::move(rule_pt_trans->parser_pt);
    }
    return std::make_optional(grammar(std::move(data)));
}

