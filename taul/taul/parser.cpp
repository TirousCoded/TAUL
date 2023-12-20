

#include "parser.h"

#include "asserts.h"
#include "node.h"


std::optional<taul::node> taul::default_parser_function(
    node_ctx&,
    const std::shared_ptr<parser_state>&,
    const taul::token_seq&,
    std::size_t,
    const std::shared_ptr<logger>&) {
    return std::nullopt;
}

taul::parser_function taul::internal::get_parser_f(const parser& x) noexcept {
    return x._f;
}

taul::parser_state* taul::internal::get_parser_state(const parser& x) noexcept {
    return x._state.get();
}

taul::parser::parser(
    parser_function f, 
    const std::shared_ptr<parser_state>& state) 
    : _f(f), 
    _state(state) {
    TAUL_ASSERT(f);
}

taul::parser::parser() 
    : parser(taul::default_parser_function) {}

taul::parser::parser(const parser& x) 
    : parser(x._f, x._state) {}

taul::parser::parser(parser&& x) noexcept 
    : parser() {
    std::swap(_f, x._f);
    std::swap(_state, x._state);
}

taul::parser& taul::parser::operator=(const parser& rhs) {
    _f = rhs._f;
    _state = rhs._state;
    return *this;
}

taul::parser& taul::parser::operator=(parser&& rhs) noexcept {
    if (&rhs != this) {
        std::swap(_f, rhs._f);
        std::swap(_state, rhs._state);
    }
    return *this;
}

std::optional<taul::node> taul::parser::operator()(
    node_ctx& ctx,
    const taul::token_seq& tkns, 
    const std::shared_ptr<logger>& lgr) const {
    return (*this)(ctx, tkns, 0, lgr);
}

std::optional<taul::node> taul::parser::operator()(
    node_ctx& ctx,
    const taul::token_seq& tkns, 
    std::size_t offset, 
    const std::shared_ptr<logger>& lgr) const {
    TAUL_IN_BOUNDS(offset, 0, tkns.size() + 1);
    std::optional<node> result{};
    if (_f) {
        result = _f(ctx, _state, tkns, offset, lgr);
    }
    else TAUL_DEADEND;
    return result;
}

