

#include "context.h"

#include "internal/migrated/tokenize.h"


taul::context::context() 
    : nodectx(std::make_unique<taul::node_ctx>()) {}

taul::context::context(
    std::shared_ptr<logger> lgr)
    : nodectx(std::make_unique<taul::node_ctx>()),
    lgr(lgr) {}

taul::context::context(
    const grammar& gram, 
    std::shared_ptr<logger> lgr)
    : context(lgr) {
    bind(gram);
}

taul::context::context(context&& x) noexcept
    : context() {
    std::swap(lgr, x.lgr);
    std::swap(nodectx, x.nodectx);
    std::swap(gram, x.gram);
}

taul::context& taul::context::operator=(context&& rhs) noexcept {
    if (&rhs != this) {
        std::swap(lgr, rhs.lgr);
        std::swap(nodectx, rhs.nodectx);
        std::swap(gram, rhs.gram);
    }
    return *this;
}

taul::node taul::context::create(const lexer_rule& lpr, std::string_view str, source_pos pos) {
    return nodectx->create(lpr, str, pos);
}

taul::node taul::context::create(token tkn) {
    return nodectx->create(tkn);
}

taul::node taul::context::create(const parser_rule& ppr, std::string_view str, source_pos pos) {
    return nodectx->create(ppr, str, pos);
}

void taul::context::bind(
    const grammar& gram) {
    this->gram = std::make_optional<grammar>(gram);
}

taul::token taul::context::match(
    std::string_view start_rule, 
    std::string_view input, 
    source_pos offset) {
    _check_grammar_bound();
    _check_has_lpr_start_rule(start_rule);
    return gram->lexer(start_rule)(input, offset, lgr);
}

taul::token taul::context::match(
    std::string_view input, 
    source_pos offset) {
    _check_grammar_bound();
    return gram->full_lexer()(input, offset, lgr);
}

taul::token_seq taul::context::tokenize(
    std::string_view start_rule, 
    std::string_view input, 
    source_pos offset) {
    _check_grammar_bound();
    _check_has_lpr_start_rule(start_rule);
    return taul::internal::tokenize(gram->lexer(start_rule), input, offset);
}

taul::token_seq taul::context::tokenize(
    std::string_view input, 
    source_pos offset,
    bool cut_skip_tokens) {
    _check_grammar_bound();
    return taul::internal::tokenize(gram->full_lexer(cut_skip_tokens), input, offset);
}

std::optional<taul::node> taul::context::parse(
    std::string_view start_rule, 
    const taul::token_seq& input, 
    std::size_t offset) {
    _check_grammar_bound();
    _check_has_ppr_start_rule(start_rule);
    return gram->parser(start_rule)(*nodectx, input, offset, lgr);
}

std::optional<taul::node> taul::context::parse(
    std::string_view start_rule, 
    std::string_view input, 
    source_pos offset) {
    //_check_grammar_bound();
    //_check_has_ppr_start_rule(start_rule);
    return parse(start_rule, tokenize(input, offset));
}

void taul::context::_check_grammar_bound() const {
    if (!(bool)gram) {
        throw taul::no_grammar_bound_error("no grammar bound!");
    }
}

void taul::context::_check_has_lpr_start_rule(std::string_view start_rule) const {
    if (!gram->contains_lpr(start_rule)) {
        throw taul::lpr_not_found_error("lpr with start_rule not found!");
    }
}

void taul::context::_check_has_ppr_start_rule(std::string_view start_rule) const {
    if (!gram->contains_ppr(start_rule)) {
        throw taul::ppr_not_found_error("ppr with start_rule not found!");
    }
}

