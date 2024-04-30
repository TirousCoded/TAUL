

#include "grammar.h"

#include <stdexcept>

#include "asserts.h"
#include "symbol_set.h"

#include "internal/grammar_data.h"


using namespace taul::string_literals;


taul::lpr_ref::lpr_ref() 
    : lpr_ref(nullptr, nullptr) {}

taul::lpr_ref::lpr_ref(const lpr_ref& other)
    : lpr_ref(other._rule, other._gram) {}

taul::lpr_ref::lpr_ref(lpr_ref&& other) noexcept
    : lpr_ref() {
    std::swap(_rule, other._rule);
    std::swap(_gram, other._gram);
}

taul::lpr_ref& taul::lpr_ref::operator=(const lpr_ref& rhs) {
    _rule = rhs._rule;
    _gram = rhs._gram;
    return *this;
}

taul::lpr_ref& taul::lpr_ref::operator=(lpr_ref&& rhs) noexcept {
    if (&rhs != this) {
        std::swap(_rule, rhs._rule);
        std::swap(_gram, rhs._gram);
    }
    return *this;
}

taul::str taul::lpr_ref::name() const {
    return _rule ? _rule->name : (TAUL_DEADEND, taul::str{});
}

std::size_t taul::lpr_ref::index() const {
    return _rule ? _rule->index : (TAUL_DEADEND, std::size_t(-1));
}

taul::qualifier taul::lpr_ref::qualifier() const {
    return _rule ? _rule->qualifier : (TAUL_DEADEND, taul::qualifier{});
}

taul::symbol_id taul::lpr_ref::id() const noexcept {
    return symbol_traits<glyph>::first_nonterminal_id + symbol_id_num(index());
}

const taul::glyph_set& taul::lpr_ref::first_set() const noexcept {
    TAUL_ASSERT(_gram);
    TAUL_ASSERT(_gram->_lpr_pt.first_sets_A.contains(id()));
    return _gram->_lpr_pt.first_sets_A.at(id());
}

const taul::glyph_set& taul::lpr_ref::follow_set() const noexcept {
    TAUL_ASSERT(_gram);
    TAUL_ASSERT(_gram->_lpr_pt.follow_sets_A.contains(id()));
    return _gram->_lpr_pt.follow_sets_A.at(id());
}

const taul::glyph_set& taul::lpr_ref::prefix_set() const noexcept {
    TAUL_ASSERT(_gram);
    TAUL_ASSERT(_gram->_lpr_pt.prefix_sets_A.contains(id()));
    return _gram->_lpr_pt.prefix_sets_A.at(id());
}

bool taul::lpr_ref::equal(const lpr_ref& lhs, const lpr_ref& rhs) noexcept {
    return lhs._rule == rhs._rule;
}

std::string taul::lpr_ref::fmt() const {
    return _rule ? _rule->fmt() : (TAUL_DEADEND, "");
}

std::size_t taul::lpr_ref::hash() const noexcept {
    return std::hash<decltype(_rule)>{}(_rule);
}

taul::lpr_ref::lpr_ref(const internal::lexer_rule* a, const internal::grammar_data* b) noexcept 
    : _rule(a), 
    _gram(b) {}

taul::ppr_ref::ppr_ref()
    : ppr_ref(nullptr, nullptr) {}

taul::ppr_ref::ppr_ref(const ppr_ref& other)
    : ppr_ref(other._rule, other._gram) {}

taul::ppr_ref::ppr_ref(ppr_ref&& other) noexcept
    : ppr_ref() {
    std::swap(_rule, other._rule);
    std::swap(_gram, other._gram);
}

taul::ppr_ref& taul::ppr_ref::operator=(const ppr_ref& rhs) {
    _rule = rhs._rule;
    _gram = rhs._gram;
    return *this;
}

taul::ppr_ref& taul::ppr_ref::operator=(ppr_ref&& rhs) noexcept {
    if (&rhs != this) {
        std::swap(_rule, rhs._rule);
        std::swap(_gram, rhs._gram);
    }
    return *this;
}

taul::str taul::ppr_ref::name() const {
    return _rule ? _rule->name : (TAUL_DEADEND, taul::str{});
}

std::size_t taul::ppr_ref::index() const {
    return _rule ? _rule->index : (TAUL_DEADEND, std::size_t(-1));
}

taul::symbol_id taul::ppr_ref::id() const noexcept {
    return symbol_traits<token>::first_nonterminal_id + symbol_id_num(index());
}

const taul::token_set& taul::ppr_ref::first_set() const noexcept {
    TAUL_ASSERT(_gram);
    TAUL_ASSERT(_gram->_ppr_pt.first_sets_A.contains(id()));
    return _gram->_ppr_pt.first_sets_A.at(id());
}

const taul::token_set& taul::ppr_ref::follow_set() const noexcept {
    TAUL_ASSERT(_gram);
    TAUL_ASSERT(_gram->_ppr_pt.follow_sets_A.contains(id()));
    return _gram->_ppr_pt.follow_sets_A.at(id());
}

const taul::token_set& taul::ppr_ref::prefix_set() const noexcept {
    TAUL_ASSERT(_gram);
    TAUL_ASSERT(_gram->_ppr_pt.prefix_sets_A.contains(id()));
    return _gram->_ppr_pt.prefix_sets_A.at(id());
}

bool taul::ppr_ref::equal(const ppr_ref& lhs, const ppr_ref& rhs) noexcept {
    return lhs._rule == rhs._rule;
}

std::string taul::ppr_ref::fmt() const {
    return _rule ? _rule->fmt() : (TAUL_DEADEND, "");
}

std::size_t taul::ppr_ref::hash() const noexcept {
    return std::hash<decltype(_rule)>{}(_rule);
}

taul::ppr_ref::ppr_ref(const internal::parser_rule* a, const internal::grammar_data* b) noexcept
    : _rule(a),
    _gram(b) {}

bool taul::operator==(const lpr_ref& lhs, const lpr_ref& rhs) noexcept {
    return lpr_ref::equal(lhs, rhs);
}

bool taul::operator==(const ppr_ref& lhs, const ppr_ref& rhs) noexcept {
    return ppr_ref::equal(lhs, rhs);
}

bool taul::operator!=(const lpr_ref& lhs, const lpr_ref& rhs) noexcept {
    return !(lhs == rhs);
}

bool taul::operator!=(const ppr_ref& lhs, const ppr_ref& rhs) noexcept {
    return !(lhs == rhs);
}

taul::grammar::grammar(internal::grammar_data&& gramdat) 
    : _data(std::make_shared<internal::grammar_data>(std::forward<internal::grammar_data&&>(gramdat))) {}

taul::grammar::grammar()
    : _data(nullptr) {}

taul::grammar::grammar(const grammar& x)
    : _data(x._data) {}

taul::grammar::grammar(grammar&& x) noexcept
    : _data(std::move(x._data)) {}

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

std::size_t taul::grammar::lprs() const noexcept {
    return _data ? _data->_lprs.size() : 0;
}

std::size_t taul::grammar::pprs() const noexcept {
    return _data ? _data->_pprs.size() : 0;
}

std::size_t taul::grammar::nonsupport_lprs() const noexcept {
    std::size_t n = 0;
    for (const auto& I : _data->_lprs) {
        if (I.qualifier != qualifier::support) {
            n++;
        }
    }
    return n;
}

taul::lpr_ref taul::grammar::lpr_at(std::size_t index) const {
    if (!_data) throw std::out_of_range("lpr_at index out-of-range!");
    return lpr_ref(&(_data->_lprs.at(index)), _data.get());
}

taul::ppr_ref taul::grammar::ppr_at(std::size_t index) const {
    if (!_data) throw std::out_of_range("ppr_at index out-of-range!");
    return ppr_ref(&(_data->_pprs.at(index)), _data.get());
}

std::optional<taul::lpr_ref> taul::grammar::lpr(const str& name) const {
    if (!_data) {
        return std::nullopt;
    }
    const auto it = _data->_lookup.find(name);
    if (it == _data->_lookup.end() || !it->second.lpr) {
        return std::nullopt;
    }
    TAUL_IN_BOUNDS(it->second.index, 0, _data->_lprs.size());
    return std::make_optional(lpr_ref(&(_data->_lprs[it->second.index]), _data.get()));
}

std::optional<taul::ppr_ref> taul::grammar::ppr(const str& name) const {
    if (!_data) {
        return std::nullopt;
    }
    const auto it = _data->_lookup.find(name);
    if (it == _data->_lookup.end() || it->second.lpr) {
        return std::nullopt;
    }
    TAUL_IN_BOUNDS(it->second.index, 0, _data->_pprs.size());
    return std::make_optional(ppr_ref(&(_data->_pprs[it->second.index]), _data.get()));
}

bool taul::grammar::has_rule(const str& name) const noexcept {
    return _data ? _data->_lookup.contains(name) : false;
}

bool taul::grammar::has_lpr(const str& name) const noexcept {
    if (!_data) {
        return false;
    }
    const auto it = _data->_lookup.find(name);
    return it != _data->_lookup.end() && it->second.lpr;
}

bool taul::grammar::has_ppr(const str& name) const noexcept {
    if (!_data) {
        return false;
    }
    const auto it = _data->_lookup.find(name);
    return it != _data->_lookup.end() && !it->second.lpr;
}

bool taul::grammar::is_associated(lpr_ref rule) const noexcept {
    return rule._gram == _data.get();
}

bool taul::grammar::is_associated(ppr_ref rule) const noexcept {
    return rule._gram == _data.get();
}

std::string taul::grammar::fmt(const char* tab) const {
    TAUL_ASSERT(tab);
    return std::format(
        "grammar"
        "\n{0}lprs ({1}):{3}"
        "\n{0}pprs ({2}):{4}",
        tab, 
        lprs(), 
        pprs(),
        [&]() -> auto {
            std::string result{};
            if (_data) {
                for (const auto& I : _data->_lprs) {
                    result += std::format("\n{0}{0}{1}", tab, I.fmt());
                }
            }
            return result;
        }(),
        [&]() -> auto {
            std::string result{};
            if (_data) {
                for (const auto& I : _data->_pprs) {
                    result += std::format("\n{0}{0}{1}", tab, I.fmt());
                }
            }
            return result;
        }());
}

std::string taul::grammar::fmt_internals(const char* tab) const {
    std::string a{};
    if (_data) {
        a += std::format(
            "_data->_lpr_pt\n{}\n_data->_ppr_pt\n{}", 
            _data->_lpr_pt.fmt(tab), _data->_ppr_pt.fmt(tab));
    }
    return std::format(
        "{}\ngrammar (internals)\n{}", 
        *this, a);
}

const taul::internal::grammar_data& taul::internal::launder_grammar_data(const grammar& x) noexcept {
    TAUL_ASSERT(x._data);
    return *(x._data);
}
