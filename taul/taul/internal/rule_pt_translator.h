

#pragma once


#include "../spec.h"

#include "nonterminal_id_alloc.h"
#include "parse_table.h"


namespace taul::internal {


    // this impls the behaviour of translating a high-level TAUL grammar
    // into a low-level parse table w/ regards to its lexer component

    // the instruction methods of this object are to be called inline w/
    // the main spec interpreter, at points appropriate for the creation
    // of the lexer component's parse table to occur


    class rule_pt_translator final {
    public:

        source_pos* pos_ptr;


        // this indicates if the loader detected that the spec is invalid, and so that
        // the rule translator should stop, as invariants may have been violated

        bool cancelled = false;


        nonterminal_id_alloc<glyph> lexer_id_alloc;
        nonterminal_id_alloc<token> parser_id_alloc;
        parse_table_build_details<glyph> lexer_ptbd;
        parse_table_build_details<token> parser_ptbd;
        parse_table<glyph> lexer_pt;
        parse_table<token> parser_pt;

        // these help lookup non-terminal symbol IDs

        std::unordered_map<std::string_view, symbol_id> main_nonterminal_name_map_lxr;
        std::unordered_map<std::string_view, symbol_id> main_nonterminal_name_map_psr;

        inline bool is_lpr_name(std::string_view name) const noexcept { return main_nonterminal_name_map_lxr.contains(name); }
        inline bool is_ppr_name(std::string_view name) const noexcept { return main_nonterminal_name_map_psr.contains(name); }

        // *most* composite exprs get their own 'mode', which allows a single subrule
        // to encapsulate it entirely, however some (ie. kleene-plus) may not be so
        // easy to summarize, and so don't get a dedicated mode all to themself

        // take note also that the below modes are thusly not restricted to just 
        // defining the exprs they're named after, and can be used liberally

        enum class mode : std::uint8_t {
            lpr,
            ppr,
            sequence,
            lookahead,
            lookahead_not,
            not0,
            optional,
            kleene_star,
        };

        static inline std::string fmt_mode(mode m) {
            std::string result{};
            switch (m) {
            case mode::lpr: result = "lpr"; break;
            case mode::ppr: result = "ppr"; break;
            case mode::sequence: result = "sequence"; break;
            case mode::lookahead: result = "lookahead"; break;
            case mode::lookahead_not: result = "lookahead-not"; break;
            case mode::not0: result = "not"; break;
            case mode::optional: result = "optional"; break;
            case mode::kleene_star: result = "kleene-star"; break;
            default: TAUL_DEADEND; break;
            }
            return result;
        }

        // the rule translator operates via a stack machine of 'frames', one for
        // each subrule

        // frame state is dictated by the semantic mode of it, and the details of
        // downstream frames nested within it, which the final output state of the
        // translator w/ regards to the frame is a function of

        struct frame final {
            symbol_id nonterminal; // the current non-terminal
            size_t rule; // the current rule index
            mode m; // the semantic 'mode'
            glyph_set set_lxr; // set for 'set-like' subrules
            token_set set_psr; // set for 'set-like' subrules

            // by default, subrule begins/ends are each intended to correspond to one
            // pair of on_###/on_close method calls, meaning that on_close need only
            // call end_subrule to end whatever the current subrule is

            // however, this is not powerful enough for things like kleene-plus, which
            // requires >1 helper subrules to define fully, so we need to communicate
            // to on_close how many additional times end_subrule should be called

            // to solve this, we'll have the begin_subrule chain for exprs like kleene-plus
            // have all but the inner-most nested subrule be marked as 'autoend', which
            // tells on_close to ALSO call end_subrule for those as well, w/ it continuing
            // to call end_subrule until the *main* subrule, and the *helpers* have all
            // been ended

            // the inner-most nested subrule of the group is NOT marked 'autoend' as we need
            // a way to tell on_close NOT to continue calling end_subrule for subrules past
            // the current group, so the next group's inner-most nested subrule not being
            // marked 'autoend' helps tell on_close when to stop

            bool autoend;


            // for lookahead, lookahead-not, and not, the way we'll do things is that we'll
            // collect up the terminals within it into this set, and then in end_subrule,
            // we'll build all of the alternatives there

            // we'll call these 'set-like' subrules

            inline bool is_set_like() const noexcept {
                return
                    m == mode::lookahead ||
                    m == mode::lookahead_not ||
                    m == mode::not0;
            }

            // for lookahead and lookahead-not, we want their terminals to be marked
            // as 'assertion' in the parse table

            // notice that we do NOT want this for not, which is not an assertion

            inline bool is_assertion() const noexcept {
                return
                    m == mode::lookahead ||
                    m == mode::lookahead_not;
            }

            // for lookahead-not and not, we want their 'sets' to be inverted

            // in both cases, the end-of-input and epsilon is removed, w/ the former
            // being that the end-of-input is supposed to be added to the FIRST set as
            // a result of an empty alternative, and NOT due to an inversion

            // in case of not, we also want end-of-input symbols removed as not exprs
            // are not supposed to match the end-of-input in any scenario

            inline bool is_inverted() const noexcept {
                return
                    m == mode::lookahead_not ||
                    m == mode::not0;
            }

            // for optional and kleene-star, we want to, upon the pushing of their subrules,
            // immediately end the first alternative and begin the next, ensuring that they
            // have their expected empty alternative

            inline bool is_optional() const noexcept {
                return
                    m == mode::optional ||
                    m == mode::kleene_star;
            }

            // for kleene-star, we need a way to indicate that after popping a subrule, 
            // that the end_subrule *immediately* pushes a recursive non-terminal term of 
            // the popped non-terminal's symbol ID

            inline bool is_recursive() const noexcept {
                return
                    m == mode::kleene_star;
            }
        };

        std::vector<frame> stk; // the central frame stack

        // if we're in the scope of an LPR/PPR, then this dictates which one it is

        // this value is not meaningful outside of LPRs/PPRs and should be disregarded
        // in those cases

        bool in_lpr_not_ppr = true;

        inline bool is_in_lpr_not_ppr() const noexcept {
            assert_in_subrule();
            return in_lpr_not_ppr;
        }

        // in 'set-like' subrules, we want 'sequence' exprs to be *transparent*, and so to 
        // do this we'll have on_sequence incr this rather than call begin_subrule, and likewise
        // if sequence_counter >= 1, on_close will decr it, rather than call end_subrule

        // this feels a bit hacky, but it gets the job done

        size_t sequence_counter = 0;


        inline rule_pt_translator(source_pos& pos) 
            : pos_ptr(&pos) {}


        inline void cancel() noexcept { cancelled = true; }


        void assert_in_subrule() const noexcept;
        void assert_not_in_subrule() const noexcept;


        // this is used to quickly fetch name objects when we can avoid an alloc

        str fetch_name_str_lxr(symbol_id x) const noexcept;
        str fetch_name_str_psr(symbol_id x) const noexcept;
        str fetch_name_str_lxr(std::string_view name) const noexcept;
        str fetch_name_str_psr(std::string_view name) const noexcept;

        // this is for quick lookup of non-terminal symbol IDs

        symbol_id fetch_symbol_lxr(std::string_view name) const noexcept;
        symbol_id fetch_symbol_psr(std::string_view name) const noexcept;


        // these add new main/helper non-terminal symbol mappings

        symbol_id add_main_symbol_lxr(str name);
        symbol_id add_main_symbol_psr(str name);
        symbol_id add_helper_symbol_lxr(str name);
        symbol_id add_helper_symbol_psr(str name);


        // these used to begin/end a new subrule, be it a main or helper one

        void begin_subrule(symbol_id x, mode m, bool autoend = false);
        void end_subrule();

        // this is used to begin a new alternative of the current subrule

        // the ***_nocheck variant doesn't care if it's inside of a set-like subrule,
        // which is only needed so we can impl the set-like stuff in end_subrule properly

        void next_alternative();
        void next_alternative_nocheck();

        // this is used to add a new terminal ref, accounting for assertion/inversion

        void next_terminal(symbol_id low, symbol_id high, bool assertion = false);
        void next_terminal(symbol_id x, bool assertion = false);

        // this version accounts for adding sets, as we can't simply iterate over a set
        // and call w/ next_terminal, as that won't work, we must invert the set as part
        // of the method call itself

        void next_terminal_set(const glyph_set& set, bool assertion = false);
        void next_terminal_set(std::u32string_view charset_str, bool assertion = false); // for charset strings

        // this is used to add a new non-terminal ref, accounting for assertion/inversion

        void next_nonterminal(symbol_id nonterminal);


        void on_startup();
        void on_shutdown();

        static_assert(spec_opcodes == 21);

        void on_pos(source_pos);
        void on_close();
        void on_alternative();
        void on_lpr_decl(std::string_view name);
        void on_ppr_decl(std::string_view name);
        void on_lpr(std::string_view name, qualifier);
        void on_ppr(std::string_view name, qualifier);

        void on_end();
        void on_any();
        void on_string(std::u32string_view s);
        void on_charset(std::u32string_view s);
        void on_token();
        void on_failure();
        void on_name(std::string_view name);
        void on_sequence();
        void on_lookahead();
        void on_lookahead_not();
        void on_not();
        void on_optional();
        void on_kleene_star();
        void on_kleene_plus();
    };
}

