

#include "token_seq.h"


taul::token_seq::token_seq(std::string_view s) 
    : _s(s), 
    _tkns(), 
    _readpos(0) {}

taul::token_seq::token_seq(const token_seq& other)
    : _s(other._s),
    _tkns(other._tkns),
    _readpos(other._readpos) {}

taul::token_seq::token_seq(token_seq&& other) noexcept
    : _s(std::move(other._s)),
    _tkns(std::move(other._tkns)),
    _readpos(std::move(other._readpos)) {}

taul::token_seq::~token_seq() noexcept {}

taul::token_seq& taul::token_seq::operator=(const token_seq& rhs) {
    _s = rhs._s;
    _tkns = rhs._tkns;
    _readpos = rhs._readpos;
    return *this;
}

taul::token_seq& taul::token_seq::operator=(token_seq&& rhs) noexcept {
    if (&rhs != this) {
        _s = std::move(rhs._s);
        _tkns = std::move(rhs._tkns);
        _readpos = std::move(rhs._readpos);
    }
    return *this;
}

std::string_view taul::token_seq::str() const noexcept {
    return _s;
}

std::size_t taul::token_seq::size() const noexcept {
    return _tkns.size();
}

bool taul::token_seq::has_size() const noexcept {
    return size() > 0;
}

bool taul::token_seq::empty() const noexcept {
    return !has_size();
}

const taul::token* taul::token_seq::data() const {
    return 
        has_size() 
        ? _tkns.data() 
        : nullptr;
}

const taul::token& taul::token_seq::at(std::size_t ind) const {
    // this'll throw for us
    return _tkns.at(ind);
}

const taul::token& taul::token_seq::operator[](std::size_t ind) const {
    return at(ind);
}

const taul::token& taul::token_seq::back() const {
    return at(size() - 1);
}

std::string_view taul::token_seq::range_str(std::size_t ind, std::size_t n) const {
    if (!_in_bounds(ind, n)) {
        throw std::out_of_range("range_str specified range out-of-bounds!");
    }
    std::size_t first_ind = at(ind).pos();
    std::size_t last_ind =
        n > 0
        ? at(ind + n - 1).pos() + at(ind + n - 1).str().length()
        : first_ind;
    return str().substr(first_ind, last_ind - first_ind);
}

taul::token_seq::const_iterator taul::token_seq::begin() const noexcept {
    return cbegin();
}

taul::token_seq::const_iterator taul::token_seq::cbegin() const noexcept {
    return _tkns.cbegin();
}

taul::token_seq::const_iterator taul::token_seq::end() const noexcept {
    return cend();
}

taul::token_seq::const_iterator taul::token_seq::cend() const noexcept {
    return _tkns.cend();
}

taul::token_seq::const_reverse_iterator taul::token_seq::rbegin() const noexcept {
    return crbegin();
}

taul::token_seq::const_reverse_iterator taul::token_seq::crbegin() const noexcept {
    return _tkns.crbegin();
}

taul::token_seq::const_reverse_iterator taul::token_seq::rend() const noexcept {
    return crend();
}

taul::token_seq::const_reverse_iterator taul::token_seq::crend() const noexcept {
    return _tkns.crend();
}

taul::source_pos taul::token_seq::read_pos() const noexcept {
    return _readpos;
}

taul::source_pos taul::token_seq::end_pos() const noexcept {
    return source_pos(str().length());
}

bool taul::token_seq::at_end() const noexcept {
    return read_pos() == end_pos();
}

void taul::token_seq::skip(
    std::uint32_t len) {
    // ensure we provide a strong guarantee
    if (end_pos() - read_pos() < len) {
        throw std::out_of_range("skip would exit bounds of source string!");
    }
    _readpos += len;
}

void taul::token_seq::undo_skips() noexcept {
    if (empty()) {
        _readpos = 0;
    }
    else {
        const token b = back();
        _readpos = b.pos() + source_pos(b.str().length());
    }
}

void taul::token_seq::push(
    const lexer_rule& lpr,
    std::uint32_t len) {
    // ensure we provide a strong guarantee
    if (end_pos() - read_pos() < len) {
        throw std::out_of_range("push would exit bounds of source string!");
    }
    token tkn(lpr, str().substr(read_pos(), len), read_pos());
    _readpos += len;
    _tkns.push_back(std::move(tkn));
}

void taul::token_seq::push_failure(
    std::uint32_t len) {
    // ensure we provide a strong guarantee
    if (end_pos() - read_pos() < len) {
        throw std::out_of_range("push_failure would exit bounds of source string!");
    }
    token tkn = token::failure(str().substr(read_pos(), len), read_pos());
    _readpos += len;
    _tkns.push_back(std::move(tkn));
}

void taul::token_seq::pop(
    std::size_t n) noexcept {
    if (n > size()) {
        n = size();
    }
    while (n > 0) {
        _readpos = _tkns.back().pos();
        _tkns.pop_back();
        n--;
    }
}

void taul::token_seq::pop_all() noexcept {
    pop(size());
}

void taul::token_seq::reset(std::string_view s) {
    _s = s;
    _tkns.clear();
    _readpos = 0;
}

std::string taul::token_seq::fmt(const char* tab) const {
    std::string result{};
    result += std::format("token seq. (size {})", size());
    std::size_t ind = 0;
    for (const auto& I : *this) {
        result += std::format("\n{}(index {}) {}", tab, ind, I);
        ind++;
    }
    return result;
}

bool taul::token_seq::_in_bounds(std::size_t ind, std::size_t n) const noexcept {
    return 
        ind <= size() &&
        n <= size() - ind;
}

