

#include "parse_tree.h"

#include "asserts.h"


#define _DUMP_LOG 0


taul::parse_tree::parse_tree(grammar gram) 
    : _gram(gram) {}

taul::parse_tree::parse_tree(const parse_tree& x) 
    : _gram(x._gram), 
    _nodes(x._nodes), 
    _current(x._current), 
    _aborted(x._aborted) {}

taul::parse_tree::parse_tree(parse_tree&& x) noexcept
    : _gram(std::move(x._gram)),
    _nodes(std::move(x._nodes)),
    _current(std::move(x._current)), 
    _aborted(std::move(x._aborted)) {}

taul::parse_tree& taul::parse_tree::operator=(const parse_tree& rhs) {
    _gram = rhs._gram;
    _nodes = rhs._nodes;
    _current = rhs._current;
    _aborted = rhs._aborted;
    return *this;
}

taul::parse_tree& taul::parse_tree::operator=(parse_tree&& rhs) noexcept {
    if (&rhs != this) {
        _gram = std::move(rhs._gram);
        _nodes = std::move(rhs._nodes);
        _current = std::move(rhs._current);
        _aborted = std::move(rhs._aborted);
    }
    return *this;
}

bool taul::parse_tree::is_sealed() const noexcept {
    return has_nodes() && _current == internal::no_index;
}

bool taul::parse_tree::is_aborted() const noexcept {
    return _aborted;
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
    if (nodes() != other.nodes()) return false;
    for (iterator it0 = begin(), it1 = other.begin(); it0 != end(); it0++, it1++) {
        if (!it0->_equal(*it1)) return false;
    }
    return true;
}

bool taul::parse_tree::operator==(const parse_tree& rhs) const noexcept {
    return equal(rhs);
}

bool taul::parse_tree::operator!=(const parse_tree& rhs) const noexcept {
    return !(*this == rhs);
}

taul::parse_tree& taul::parse_tree::skip(source_len len) {
    _contribute_to_current_len(len);
    return *this;
}

taul::parse_tree& taul::parse_tree::lexical(token tkn) {
    TAUL_ASSERT(bool(tkn) ? _gram.is_associated(tkn.lpr.value()) : true);
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "taul::parse_tree::lexical({})", tkn);
#endif
    _leaf(tkn.id, tkn.pos, tkn.len, bool(tkn) ? _make_lpr_rule(tkn.lpr.value()) : _make_no_rule());
    return *this;
}

taul::parse_tree& taul::parse_tree::lexical(lpr_ref lpr, source_pos pos, source_len len) {
    return lexical(token::normal(lpr, pos, len));
}

taul::parse_tree& taul::parse_tree::lexical(const str& name, source_pos pos, source_len len) {
    TAUL_ASSERT(_gram.has_lpr(name));
    return lexical(_gram.lpr(name).value(), pos, len);
}

taul::parse_tree& taul::parse_tree::failure(source_pos pos, source_len len) {
    return lexical(token::failure(pos, len));
}

taul::parse_tree& taul::parse_tree::end(source_pos pos) {
    return lexical(token::end(pos));
}

taul::parse_tree& taul::parse_tree::syntactic(ppr_ref ppr, source_pos pos) {
    TAUL_ASSERT(_gram.is_associated(ppr));
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "taul::parse_tree::syntactic({}, {})", ppr, size_t(pos));
#endif
    _open_branch(ppr.id(), pos, _make_ppr_rule(ppr));
    return *this;
}

taul::parse_tree& taul::parse_tree::syntactic(const str& name, source_pos pos) {
    TAUL_ASSERT(_gram.has_ppr(name));
    return syntactic(_gram.ppr(name).value(), pos);
}

taul::parse_tree& taul::parse_tree::close() noexcept {
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "taul::parse_tree::close()");
#endif
    _close_branch();
    return *this;
}

taul::parse_tree& taul::parse_tree::abort() {
#if _DUMP_LOG
    TAUL_LOG(make_stderr_logger(), "taul::parse_tree::abort()");
#endif
    _mark_abort();
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

void taul::parse_tree::_create_latest_node() {
    _nodes.push_back({});
}

void taul::parse_tree::_setup_latest_node_basic_info(symbol_id id, source_pos pos, source_len len, std::optional<std::variant<lpr_ref, ppr_ref>> rule) {
    TAUL_ASSERT(!_nodes.empty());
    const size_t latest_node_index = nodes() - 1;
    auto& latest_node = _nodes.back();
    latest_node._data._owner = this;
    latest_node._data._index = latest_node_index;
    latest_node._data._level = _has_current() ? _nodes[_current].level() + 1 : 0;
    latest_node._data._id = id;
    latest_node._data._pos = pos;
    latest_node._data._len = len;
    latest_node._data._rule = rule;
    TAUL_ASSERT(latest_node._data._rule ? !latest_node._data._rule->valueless_by_exception() : true);
}

void taul::parse_tree::_setup_latest_node_relationships() {
    if (!_has_current()) return; // if setting up root, exit
    TAUL_ASSERT(!_nodes.empty());
    const size_t latest_node_index = nodes() - 1;
    const size_t current_node_index = _current;
    auto& latest_node = _nodes.back();
    auto& current_node = _nodes[current_node_index];
    latest_node._data._parent_index = current_node_index; // make new node's parent the current node
    if (current_node.has_children()) { // setup relationship w/ new left sibling, if any
        const size_t new_left_sibling_index = current_node.right_child()->index();
        auto& new_left_sibling = _nodes[new_left_sibling_index];
        new_left_sibling._data._right_sibling_index = latest_node_index;
        latest_node._data._left_sibling_index = new_left_sibling_index;
    }
    current_node._data._children++;
    current_node._data._right_child_index = latest_node_index;
}

void taul::parse_tree::_setup_latest_node(symbol_id id, source_pos pos, source_len len, std::optional<std::variant<lpr_ref, ppr_ref>> rule) {
    _setup_latest_node_basic_info(id, pos, len, rule);
    _setup_latest_node_relationships();
}

void taul::parse_tree::_make_latest_node_the_current_node() {
    _current = _nodes.size() - 1;
}

void taul::parse_tree::_contribute_latest_node_to_parent_len() {
    auto& latest_node = _nodes.back();
    if (!latest_node.has_parent()) return; // if no parent, exit
    const size_t parent_node_index = latest_node._data._parent_index;
    auto& parent_node = _nodes[parent_node_index];
    parent_node._data._len = std::max(latest_node.high_pos(), parent_node.high_pos()) - parent_node.low_pos();
}

void taul::parse_tree::_contribute_current_node_to_parent_len() {
    TAUL_ASSERT(_has_current());
    auto& current_node = current();
    if (!current_node.has_parent()) return; // if no parent, exit
    const size_t parent_node_index = current_node._data._parent_index;
    auto& parent_node = _nodes[parent_node_index];
    parent_node._data._len = std::max(current_node.high_pos(), parent_node.high_pos()) - parent_node.low_pos();
}

void taul::parse_tree::_close_current_node() {
    _current =
        current().has_parent()
        ? current().parent()->index()
        : internal::no_index;
}

void taul::parse_tree::_contribute_to_current_len(source_len len) {
    TAUL_ASSERT(_has_current());
    const size_t current_node_index = _current;
    auto& current_node = _nodes[current_node_index];
    current_node._data._len += len;
}

void taul::parse_tree::_leaf(symbol_id id, source_pos pos, source_len len, std::optional<std::variant<lpr_ref, ppr_ref>> rule) {
    TAUL_ASSERT(!is_sealed());
    _create_latest_node();
    _setup_latest_node(id, pos, len, rule);
    // contribute immediately, as leaf nodes know their lengths up front
    _contribute_latest_node_to_parent_len();
}

void taul::parse_tree::_open_branch(symbol_id id, source_pos pos, std::optional<std::variant<lpr_ref, ppr_ref>> rule) {
    TAUL_ASSERT(!is_sealed());
    _create_latest_node();
    _setup_latest_node(id, pos, 0, rule);
    _make_latest_node_the_current_node();
}

void taul::parse_tree::_close_branch() {
    TAUL_ASSERT(_has_current());
    // contribute prior to closing, as branch nodes only know their lengths
    // after all their children have had a chance to update it
    _contribute_current_node_to_parent_len();
    _close_current_node();
}

void taul::parse_tree::_mark_abort() {
    _aborted = true;
}

std::optional<std::variant<taul::lpr_ref, taul::ppr_ref>> taul::parse_tree::_make_no_rule() const {
    return std::nullopt;
}

std::optional<std::variant<taul::lpr_ref, taul::ppr_ref>> taul::parse_tree::_make_lpr_rule(lpr_ref ref) const {
    TAUL_ASSERT(_gram.is_associated(ref));
    return std::make_optional(std::variant<lpr_ref, ppr_ref>{ std::in_place_index<0>, ref });
}

std::optional<std::variant<taul::lpr_ref, taul::ppr_ref>> taul::parse_tree::_make_ppr_rule(ppr_ref ref) const {
    TAUL_ASSERT(_gram.is_associated(ref));
    return std::make_optional(std::variant<lpr_ref, ppr_ref>{ std::in_place_index<1>, ref });
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
    return is_lpr_id(id());
}

bool taul::parse_tree::node::is_syntactic() const noexcept {
    return is_ppr_id(id());
}

bool taul::parse_tree::node::is_normal() const noexcept {
    return is_normal_id(id());
}

bool taul::parse_tree::node::is_failure() const noexcept {
    return is_failure_id(id());
}

bool taul::parse_tree::node::is_end() const noexcept {
    return is_end_id(id());
}

taul::symbol_id taul::parse_tree::node::id() const noexcept {
    return _data._id;
}

taul::source_pos taul::parse_tree::node::pos() const noexcept {
    return _data._pos;
}

taul::source_len taul::parse_tree::node::len() const noexcept {
    return _data._len;
}

std::optional<taul::lpr_ref> taul::parse_tree::node::lpr() const {
    return
        (_data._rule && is_lexical() && !is_failure())
        ? (TAUL_ASSERT(std::holds_alternative<lpr_ref>(_data._rule.value())), std::make_optional(std::get<lpr_ref>(_data._rule.value())))
        : std::nullopt;
}

std::optional<taul::ppr_ref> taul::parse_tree::node::ppr() const {
    return
        is_syntactic()
        ? (TAUL_ASSERT(std::holds_alternative<ppr_ref>(_data._rule.value())), std::make_optional(std::get<ppr_ref>(_data._rule.value())))
        : std::nullopt;
}

taul::str taul::parse_tree::node::str(taul::str src) const {
    TAUL_ASSERT(pos() <= src.length());
    TAUL_ASSERT(len() <= src.length() - pos());
    return src.substr(pos(), len());
}

std::optional<taul::token> taul::parse_tree::node::tkn() const {
    if (!is_lexical()) return std::nullopt;
    token tkn{};
    if (is_normal()) tkn = token::normal(lpr().value(), pos(), len());
    else if (is_failure()) tkn = token::failure(pos(), len());
    else if (is_end()) tkn = token::end(pos());
    else TAUL_DEADEND;
    return std::make_optional(std::move(tkn));
}

std::string taul::parse_tree::node::fmt() const {
    if (is_lexical()) return tkn().value().fmt();
    else return std::format("{} {} {}", fmt_pos_and_len(pos(), len()), id(), ppr().value().name());
}

taul::parse_tree& taul::parse_tree::node::_get_owner() const noexcept {
    TAUL_ASSERT(_data._owner);
    return *_data._owner;
}

bool taul::parse_tree::node::_equal(const node& other) const noexcept {
    // this bit up here is for handling the comparing of the two nodes' LPRs/PPRs
    if (is_lexical() != other.is_lexical()) return false; // can exit early if false
    else if (is_lexical()) {
        TAUL_ASSERT(other.is_lexical());
        if ((bool)lpr() != (bool)other.lpr()) return false;
        if ((bool)lpr() && (bool)other.lpr()) {
            if (lpr().value() != other.lpr().value()) return false;
        }
    }
    else if (is_syntactic()) {
        TAUL_ASSERT(other.is_syntactic());
        if ((bool)ppr() != (bool)other.ppr()) return false;
        if ((bool)ppr() && (bool)other.ppr()) {
            if (ppr().value() != other.ppr().value()) return false;
        }
    }
    else TAUL_DEADEND;
    return
        _data._index == other._data._index &&
        _data._level == other._data._level &&
        _data._id == other._data._id &&
        _data._pos == other._data._pos &&
        _data._len == other._data._len &&
        _data._parent_index == other._data._parent_index &&
        _data._left_sibling_index == other._data._left_sibling_index &&
        _data._right_sibling_index == other._data._right_sibling_index &&
        _data._children == other._data._children &&
        _data._right_child_index == other._data._right_child_index;
}

