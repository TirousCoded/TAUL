

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
    return _data ? _data->_bias : bias::fl;
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

taul::lexer taul::grammar::full_lexer(bool cut_skip_tokens) const {
    // if we're a default ctor grammar, return default ctor lexer as dummy
    if (!_data) {
        return taul::lexer();
    }
    return
        cut_skip_tokens
        ? taul::lexer(taul::internal::grammar_wide_lexer_function_cut_skip_tokens, _data->_gwls)
        : taul::lexer(taul::internal::grammar_wide_lexer_function_dont_cut_skip_tokens, _data->_gwls);
}

taul::grammar::operator taul::lexer() const {
    return full_lexer();
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

taul::parser taul::grammar::parser(const std::string& name) const {
    return parser(std::string_view(name));
}

taul::parser taul::grammar::parser(std::string_view name) const {
    // ppr will handle throwing
    return ppr(name).parser();
}

taul::parser taul::grammar::parser(const char* name) const {
    TAUL_ASSERT(name != nullptr);
    return parser(std::string_view(name));
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

std::size_t taul::grammar::nonsupport_lprs() const noexcept {
    std::size_t n = 0;
    for (const auto& I : lprs()) {
        if (I.qualifer != taul::qualifier::support) {
            n++;
        }
    }
    return n;
}

std::string taul::grammar::fmt(std::string_view tab) const {
    return std::format(
        "taul::grammar (details)"
        "\n{0}bias: {1}"
        "\n{0}lprs ({2}):"
        "{4}"
        "\n{0}pprs ({3}):"
        "{5}",
        tab, bias(),
        lprs().size(), 
        pprs().size(),
        [&]() -> auto {
            std::string result{};
            for (const auto& I : lprs()) {
                result += std::format("\n{0}{0}[{1}] {2}", tab, I.index, I.name);
            }
            return result;
        }(),
        [&]() -> auto {
            std::string result{};
            for (const auto& I : pprs()) {
                result += std::format("\n{0}{0}[{1}] {2}", tab, I.index, I.name);
            }
            return result;
        }());
}

void taul::internal::grammar_data::build_lookup() {
    TAUL_ASSERT(_lookup.empty());
    _lookup.reserve(_lprs.size() + _pprs.size());
    for (const auto& I : _lprs) { _lookup[I.name] = entry{ true, I.index }; }
    for (const auto& I : _pprs) { _lookup[I.name] = entry{ false, I.index }; }
    // ensure no lprs/pprs shared names
    TAUL_ASSERT(_lookup.size() == _lprs.size() + _pprs.size());
}

void taul::internal::grammar_data::build_gwls() {
    TAUL_ASSERT(_gwls == nullptr);
    _gwls = std::make_shared<grammar_wide_lexer_state>(*this);
}

taul::internal::grammar_wide_lexer_state::grammar_wide_lexer_state(const grammar_data& gd)
    : lexer_state(),
    _bias(gd._bias),
    _lprs(std::span{ gd._lprs.begin(), gd._lprs.size() }) {}

taul::token taul::internal::base_grammar_wide_lexer_function(
    const std::shared_ptr<lexer_state>& state, 
    std::string_view txt, 
    source_pos offset, 
    const std::shared_ptr<logger>& lgr, 
    bool cut_skip_tokens) {
    TAUL_ASSERT(state != nullptr);
    const auto _state = std::static_pointer_cast<grammar_wide_lexer_state>(state);
    token selection = token::failure("", offset);
    bool has_selection = false;
    // TODO: could we make bias::last run *faster* by iterating backwards? or would increase be too small?
    for (const auto& I : _state->_lprs) {
        // skip if support
        if (I.qualifer == qualifier::support) {
            continue;
        }
        const auto tkn = I.lexer()(txt, offset, lgr);
        // skip if fails
        if (!(bool)tkn) {
            continue;
        }
        // select first successful match no matter what
        bool select = !(bool)selection;
        // otherwise, select if new match is preferrable
        if (!select) {
            switch (_state->_bias) {
            case bias::fl:  select = tkn.str().length() > selection.str().length();     break;
            case bias::fs:  select = tkn.str().length() < selection.str().length();     break;
            case bias::ll:  select = tkn.str().length() >= selection.str().length();    break;
            case bias::ls:  select = tkn.str().length() <= selection.str().length();    break;
            case bias::f:   select = false;                                             break;
            case bias::l:   select = true;                                              break;
            default:        TAUL_DEADEND;                                               break;
            }
        }
        // if above two make select == true, select tkn
        if (select) {
            selection = tkn;
        }
        // if our bias is 'first', then we may exit early
        if (select && _state->_bias == bias::f) {
            break;
        }
    }
    return selection;
}
