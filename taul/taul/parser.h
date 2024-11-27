

#pragma once


#include "base_parser.h"
#include "error_handler.h"

#include "internal/parse_table.h"
#include "internal/parsing_system.h"


namespace taul {


    class parser final : public base_parser {
    public:

        parser(grammar gram, std::shared_ptr<logger> lgr = nullptr);

        virtual ~parser() noexcept = default;


        void bind_source(token_stream* source) override final;
        void bind_source(std::shared_ptr<token_stream> source) override final;
        void bind_listener(listener* listener) override final;
        void bind_listener(std::shared_ptr<listener> listener) override final;
        void bind_error_handler(error_handler* error_handler) override final;
        void bind_error_handler(std::shared_ptr<error_handler> error_handler) override final;
        parse_tree parse(ppr_ref start_rule) override final;
        parse_tree parse(const str& name) override final;
        void parse_notree(ppr_ref start_rule) override final;
        void parse_notree(const str& name) override final;
        token eh_peek() override final;
        token eh_next() override final;
        bool eh_done() override final;
        bool eh_check() override final;
        void reset() override final;


    private:

        struct _policy final {
            using symbol_type = token;
            using rule_ref_type = ppr_ref;
            static const internal::nonterminal_id_allocs<symbol_type>& fetch_ntia(grammar x);
            static const internal::parse_table<symbol_type>& fetch_pt(grammar x);
            symbol_type peek();
            symbol_type next();
            void reinit_output(rule_ref_type start_rule);
            std::string fmt_output() const;
            void output_startup();
            void output_shutdown();
            void output_terminal(symbol_type terminal);
            void output_nonterminal_begin(symbol_id nonterminal);
            void output_nonterminal_end();
            void output_terminal_error(symbol_range<symbol_type> ids, symbol_type input);
            void output_nonterminal_error(symbol_id id, symbol_type input);
            static constexpr bool uses_eh() noexcept { return true; }
            void eh_startup();
            void eh_shutdown();
            void eh_terminal_error(symbol_range<symbol_type> ids, symbol_type input);
            void eh_nonterminal_error(symbol_id id, symbol_type input);
            void eh_recovery_failed();


            parser* _self_ptr = nullptr; // link to parser

            parser& _get_self() const noexcept;
        };


        bool _valid = true;

        token_stream* _source;
        std::shared_ptr<token_stream> _source_ownership;
        listener* _listener;
        std::shared_ptr<listener> _listener_ownership;
        error_handler* _eh;
        std::shared_ptr<error_handler> _eh_ownership;


        bool _aborted = false; // if an abort occurred

        std::optional<parse_tree> _result; // the parse tree in production, if any

        internal::parsing_system<_policy> _ps; // the parsing system backend


        static constexpr size_t _reserved_mem_for_parse_stack = 32;


        void _perform_parse(ppr_ref start_rule);

        parse_tree _parse(ppr_ref start_rule);
        void _parse_notree(ppr_ref start_rule);
    };
}

