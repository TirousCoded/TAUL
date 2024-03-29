

#pragma once


#include <unordered_set>
#include <optional>
#include <format>

#include "logger.h"
#include "spec_error.h"
#include "spec_opcode.h"
#include "spec.h"
#include "grammar.h"

#include "internal/lexer_pat.h"
#include "internal/parser_pat.h"


namespace taul {


    // our unit tests will only test the spec_error_counter overloads, and presume the ones 
    // w/out spec_error_counter simply *forward* to the ones that do

    std::optional<grammar> load(
        const spec& s, 
        const std::shared_ptr<logger>& lgr = nullptr);
    
    std::optional<grammar> load(
        const spec& s, 
        spec_error_counter& ec, 
        const std::shared_ptr<logger>& lgr = nullptr);


    // these wrap usage of taul::compile

    // these have not been unit tested

    std::optional<grammar> load(
        node_ctx& ctx,
        const std::shared_ptr<source_code>& src,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        node_ctx& ctx,
        const std::shared_ptr<source_code>& src,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        const std::shared_ptr<source_code>& src,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        const std::shared_ptr<source_code>& src,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        node_ctx& ctx,
        const std::string& src,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        node_ctx& ctx,
        const std::string& src,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        const std::string& src,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        const std::string& src,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        node_ctx& ctx,
        const std::filesystem::path& src_path,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        node_ctx& ctx,
        const std::filesystem::path& src_path,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        const std::filesystem::path& src_path,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        const std::filesystem::path& src_path,
        const std::shared_ptr<logger>& lgr = nullptr);


    namespace internal {


        class load_spec_interpreter final : public spec_interpreter {
        public:

            spec_error_counter* ec = nullptr;
            std::shared_ptr<logger> lgr = nullptr;

            bool success = false;

            // using these to keep track of next lpr/ppr index value

            std::size_t lpr_count = 0, ppr_count = 0;

            // the std::string_view keys below are to use the 'name' parameters
            // passed into the below virtual methods (don't use the name field
            // in the mapped value, as I worry about issues w/ SSO otherwise)

            // due to the string views pointing to memory in the spec's binary,
            // I think we'll be safe using std::string_view as the key, so long
            // as this invariant holds

            std::unordered_map<std::string_view, lexer_rule> lprs;
            std::unordered_map<std::string_view, parser_rule> pprs;

            inline bool has_lpr_decl(std::string_view name) const noexcept { return lprs.contains(name); }
            inline bool has_ppr_decl(std::string_view name) const noexcept { return pprs.contains(name); }
            inline bool has_lpr_or_ppr_decl(std::string_view name) const noexcept { return has_lpr_decl(name) || has_ppr_decl(name); }

            inline void add_lpr_decl(std::string_view name) {
                if (has_lpr_or_ppr_decl(name)) return; // name cannot already be used for another lpr/ppr
                lprs[name] = lexer_rule{ std::string(name), lpr_count };
                lpr_count++;
            }

            inline void add_ppr_decl(std::string_view name) {
                if (has_lpr_or_ppr_decl(name)) return; // name cannot already be used for another lpr/ppr
                pprs[name] = parser_rule{ std::string(name), ppr_count };
                ppr_count++;
            }

            // this is for keeping track of what decls have been defined

            // we don't need to differentiate between lprs/pprs here, as we assume
            // that there will not be any potential for overlap between the two due
            // to the code above

            std::unordered_set<std::string_view> defined;

            inline bool has_lpr_def(std::string_view name) const noexcept { return has_lpr_decl(name) && defined.contains(name); }
            inline bool has_ppr_def(std::string_view name) const noexcept { return has_ppr_decl(name) && defined.contains(name); }
            inline bool has_lpr_decl_nodef(std::string_view name) const noexcept { return has_lpr_decl(name) && !defined.contains(name); }
            inline bool has_ppr_decl_nodef(std::string_view name) const noexcept { return has_ppr_decl(name) && !defined.contains(name); }

            inline void add_lpr_def(std::string_view name, qualifier q) {
                if (!has_lpr_decl_nodef(name)) return;
                lprs.at(name).qualifer = q;
                defined.emplace(name);
            }
            
            inline void add_ppr_def(std::string_view name) {
                if (!has_ppr_decl_nodef(name)) return;
                defined.emplace(name);
            }

            // TODO: we REALLY should formalize the below later on...

            // for composite exprs, we define two stacks: the 'ESS' and 'ETS'

            // these stack for 'Expr. Scope Stack', 'Expr. Type Stack', and 
            // 'Local-End Stack', respectively

            // the ESS is the main one, and carries general info about the composite expr

            // the ETS is used to define if the evaluation is in the context of a lexer
            // or parser expr, or not such context at all

            // the ETS is pushed to, and thus popped from, for exprs which imbue this
            // association as part of their semantics

            // the ETS is used so that the top-most expr has access to contextual
            // info which the top-most expr may not provide, and that thus may be a result
            // of a lower expr which'll need a mechanism to expose that info, and so the
            // ETS is provided

            enum class ets_type : std::uint8_t {
                none,       // do not push/pop ETS
                lexer,      // push/pop lpr association to/from ETS
                parser,     // push/pop ppr association to/from ETS
            };

            struct ess_entry final {
                spec_opcode         opcode;
                ets_type            type            = ets_type::none;
                std::string_view    name            = "";               // name of lpr/ppr, if any
                bool                has_lpr_pat     = false;            // pops from lexer_pats on pop (push must be done manually)
                bool                has_ppr_pat     = false;            // pops from parser_pats on pop (push must be done manually)
                bool                in_sts          = false;            // pops single-terminal scope on pop (push must be done manually)

                // the below are *not* to be initialized explicitly
                
                std::size_t         index           = std::size_t(-1);  // index of lpr/ppr, if any
            };

            std::vector<ess_entry> ess;
            std::vector<ets_type> ets;

            inline void push_expr(ess_entry entry) {
                ess.push_back(entry);
                if (entry.type != ets_type::none) {
                    ets.push_back(entry.type);
                }
            }

            inline void pop_expr() noexcept {
                if (!ess.empty()) {
                    if (ess.back().type != ets_type::none) {
                        ets.pop_back();
                    }
                    ess.pop_back();
                }
            }

            inline bool in_composite_expr() const noexcept { return !ess.empty(); }
            inline bool in_lpr_or_ppr() const noexcept { return !ets.empty(); }
            inline bool in_lpr() const noexcept { return in_lpr_or_ppr() && ets.back() == ets_type::lexer; }
            inline bool in_ppr() const noexcept { return in_lpr_or_ppr() && ets.back() == ets_type::parser; }


            // these are used to handle enter/exiting 'single terminal' scope

            std::size_t single_terminal_scope_count = 0;

            inline bool in_single_terminal_scope() const noexcept {
                return single_terminal_scope_count > 0;
            }

            inline void push_single_terminal_scope() noexcept {
                TAUL_ASSERT(single_terminal_scope_count < std::numeric_limits<std::size_t>::max());
                single_terminal_scope_count++;
            }
            inline void pop_single_terminal_scope() noexcept {
                if (in_single_terminal_scope()) {
                    single_terminal_scope_count--;
                }
            }

            inline void handle_single_terminal_scope_for_top_ess() {
                if (!in_lpr_or_ppr()) return;
                if (!ess.back().in_sts) return;
                pop_single_terminal_scope();
            }


            // these stacks are for handling the lexer/parser 'pattern' expr trees, which get
            // built via them

            std::vector<std::shared_ptr<lexer_pat>> lexer_pats;
            std::vector<std::shared_ptr<parser_pat>> parser_pats;

            // this is for simple exprs (eg. 'char') to *bind* their lexer pattern object
            // to the top one of the lexer_pats stack, instead of pushing to the stack

            template<typename LexerPat, typename... Args>
            inline std::shared_ptr<lexer_pat> bind_lexer_pat(Args&&... args) {
                auto _new = std::make_shared<LexerPat>(std::forward<Args>(args)...);
                if (!lexer_pats.empty()) {
                    lexer_pats.back()->children.push_back(_new);
                }
                return _new;
            }

            // this is the parser equivalent of the above
            
            template<typename ParserPat, typename... Args>
            inline std::shared_ptr<parser_pat> bind_parser_pat(Args&&... args) {
                auto _new = std::make_shared<ParserPat>(std::forward<Args>(args)...);
                if (!parser_pats.empty()) {
                    parser_pats.back()->children.push_back(_new);
                }
                return _new;
            }

            // this is for composite exprs to push their lexer pattern object to the top of lexer_pats

            template<typename LexerPat, typename... Args>
            inline std::shared_ptr<lexer_pat> push_lexer_pat(Args&&... args) {
                auto _new = bind_lexer_pat<LexerPat>(std::forward<Args>(args)...);
                lexer_pats.push_back(_new);
                return _new;
            }

            // this is the parser equivalent of the above

            template<typename ParserPat, typename... Args>
            inline std::shared_ptr<parser_pat> push_parser_pat(Args&&... args) {
                auto _new = bind_parser_pat<ParserPat>(std::forward<Args>(args)...);
                parser_pats.push_back(_new);
                return _new;
            }

            // this is the pop counterpart to push_lexer_pat, and is called automatically in on_close

            inline std::shared_ptr<lexer_pat> pop_lexer_pat() {
                std::shared_ptr<lexer_pat> result = nullptr;
                if (!lexer_pats.empty()) {
                    result = lexer_pats.back();
                    lexer_pats.pop_back();
                }
                return result;
            }
            
            // this is the parser equivalent of the above

            inline std::shared_ptr<parser_pat> pop_parser_pat() {
                std::shared_ptr<parser_pat> result = nullptr;
                if (!parser_pats.empty()) {
                    result = parser_pats.back();
                    parser_pats.pop_back();
                }
                return result;
            }

            inline void handle_pop_lexer_pat_for_top_ess() {
                if (!in_lpr()) return;
                if (!ess.back().has_lpr_pat) return;
                auto popped = pop_lexer_pat();
                if (ess.back().opcode == taul::spec_opcode::lpr) {
                    bind_lexer_pat_to_lpr(ess.back().name, popped);
                }
            }

            inline void handle_pop_parser_pat_for_top_ess() {
                if (!in_ppr()) return;
                if (!ess.back().has_ppr_pat) return;
                auto popped = pop_parser_pat();
                if (ess.back().opcode == taul::spec_opcode::ppr) {
                    bind_parser_pat_to_ppr(ess.back().name, popped);
                }
            }


            // these maps lpr/ppr names to their lexer/parser 'pattern' expr trees, for us to
            // then use to build final lexers/parsers during taul::load

            std::unordered_map<std::string_view, std::shared_ptr<lexer_pat>> lexer_pat_map;
            std::unordered_map<std::string_view, std::shared_ptr<parser_pat>> parser_pat_map;

            inline void bind_lexer_pat_to_lpr(std::string_view lpr, std::shared_ptr<lexer_pat> pat) {
                TAUL_ASSERT(!lexer_pat_map.contains(lpr));
                lexer_pat_map[lpr] = std::move(pat);
            }
            
            inline void bind_parser_pat_to_ppr(std::string_view ppr, std::shared_ptr<parser_pat> pat) {
                TAUL_ASSERT(!parser_pat_map.contains(ppr));
                parser_pat_map[ppr] = std::move(pat);
            }


            template<typename... Args>
            inline void raise(spec_error err, std::format_string<Args...> fmt, Args&&... args) {
                raise_spec_error(ec, err, lgr, fmt, std::forward<Args&&>(args)...);
                // mark load as a failure if error arises
                success = false;
            }


            void check_not_in_lpr_scope(spec_opcode opcode);
            void check_not_in_ppr_scope(spec_opcode opcode);
            void check_not_in_lpr_nor_ppr_scope(spec_opcode opcode);
            void check_in_lpr_or_ppr_scope(spec_opcode opcode);
            void check_scope_exists_to_close();
            void check_rule_name_available(std::string_view name);
            void check_lpr_was_declared(std::string_view name);
            void check_ppr_was_declared(std::string_view name);
            void check_lpr_not_already_defined(std::string_view name);
            void check_ppr_not_already_defined(std::string_view name);
            void check_rule_is_not_ppr(std::string_view name);

            void check_lpr_or_ppr_exists_with_name(std::string_view name);

            void check_ppr_has_no_qualifier(std::string_view name, qualifier qualifier);

            void check_all_scopes_closed();
            void check_all_lprs_are_defined();
            void check_all_pprs_are_defined();
            void check_all_lprs_and_pprs_are_defined();

            void check_subexpr_count_legal(spec_opcode opcode, std::size_t n);

            void check_not_in_single_terminal_scope(spec_opcode opcode);
            void check_string_in_single_terminal_scope_in_lpr_has_only_one_char(std::string_view s);


        protected:

            void on_startup() override final;
            void on_shutdown() override final;

            static_assert(spec_opcodes == 20);

            void on_close() override final;
            void on_lpr_decl(std::string_view name) override final;
            void on_ppr_decl(std::string_view name) override final;
            void on_lpr(std::string_view name, qualifier qualifier) override final;
            void on_ppr(std::string_view name, qualifier qualifier) override final;

            void on_end() override final;
            void on_any() override final;
            void on_string(std::string_view s) override final;
            void on_charset(std::string_view s) override final;
            void on_token() override final;
            void on_failure() override final;
            void on_name(std::string_view name) override final;
            void on_sequence() override final;
            void on_set() override final;
            void on_lookahead();
            void on_lookahead_not();
            void on_not();
            void on_optional();
            void on_kleene_star();
            void on_kleene_plus();
        };
    }
}

