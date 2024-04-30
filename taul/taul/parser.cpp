

#include "parser.h"

#include "grammar.h"

#include "internal/grammar_data.h"


taul::parser::parser(grammar gram, std::shared_ptr<logger> lgr)
    : base_parser(gram, lgr),
    _source(nullptr),
    _listener(nullptr), 
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
    token result = token::end();
    TAUL_DEREF_SAFE(_self_ptr) {
        if (_self_ptr->_source) result = _self_ptr->_source->peek();
    }
    return result;
}

taul::token taul::parser::_policy::next() {
    token result = token::end();
    TAUL_DEREF_SAFE(_self_ptr) {
        if (_self_ptr->_source) result = _self_ptr->_source->next();
    }
    return result;
}

void taul::parser::_policy::reinit_output(rule_ref_type start_rule) {
    // do nothing
}

std::string taul::parser::_policy::fmt_output() const {
    std::string result{};
    TAUL_DEREF_SAFE(_self_ptr) {
        if (_self_ptr->_result) result = _self_ptr->_result.value().fmt();
    }
    return result;
}

void taul::parser::_policy::output_startup() {
    TAUL_DEREF_SAFE(_self_ptr) {
        if (_self_ptr->_listener) _self_ptr->_listener->on_startup();
    }
}

void taul::parser::_policy::output_shutdown() {
    TAUL_DEREF_SAFE(_self_ptr) {
        if (_self_ptr->_listener) _self_ptr->_listener->on_shutdown();
    }
}

void taul::parser::_policy::output_terminal(symbol_type terminal) {
    TAUL_DEREF_SAFE(_self_ptr) {
        if (_self_ptr->_result) _self_ptr->_result->lexical(terminal);
        if (_self_ptr->_listener) _self_ptr->_listener->on_lexical(terminal);
    }
}

void taul::parser::_policy::output_nonterminal_begin(symbol_id nonterminal) {
    TAUL_DEREF_SAFE(_self_ptr) {
        size_t ppr_index = size_t(nonterminal) - size_t(symbol_traits<token>::first_nonterminal_id);
        ppr_ref ppr = _self_ptr->gram.ppr_at(ppr_index);
        source_pos pos = peek().pos;
        if (_self_ptr->_result) _self_ptr->_result->syntactic(ppr, pos);
        if (_self_ptr->_listener) _self_ptr->_listener->on_syntactic(ppr, pos);
    }
}

void taul::parser::_policy::output_nonterminal_end() {
    TAUL_DEREF_SAFE(_self_ptr) {
        if (_self_ptr->_result) _self_ptr->_result->close();
        if (_self_ptr->_listener) _self_ptr->_listener->on_close();
    }
}

void taul::parser::_policy::output_terminal_error(symbol_range<symbol_type> ids, symbol_type input) {
    TAUL_DEREF_SAFE(_self_ptr) {
        if (_self_ptr->_result) _self_ptr->_result->abort(input.pos);
        if (_self_ptr->_listener) _self_ptr->_listener->on_abort(input.pos);
    }
}

void taul::parser::_policy::output_nonterminal_error(symbol_id id, symbol_type input) {
    TAUL_DEREF_SAFE(_self_ptr) {
        if (_self_ptr->_result) _self_ptr->_result->abort(input.pos);
        if (_self_ptr->_listener) _self_ptr->_listener->on_abort(input.pos);
    }
}

void taul::parser::_perform_parse(ppr_ref start_rule) {
    TAUL_ASSERT(_valid);
    TAUL_ASSERT(gram.is_associated(start_rule));
    _ps.parse(start_rule);
}

taul::parse_tree taul::parser::_parse(ppr_ref start_rule) {
    TAUL_ASSERT(!_result);
    _result = std::make_optional(parse_tree(gram));
    _perform_parse(start_rule);
    auto result = std::move(_result.value());
    _result.reset();
    return result;
}

void taul::parser::_parse_notree(ppr_ref start_rule) {
    TAUL_ASSERT(!_result);
    _perform_parse(start_rule);
}

