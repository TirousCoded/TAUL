

#include "token.h"

#include "asserts.h"
#include "rules.h"


taul::token::token(const lexer_rule& lpr, std::string_view str, source_pos pos) noexcept 
    : _lpr(&lpr), 
    _str(str), 
    _pos(pos) {}

taul::token::token()
    : _lpr(nullptr),
    _str(""),
    _pos(0) {}

taul::token::token(const token& x)
    : _lpr(x._lpr),
    _str(x._str),
    _pos(x._pos) {}

taul::token::token(token&& x) noexcept
    : _lpr(std::move(x._lpr)),
    _str(std::move(x._str)),
    _pos(std::move(x._pos)) {}

taul::token& taul::token::operator=(const token& rhs) {
    _lpr = rhs._lpr;
    _str = rhs._str;
    _pos = rhs._pos;
    return *this;
}

taul::token& taul::token::operator=(token&& rhs) noexcept {
    if (&rhs != this) {
        _lpr = std::move(rhs._lpr);
        _str = std::move(rhs._str);
        _pos = std::move(rhs._pos);
    }
    return *this;
}

bool taul::token::is_failure() const noexcept {
    return !_lpr;
}

const taul::lexer_rule& taul::token::lpr() const {
    if (_lpr) {
        return *_lpr;
    }
    else {
        TAUL_ASSERT(is_failure());
        throw taul::lpr_association_error("failure token has no associated lpr!");
    }
}

std::string_view taul::token::str() const noexcept {
    return _str;
}

taul::source_pos taul::token::pos() const noexcept {
    return _pos;
}

bool taul::token::equal(const token& other) const noexcept {
    return 
        _lpr == other._lpr && 
        _str == other._str && 
        _pos == other._pos;
}

std::string taul::token::fmt() const {
    return
        is_failure() 
        ? std::format("[(lpr) ({}) <failure-token> \"{}\"]", pos(), (std::string)str()) 
        : std::format("[(lpr) ({}) {} \"{}\"]", pos(), lpr().name, (std::string)str());
}

taul::token taul::token::failure(std::string_view str, source_pos pos) noexcept {
    token result{};
    TAUL_ASSERT(result.is_failure());
    result._str = str;
    result._pos = pos;
    return result;
}

