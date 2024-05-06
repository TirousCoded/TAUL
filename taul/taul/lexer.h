

#pragma once


#include "base_lexer.h"

#include "internal/parse_table.h"
#include "internal/parsing_system.h"


namespace taul {


    class lexer final : public base_lexer {
    public:

        lexer(grammar gram, std::shared_ptr<logger> lgr = nullptr);

        virtual ~lexer() noexcept = default;


        void bind_source(glyph_stream* source) override final;
        void bind_source(std::shared_ptr<glyph_stream> source) override final;
        void bind_observer(token_observer* observer) override final;
        void bind_observer(std::shared_ptr<token_observer> observer) override final;
        token peek() override final;
        token next() override final;
        bool done() override final;
        void reset() override final;


    private:

        class output_queue;
        class input_queue;
        class matcher;
        class puller;

        // the input queue caches the glyphs input into the lexer during a pull

        // an important feature of the input queue is the ability to *replay* the
        // sequence of inputs for each LPR the lexer attempts to match against, w/
        // the input queue only pulling on the upstream stream of the lexer when
        // the input queue runs out of cached inputs to playback

        class input_queue final {
        public:

            lexer* _self;

            lexer& self() const noexcept;


            // when current_input < recorded_inputs.size(), then we know we're
            // *playing back* the cached inputs, w/ current_input telling us
            // which input we're to playback next

            // when current_input == recorded_inputs.size(), then we know we've
            // run out of cached inputs, and its time to pull from upstream

            std::vector<glyph> recorded_inputs;
            size_t current_input = 0;

            // this records the total number of inputs forgot by the 'forget' method

            size_t total_forgot = 0;


            input_queue(lexer& self, size_t initial_recorded_inputs_capacity);


            // number returns the number of inputs which have been processed, w/
            // this accounting for playback

            size_t number() const;


            void playback(); // keep recorded_inputs, reset current_input for playback
            void reset(); // clear recorded_inputs, and total reset

            // lets say the grammar has two LPRs: 'A' and 'B', and lets say that during
            // a round of pulling A's eval ends up caching 3 upstream inputs in the
            // input queue while B caches 2, and lets say that B ends up being chosen
            // for the token produced by the pull

            // in this scenario we need a way to throw out the two inputs consumed by
            // the production of the token matched by B, while still keeping the one
            // remaining input cached by A, as that one will be needed for next pull

            // thus, forget makes the input queue drop any cached data which has already
            // been played back, meaning that the current playback position is the 
            // furthest back the input queue is hereafter able to remember, thus 
            // preparing the input queue for the next round of pulling

            void forget();


            // IMPORTANT: input queue expects lexer impl to NEVER CALL peek/next if
            //            no source was injected into lexer

            glyph peek();
            glyph next();


            void skip(size_t n); // skips next n inputs
        };

        // the lexer iterates over the LPRs of its grammar, and tries to match
        // against each one, finishing when it succeeds, or exhausts all of them,
        // and thus fails

        // to this end, the matcher is responsible for each individual attempt
        // to match against a particular LPR

        // the matcher is the subcomponent which actual contains the 'parse stack'
        // of the system, and is thus responsible for using it

        class matcher final {
        public:

            lexer* _self;

            lexer& self() const noexcept;


            matcher(lexer& self, size_t initial_stack_capacity);


            token match(lpr_ref start_rule);


        private:

            struct _policy final {
                using symbol_type = glyph;
                using rule_ref_type = lpr_ref;
                static const internal::nonterminal_id_allocs<symbol_type>& fetch_ntia(grammar x);
                static const internal::parse_table<symbol_type>& fetch_pt(grammar x);
                symbol_type peek();
                symbol_type next();
                void reinit_output(rule_ref_type start_rule);
                std::string fmt_output() const;
                void output_startup();
                void output_shutdown();
                void output_terminal(symbol_type terminal);
                void output_nonterminal_begin(symbol_id);
                void output_nonterminal_end();
                void output_terminal_error(symbol_range<symbol_type> ids, symbol_type input);
                void output_nonterminal_error(symbol_id id, symbol_type input);
                static constexpr bool uses_eh() noexcept { return false; }
                inline void eh_startup() {}
                inline void eh_shutdown() {}
                inline void eh_terminal_error(symbol_range<symbol_type>, symbol_type) {}
                inline void eh_nonterminal_error(symbol_id, symbol_type) {}
                inline void eh_recovery_failed() {}


                lexer* _self_ptr = nullptr; // link to _self
                token* _result_ptr = nullptr; // link to _result
            };


            internal::parsing_system<_policy> _ps; // the parsing system backend
            token _result; // the token being built by the lexer
        };

        // the puller is responsible for performing each round of resolution of the
        // next token of the sequence output by the lexer

        // the puller is responsible for failure token concatenation
        // the puller is responsible for cutting skip tokens

        class puller final {
        public:

            lexer* _self;

            lexer& self() const noexcept;


            puller(lexer& self);


            token pull(); // perform next round of pulling

            void reset(); // reset as part of pipeline reset


        private:

            std::optional<token> _current, _pending;
            bool _last_pending_consumed_no_input = false;


            bool _at_end_of_input();

            token _match_with_lpr(size_t lpr_index);
            token _match_with_all_lpr();

            bool _has_current() const noexcept;
            bool _has_pending() const noexcept;

            token _resolve_pending();
            void _generate_pending();
            bool _try_merge_pending_into_current();
            token _consume_current();

            token _pull_no_cut();
        };


        bool _valid = true;

        glyph_stream* _source;
        std::shared_ptr<glyph_stream> _source_ownership;
        token_observer* _observer;
        std::shared_ptr<token_observer> _observer_ownership;

        input_queue _input;
        matcher _matcher;
        puller _puller;

        static constexpr size_t _reserved_mem_for_input_cache = 64;
        static constexpr size_t _reserved_mem_for_matcher_stack = 32;

        // when _next advances the stream of lexer outputs, it does so by
        // making _latest == std::nullopt to mandate the pulling of next one

        std::optional<token> _latest = std::nullopt; // the latest token pulled, if any


        void _resolve_latest_token();
        void _advance_output_stream();


        // these help avoid virtual call indirection

        token _peek();
        token _next();
        bool _done();
        void _reset();
    };
}

