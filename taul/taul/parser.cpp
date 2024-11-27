

#include "parser.h"

#include "grammar.h"

#include "internal/grammar_data.h"


taul::parser::parser(grammar gram, std::shared_ptr<logger> lgr)
    : base_parser(gram, lgr),
    _source(nullptr),
    _listener(nullptr),
    _eh(nullptr),
    _ps(_policy{ this }, gram, _reserved_mem_for_parse_stack, lgr) {}

void taul::parser::bind_source(token_stream* source) {
    _source = source;
    _source_ownership = nullptr;
    _valid = false;
}

void taul::parser::bind_source(std::shared_ptr<token_stream> source) {
    _source = source.get();
    _source_ownership = source;
    _valid = false;
}

void taul::parser::bind_listener(listener* listener) {
    _listener = listener;
    _listener_ownership = nullptr;
    //_valid = false; // <- listener change doesn't require reset
}

void taul::parser::bind_listener(std::shared_ptr<listener> listener) {
    _listener = listener.get();
    _listener_ownership = listener;
    //_valid = false; // <- listener change doesn't require reset
}

void taul::parser::bind_error_handler(error_handler* error_handler) {
    _eh = error_handler;
    _eh_ownership = nullptr;
    //_valid = false; // <- error handler change doesn't require reset
}

void taul::parser::bind_error_handler(std::shared_ptr<error_handler> error_handler) {
    _eh = error_handler.get();
    _eh_ownership = error_handler;
    //_valid = false; // <- error handler change doesn't require reset
}

taul::parse_tree taul::parser::parse(ppr_ref start_rule) {
    return _parse(start_rule);
}

taul::parse_tree taul::parser::parse(const str& name) {
    TAUL_ASSERT(gram.has_ppr(name));
    return _parse(gram.ppr(name).value());
}

void taul::parser::parse_notree(ppr_ref start_rule) {
    _parse_notree(start_rule);
}

void taul::parser::parse_notree(const str& name) {
    TAUL_ASSERT(gram.has_ppr(name));
    _parse_notree(gram.ppr(name).value());
}

taul::token taul::parser::eh_peek() {
    return
        _source
        ? _source->peek()
        : token::end(0);
}

taul::token taul::parser::eh_next() {
    return
        _source
        ? _source->next()
        : token::end(0);
}

bool taul::parser::eh_done() {
    return
        _source
        ? _source->done()
        : true;
}

bool taul::parser::eh_check() {
    return _ps.check();
}

void taul::parser::reset() {
    TAUL_ASSERT(!_result);
    if (_source) _source->reset();
    _valid = true;
}

const taul::internal::nonterminal_id_allocs<taul::token>& taul::parser::_policy::fetch_ntia(grammar x) {
    return internal::launder_grammar_data(x)._ppr_id_allocs;
}

const taul::internal::parse_table<taul::token>& taul::parser::_policy::fetch_pt(grammar x) {
    return internal::launder_grammar_data(x)._ppr_pt;
}

taul::token taul::parser::_policy::peek() {
    return
        _get_self()._source
        ? _get_self()._source->peek()
        : token::end();
}

taul::token taul::parser::_policy::next() {
    return
        _get_self()._source
        ? _get_self()._source->next()
        : token::end();
}

void taul::parser::_policy::reinit_output(rule_ref_type start_rule) {
    // do nothing
}

std::string taul::parser::_policy::fmt_output() const {
    return
        _get_self()._result
        ? _get_self()._result.value().fmt()
        : std::string{};
}

void taul::parser::_policy::output_startup() {
    _get_self()._aborted = false;
    if (_get_self()._listener) _get_self()._listener->on_startup();
}

void taul::parser::_policy::output_shutdown() {
    if (_get_self()._aborted) {
        if (_get_self()._result) _get_self()._result->abort();
        if (_get_self()._listener) _get_self()._listener->on_abort();
    }
    if (_get_self()._listener) _get_self()._listener->on_shutdown();
}

void taul::parser::_policy::output_terminal(symbol_type terminal) {
    if (_get_self()._result) _get_self()._result->lexical(terminal);
    if (_get_self()._listener) _get_self()._listener->on_lexical(terminal);
}

void taul::parser::_policy::output_nonterminal_begin(symbol_id nonterminal) {
    size_t ppr_index = size_t(nonterminal) - size_t(symbol_traits<token>::first_nonterminal_id);
    ppr_ref ppr = _get_self().gram.ppr_at(ppr_index);
    source_pos pos = peek().pos;
    if (_get_self()._result) _get_self()._result->syntactic(ppr, pos);
    if (_get_self()._listener) _get_self()._listener->on_syntactic(ppr, pos);
}

void taul::parser::_policy::output_nonterminal_end() {
    if (_get_self()._result) _get_self()._result->close();
    if (_get_self()._listener) _get_self()._listener->on_close();
}

void taul::parser::_policy::output_terminal_error(symbol_range<symbol_type> ids, symbol_type input) {
    if (_get_self()._listener) _get_self()._listener->on_terminal_error(ids, input);
}

void taul::parser::_policy::output_nonterminal_error(symbol_id id, symbol_type input) {
    if (_get_self()._listener) _get_self()._listener->on_nonterminal_error(id, input);
}

void taul::parser::_policy::eh_startup() {
    if (_get_self()._eh) _get_self()._eh->startup(&_get_self());
}

void taul::parser::_policy::eh_shutdown() {
    if (_get_self()._eh) _get_self()._eh->shutdown();
}

void taul::parser::_policy::eh_terminal_error(symbol_range<symbol_type> ids, symbol_type input) {
    if (_get_self()._eh) _get_self()._eh->terminal_error(ids, input);
}

void taul::parser::_policy::eh_nonterminal_error(symbol_id id, symbol_type input) {
    if (_get_self()._eh) _get_self()._eh->nonterminal_error(id, input);
}

void taul::parser::_policy::eh_recovery_failed() {
    _get_self()._aborted = true;
}

taul::parser& taul::parser::_policy::_get_self() const noexcept {
    return deref_assert(_self_ptr);
}

void taul::parser::_perform_parse(ppr_ref start_rule) {
    TAUL_ASSERT(_valid);
    TAUL_ASSERT(gram.is_associated(start_rule));
    _ps.parse(start_rule);
}

taul::parse_tree taul::parser::_parse(ppr_ref start_rule) {
    TAUL_ASSERT(!_result);
    _result = parse_tree(gram);
    _perform_parse(start_rule);
    const auto result = std::move(_result.value());
    _result.reset();
    return result;
}

void taul::parser::_parse_notree(ppr_ref start_rule) {
    TAUL_ASSERT(!_result);
    _perform_parse(start_rule);
}

