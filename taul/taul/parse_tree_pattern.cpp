

#include "parse_tree_pattern.h"


taul::parse_tree_pattern::parse_tree_pattern(grammar gram)
    : _gram(gram) {}

bool taul::parse_tree_pattern::is_sealed() const noexcept {
    return !_nodes.empty() && !_has_current();
}

bool taul::parse_tree_pattern::match(const parse_tree& x) const noexcept {
    return _match(x);
}

taul::parse_tree_pattern& taul::parse_tree_pattern::lexical(token tkn) {
    TAUL_ASSERT(!is_sealed());
    _try_incr_current_node_child_count();
    _try_contribute_to_current_node_len(tkn.high_pos());
    _nodes.push_back(_node_t{
            .parent = _current, // will be _no_node if no root
            .id = tkn.id,
            .pos = tkn.pos,
            .len = tkn.len,
        });
    return *this;
}

taul::parse_tree_pattern& taul::parse_tree_pattern::lexical(lpr_ref lpr, source_pos pos, source_len len) {
    return lexical(token::normal(lpr, pos, len));
}

taul::parse_tree_pattern& taul::parse_tree_pattern::lexical(const str& name, source_pos pos, source_len len) {
    return lexical(token::normal(_gram, name, pos, len));
}

taul::parse_tree_pattern& taul::parse_tree_pattern::failure(source_pos pos, source_len len) {
    return lexical(token::failure(pos, len));
}

taul::parse_tree_pattern& taul::parse_tree_pattern::end(source_pos pos) {
    return lexical(token::end(pos));
}

taul::parse_tree_pattern& taul::parse_tree_pattern::syntactic(ppr_ref ppr, source_pos pos) {
    TAUL_ASSERT(!is_sealed());
    _try_incr_current_node_child_count();
    _nodes.push_back(_node_t{
            .parent = _current, // will be _no_node if no root
            .id = ppr.id(),
            .pos = pos,
            .len = 0,
        });
    _make_latest_node_the_current_node();
    return *this;
}

taul::parse_tree_pattern& taul::parse_tree_pattern::syntactic(const str& name, source_pos pos) {
    return syntactic(_gram.ppr(name).value(), pos);
}

taul::parse_tree_pattern& taul::parse_tree_pattern::close() noexcept {
    TAUL_ASSERT(!is_sealed());
    const source_pos child_node_high_pos = _get_current().high_pos();
    _make_parent_node_the_current_node();
    _try_contribute_to_current_node_len(child_node_high_pos);
    return *this;
}

taul::parse_tree_pattern& taul::parse_tree_pattern::skip(source_len len) {
    TAUL_ASSERT(!is_sealed());
    _get_current().len += len;
    return *this;
}

taul::parse_tree_pattern& taul::parse_tree_pattern::loose_syntactic(ppr_ref ppr, source_pos pos, source_len len) {
    TAUL_ASSERT(!is_sealed());
    _try_incr_current_node_child_count();
    _try_contribute_to_current_node_len(pos + len);
    _nodes.push_back(_node_t{
            .parent = _current, // will be _no_node if no root
            .id = ppr.id(),
            .pos = pos,
            .len = len,
            .loose = true,
        });
    return *this;
}

taul::parse_tree_pattern& taul::parse_tree_pattern::loose_syntactic(const str& name, source_pos pos, source_len len) {
    return loose_syntactic(_gram.ppr(name).value(), pos, len);
}

bool taul::parse_tree_pattern::_has_current() const noexcept {
    return _current != _no_node;
}

taul::parse_tree_pattern::_node_t& taul::parse_tree_pattern::_get_current() noexcept {
    TAUL_ASSERT(_has_current());
    TAUL_ASSERT(_nodes[_current].is_syntactic());
    return _nodes[_current];
}

const taul::parse_tree_pattern::_node_t& taul::parse_tree_pattern::_get_current() const noexcept {
    TAUL_ASSERT(_has_current());
    TAUL_ASSERT(_nodes[_current].is_syntactic());
    return _nodes[_current];
}

void taul::parse_tree_pattern::_try_incr_current_node_child_count() {
    if (!_has_current()) return;
    _get_current().children++;
}

void taul::parse_tree_pattern::_try_contribute_to_current_node_len(source_pos high_pos) {
    if (!_has_current()) return;
    if (high_pos < _get_current().high_pos()) return;
    skip(high_pos - _get_current().high_pos());
}

void taul::parse_tree_pattern::_make_latest_node_the_current_node() {
    TAUL_ASSERT(!_nodes.empty());
    _current = _nodes.size() - 1;
}

void taul::parse_tree_pattern::_make_parent_node_the_current_node() {
    _current = _get_current().parent;
}

bool taul::parse_tree_pattern::_match(const parse_tree& x) const {
    TAUL_ASSERT(is_sealed());
    TAUL_ASSERT(x.is_sealed());
    _match_state_t s{
        .it = _nodes.begin(),
        .end = _nodes.end(),
        .curr = x.begin(),
        .last = x.end(),
    };
    while (s.has_it()) {
        if (!_match_step(s, x.end())) return false; // x.end() here means no parent
    }
    return true;
}

bool taul::parse_tree_pattern::_match_step(_match_state_t& s, parse_tree::const_iterator parent) const {
    TAUL_ASSERT(s.has_it());
    const auto our_it = s.it;
    std::advance(s.it, 1); // consume node of this step
    if (our_it->is_lexical()) {
        if (!_match_lexical(s, our_it, parent)) return false;
    }
    else if (our_it->is_syntactic() && !our_it->loose) {
        if (!_match_syntactic(s, our_it, parent)) return false;
    }
    else if (our_it->is_syntactic() && our_it->loose) {
        if (!_match_loose_syntactic(s, our_it, parent)) return false;
    }
    else TAUL_DEADEND;
    return true;
}

bool taul::parse_tree_pattern::_match_lexical(_match_state_t& s, decltype(_nodes)::const_iterator our_it, parse_tree::const_iterator parent) const {
    const bool result =
        s.has_curr() &&
        s.curr->is_lexical() &&
        s.curr->parent() == parent &&
        s.curr->id() == our_it->id &&
        s.curr->pos() == our_it->pos &&
        s.curr->len() == our_it->len;
    if (result) s.consume_one();
    return result;
}

bool taul::parse_tree_pattern::_match_syntactic(_match_state_t& s, decltype(_nodes)::const_iterator our_it, parse_tree::const_iterator parent) const {
    const bool result0 =
        s.has_curr() &&
        s.curr->is_syntactic() &&
        s.curr->parent() == parent &&
        s.curr->id() == our_it->id &&
        s.curr->pos() == our_it->pos &&
        s.curr->len() == our_it->len &&
        s.curr->children() == our_it->children;
    if (result0) {
        const auto parent_iter_to_use = s.curr;
        s.consume_one();
        return _match_children(s, parent_iter_to_use); // must also match children
    }
    else return false;
}

bool taul::parse_tree_pattern::_match_loose_syntactic(_match_state_t& s, decltype(_nodes)::const_iterator our_it, parse_tree::const_iterator parent) const {
    const bool result =
        s.has_curr() &&
        s.curr->is_syntactic() &&
        s.curr->parent() == parent &&
        s.curr->id() == our_it->id &&
        s.curr->pos() == our_it->pos &&
        s.curr->len() == our_it->len;
    if (result) s.consume_subtree();
    return result;
}

bool taul::parse_tree_pattern::_match_children(_match_state_t& s, parse_tree::const_iterator parent) const {
    for (size_t i = 0; i < parent->children(); i++) {
        if (!s.has_it()) return false; // no child, but expected one
        if (!_match_step(s, parent)) return false; // child isn't as expected
    }
    return true;
}

bool taul::parse_tree_pattern::_node_t::has_parent() const noexcept {
    return parent != _no_node;
}

taul::source_pos taul::parse_tree_pattern::_node_t::low_pos() const noexcept {
    return pos;
}

taul::source_pos taul::parse_tree_pattern::_node_t::high_pos() const noexcept {
    return low_pos() + len;
}

bool taul::parse_tree_pattern::_node_t::valid() const noexcept {
    return is_lexical() || is_syntactic();
}

bool taul::parse_tree_pattern::_node_t::is_lexical() const noexcept {
    return is_lpr_id(id);
}

bool taul::parse_tree_pattern::_node_t::is_syntactic() const noexcept {
    return is_ppr_id(id);
}

bool taul::parse_tree_pattern::_match_state_t::has_it() const {
    return it != end;
}

bool taul::parse_tree_pattern::_match_state_t::has_curr() const {
    return curr != last;
}

void taul::parse_tree_pattern::_match_state_t::consume_one() {
    TAUL_ASSERT(has_curr());
    std::advance(curr, 1);
}

void taul::parse_tree_pattern::_match_state_t::consume_subtree() {
    TAUL_ASSERT(has_curr());
    const size_t child_level = curr->level() + 1;
    consume_one(); // consume subtree root
    while (has_curr() && curr->level() >= child_level) { // consume descendents
        consume_one();
    }
}

