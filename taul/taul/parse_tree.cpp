

#include "parse_tree.h"


taul::parse_tree::parse_tree() {}

taul::parse_tree::parse_tree(const parse_tree& x) 
    : _nodes(x._nodes), 
    _current(x._current) {}

taul::parse_tree::parse_tree(parse_tree&& x) noexcept
    : _nodes(std::move(x._nodes)),
    _current(std::move(x._current)) {}

taul::parse_tree& taul::parse_tree::operator=(const parse_tree& rhs) {
    _nodes = rhs._nodes;
    _current = rhs._current;
    return *this;
}

taul::parse_tree& taul::parse_tree::operator=(parse_tree&& rhs) noexcept {
    if (&rhs != this) {
        _nodes = std::move(rhs._nodes);
        _current = std::move(rhs._current);
    }
    return *this;
}

bool taul::parse_tree::finished() const noexcept {
    return has_nodes() && _current == internal::no_index;
}

std::size_t taul::parse_tree::nodes() const noexcept {
    return _nodes.size();
}

bool taul::parse_tree::has_nodes() const noexcept {
    return nodes() > 0;
}

const taul::parse_tree::node& taul::parse_tree::at(std::size_t ind) const {
    return _nodes.at(ind);
}

const taul::parse_tree::node& taul::parse_tree::current() const {
    return at(_current);
}

const taul::parse_tree::node& taul::parse_tree::root() const {
    return at(0);
}

taul::parse_tree::iterator taul::parse_tree::cbegin() const noexcept {
    return _nodes.begin();
}

taul::parse_tree::iterator taul::parse_tree::begin() const noexcept {
    return cbegin();
}

taul::parse_tree::iterator taul::parse_tree::cend() const noexcept {
    return _nodes.end();
}

taul::parse_tree::iterator taul::parse_tree::end() const noexcept {
    return cend();
}

bool taul::parse_tree::equal(const parse_tree& other) const noexcept {
    if (nodes() == other.nodes()) {
        for (
            iterator it0 = begin(), it1 = other.begin(); 
            it0 != end(); 
            it0++, it1++) {
            if (!it0->_equal(*it1)) {
                return false;
            }
        }
        return true;
    }
    else return false;
}

bool taul::parse_tree::operator==(const parse_tree& rhs) const noexcept {
    return equal(rhs);
}

bool taul::parse_tree::operator!=(const parse_tree& rhs) const noexcept {
    return !(*this == rhs);
}

taul::parse_tree& taul::parse_tree::open_lexical(const lexer_rule& lpr, std::string_view str, source_pos pos) {
    return open_lexical(taul::token(lpr, str, pos));
}

taul::parse_tree& taul::parse_tree::open_lexical(token tkn) {
    TAUL_ASSERT(!finished());
    const bool _has_current = _current != internal::no_index;
    const std::size_t _new_index = nodes();
    _nodes.push_back(node{});
    auto& nd = _nodes.back();
    nd._data._owner = this;
    nd._data._index = _new_index;
    nd._data._level =
        _has_current
        ? _nodes[_current].level() + 1
        : 0;
    nd._data._lexical = true;
    nd._data._lpr =
        !tkn.is_failure()
        ? &(tkn.lpr())
        : nullptr;
    nd._data._str = tkn.str();
    nd._data._pos = tkn.pos();
    nd._data._parent_index = _current;
    if (_has_current) {
        if (current().has_children()) {
            _nodes[current().right_child()->index()]._data._right_sibling_index = _new_index;
            nd._data._left_sibling_index = current().right_child()->index();
        }
        _nodes[_current]._data._children++;
        _nodes[_current]._data._right_child_index = _new_index;
    }
    _current = _nodes.size() - 1;
    return *this;
}

taul::parse_tree& taul::parse_tree::open_syntactic(const parser_rule& ppr, std::string_view str, source_pos pos) {
    TAUL_ASSERT(!finished());
    const bool _has_current = _current != internal::no_index;
    const std::size_t _new_index = nodes();
    _nodes.push_back(node{});
    auto& nd = _nodes.back();
    nd._data._owner = this;
    nd._data._index = _new_index;
    nd._data._level =
        _has_current
        ? _nodes[_current].level() + 1
        : 0;
    nd._data._lexical = false;
    nd._data._ppr = &ppr;
    nd._data._str = str;
    nd._data._pos = pos;
    nd._data._parent_index = _current;
    if (_has_current) {
        if (current().has_children()) {
            _nodes[current().right_child()->index()]._data._right_sibling_index = _new_index;
            nd._data._left_sibling_index = current().right_child()->index();
        }
        _nodes[_current]._data._children++;
        _nodes[_current]._data._right_child_index = _new_index;
    }
    _current = _nodes.size() - 1;
    return *this;
}

taul::parse_tree& taul::parse_tree::close() noexcept {
    TAUL_ASSERT(_current != internal::no_index);
    _current = 
        current().has_parent() 
        ? current().parent()->index() 
        : internal::no_index;
    return *this;
}

std::string taul::parse_tree::fmt(const char* tab) const {
    TAUL_ASSERT(tab);
    std::string result{};
    for (const auto& I : *this) {
        if (!result.empty()) {
            result += '\n';
        }
        for (std::size_t i = 0; i < I.level(); i++) {
            result += tab;
        }
        result += I.fmt();
    }
    return result;
}

taul::parse_tree::node::node() 
    : _data({}) {}

taul::parse_tree::node::node(const node& x) 
    : _data(x._data) {}

taul::parse_tree::node::node(node&& x) noexcept 
    : _data(std::move(x._data)) {}

taul::parse_tree::node& taul::parse_tree::node::operator=(const node& rhs) {
    _data = rhs._data;
    return *this;
}

taul::parse_tree::node& taul::parse_tree::node::operator=(node&& rhs) noexcept {
    if (&rhs != this) {
        _data = std::move(rhs._data);
    }
    return *this;
}

std::size_t taul::parse_tree::node::index() const noexcept {
    return _data._index;
}

std::size_t taul::parse_tree::node::level() const noexcept {
    return _data._level;
}

taul::parse_tree::iterator taul::parse_tree::node::parent() const noexcept {
    return 
        has_parent()
        ? std::next(_get_owner().begin(), _data._parent_index)
        : _get_owner().end();
}

taul::parse_tree::iterator taul::parse_tree::node::left_sibling() const noexcept {
    return
        has_left_sibling()
        ? std::next(_get_owner().begin(), _data._left_sibling_index)
        : _get_owner().end();
}

taul::parse_tree::iterator taul::parse_tree::node::right_sibling() const noexcept {
    return
        has_right_sibling()
        ? std::next(_get_owner().begin(), _data._right_sibling_index)
        : _get_owner().end();
}

taul::parse_tree::iterator taul::parse_tree::node::left_child() const noexcept {
    return
        has_children()
        ? std::next(_get_owner().begin(), index() + 1)
        : _get_owner().end();
}

taul::parse_tree::iterator taul::parse_tree::node::right_child() const noexcept {
    return
        has_children()
        ? std::next(_get_owner().begin(), _data._right_child_index)
        : _get_owner().end();
}

bool taul::parse_tree::node::has_parent() const noexcept {
    return _data._parent_index != internal::no_index;
}

bool taul::parse_tree::node::has_left_sibling() const noexcept {
    return _data._left_sibling_index != internal::no_index;
}

bool taul::parse_tree::node::has_right_sibling() const noexcept {
    return _data._right_sibling_index != internal::no_index;
}

std::size_t taul::parse_tree::node::children() const noexcept {
    return _data._children;
}

bool taul::parse_tree::node::has_children() const noexcept {
    return children() > 0;
}

bool taul::parse_tree::node::is_lexical() const noexcept {
    return _data._lexical;
}

bool taul::parse_tree::node::is_syntactic() const noexcept {
    return !is_lexical();
}

bool taul::parse_tree::node::is_failure() const noexcept {
    return is_lexical() && !_data._lpr;
}

const taul::lexer_rule& taul::parse_tree::node::lpr() const {
    if (is_syntactic() || is_failure()) {
        throw lpr_association_error("node has no associated LPR!");
    }
    return *_data._lpr;
}

const taul::parser_rule& taul::parse_tree::node::ppr() const {
    if (is_lexical()) {
        throw ppr_association_error("node has no associated PPR!");
    }
    return *_data._ppr;
}

std::string_view taul::parse_tree::node::str() const noexcept {
    return _data._str;
}

taul::source_pos taul::parse_tree::node::pos() const noexcept {
    return _data._pos;
}

std::optional<taul::token> taul::parse_tree::node::tkn() const noexcept {
    if (is_lexical()) {
        return
            !is_failure()
            ? std::make_optional<token>(lpr(), str(), pos())
            : std::make_optional<token>(token::failure(str(), pos()));
    }
    else {
        return std::nullopt;
    }
}

std::string taul::parse_tree::node::fmt() const {
    return
        is_lexical()
        ? tkn()->fmt()
        : std::format("[(ppr) ({}) {} (len {})]", pos(), ppr().name, str().length());
}

taul::parse_tree& taul::parse_tree::node::_get_owner() const noexcept {
    TAUL_ASSERT(_data._owner);
    return *_data._owner;
}

bool taul::parse_tree::node::_equal(const node& other) const noexcept {
    return
        _data._index == other._data._index &&
        _data._level == other._data._level &&
        _data._lexical == other._data._lexical &&
        _data._lpr == other._data._lpr &&
        _data._str == other._data._str &&
        _data._pos == other._data._pos &&
        _data._parent_index == other._data._parent_index &&
        _data._left_sibling_index == other._data._left_sibling_index &&
        _data._right_sibling_index == other._data._right_sibling_index &&
        _data._children == other._data._children &&
        _data._right_child_index == other._data._right_child_index;
}

