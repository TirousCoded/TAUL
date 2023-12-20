

#include "node.h"


taul::node::node(node&& other) noexcept
    : _ctx(other._ctx),
    _data(other._data) {
    other._forget();
    TAUL_ASSERT(_is_bound());
    _retarget();
}

taul::node::~node() noexcept {
    TAUL_LOG_IF(_is_bound(), _ctx->_lgr, "~node w/ block at {}", (std::uintptr_t)_data);
    _drop();
}

taul::node& taul::node::operator=(node&& rhs) noexcept {
    if (this != &(rhs)) {
        _drop();
        TAUL_ASSERT(!_is_bound());
        std::swap(_ctx, rhs._ctx);
        std::swap(_data, rhs._data);
        _retarget();
        TAUL_ASSERT(_is_bound());
    }
    return *this;
}

const taul::node_ctx* taul::node::ctx() const noexcept {
    return _ctx;
}

bool taul::node::is_lexical() const noexcept {
    return _data ? _data->_lexical : (TAUL_DEADEND, false);
}

bool taul::node::is_syntactic() const noexcept {
    return _data ? !_data->_lexical : (TAUL_DEADEND, false);
}

bool taul::node::is_failure() const noexcept {
    return _data ? (is_lexical() && !(bool)_data->_lpr) : (TAUL_DEADEND, false);
}

const taul::lexer_rule& taul::node::lpr() const {
    if (is_syntactic() || is_failure()) {
        throw lpr_association_error("node has no associated lpr!");
    }
    TAUL_ASSERT(_is_bound());
    return *(_data->_lpr);
}

const taul::parser_rule& taul::node::ppr() const {
    TAUL_ASSERT(_data);
    if (is_lexical()) {
        throw ppr_association_error("node has no associated ppr!");
    }
    TAUL_ASSERT(_is_bound());
    return *(_data->_ppr);
}

std::string_view taul::node::str() const noexcept {
    return _data ? _data->_str : (TAUL_DEADEND, "");
}

taul::source_pos taul::node::pos() const noexcept {
    return _data ? _data->_pos : (TAUL_DEADEND, 0);
}

std::optional<taul::token> taul::node::tkn() const noexcept {
    TAUL_ASSERT(_data);
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

taul::node::iterator taul::node::iter() noexcept {
    return iterator(_data, nullptr);
}

taul::node::iterator taul::node::parent() noexcept {
    return _data ? iterator(_data->_parent, nullptr) : (TAUL_DEADEND, iterator());
}

taul::node::iterator taul::node::left_child() noexcept {
    //TAUL_LOG(taul::make_stderr_logger(), "left_child => {}", *this);
    return _data ? iterator(_data->_get_lc_data(), nullptr) : (TAUL_DEADEND, iterator());
}

taul::node::iterator taul::node::right_child() noexcept {
    return _data ? iterator(_data->_rc, nullptr) : (TAUL_DEADEND, iterator());
}

taul::node::iterator taul::node::left_sibling() noexcept {
    return _data ? iterator(_data->_ls, nullptr) : (TAUL_DEADEND, iterator());
}

taul::node::iterator taul::node::right_sibling() noexcept {
    return _data ? iterator(_data->_get_rs_data(), nullptr) : (TAUL_DEADEND, iterator());
}

taul::node::const_iterator taul::node::iter() const noexcept {
    return const_iterator(_data, nullptr);
}

taul::node::const_iterator taul::node::parent() const noexcept {
    return _data ? const_iterator(_data->_parent, nullptr) : (TAUL_DEADEND, const_iterator());
}

taul::node::const_iterator taul::node::left_child() const noexcept {
    return _data ? const_iterator(_data->_get_lc_data(), nullptr) : (TAUL_DEADEND, const_iterator());
}

taul::node::const_iterator taul::node::right_child() const noexcept {
    return _data ? const_iterator(_data->_rc, nullptr) : (TAUL_DEADEND, const_iterator());
}

taul::node::const_iterator taul::node::left_sibling() const noexcept {
    return _data ? const_iterator(_data->_ls, nullptr) : (TAUL_DEADEND, const_iterator());
}

taul::node::const_iterator taul::node::right_sibling() const noexcept {
    return _data ? const_iterator(_data->_get_rs_data(), nullptr) : (TAUL_DEADEND, const_iterator());
}

std::size_t taul::node::children() const noexcept {
    return _data ? _data->_children : (TAUL_DEADEND, 0);
}

taul::node::iterator taul::node::begin() noexcept {
    return left_child();
}

taul::node::const_iterator taul::node::cbegin() const noexcept {
    return left_child();
}

taul::node::const_iterator taul::node::begin() const noexcept {
    return cbegin();
}

taul::node::reverse_iterator taul::node::rbegin() noexcept {
    return std::make_reverse_iterator(end());
}

taul::node::const_reverse_iterator taul::node::crbegin() const noexcept {
    return std::make_reverse_iterator(end());
}

taul::node::const_reverse_iterator taul::node::rbegin() const noexcept {
    return crbegin();
}

taul::node::iterator taul::node::end() noexcept {
    return 
        children() > 0 
        ? std::next(right_child()) 
        : right_child();
}

taul::node::const_iterator taul::node::cend() const noexcept {
    return
        children() > 0
        ? std::next(right_child())
        : right_child();
}

taul::node::const_iterator taul::node::end() const noexcept {
    return cend();
}

taul::node::reverse_iterator taul::node::rend() noexcept {
    return std::make_reverse_iterator(begin());
}

taul::node::const_reverse_iterator taul::node::crend() const noexcept {
    return std::make_reverse_iterator(begin());
}

taul::node::const_reverse_iterator taul::node::rend() const noexcept {
    return crend();
}

bool taul::node::equiv(const node& other) const noexcept {
    TAUL_ASSERT(_data);
    //TAUL_LOG(taul::make_stderr_logger(), "{} equiv {}", iter(), other.iter());
    if (is_lexical() != other.is_lexical()) {
        //TAUL_LOG(taul::make_stderr_logger(), "failure A");
        return false;
    }
    if (is_failure() != other.is_failure()) {
        //TAUL_LOG(taul::make_stderr_logger(), "failure B");
        return false;
    }
    if (is_lexical() && !is_failure() && &(lpr()) != &(other.lpr())) {
        //TAUL_LOG(taul::make_stderr_logger(), "failure C");
        return false;
    }
    if (is_syntactic() && &(ppr()) != &(other.ppr())) {
        //TAUL_LOG(taul::make_stderr_logger(), "failure D");
        return false;
    }
    if (str() != other.str()) {
        //TAUL_LOG(taul::make_stderr_logger(), "failure E\nstr()=={}\nother.str()=={}", str(), other.str());
        return false;
    }
    if (pos() != other.pos()) {
        //TAUL_LOG(taul::make_stderr_logger(), "failure F");
        return false;
    }
    if (children() != other.children()) {
        //TAUL_LOG(taul::make_stderr_logger(), "failure G");
        return false;
    }
    auto it0 = begin();
    auto it1 = other.begin();
    for (; it0 != end(); it0++, it1++) {
        //TAUL_LOG(taul::make_stderr_logger(), "(iter) {} equiv {}", it0, it1);
        TAUL_ASSERT(it1 != other.end());
        if (!(it0->equiv(*it1))) {
            //TAUL_LOG(taul::make_stderr_logger(), "aborted!");
            return false;
        }
        //TAUL_LOG(taul::make_stderr_logger(), "iter complete!");
    }
    return true;
}

void taul::node::attach(node&& x) {
    TAUL_ASSERT(ctx() == x.ctx());
    if (children() == 0) {
        _attach_first_child(std::forward<node>(x));
    }
    else {
        _attach_nonfirst_child_to_end(std::forward<node>(x));
    }
}

void taul::node::attach(node&& x, iterator place_before) {
    _attach_nonfirst_child_behind(std::forward<node>(x), place_before);
}

taul::node taul::node::detach(iterator x) {
    return 
        x == begin() 
        ? _detach_first_child(x)
        : _detach_nonfirst_child(x);
}

void taul::node::detach_range(iterator first, iterator last) {
    TAUL_ASSERT(_data);
    for (auto it = first; it != last;) {
        auto next = std::next(it);
        (void)detach(it);
        it = std::move(next);
    }
}

void taul::node::detach_all() {
    if (_data) {
        // this should recursively destroy all child nodes
        _data->_lc = std::nullopt;
        _data->_rc = nullptr;
        _data->_children = 0;
    }
    else TAUL_DEADEND;
}

std::string taul::node::fmt() const {
    TAUL_ASSERT(_data);
    return
        is_lexical()
        ? tkn()->fmt()
        : std::format("[(ppr) ({}) {} (len {})]", pos(), ppr().name, str().length());
}

std::string taul::node::fmt_tree(const char* tab) const {
    std::string result{};
    _fmt_tree(result, 0, tab);
    return result;
}

taul::node::node(
    internal::for_internal_use_tag,
    node_ctx& ctx,
    const lexer_rule& lpr,
    std::string_view str,
    source_pos pos) 
    : _ctx(&ctx), 
    _data(ctx._create_node_data()) {
    TAUL_ASSERT(_is_bound());
    if (_data) {
        _data->_lexical = true;
        _data->_lpr = &lpr;
        _data->_str = str;
        _data->_pos = pos;
        _data->_node = this;
    }
    else TAUL_DEADEND;
}

taul::node::node(
    internal::for_internal_use_tag,
    node_ctx& ctx,
    token tkn)
    : _ctx(&ctx),
    _data(ctx._create_node_data()) {
    TAUL_ASSERT(_is_bound());
    if (_data) {
        _data->_lexical = true;
        _data->_lpr = bool(tkn) ? &(tkn.lpr()) : nullptr;
        _data->_str = tkn.str();
        _data->_pos = tkn.pos();
        _data->_node = this;
    }
    else TAUL_DEADEND;
}

taul::node::node(
    internal::for_internal_use_tag,
    node_ctx& ctx,
    const parser_rule& ppr,
    std::string_view str,
    source_pos pos)
    : _ctx(&ctx),
    _data(ctx._create_node_data()) {
    TAUL_ASSERT(_is_bound());
    if (_data) {
        _data->_lexical = false;
        _data->_ppr = &ppr;
        _data->_str = str;
        _data->_pos = pos;
        _data->_node = this;
    }
    else TAUL_DEADEND;
}

bool taul::node::_is_bound() const noexcept {
    return _ctx && _data;
}

void taul::node::_drop() noexcept {
    if (_is_bound() && _ctx) {
        // this *should* recursively detach/destroy all descendents
        _ctx->_cleanup_node_data(_data);
    }
    _forget();
}

void taul::node::_forget() noexcept {
    _ctx = nullptr;
    _data = nullptr;
}

void taul::node::_retarget() noexcept {
    if (_is_bound() && _ctx) {
        _ctx->_retarget_node_data(_data, this);
    }
}

void taul::node::_attach_first_child(node&& x) {
    // if we've got 0 children, move-assign x to _data->_lc
    TAUL_ASSERT(children() == 0);
    // get x._data so we have it after we move-assign w/ x
    const auto child_data = x._data;
    if (_data && child_data) {
        child_data->_parent = _data;
        _data->_lc = std::move(std::make_optional<node>(std::forward<node&&>(x)));
        _data->_rc = child_data;
        _data->_children++;
    }
    else TAUL_DEADEND;
}

void taul::node::_attach_nonfirst_child_to_end(node&& x) {
    // if we've got >0 children, move-assign x to _data->_rs
    TAUL_ASSERT(children() > 0);
    // get x._data so we have it after we move-assign w/ x
    const auto child_data = x._data;
    if (_data && child_data) {
        child_data->_parent = _data;
        child_data->_ls = _data->_rc;
        if (_data->_rc) {
            _data->_rc->_rs = std::move(std::make_optional<node>(std::move(x)));
        }
        else TAUL_DEADEND;
        _data->_rc = child_data;
        _data->_children++;
    }
    else TAUL_DEADEND;
}

void taul::node::_attach_nonfirst_child_behind(node&& x, iterator place_before) {
    TAUL_ASSERT(children() > 0);
    TAUL_ASSERT(!place_before.is_past_the_end());
    TAUL_ASSERT(place_before->parent() == iter());
    TAUL_ASSERT(children() > 0);
    const auto pb_data = place_before->_data;
    const auto pb_ls_data = pb_data->_ls;
    // get x._data so we have it after we move-assign w/ x
    const auto child_data = x._data;
    if (_data && pb_data && child_data) {
        child_data->_parent = _data;
        pb_data->_ls = child_data;
        // if place_before node has a left-sibling
        if (pb_ls_data) {
            child_data->_ls = pb_ls_data;
            // move ownership of pb_data from pb_ls_data to child_data
            child_data->_rs = std::move(pb_ls_data->_rs);
            // move ownership of child_data from x to pb_ls_data
            pb_ls_data->_rs = std::forward<node>(x);
        }
        // if place_before node has no left-sibling
        else {
            // move ownership of pb_data from _data to child_data
            child_data->_rs = std::move(_data->_lc);
            // move ownership of child_data from x to _data
            _data->_lc = std::forward<node>(x);
        }
        _data->_children++;
    }
    else TAUL_DEADEND;
}

taul::node taul::node::_detach_first_child(iterator x) {
    TAUL_ASSERT(x == begin());
    TAUL_ASSERT(!x.is_past_the_end());
    TAUL_ASSERT(x->parent() == iter());
    std::optional<node> result{};
    if (_data && x->_data) {
        // take ownership of x node
        std::swap(result, _data->_lc);
        if ((bool)result && result->_data) {
            result->_data->_parent = nullptr;
            result->_data->_ls = nullptr;
            // if has result->_data->_rs, give _data ownership of it,
            // and change other pointers
            if ((bool)result->_data->_rs) {
                if ((bool)result->_data->_rs->_data) {
                    result->_data->_rs->_data->_ls = nullptr;
                }
                else TAUL_DEADEND;
                std::swap(_data->_lc, result->_data->_rs);
            }
            else {
                // if x node had no right-sibling, change this pointer
                _data->_rc = nullptr;
            }
        }
        else TAUL_DEADEND;
        _data->_children--;
    }
    else TAUL_DEADEND;
    TAUL_ASSERT((bool)result);
    return std::move(*result);
}

taul::node taul::node::_detach_nonfirst_child(iterator x) {
    TAUL_ASSERT(children() > 1);
    TAUL_ASSERT(x != begin());
    TAUL_ASSERT(!x.is_past_the_end());
    TAUL_ASSERT(x->parent() == iter());
    std::optional<node> result{};
    if (_data) {
        const auto x_ls_data = x->_data->_ls;
        if (x_ls_data) {
            // take ownership of x node
            std::swap(result, x_ls_data->_rs);
            if ((bool)result && result->_data) {
                result->_data->_parent = nullptr;
                result->_data->_ls = nullptr;
                // if has result->_data->_rs, give _data ownership of it,
                // and change other pointers
                if ((bool)result->_data->_rs) {
                    if ((bool)result->_data->_rs->_data) {
                        result->_data->_rs->_data->_ls = x_ls_data;
                    }
                    else TAUL_DEADEND;
                    std::swap(x_ls_data->_rs, result->_data->_rs);
                }
                else {
                    // if x node had no right-sibling, change this pointer
                    _data->_rc = x_ls_data;
                }
            }
            else TAUL_DEADEND;
            _data->_children--;
            TAUL_ASSERT(children() >= 1);
        }
        else TAUL_DEADEND;
    }
    else TAUL_DEADEND;
    TAUL_ASSERT((bool)result);
    return std::move(*result);
}

void taul::node::_fmt_tree(std::string& result, std::size_t tabs, const char* tab) const {
    if (!result.empty()) {
        result += '\n';
    }
    for (std::size_t i = 0; i < tabs; i++) {
        result += tab;
    }
    result += fmt();
    for (const auto& I : *this) {
        I._fmt_tree(result, tabs + 1, tab);
    }
}

taul::internal::node_data* taul::internal::node_data::_get_lc_data() const noexcept {
    return 
        _lc.has_value() 
        ? _lc->_data 
        : nullptr;
}

taul::internal::node_data* taul::internal::node_data::_get_rs_data() const noexcept {
    return
        _rs.has_value()
        ? _rs->_data
        : nullptr;
}

taul::node_ctx::node_ctx(const std::shared_ptr<logger>& lgr) 
    : _lgr(lgr), 
    _ba(lgr) {}

taul::node_ctx::~node_ctx() noexcept {
    TAUL_LOG(_lgr, "~node_ctx");
}

std::size_t taul::node_ctx::count() const noexcept {
    return _ba.blocks_outstanding();
}

taul::node taul::node_ctx::create(const lexer_rule& lpr, std::string_view str, source_pos pos) {
    return
        node(
            internal::for_internal_use_tag{},
            *this,
            lpr,
            str,
            pos);
}

taul::node taul::node_ctx::create(token tkn) {
    return
        node(
            internal::for_internal_use_tag{},
            *this,
            tkn);
}

taul::node taul::node_ctx::create(const parser_rule& ppr, std::string_view str, source_pos pos) {
    return
        node(
            internal::for_internal_use_tag{},
            *this,
            ppr,
            str,
            pos);
}

taul::node taul::node_ctx::clone(const node& target) {
    node result =
        target.is_lexical()
        ? create(*target.tkn())
        : create(target.ppr(), target.str(), target.pos());
    for (const auto& I : target) {
        result.attach(clone(I));
    }
    return result;
}

taul::internal::node_data* taul::node_ctx::_create_node_data() {
    internal::node_data* result = (internal::node_data*)_ba.alloc();
    TAUL_ASSERT((bool)result);
    _init_node_data(result);
    TAUL_LOG(_lgr, "creating node w/ block at {}", (std::uintptr_t)result);
    return result;
}

void taul::node_ctx::_cleanup_node_data(internal::node_data* x) noexcept {
    TAUL_ASSERT((bool)x);
    TAUL_LOG(_lgr, "cleaning-up node w/ block at {}", (std::uintptr_t)x);
    _deinit_node_data(x);
    _ba.free((decltype(_ba)::block_t*)x);
}

void taul::node_ctx::_init_node_data(internal::node_data* x) noexcept {
    TAUL_ASSERT((bool)x);
    TAUL_LOG(_lgr, "init block of node w/ block at {}", (std::uintptr_t)x);
    std::construct_at(x);
}

void taul::node_ctx::_deinit_node_data(internal::node_data* x) noexcept {
    TAUL_ASSERT((bool)x);
    TAUL_LOG(_lgr, "deinit block of node w/ block at {}", (std::uintptr_t)x);
    std::destroy_at(x);
}

void taul::node_ctx::_retarget_node_data(internal::node_data* x, taul::node* new_target) noexcept {
    TAUL_ASSERT((bool)x);
    TAUL_ASSERT((bool)new_target);
    if (x) {
        x->_node = new_target;
    }
    else TAUL_DEADEND;
}

taul::internal::node_data& taul::internal::get_node_data(node& target) noexcept {
    TAUL_ASSERT(target._data);
    return *target._data;
}

void taul::internal::patch_node(
    node& target, 
    token tkn) {
    auto& nd = get_node_data(target);
    nd._lexical = true;
    nd._lpr = &(tkn.lpr());
    nd._str = tkn.str();
    nd._pos = tkn.pos();
}

void taul::internal::patch_node(
    node& target, 
    const parser_rule& ppr, 
    std::string_view str, 
    source_pos pos) {
    auto& nd = get_node_data(target);
    nd._lexical = false;
    nd._ppr = &ppr;
    nd._str = str;
    nd._pos = pos;
}
