

#pragma once


#include "../source_code.h"
#include "../spec_error.h"
#include "../spec.h"
#include "../listener.h"

#include "util.h"


namespace taul::internal {


    class compiler_backend final : public listener {
    public:

        const bool dbgsyms = false; // if debug symbol 'pos' instructions should be generated

        const source_code* src = nullptr;
        spec_error_counter* ec = nullptr;

        grammar gram; // the TAUL grammar


        std::optional<spec> result; // will be std::nullopt if compiler encountered an error


        bool cancelled = false; // if an error occurred during syntax compilation

        template<typename... Args>
        inline void raise(source_pos pos, spec_error err, std::format_string<Args...> fmt, Args&&... args) {
            raise_error(lgr, ec, src, pos, err, fmt, std::forward<Args&&>(args)...);
            cancelled = true;
        }


        // this is compiler info that need-not be organized in a stack, as the parse tree
        // guarantees it'll be *flat*

        enum class section : std::uint8_t {
            lexer,
            parser,
        };

        // TAUL defaults to lexer section implicitly if no explicit one is specified

        section current_section = section::lexer; // what section we're currently in

        qualifier current_qualifier = qualifier::none; // the current rule qualifier
        size_t qualifier_count = 0; // how many qualifiers in a row have appeared

        str rule_name; // the name of the current rule

        spec_writer sw_lpr_decls, sw_ppr_decls; // these are used to write the LPR/PPR forward declarations
        spec_writer sw_lpr_defs, sw_ppr_defs; // these are used to write the definitions of LPRs/PPRs

        inline bool in_lpr() const noexcept { return current_section == section::lexer; }
        inline bool in_ppr() const noexcept { return current_section == section::parser; }


        // NOTE: the below 'exprs' actually correspond more to 'Alts', than to 'Expr', non-terminals

        // in order to eval suffix-based exprs correctly, we need a way to write to
        // *before* the main body of a given expr, for the suffixes' code, and they'll
        // need to be able to write in reverse order in which the suffixes appear

        // to this end we'll have two stacks: one for each nested expr, and one for
        // suffixes to push to, such that we can get their reverse order by popping

        // each frame of the former will have a number for how many frames of the
        // ladder belong to it, and thus should be processed upon popping

        struct expr final {
            spec_writer sw_main; // the *main* code of the expr
            size_t suffixes; // how many suffix stack frames belong to this expr frame
        };

        enum class suffix_type : std::uint8_t {
            optional,
            kleene_star,
            kleene_plus,
        };

        struct suffix final {
            suffix_type type;
            source_pos pos;
        };

        std::vector<expr> expr_stk;
        std::vector<suffix> suffix_stk;

        inline bool in_expr() const noexcept { return !expr_stk.empty(); }

        void push_expr();
        void pop_expr();

        void push_suffix(suffix_type type, source_pos pos);
        void pop_suffix();


        // this stack maintains which non-terminal we're in the scope of

        // this is important for the functioning of this system *in general*

        struct nonterminal final {
            ppr_ref ppr;
            source_pos pos;
        };

        std::vector<nonterminal> nonterminal_stk;

        inline bool in_nonterminal() const noexcept { return !nonterminal_stk.empty(); }

        void push_nonterminal(ppr_ref x, source_pos pos);
        void pop_nonterminal();


        compiler_backend(
            const source_code& src,
            spec_error_counter& ec,
            grammar gram,
            std::shared_ptr<logger> lgr,
            bool dbgsyms);


        void on_startup() override final;
        void on_shutdown() override final;
        void on_lexical(token tkn) override final;
        void on_syntactic(ppr_ref ppr, source_pos pos) override final;
        void on_close() override final;
        void on_abort() override final;
        void on_terminal_error(token_range ids, token input) override final;
        void on_nonterminal_error(symbol_id id, token input) override final;


    private:

        spec _build_expr_spec();

        // write to either lower expr frame, or to LPR/PPR def

        void _merge_down(const spec& x);

        void _build_result();

        void _handle_lexical(token tkn);
        void _handle_syntactic_begin();
        void _handle_syntactic_end();
    };
}

