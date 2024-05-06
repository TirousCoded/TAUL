

#pragma once


#include "nonterminal_id_alloc.h"
#include "parse_table.h"


#define _DUMP_LEXER_PARSING_SYSTEM_LOG 0
#define _DUMP_PARSER_PARSING_SYSTEM_LOG 0


namespace taul::internal {


    // this parsing_system class encapsulates the engine used to
    // parse some input via use of a grammar

    // herein, 'parse' is generalized to refer both to the behaviour
    // of the lexer, and the parser

    // the policy object must define:
    //      using symbol_type = ...
    //          * the symbol type used (ie. glyph or token)
    //      using rule_ref_type = ...
    //          * either lpr_ref or ppr_ref
    //      static const nonterminal_id_allocs<symbol_type>& fetch_ntia(grammar x)
    //          * returns from x the current nonterminal_id_allocs to use
    //      static const parse_table<symbol_type>& fetch_pt(grammar x)
    //          * returns from x the correct parse_table to use
    //      symbol_type peek()
    //      symbol_type next()
    //          * these are used to query the input
    //      void reinit_output(rule_ref_type start_rule)
    //          * prepares output state for new round of parsing
    //      std::string fmt_output() const
    //          * returns a string summarizing parser output
    //      void output_startup()
    //      void output_shutdown()
    //          * reports the beginning/ending of a round of usage
    //      void output_terminal(symbol_type terminal)
    //          * reports an output terminal
    //      void output_nonterminal_begin(symbol_id nonterminal)
    //      void output_nonterminal_end()
    //          * reports the begin/end of the scope of an output non-terminal
    //          * transparent helper non-terminals will not be output
    //      void output_terminal_error(symbol_range<symbol_type> ids, symbol_type input)
    //      void output_nonterminal_error(symbol_id id, symbol_type input)
    //          * reports errors arising w/ terminal/non-terminal symbols
    //          * reports symbol ID(s) and input at place of error
    //      static constexpr bool uses_eh() noexcept
    //          * returns if the system uses an error handler at all (ie. it's a parser)
    //      void eh_startup()
    //      void eh_shutdown()
    //      void eh_terminal_error(symbol_range<symbol_type> ids, symbol_type input)
    //      void eh_nonterminal_error(symbol_id id, symbol_type input)
    //      void eh_recovery_failed()
    //          * these invoke the error handler
    //          * eh_recovery_failed is called upon a failed recovery attempt

    
    template<typename Policy>
    class parsing_system final {
    public:

        using symbol_type = typename Policy::symbol_type;
        using rule_ref_type = typename Policy::rule_ref_type;


        std::shared_ptr<logger> lgr;


        inline parsing_system(
            Policy policy,
            grammar gram,
            size_t initial_stack_capacity,
            std::shared_ptr<logger> lgr = nullptr);


        inline bool parse(rule_ref_type start_rule);


        // this is used to impl eh_check for the parser

        inline bool check();


        inline std::string fmt_stack(const char* tab = "    ") const;


    private:

        Policy _policy; // the policy object
        grammar _gram; // the grammar being used

        // remember that this stack should be thought of as growing
        // from right-to-left, not left-to-right

        struct _item final {
            pt_term<symbol_type> term; // the terminal/non-terminal symbol

            // the 'depth' of a symbol's stack item describes how many
            // layers of non-terminal scopes it is nested within

            // when we process a non-terminal, we give its resulting symbols
            // a depth one greater than the non-terminal which created them,
            // thus showing them to be *nested within* it

            // transparent helper non-terminals do not do this however,
            // instead just passing their depth to the symbols they create,
            // thus denying said transparent non-terminals identity

            size_t depth;
        };

        std::vector<_item> _stack; // the parse stack

        // this measures the 'current depth' of the parsing system

        // whenever we enter the scope of a non-transparent non-terminal, we
        // increment this value, indicating we're within its scope

        // whenever we process a symbol w/ a depth which is lower than the
        // current depth, we low the current depth to match it

        // for every unit of current depth removed during this process, we
        // report to the output that we've exited a non-terminal's scope, as
        // this act of lower is the indicator that its scope has ended

        size_t _current_depth = 0;


        inline std::optional<size_t> _lookup_in_pt(symbol_id nonterminal, symbol_id terminal) const;


        inline void _setup(rule_ref_type start_rule);
        inline void _reinit(rule_ref_type start_rule);

        inline std::optional<_item> _consume_top();

        inline void _handle_nonterminal_ending(size_t target_depth);

        inline symbol_type _fetch_input_noadvance();
        inline symbol_type _fetch_input_advance();

        inline bool _try_apply_terminal(const pt_terminal<symbol_type>& terminal, symbol_type input);
        inline bool _try_apply_nonterminal(const pt_nonterminal& nonterminal, symbol_type input);

        inline void _push_terms(size_t pt_index);

        // these handle pushing/popping to/from parse stack

        inline void _push_terminal(symbol_id terminal, bool assertion = false);
        inline void _push_nonterminal(symbol_id nonterminal);
        inline void _push(pt_term<symbol_type>&& x);
        inline _item _pop();

        // these manage _current_depth, and outputting to _policy

        inline void _output_startup();
        inline void _output_shutdown();
        inline void _output_terminal(symbol_type terminal);
        inline void _output_nonterminal_begin(symbol_id nonterminal);
        inline void _output_nonterminal_end();

        inline void _startup();
        inline void _shutdown();

        inline bool _match_terminal_with_eh(const pt_terminal<symbol_type>& terminal);
        inline bool _match_nonterminal_with_eh(const pt_nonterminal& nonterminal);
        inline bool _match_term(const pt_term<symbol_type>& term);

        inline bool _parse(rule_ref_type start_rule);


        bool _check_is_for_terminal = true;
        symbol_range<symbol_type> _check_terminal_ids = {};
        symbol_id _check_nonterminal_id = {};


        inline void _bind_check_for_terminal(const pt_terminal<symbol_type>& terminal);
        inline void _bind_check_for_nonterminal(const pt_nonterminal& nonterminal);

        inline bool _check();


        inline void _debug_transparent_nonterminal();
        inline void _debug_match_terminal_fail();
        inline void _debug_match_nonterminal_fail();
        inline void _debug_parse_begin();
        inline void _debug_parse_end_success();
        inline void _debug_parse_step();
    };


    template<typename Policy>
    inline taul::internal::parsing_system<Policy>::parsing_system(Policy policy, grammar gram, size_t initial_stack_capacity, std::shared_ptr<logger> lgr)
        : lgr(lgr),
        _policy(std::move(policy)),
        _gram(gram) {
        _stack.reserve(initial_stack_capacity);
    }

    template<typename Policy>
    inline bool taul::internal::parsing_system<Policy>::parse(rule_ref_type start_rule) {
        return _parse(start_rule);
    }

    template<typename Policy>
    inline bool parsing_system<Policy>::check() {
        return _check();
    }

    template<typename Policy>
    inline std::string parsing_system<Policy>::fmt_stack(const char* tab) const {
        TAUL_ASSERT(tab);
        std::string stk{};
        for (auto it = _stack.rbegin(); it != _stack.rend(); it++) {
            stk += std::format("\n{}{}", tab, it->term.fmt());
        }
        return std::format("stack (top -> bottom): {}", stk);
    }
    
    template<typename Policy>
    inline std::optional<size_t> parsing_system<Policy>::_lookup_in_pt(symbol_id nonterminal, symbol_id terminal) const {
        const auto& pt = _policy.fetch_pt(_gram);
        return pt.lookup(nonterminal, pt.grouper(terminal));
    }
    
    template<typename Policy>
    inline void parsing_system<Policy>::_setup(rule_ref_type start_rule) {
        _reinit(start_rule); // reinit state
        // prepare our parse stack
        _push_nonterminal(start_rule.id());
    }

    template<typename Policy>
    inline void parsing_system<Policy>::_reinit(rule_ref_type start_rule) {
        _stack.clear();
        _current_depth = 0;
        _policy.reinit_output(start_rule);
    }
    
    template<typename Policy>
    inline std::optional<typename parsing_system<Policy>::_item> parsing_system<Policy>::_consume_top() {
        return
            _stack.empty()
            ? std::nullopt
            : std::make_optional(_pop());
    }

    template<typename Policy>
    inline void parsing_system<Policy>::_handle_nonterminal_ending(size_t target_depth) {
        while (_current_depth > target_depth) _output_nonterminal_end();
    }
    
    template<typename Policy>
    inline typename Policy::symbol_type parsing_system<Policy>::_fetch_input_noadvance() {
        return _policy.peek();
    }

    template<typename Policy>
    inline typename Policy::symbol_type parsing_system<Policy>::_fetch_input_advance() {
        return _policy.next();
    }
    
    template<typename Policy>
    inline bool parsing_system<Policy>::_try_apply_terminal(const pt_terminal<symbol_type>& terminal, symbol_type input) {
        const bool success = terminal.ids.contains(input.id);
        if (success) {
            if (!terminal.assertion) _output_terminal(input);
        }
        return success;
    }
    
    template<typename Policy>
    inline bool parsing_system<Policy>::_try_apply_nonterminal(const pt_nonterminal& nonterminal, symbol_type input) {
        const auto pt_index = _lookup_in_pt(nonterminal.id, input.id);
        if (pt_index) {
            _output_nonterminal_begin(nonterminal.id);
            _push_terms(pt_index.value());
        }
        return (bool)pt_index;
    }
    
    template<typename Policy>
    inline void parsing_system<Policy>::_push_terms(size_t pt_index) {
        const auto& rules = _policy.fetch_pt(_gram).rules;
        TAUL_ASSERT(pt_index < rules.size());
        const auto& terms = rules[pt_index].terms;
        // iterate backwards through terms to push them to parse stack
        for (auto it = terms.crbegin(); it != terms.crend(); it++) {
            _push(pt_term<symbol_type>(*it));
        }
    }

    template<typename Policy>
    inline void parsing_system<Policy>::_push_terminal(symbol_id terminal, bool assertion) {
        _push(pt_term<symbol_type>::init_terminal(terminal, terminal, assertion));
    }

    template<typename Policy>
    inline void parsing_system<Policy>::_push_nonterminal(symbol_id nonterminal) {
        _push(pt_term<symbol_type>::init_nonterminal(nonterminal));
    }

    template<typename Policy>
    inline void parsing_system<Policy>::_push(pt_term<symbol_type>&& x) {
        _item item{
            .term = std::forward<pt_term<symbol_type>&&>(x),
            .depth = _current_depth,
        };
        _stack.push_back(std::move(item));
    }

    template<typename Policy>
    inline typename parsing_system<Policy>::_item parsing_system<Policy>::_pop() {
        TAUL_ASSERT(!_stack.empty());
        const auto result = _stack.back();
        _stack.pop_back();
        return result;
    }

    template<typename Policy>
    inline void parsing_system<Policy>::_output_startup() {
        _policy.output_startup();
    }

    template<typename Policy>
    inline void parsing_system<Policy>::_output_shutdown() {
        _handle_nonterminal_ending(0);
        _policy.output_shutdown();
    }

    template<typename Policy>
    inline void parsing_system<Policy>::_output_terminal(symbol_type terminal) {
        _policy.output_terminal(terminal);
    }
    
    template<typename Policy>
    inline void parsing_system<Policy>::_output_nonterminal_begin(symbol_id nonterminal) {
        if (_policy.fetch_ntia(_gram).is_transparent(nonterminal)) {
            _debug_transparent_nonterminal();
            return;
        }
        _policy.output_nonterminal_begin(nonterminal);
        _current_depth++;
    }
    
    template<typename Policy>
    inline void parsing_system<Policy>::_output_nonterminal_end() {
        TAUL_ASSERT(_current_depth > 0);
        _policy.output_nonterminal_end();
        _current_depth--;
    }

    template<typename Policy>
    inline void parsing_system<Policy>::_startup() {
        _output_startup();
        if constexpr (Policy::uses_eh()) _policy.eh_startup();
    }
    
    template<typename Policy>
    inline void parsing_system<Policy>::_shutdown() {
        if constexpr (Policy::uses_eh()) _policy.eh_shutdown();
        _output_shutdown();
    }

    template<typename Policy>
    inline bool parsing_system<Policy>::_match_terminal_with_eh(const pt_terminal<symbol_type>& terminal) {
        auto input = _fetch_input_noadvance();
        auto result = _try_apply_terminal(terminal, input);

        if (!result) { // if failed, try recovering, then try again
            _policy.output_terminal_error(terminal.ids, input); // report error
            if constexpr (Policy::uses_eh()) { // only attempt recovery if uses eh
                _bind_check_for_terminal(terminal); // prep for recovery attempt
                _policy.eh_terminal_error(terminal.ids, input); // attempt recovery

                input = _fetch_input_noadvance(); // resample input
                result = _try_apply_terminal(terminal, input); // retry
            }
        }
        if (result) {
            // for terminals, for the error handler to work as expected, we want to
            // NOT advance the input state until AFTER we've finished everything above
            if (!terminal.assertion) _fetch_input_advance();
        }
        else {
            _policy.eh_recovery_failed();
            _debug_match_terminal_fail();
        }
        return result;
    }

    template<typename Policy>
    inline bool parsing_system<Policy>::_match_nonterminal_with_eh(const pt_nonterminal& nonterminal) {
        auto input = _fetch_input_noadvance();
        auto result = _try_apply_nonterminal(nonterminal, input);

        if (!result) { // if failed, try recovering, then try again
            _policy.output_nonterminal_error(nonterminal.id, input); // report error
            if constexpr (Policy::uses_eh()) { // only attempt recovery if uses eh
                _bind_check_for_nonterminal(nonterminal); // prep for recovery attempt
                _policy.eh_nonterminal_error(nonterminal.id, input); // attempt recovery

                input = _fetch_input_noadvance(); // resample input
                result = _try_apply_nonterminal(nonterminal, input); // retry
            }
        }
        if (!result) {
            _policy.eh_recovery_failed();
            _debug_match_nonterminal_fail();
        }
        return result;
    }

    template<typename Policy>
    inline bool parsing_system<Policy>::_match_term(const pt_term<symbol_type>& term) {
        TAUL_ASSERT(term.is_terminal() || term.is_nonterminal());
        return
            term.is_terminal()
            ? _match_terminal_with_eh(term.terminal())
            : _match_nonterminal_with_eh(term.nonterminal());
    }

    template<typename Policy>
    inline bool parsing_system<Policy>::_parse(rule_ref_type start_rule) {
        _setup(start_rule);
        _debug_parse_begin();
        _startup();
        while (true) {
            _debug_parse_step();
            const auto top = _consume_top();
            if (!top) break; // exiting here means we're successful
            _handle_nonterminal_ending(top.value().depth);
            if (!_match_term(top.value().term)) {
                _shutdown();
                return false; // failure
            }
        }
        _shutdown();
        _debug_parse_end_success();
        return true; // success
    }

    template<typename Policy>
    inline void parsing_system<Policy>::_bind_check_for_terminal(const pt_terminal<symbol_type>& terminal) {
        _check_is_for_terminal = true;
        _check_terminal_ids = terminal.ids;
    }
    
    template<typename Policy>
    inline void parsing_system<Policy>::_bind_check_for_nonterminal(const pt_nonterminal& nonterminal) {
        _check_is_for_terminal = false;
        _check_nonterminal_id = nonterminal.id;
    }
    
    template<typename Policy>
    inline bool parsing_system<Policy>::_check() {
        return
            _check_is_for_terminal
            ? _check_terminal_ids.contains(_fetch_input_noadvance().id)
            : _lookup_in_pt(_check_nonterminal_id, _fetch_input_noadvance().id).has_value();
    }

    template<typename Policy>
    inline void parsing_system<Policy>::_debug_transparent_nonterminal() {
        if constexpr (std::is_same_v<symbol_type, glyph>) {
#if _DUMP_LEXER_PARSING_SYSTEM_LOG
            TAUL_LOG(lgr, "forgoing begin call for transparent non-terminal {}", nonterminal);
#endif
        }
        else {
#if _DUMP_PARSER_PARSING_SYSTEM_LOG
            TAUL_LOG(lgr, "forgoing begin call for transparent non-terminal {}", nonterminal);
#endif
        }
    }

    template<typename Policy>
    inline void parsing_system<Policy>::_debug_match_terminal_fail() {
        if constexpr (std::is_same_v<symbol_type, glyph>) {
#if _DUMP_LEXER_PARSING_SYSTEM_LOG
            TAUL_LOG(lgr, "\n*** end (failure) ***");
            TAUL_LOG(lgr, "cause: failed terminal comparison");
#endif
        }
        else {
#if _DUMP_PARSER_PARSING_SYSTEM_LOG
            TAUL_LOG(lgr, "\n*** end (failure) ***");
            TAUL_LOG(lgr, "cause: failed terminal comparison");
#endif
        }
    }

    template<typename Policy>
    inline void parsing_system<Policy>::_debug_match_nonterminal_fail() {
        if constexpr (std::is_same_v<symbol_type, glyph>) {
#if _DUMP_LEXER_PARSING_SYSTEM_LOG
            TAUL_LOG(lgr, "\n*** end (failure) ***");
            TAUL_LOG(lgr, "cause: failed parse table lookup");
#endif
        }
        else {
#if _DUMP_PARSER_PARSING_SYSTEM_LOG
            TAUL_LOG(lgr, "\n*** end (failure) ***");
            TAUL_LOG(lgr, "cause: failed parse table lookup");
#endif
        }
    }

    template<typename Policy>
    inline void parsing_system<Policy>::_debug_parse_begin() {
        if constexpr (std::is_same_v<symbol_type, glyph>) {
#if _DUMP_LEXER_PARSING_SYSTEM_LOG
            TAUL_LOG(lgr, "\ninternal::parsing_system<Policy>::parse({})", start_rule);
            TAUL_LOG(lgr, "*** begin ***");
#endif
        }
        else {
#if _DUMP_PARSER_PARSING_SYSTEM_LOG
            TAUL_LOG(lgr, "\ninternal::parsing_system<Policy>::parse({})", start_rule);
            TAUL_LOG(lgr, "*** begin ***");
#endif
        }
    }
    
    template<typename Policy>
    inline void parsing_system<Policy>::_debug_parse_end_success() {
        if constexpr (std::is_same_v<symbol_type, glyph>) {
#if _DUMP_LEXER_PARSING_SYSTEM_LOG
            TAUL_LOG(lgr, "\n*** end (success) ***");
            TAUL_LOG(lgr, "result: {}", _policy.fmt_output());
#endif
        }
        else {
#if _DUMP_PARSER_PARSING_SYSTEM_LOG
            TAUL_LOG(lgr, "\n*** end (success) ***");
            TAUL_LOG(lgr, "result: {}", _policy.fmt_output());
#endif
        }
    }
    
    template<typename Policy>
    inline void parsing_system<Policy>::_debug_parse_step() {
        if constexpr (std::is_same_v<symbol_type, glyph>) {
#if _DUMP_LEXER_PARSING_SYSTEM_LOG
            TAUL_LOG(lgr, "\n*** step ***");
            TAUL_LOG(lgr, "input: {}", _policy.peek());
            TAUL_LOG(lgr, "output: {}", _policy.fmt_output());
            TAUL_LOG(lgr, "{}", fmt_stack());
#endif
        }
        else {
#if _DUMP_PARSER_PARSING_SYSTEM_LOG
            TAUL_LOG(lgr, "\n*** step ***");
            TAUL_LOG(lgr, "input: {}", _policy.peek());
            TAUL_LOG(lgr, "output: {}", _policy.fmt_output());
            TAUL_LOG(lgr, "{}", fmt_stack());
#endif
        }
    }
}

