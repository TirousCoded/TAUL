

#pragma once


#include <optional>

#include "../grammar.h"

#include "grammar_data.h"
#include "rule_pt_translator.h"


namespace taul::internal {


    class grammar_builder final {
    public:

        grammar_data data; // the data we're building

        rule_pt_translator* rule_pt_trans = nullptr;

        // this indicates if the loader has detected that the spec is invalid, and
        // that grammar builders, and LPR/PPR translators, should stop

        // call 'cancel' to cancel, not assigning cancelled, as the former also
        // calls cancel on lpr_pt_trans and ppr_pt_trans

        bool cancelled = false;


        grammar_builder(rule_pt_translator& rule_pt_trans);


        void cancel() noexcept;


        // call these in the spec loader to add each LPR/PPR to the grammar

        // TODO: maybe write some unit tests checking that impl can handle
        //       cases where decls and defs in a spec are in different orders
        //
        //       such unit tests would need to test grammar *behaviour*

        // we need to seperate LPR/PPR decls and defs as otherwise we'd get a
        // bug where the system will naively use the *indices* of the defs,
        // rather than their *names* to discern their corresponding decl,
        // which is a subtle bug I'd like to avoid

        void add_lpr_decl(str name);
        void add_ppr_decl(str name);

        void add_lpr(str name, qualifier q);
        void add_ppr(str name, qualifier q);


        // these are used to get diagnostic info needed for checking for 
        // internal errors, and collisions, in LPR/PPR translation

        const parse_table_build_details<glyph>& get_lpr_diagnostics() const noexcept;
        const parse_table_build_details<token>& get_ppr_diagnostics() const noexcept;


        // call 'get_result' after finishing loading to perform final build

        std::optional<taul::grammar> get_result();


    private:

        std::unordered_map<str, size_t> lpr_index_map, ppr_index_map;
    };
}

