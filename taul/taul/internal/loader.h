

#pragma once


#include <vector>
#include <unordered_set>
#include <optional>
#include <format>

#include "../logger.h"
#include "../source_code.h"
#include "../spec.h"
#include "../spec_error.h"

#include "util.h"
#include "rule_pt_translator.h"
#include "grammar_builder.h"
#include "spec_lowerer.h"


namespace taul::internal {


    class loader final {
    public:

        // the loader performs the following:
        //
        //      spec -> llspec -> parse table

        // spec_stage is a spec interpreter which performs the
        // 'spec -> llspec' above, reporting most semantic errors

        class spec_stage;

        // llspec_stage is a llspec interpreter which performs the
        // 'llspec -> parse table' above, reporting semantic errors
        // which require parse table info to detect

        class llspec_stage;


        std::shared_ptr<source_code> src = nullptr;
        spec_error_counter* ec = nullptr;
        std::shared_ptr<logger> lgr = nullptr;

        spec_lowerer sl;
        rule_pt_translator rule_pt_trans;
        grammar_builder gb;


        // success specifies whether or not the loading process has failed yet

        // if loading fails, interpretation will continue, to catch all errors,
        // but spec lowering, and thus grammar generation, will be cancelled

        // our system is designed so that an invalid spec can still be interpreted
        // properly, meaning w/out invalid spec data *corrupting* later interpretation
        // state such that errors which arise from it are not meaningful

        bool success = false;

        // this records the current source code position of the loader,
        // in order to report helpful error info

        // pos is used both by spec_stage and llspec_stage

        source_pos pos = 0;


        loader(std::shared_ptr<source_code> src, spec_error_counter& ec, std::shared_ptr<logger> lgr);


        std::optional<grammar> load(const spec& s);


        template<typename... Args>
        inline void raise_at(source_pos pos, spec_error err, std::format_string<Args...> fmt, Args&&... args) {
            raise_error(lgr, ec, src.get(), pos, err, fmt, std::forward<Args&&>(args)...);
            if (success) {
                gb.cancel(); // <- needed by llspec_stage
                success = false;
            }
        }
        template<typename... Args>
        inline void raise(spec_error err, std::format_string<Args...> fmt, Args&&... args) {
            raise_at(pos, err, fmt, std::forward<Args&&>(args)...);
        }
    };

    class loader::spec_stage final : public spec_interpreter {
    public:

        loader* owner_ptr;

        inline loader& owner() const noexcept { return deref_assert(owner_ptr); }


        spec_stage(loader& owner);


        std::optional<llspec> result();


        // the 'expr scope stack' (ess) defines the current expr

        // this is for composite exprs only, w/ non-composite exprs never pushing to ess

        struct ess_frame final {
            spec_opcode         opcode;
            std::size_t         alts = 0;
            std::size_t         subexprs = 0; // subexprs of the *current alt*
            // if the ess frame is *marked*, it will auto-pop from dss
            bool                pop_dss = false;
            // for our special *non-propagating* scopes, we'll use flags instead of stacks
            bool                single_terminal_scope = false;
            bool                no_alternation_scope = false;
            bool                single_subexpr_scope = false;
            bool                no_end_subexpr_scope = false;
        };

        std::vector<ess_frame> ess;


        // the 'definition scope stack' (dss) defines what LPR/PPR definition 
        // the loading process is currently in the scope of

        // in proper specs, this should either be empty, or at any given time
        // have only a single entry, but this is a stack to ensure that invalid
        // specs can still be evaluated

        struct dss_frame final {
            std::string_view    name;
            bool                lpr;   // if the definition is for an LPR (and if not, a PPR)
        };

        std::vector<dss_frame> dss;


        inline void push_ess(ess_frame&& x) {
            ess.push_back(std::forward<ess_frame&&>(x));
        }
        inline void push_dss(dss_frame&& x) {
            dss.push_back(std::forward<dss_frame&&>(x));
            ess.back().pop_dss = true;
        }

        inline void mark_single_terminal_scope() {
            TAUL_ASSERT(!ess.empty());
            ess.back().single_terminal_scope = true;
            mark_single_subexpr_scope(); // implicit
        }
        inline void mark_no_alternation_scope() {
            TAUL_ASSERT(!ess.empty());
            ess.back().no_alternation_scope = true;
        }
        inline void mark_single_subexpr_scope() {
            TAUL_ASSERT(!ess.empty());
            ess.back().single_subexpr_scope = true;
        }
        inline void mark_no_end_subexpr_scope() {
            TAUL_ASSERT(!ess.empty());
            ess.back().no_end_subexpr_scope = true;
        }

        inline void pop_ess() noexcept {
            if (ess.empty()) return;
            if (ess.back().pop_dss) pop_dss();
            ess.pop_back();
        }
        inline void pop_dss() noexcept { if (!dss.empty()) dss.pop_back(); }

        inline void count_subexpr_for_top_ess() noexcept {
            if (!ess.empty()) ess.back().subexprs++;
        }

        inline void next_alternative_for_top_ess() noexcept {
            if (ess.empty()) return;
            ess.back().alts++;
            ess.back().subexprs = 0;
        }


        // these sets are used to keep track of declared LPRs/PPRs

        std::unordered_set<std::string_view> lprs;
        std::unordered_set<std::string_view> pprs;

        inline bool has_lpr_decl(std::string_view name) const noexcept { return lprs.contains(name); }
        inline bool has_ppr_decl(std::string_view name) const noexcept { return pprs.contains(name); }
        inline bool has_lpr_or_ppr_decl(std::string_view name) const noexcept { return has_lpr_decl(name) || has_ppr_decl(name); }

        inline void add_lpr_decl(std::string_view name) {
            if (has_lpr_or_ppr_decl(name)) return; // name cannot already be used for another LPR/PPR
            lprs.emplace(name);
        }

        inline void add_ppr_decl(std::string_view name) {
            if (has_lpr_or_ppr_decl(name)) return; // name cannot already be used for another LPR/PPR
            pprs.emplace(name);
        }

        // this is for keeping track of what decls have been defined

        // we don't need to differentiate between LPRs/PPRs here, as we assume
        // that there will not be any potential for overlap between the two due
        // to the code above

        std::unordered_set<std::string_view> defined;

        inline bool has_lpr_def(std::string_view name) const noexcept { return has_lpr_decl(name) && defined.contains(name); }
        inline bool has_ppr_def(std::string_view name) const noexcept { return has_ppr_decl(name) && defined.contains(name); }
        inline bool has_lpr_decl_nodef(std::string_view name) const noexcept { return has_lpr_decl(name) && !defined.contains(name); }
        inline bool has_ppr_decl_nodef(std::string_view name) const noexcept { return has_ppr_decl(name) && !defined.contains(name); }

        inline void add_lpr_def(std::string_view name) { if (has_lpr_decl_nodef(name)) defined.emplace(name); }
        inline void add_ppr_def(std::string_view name) { if (has_ppr_decl_nodef(name)) defined.emplace(name); }

        inline bool legal_to_decl() const noexcept { return defined.empty(); }


        inline bool in_composite_expr() const noexcept { return !ess.empty(); }
        inline bool in_lpr_or_ppr() const noexcept { return !dss.empty(); }
        inline bool in_lpr() const noexcept { return in_lpr_or_ppr() && dss.back().lpr; }
        inline bool in_ppr() const noexcept { return in_lpr_or_ppr() && !dss.back().lpr; }

        inline bool in_single_terminal_scope() const noexcept { return !ess.empty() && ess.back().single_terminal_scope; }
        inline bool in_no_alternation_scope() const noexcept { return !ess.empty() && ess.back().no_alternation_scope; }
        inline bool in_single_subexpr_scope() const noexcept { return !ess.empty() && ess.back().single_subexpr_scope; }
        inline bool in_no_end_subexpr_scope() const noexcept { return !ess.empty() && ess.back().no_end_subexpr_scope; }


        static_assert(spec_errors == 24);

        void check_err_scope_not_closed();
        void check_err_stray_close();
        void check_err_rule_name_conflict(std::string_view name);
        void check_err_rule_never_declared(std::string_view name, bool lpr);
        void check_err_rule_never_defined();
        void check_err_rule_already_defined(std::string_view name, bool lpr);
        void check_err_rule_not_found(std::string_view name);
        void check_err_rule_may_not_be_ppr(std::string_view name);
        void check_err_illegal_rule_declare(spec_opcode opcode);
        void check_err_illegal_in_lpr_scope(spec_opcode opcode);
        void check_err_illegal_in_ppr_scope(spec_opcode opcode);
        void check_err_illegal_in_no_scope(spec_opcode opcode);
        void check_err_illegal_qualifier(std::string_view name, taul::qualifier qualifier);
        void check_err_illegal_in_single_terminal_scope(spec_opcode opcode, std::string_view s = {});
        void check_err_illegal_in_single_terminal_scope(spec_opcode opcode, std::u32string_view s); // for string
        void check_err_illegal_in_no_alternation_scope(spec_opcode opcode);
        void check_err_illegal_in_single_subexpr_scope(spec_opcode opcode);
        void check_err_illegal_in_no_end_subexpr_scope(spec_opcode opcode);
        void check_err_illegal_string_literal_due_to_illegal_codepoints(std::u32string_view s);
        void check_err_illegal_string_literal_due_to_nonvisible_ascii(std::string_view unparsed_literal);
        void check_err_illegal_charset_literal_due_to_illegal_codepoints(std::u32string_view s);
        void check_err_illegal_charset_literal_due_to_nonvisible_ascii(std::string_view unparsed_literal);


    protected:

        void on_startup() override final;
        void on_shutdown() override final;

        static_assert(spec_opcodes == 21);

        void on_pos(source_pos new_pos) override final;
        void on_close() override final;
        void on_alternative() override final;
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
        void on_lookahead() override final;
        void on_lookahead_not() override final;
        void on_not() override final;
        void on_optional() override final;
        void on_kleene_star() override final;
        void on_kleene_plus() override final;
    };

    class loader::llspec_stage final : public llspec_interpreter {
    public:

        loader* owner_ptr;

        inline loader& owner() const noexcept { return deref_assert(owner_ptr); }


        llspec_stage(loader& owner);


        std::optional<grammar> result();


        static_assert(spec_errors == 24);

        void check_err_illegal_ambiguity();
        void check_err_illegal_ambiguity_due_to_trivial_left_recursion();

        void check_for_internal_errors();


    protected:

        void on_startup() override final;
        void on_shutdown() override final;

        static_assert(llspec_opcodes == 21);

        void on_pos(source_pos new_pos) override final;
        void on_close() override final;
        void on_alternative() override final;
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
        void on_lookahead() override final;
        void on_lookahead_not() override final;
        void on_not() override final;
        void on_optional() override final;
        void on_kleene_star() override final;
        void on_kleene_plus() override final;
    };
}

