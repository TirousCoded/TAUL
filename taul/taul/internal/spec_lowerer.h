

#pragma once


#include "../spec.h"

#include "llspec.h"


namespace taul::internal {


    class spec_lowerer final {
    public:

        spec_lowerer(source_pos& pos);


        // ptr to the pos field of upstream component

        source_pos* pos_ptr = nullptr;

        inline source_pos pos() const noexcept { return deref_assert(pos_ptr); }


        // stop writing if cancelled so our lowerer code can presume it'll
        // only execute if not cancelled

        bool cancelled = false;

        void cancel();


        llspec_writer output; // main output target of lowerer
        llspec_writer temp; // output target for impl of precedence PPR left-recursion rewriting


        // when rewriting, we bind an output target to write to

        llspec_writer* target_ptr = nullptr;

        llspec_writer& target();
        void bind_target(llspec_writer& new_target);
        void reset_target();


        // this counter records how deeply nested the eval currently is within composite exprs

        size_t depth = 0;

        void reset_depth();
        void begin_composite_expr();
        void end_composite_expr();

        bool at_rule_depth(); // if depth == 1, then we're inside LPR/PPR, but NOT inside anything nested therein


        // this counts the number of subexprs are nested *immediately* within an LPR/PPR (ie. it
        // does not count things further nested within composite exprs therein)

        size_t rule_subexprs = 0;

        void reset_rule_subexprs();
        void acknowledge_subexpr();


        // this records the name of the current LPR/PPR, in order to resolve self-refs

        std::string_view rule_name = {};

        void bind_rule_name(std::string_view name);
        bool is_self_ref(std::string_view name);


        // flag indicating if precedence PPR left-recursion rewriting is occurring

        bool in_preced_ppr_flag = false;

        void reset_in_preced_ppr_flag();


        // these vectors carry the llspec portions for the base/recurse alts, w/ the ladder
        // excluding prefix-ref name exprs

        std::vector<llspec> base_alts, recurse_alts;
        
        bool in_base_alt = false; // flag indicating if we're processing a base (and if not, recurse) alt

        void consider_next_alt_a_base_alt_until_discerned_otherwise();
        bool in_recurse_alt();

        void handle_begin_preced_ppr(std::string_view name, qualifier qualifier);
        void handle_end_preced_ppr();
        void handle_preced_ppr_alt();

        void push_temp_to_base_or_recurse_alts_vector();


        // flag indicating when we have been told that the next expr is the prefix-ref name expr

        bool next_is_prefix_ref_flag = false;

        void reset_next_is_prefix_ref_flag();


        // this counter is used to resolve precedence values/maximums when lowering precedence PPRs

        preced_t current_preced = no_preced_val;

        void reset_current_preced();
        void incr_current_preced();


        void on_startup();
        void on_shutdown();

        static_assert(spec_opcodes == 20);

        void on_close();
        void on_alternative();
        void on_lpr_decl(std::string_view name);
        void on_ppr_decl(std::string_view name);
        void on_lpr(std::string_view name, qualifier qualifier);
        void on_ppr(std::string_view name, qualifier qualifier);

        void on_end();
        void on_any();
        void on_string(std::string_view s);
        void on_charset(std::string_view s);
        void on_token();
        void on_failure();
        void on_name(std::string_view name, std::optional<spec_opcode> peek);
        void on_sequence();
        void on_lookahead();
        void on_lookahead_not();
        void on_not();
        void on_optional();
        void on_kleene_star();
        void on_kleene_plus();

        // detect_prefix_ref is used by loader to tell us when the next expr is
        // the prefix-ref node expr of a recurse alt

        void detect_prefix_ref();
    };
}

