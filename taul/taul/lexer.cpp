

#include "lexer.h"

#include "asserts.h"


taul::token taul::default_lexer_function(
    const std::shared_ptr<lexer_state>&,
    std::string_view,
    source_pos,
    const std::shared_ptr<logger>&) {
    return token{};
}

taul::lexer_function taul::internal::get_lexer_f(const lexer& x) noexcept {
    return x._f;
}

taul::lexer::lexer(
    lexer_function f, 
    const std::shared_ptr<lexer_state>& state) 
    : _f(f), 
    _state(state) {
    TAUL_ASSERT(f);
}

taul::lexer::lexer() 
    : lexer(default_lexer_function) {}

taul::lexer::lexer(const lexer& x) 
    : lexer(x._f, x._state) {}

taul::lexer::lexer(lexer&& x) noexcept
    : lexer() {
    std::swap(_f, x._f);
    std::swap(_state, x._state);
}

taul::lexer& taul::lexer::operator=(const lexer& rhs) {
    _f = rhs._f;
    _state = rhs._state;
    return *this;
}

taul::lexer& taul::lexer::operator=(lexer&& rhs) noexcept {
    if (&rhs != this) {
        std::swap(_f, rhs._f);
        std::swap(_state, rhs._state);
    }
    return *this;
}

taul::token taul::lexer::operator()(std::string_view txt, const std::shared_ptr<logger>& lgr) const {
    return (*this)(txt, 0, lgr);
}

taul::token taul::lexer::operator()(std::string_view txt, source_pos offset, const std::shared_ptr<logger>& lgr) const {
    TAUL_IN_BOUNDS(offset, 0, txt.length() + 1);
    token result{};
    if (_f) {
        result = _f(_state, txt, offset, lgr);
    }
    else TAUL_DEADEND;
    return result;
}

