

#include "lexer.h"

#include "internal/grammar_data.h"


#define _DUMP_LOG 0


taul::lexer::lexer(grammar gram, std::shared_ptr<logger> lgr)
    : base_lexer(gram, lgr),
    _source(nullptr),
    _observer(nullptr),
    _input(*this, _reserved_mem_for_input_cache),
    _matcher(*this, _reserved_mem_for_matcher_stack),
    _puller(*this) {}

void taul::lexer::bind_source(glyph_stream* source) {
    _source = source;
    _source_ownership = nullptr;
    _valid = false;
}

void taul::lexer::bind_source(std::shared_ptr<glyph_stream> source) {
    _source = source.get();
    _source_ownership = source;
    _valid = false;
}

void taul::lexer::bind_observer(token_observer* observer) {
    _observer = observer;
    _observer_ownership = nullptr;
    //_valid = false; // <- observer change doesn't require reset
}

void taul::lexer::bind_observer(std::shared_ptr<token_observer> observer) {
    _observer = observer.get();
    _observer_ownership = observer;
    //_valid = false; // <- observer change doesn't require reset
}

taul::token taul::lexer::peek() {
    return _peek();
}

taul::token taul::lexer::next() {
    return _next();
}

bool taul::lexer::done() {
    return _done();
}

void taul::lexer::reset() {
    return _reset();
}

taul::lexer& taul::lexer::input_queue::self() const noexcept {
    TAUL_ASSERT(_self);
    return *_self;
}

taul::lexer::input_queue::input_queue(lexer& self, size_t initial_recorded_inputs_capacity)
    : _self(&self) {
    recorded_inputs.reserve(initial_recorded_inputs_capacity); // alloc up-front to minimize reallocs later
}

size_t taul::lexer::input_queue::number() const {
    return current_input + total_forgot;
}

void taul::lexer::input_queue::playback() {
    current_input = 0;
}

void taul::lexer::input_queue::reset() {
    recorded_inputs.clear();
    current_input = 0;
}

void taul::lexer::input_queue::forget() {
    TAUL_ASSERT(current_input <= recorded_inputs.size());
    total_forgot += current_input;
    recorded_inputs.erase(
        recorded_inputs.begin(),
        recorded_inputs.begin() + current_input);
    current_input = 0;
}

taul::glyph taul::lexer::input_queue::peek() {
    TAUL_ASSERT(current_input <= recorded_inputs.size());
    if (current_input == recorded_inputs.size()) {
        // lexer impl must not call peek/next w/out source!
        TAUL_DEREF_SAFE(self()._source) recorded_inputs.push_back(self()._source->next());
    }
    return recorded_inputs[current_input];
}

taul::glyph taul::lexer::input_queue::next() {
    glyph result = peek();
    // don't advance if we're at end-of-input
    // this is actually REALLY important for the logic of our 'puller' below
    if (!result.is_end()) current_input++;
    return result;
}

void taul::lexer::input_queue::skip(size_t n) {
    current_input = std::min(current_input + n, recorded_inputs.size());
}

taul::lexer& taul::lexer::matcher::self() const noexcept {
    TAUL_ASSERT(_self);
    return *_self;
}

taul::lexer::matcher::matcher(lexer& self, size_t initial_stack_capacity)
    : _self(&self),
    _ps(_policy{ ._self_ptr = &self, ._result_ptr = &_result }, self.gram, initial_stack_capacity, self.lgr) {}

taul::token taul::lexer::matcher::match(lpr_ref start_rule) {
    _ps.parse(start_rule);
    return _result;
}

const taul::internal::nonterminal_id_allocs<taul::glyph>& taul::lexer::matcher::_policy::fetch_ntia(grammar x) {
    return internal::launder_grammar_data(x)._lpr_id_allocs;
}

const taul::internal::parse_table<taul::glyph>& taul::lexer::matcher::_policy::fetch_pt(grammar x) {
    return internal::launder_grammar_data(x)._lpr_pt;
}

taul::glyph taul::lexer::matcher::_policy::peek() {
    return _self_ptr->_input.peek();
}

taul::glyph taul::lexer::matcher::_policy::next() {
    return _self_ptr->_input.next();
}

void taul::lexer::matcher::_policy::reinit_output(rule_ref_type start_rule) {
    source_pos _pos{};
    TAUL_DEREF_SAFE(_self_ptr) _pos = _self_ptr->_input.peek().pos;
    TAUL_DEREF_SAFE(_result_ptr) *_result_ptr = token::normal(start_rule, _pos);
}

std::string taul::lexer::matcher::_policy::fmt_output() const {
    std::string result{};
    TAUL_DEREF_SAFE(_result_ptr) result = _result_ptr->fmt();
    return result;
}

void taul::lexer::matcher::_policy::output_startup() {
    // do nothing
}

void taul::lexer::matcher::_policy::output_shutdown() {
    // do nothing
}

void taul::lexer::matcher::_policy::output_terminal(symbol_type terminal) {
    TAUL_DEREF_SAFE(_result_ptr) {
        _result_ptr->pos = std::min(_result_ptr->low_pos(), terminal.low_pos());
        _result_ptr->len = std::max(_result_ptr->high_pos(), terminal.high_pos()) - _result_ptr->pos;
    }
}

void taul::lexer::matcher::_policy::output_nonterminal_begin(symbol_id) {
    // do nothing
}

void taul::lexer::matcher::_policy::output_nonterminal_end() {
    // do nothing
}

void taul::lexer::matcher::_policy::output_terminal_error(symbol_range<symbol_type> ids, symbol_type input) {
    TAUL_DEREF_SAFE(_result_ptr) *_result_ptr = token::failure(input.pos);
}

void taul::lexer::matcher::_policy::output_nonterminal_error(symbol_id id, symbol_type input) {
    TAUL_DEREF_SAFE(_result_ptr) *_result_ptr = token::failure(input.pos);
}

taul::lexer& taul::lexer::puller::self() const noexcept {
    TAUL_ASSERT(_self);
    return *_self;
}

taul::lexer::puller::puller(lexer& self)
    : _self(&self) {}

taul::token taul::lexer::puller::pull() {
#if _DUMP_LOG
    TAUL_LOG(self().lgr, "\nlexer::puller::pull()");
    TAUL_LOG(self().lgr, "*** begin-pull ***");
#endif
    token result{};
    while (true) {
        result = _pull_no_cut();
        if (self()._observer) self()._observer->observe(result); // observe regardless of whether we keep the token
        if (!self().cut_skip_tokens) break; // don't cut if cutting skip tokens is disabled
        if (!result.is_normal()) break; // don't cut failure and end-of-input tokens
        if (result.lpr && result.lpr.value().qualifier() != skip) break; // don't cut non-skip tokens
#if _DUMP_LOG
        TAUL_LOG(self().lgr, "skip token cut!");
#endif
    }
#if _DUMP_LOG
    TAUL_LOG(self().lgr, "*** end-pull ***");
    TAUL_LOG(self().lgr, "result: {}", result);
#endif
    return result;
}

void taul::lexer::puller::reset() {
    _current.reset();
    _pending.reset();
    _last_pending_consumed_no_input = false;
}

bool taul::lexer::puller::_at_end_of_input() {
    return self()._input.peek().is_end();
}

taul::token taul::lexer::puller::_match_with_lpr(size_t lpr_index) {
    TAUL_ASSERT(lpr_index < self().gram.lprs());
    auto result = self()._matcher.match(self().gram.lpr_at(lpr_index));
    if (result.is_failure()) self()._input.playback();
    return result;
}

taul::token taul::lexer::puller::_match_with_all_lpr() {
    token result = token::failure(self()._input.peek().pos); // for edge case if ALL LPRs are support
    for (size_t i = 0; i < self().gram.lprs(); i++) {
        if (self().gram.lpr_at(i).qualifier() == support) continue; // skip if LPR is support
        result = _match_with_lpr(i);
        if (!result.is_failure()) break; // stop upon first success
    }
    return result;
}

bool taul::lexer::puller::_has_current() const noexcept {
    return _current.has_value();
}

bool taul::lexer::puller::_has_pending() const noexcept {
    return _pending.has_value();
}

taul::token taul::lexer::puller::_resolve_pending() {
    TAUL_ASSERT(!_has_pending());
#if _DUMP_LOG
    TAUL_LOG(self().lgr, "\nlexer::puller::_resolve_pending()");
    TAUL_LOG(self().lgr, "*** begin-_resolve_pending ***");
#endif
    token result{};
    if (_last_pending_consumed_no_input) {
#if _DUMP_LOG
        TAUL_LOG(self().lgr, "route #1 (ie. _last_pending_consumed_no_input == true)");
#endif
        glyph input = self()._input.next();
        result =
            input.is_end()
            ? token::end(input.pos)
            : token::failure(input.pos, input.len);
    }
    else {
#if _DUMP_LOG
        TAUL_LOG(self().lgr, "route #2 (ie. _last_pending_consumed_no_input == false)");
#endif
        result = _match_with_all_lpr();
        // if length 0 failure, w/ peek being to end-of-input, then
        // we want to actually replace this w/ end-of-input for our
        // system to work properly
        if (result.is_failure() && 
            result.len == 0 &&
            self()._input.peek().is_end()) {
            result = token::end(result.pos);
        }
    }
#if _DUMP_LOG
    TAUL_LOG(self().lgr, "*** end-_resolve_pending ***");
    TAUL_LOG(self().lgr, "result: {}", result);
#endif
    return result;
}

void taul::lexer::puller::_generate_pending() {
    if (_has_pending()) return;
#if _DUMP_LOG
    TAUL_LOG(self().lgr, "\nlexer::puller::_generate_pending()");
    TAUL_LOG(self().lgr, "*** begin-_generate_pending ***");
#endif
    size_t before = self()._input.number();
    _pending = _resolve_pending();
    size_t after = self()._input.number();
    _last_pending_consumed_no_input = before == after;
    self()._input.forget();
#if _DUMP_LOG
    TAUL_LOG(self().lgr, "*** end-_generate_pending ***");
    TAUL_LOG(self().lgr, "_pending == {}", _pending ? _pending->fmt() : "n/a");
    TAUL_LOG(self().lgr, "_last_pending_consumed_no_input == {}", _last_pending_consumed_no_input);
#endif
}

bool taul::lexer::puller::_try_merge_pending_into_current() {
    TAUL_ASSERT(_has_pending());
    if (!_has_current()) { // merge succeeds if no current
        std::swap(_current, _pending);
        return true;
    }
    if (_current->is_failure() && // merge succeeds if both are failures, and are contiguous
        _pending->is_failure() &&
        _current->high_pos() == _pending->low_pos()) {
        _current->len += _pending->len;
        _pending.reset();
        return true;
    }
    return false; // merge failure
}

taul::token taul::lexer::puller::_consume_current() {
    TAUL_ASSERT(_has_current());
    token result = _current.value();
    _current.reset();
    return result;
}

taul::token taul::lexer::puller::_pull_no_cut() {
    TAUL_ASSERT(!_has_current());
#if _DUMP_LOG
    TAUL_LOG(self().lgr, "\nlexer::puller::_pull_no_cut()");
    TAUL_LOG(self().lgr, "*** begin-_pull_no_cut ***");
#endif
    do {
        _generate_pending();
        TAUL_ASSERT(_has_pending());
    } while (_try_merge_pending_into_current());
    auto result = _consume_current();
#if _DUMP_LOG
    TAUL_LOG(self().lgr, "*** end-_pull_no_cut ***");
    TAUL_LOG(self().lgr, "result: {}", result);
#endif
    return result;
}

void taul::lexer::_resolve_latest_token() {
    if (!_latest) _latest = std::make_optional(_puller.pull());
    TAUL_ASSERT(_latest);
}

void taul::lexer::_advance_output_stream() {
    _latest.reset(); // force next _peek call to pull
}

taul::token taul::lexer::_peek() {
    TAUL_ASSERT(_valid);
    _resolve_latest_token();
    return _latest.value();
}

taul::token taul::lexer::_next() {
    TAUL_ASSERT(_valid);
    token result = _peek();
    _advance_output_stream();
    return result;
}

bool taul::lexer::_done() {
    TAUL_ASSERT(_valid);
    return _peek().is_end();
}

void taul::lexer::_reset() {
    _input.reset();
    _puller.reset();
    _latest.reset();
    if (_source) _source->reset();
    _valid = true;
}

