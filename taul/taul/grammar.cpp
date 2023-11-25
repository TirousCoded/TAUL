

#include "grammar.h"

#include <stdexcept>

#include "asserts.h"


taul::grammar::grammar()
    : _data(nullptr) {}

taul::grammar::grammar(const grammar& x)
    : _data(x._data) {}

taul::grammar::grammar(grammar&& x) noexcept
    : _data(std::move(x._data)) {
}

taul::grammar& taul::grammar::operator=(const grammar& rhs) {
    _data = rhs._data;
    return *this;
}

taul::grammar& taul::grammar::operator=(grammar&& rhs) noexcept {
    if (&rhs != this) {
        _data = std::move(rhs._data);
    }
    return *this;
}

taul::grammar::grammar(
    internal::for_internal_use_tag, 
    std::shared_ptr<internal::grammar_data> data) 
    : _data(std::move(data)) {
    TAUL_ASSERT(this->_data);
}

taul::bias taul::grammar::bias() const noexcept {
    return _data ? _data->_bias : bias::first_longest;
}

std::span<const taul::lexer_rule> taul::grammar::lprs() const noexcept {
    return _data ? std::span{ _data->_lprs.begin(), _data->_lprs.size() } : std::span<const lexer_rule>{};
}

std::span<const taul::parser_rule> taul::grammar::pprs() const noexcept {
    return _data ? std::span{ _data->_pprs.begin(), _data->_pprs.size() } : std::span<const parser_rule>{};
}

const taul::lexer_rule& taul::grammar::lpr(const std::string& name) const {
    return lpr(std::string_view(name));
}

const taul::lexer_rule& taul::grammar::lpr(std::string_view name) const {
    if (!_data) {
        throw std::out_of_range("lpr not found!");
    }
    const auto it = _data->_lookup.find(name);
    if (it == _data->_lookup.end() || !it->second.lpr) {
        throw std::out_of_range("lpr not found!");
    }
    TAUL_IN_BOUNDS(it->second.index, 0, _data->_lprs.size());
    return _data->_lprs[it->second.index];
}

const taul::lexer_rule& taul::grammar::lpr(const char* name) const {
    TAUL_ASSERT(name != nullptr);
    return lpr(std::string_view(name));
}

const taul::parser_rule& taul::grammar::ppr(const std::string& name) const {
    return ppr(std::string_view(name));
}

const taul::parser_rule& taul::grammar::ppr(std::string_view name) const {
    if (!_data) {
        throw std::out_of_range("ppr not found!");
    }
    const auto it = _data->_lookup.find(name);
    if (it == _data->_lookup.end() || it->second.lpr) {
        throw std::out_of_range("ppr not found!");
    }
    TAUL_IN_BOUNDS(it->second.index, 0, _data->_pprs.size());
    return _data->_pprs[it->second.index];
}

const taul::parser_rule& taul::grammar::ppr(const char* name) const {
    TAUL_ASSERT(name != nullptr);
    return ppr(std::string_view(name));
}

taul::lexer taul::grammar::lexer(const std::string& name) const {
    return lexer(std::string_view(name));
}

taul::lexer taul::grammar::lexer(std::string_view name) const {
    // lpr will handle throwing
    return lpr(name).lexer();
}

taul::lexer taul::grammar::lexer(const char* name) const {
    TAUL_ASSERT(name != nullptr);
    return lexer(std::string_view(name));
}

bool taul::grammar::contains(const std::string& name) const noexcept {
    return contains(std::string_view(name));
}

bool taul::grammar::contains(std::string_view name) const noexcept {
    return _data ? _data->_lookup.contains(name) : false;
}

bool taul::grammar::contains(const char* name) const noexcept {
    TAUL_ASSERT(name != nullptr);
    return contains(std::string_view(name));
}

bool taul::grammar::contains_lpr(const std::string& name) const noexcept {
    return contains_lpr(std::string_view(name));
}

bool taul::grammar::contains_lpr(std::string_view name) const noexcept {
    if (!_data) {
        return false;
    }
    const auto it = _data->_lookup.find(name);
    return it != _data->_lookup.end() && it->second.lpr;
}

bool taul::grammar::contains_lpr(const char* name) const noexcept {
    TAUL_ASSERT(name != nullptr);
    return contains_lpr(std::string_view(name));
}

bool taul::grammar::contains_ppr(const std::string& name) const noexcept {
    return contains_ppr(std::string_view(name));
}

bool taul::grammar::contains_ppr(std::string_view name) const noexcept {
    if (!_data) {
        return false;
    }
    const auto it = _data->_lookup.find(name);
    return it != _data->_lookup.end() && !it->second.lpr;
}

bool taul::grammar::contains_ppr(const char* name) const noexcept {
    TAUL_ASSERT(name != nullptr);
    return contains_ppr(std::string_view(name));
}

std::string taul::grammar::fmt(std::string_view tab) const {
    std::string result{};
    result += "taul::grammar (details)";
    result += std::format("\n{}bias: {}", tab, bias());
    result += std::format("\n{}lprs ({}):", tab, lprs().size());
    for (const auto& I : lprs()) {
        result += std::format("\n{0}{0}[{1}] {2}", tab, I.index, I.name);
    }
    result += std::format("\n{}pprs ({}):", tab, pprs().size());
    for (const auto& I : pprs()) {
        result += std::format("\n{0}{0}[{1}] {2}", tab, I.index, I.name);
    }
    return result;
}

void taul::internal::grammar_data::build_lookup() {
    TAUL_ASSERT(_lookup.empty());
    _lookup.reserve(_lprs.size() + _pprs.size());
    for (const auto& I : _lprs) {
        _lookup[I.name] = entry{ true, I.index };
    }
    for (const auto& I : _pprs) {
        _lookup[I.name] = entry{ false, I.index };
    }
    // ensure no lprs/pprs shared names
    TAUL_ASSERT(_lookup.size() == _lprs.size() + _pprs.size());
}
