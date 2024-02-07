

#include "parser.h"

#include "../../asserts.h"
#include "../../node.h"


std::optional<taul::node> taul::internal::default_parser_function(
    node_ctx&,
    const std::shared_ptr<parser_state>&,
    const taul::token_seq&,
    std::size_t,
    const std::shared_ptr<logger>&) {
    return std::nullopt;
}

taul::internal::parser_function taul::internal::get_parser_f(const parser& x) noexcept {
    return x._f;
}

taul::internal::parser_state* taul::internal::get_parser_state(const parser& x) noexcept {
    return x._state.get();
}

taul::internal::parser::parser(
    parser_function f,
    const std::shared_ptr<parser_state>& state)
    : _f(f),
    _state(state) {
    TAUL_ASSERT(f);
}

taul::internal::parser::parser()
    : parser(taul::internal::default_parser_function) {}

taul::internal::parser::parser(const parser& x)
    : parser(x._f, x._state) {}

taul::internal::parser::parser(parser&& x) noexcept
    : parser() {
    std::swap(_f, x._f);
    std::swap(_state, x._state);
}

taul::internal::parser& taul::internal::parser::operator=(const parser& rhs) {
    _f = rhs._f;
    _state = rhs._state;
    return *this;
}

taul::internal::parser& taul::internal::parser::operator=(parser&& rhs) noexcept {
    if (&rhs != this) {
        std::swap(_f, rhs._f);
        std::swap(_state, rhs._state);
    }
    return *this;
}

std::optional<taul::node> taul::internal::parser::operator()(
    node_ctx& ctx,
    const taul::token_seq& tkns,
    const std::shared_ptr<logger>& lgr) const {
    return (*this)(ctx, tkns, 0, lgr);
}

std::optional<taul::node> taul::internal::parser::operator()(
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

