

#pragma once


#include <unordered_set>
#include <optional>
#include <format>

#include "logger.h"
#include "bias.h"
#include "spec_error.h"
#include "spec_opcode.h"
#include "spec.h"
#include "grammar.h"


namespace taul {


    // our unit tests will only test the spec_error_counter overloads, and presume the ones 
    // w/out spec_error_counter simply *forward* to the ones that do

    std::optional<grammar> load(const spec& s, const logger_ref& lgr = nullptr);
    std::optional<grammar> load(const spec& s, spec_error_counter& ec, const logger_ref& lgr = nullptr);


    namespace internal {


        class load_spec_interpreter final : public spec_interpreter {
        public:

            spec_error_counter* ec = nullptr;
            logger_ref lgr = nullptr;

            bool success = false;

            // last assigned bias

            bias last_bias = {};

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

            inline void add_lpr_def(std::string_view name) {
                if (!has_lpr_decl_nodef(name)) return;
                defined.emplace(name);
            }
            
            inline void add_ppr_def(std::string_view name) {
                if (!has_ppr_decl_nodef(name)) return;
                defined.emplace(name);
            }

            // TODO: we REALLY should formalize the below later on...

            // for composite exprs, we define three stacks: the 'ESS', 'ETS', and 'LES'

            // these stack for 'Expr. Scope Stack', 'Expr. Type Stack', and 
            // 'Local-End Stack', respectively

            // the ESS is the main one, and carries general info about the composite expr

            // the ETS is used to define if the evaluation is in the context of a lexer
            // or parser expr, or not such context at all

            // the ETS is pushed to, and thus popped from, for exprs which imbue this
            // association as part of their semantics

            // TODO: we haven't added LES yet, so be sure to do that when we need it!

            // the LES is used to push local-end info

            // the ETS and LES are used so that the top-most expr has access to contextual
            // info which the top-most expr may not provide, and that thus may be a result
            // of a lower expr which'll need a mechanism to expose that info, and so the
            // ETS and LES are provided

            enum class ets_type : std::uint8_t {
                none,       // do not push/pop ETS
                lexer,      // push/pop lpr association to/from ETS
                parser,     // push/pop ppr association to/from ETS
            };

            struct ess_entry final {
                spec_opcode         opcode;
                ets_type            type;
                std::string_view    name;       // name of lpr/ppr, if any
                std::size_t         index;      // index of lpr/ppr, if any
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


            template<typename... Args>
            inline void raise(spec_error err, std::format_string<Args...> fmt, Args&&... args) {
                if (ec) {
                    ec->raise(err);
                }
                TAUL_LOG(lgr, "TAUL error: ({}) {}", err, std::format(fmt, std::forward<Args&&>(args)...));
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

            void check_all_scopes_closed();
            void check_all_lprs_are_defined();
            void check_all_pprs_are_defined();
            void check_all_lprs_and_pprs_are_defined();


        protected:

            void on_startup() override final;
            void on_shutdown() override final;

            static_assert(spec_opcodes == 7);

            void on_grammar_bias(bias b) override final;
            void on_close() override final;
            void on_lpr_decl(std::string_view name) override final;
            void on_ppr_decl(std::string_view name) override final;
            void on_lpr(std::string_view name) override final;
            void on_ppr(std::string_view name) override final;
            void on_char() override final;
        };
    }
}

