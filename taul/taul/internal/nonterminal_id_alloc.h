

#pragma once


#include "../str.h"
#include "../source_code.h"
#include "../symbol_id.h"
#include "../symbol_traits.h"


namespace taul::internal {


    // this is used to allocate non-terminal IDs for use specifying the
    // main non-terminals, and the helper non-terminals

    // this produces outputs used to help the grammar differentiate said
    // non-terminal types, and debug outputs for diagnostics, specifying
    // source position info and string name debug symbols


    // non-debug output struct

    template<typename Symbol>
    struct nonterminal_id_allocs final {
        symbol_id first_helper_id; // specifies boundary between main and helpers


        inline bool is_main(symbol_id x) const noexcept;
        inline bool is_helper(symbol_id x) const noexcept;

        inline bool is_transparent(symbol_id x) const noexcept { return is_helper(x); }
    };

    template<typename Symbol>
    inline bool taul::internal::nonterminal_id_allocs<Symbol>::is_main(symbol_id x) const noexcept {
        return 
            symbol_traits<Symbol>::legal_nonterminal_id(x) && 
            x < first_helper_id;
    }

    template<typename Symbol>
    inline bool taul::internal::nonterminal_id_allocs<Symbol>::is_helper(symbol_id x) const noexcept {
        return 
            symbol_traits<Symbol>::legal_nonterminal_id(x) && 
            x >= first_helper_id;
    }


    // debug output struct

    template<typename Symbol>
    struct nonterminal_id_allocs_debug final {
        struct dbgsym final {
            source_pos pos = 0;
            str name;
        };


        std::unordered_map<symbol_id, dbgsym> dbgsyms;


        inline std::optional<dbgsym> query(symbol_id x) const;
    };

    template<typename Symbol>
    inline std::optional<typename taul::internal::nonterminal_id_allocs_debug<Symbol>::dbgsym> taul::internal::nonterminal_id_allocs_debug<Symbol>::query(symbol_id x) const {
        const auto it = dbgsyms.find(x);
        return
            it != dbgsyms.end()
            ? std::make_optional(it->second)
            : std::nullopt;
    }


    // the non-terminal ID allocator itself

    template<typename Symbol>
    struct nonterminal_id_alloc final {
        // allocator state

        bool defining_main = true; // if main non-terminals are being defined
        symbol_id next_id = symbol_traits<Symbol>::first_nonterminal_id;

        // output state

        nonterminal_id_allocs<Symbol> output = {}; // the final non-debug output
        nonterminal_id_allocs_debug<Symbol> output_debug = {}; // the final debug output


        inline bool is_defining_main() const noexcept { return defining_main; }
        inline bool is_defining_helpers() const noexcept { return !defining_main; }


        inline void done_defining_main() {
            TAUL_ASSERT(is_defining_main());
            defining_main = false;
            output.first_helper_id = next_id;
            TAUL_ASSERT(symbol_traits<Symbol>::legal_nonterminal_id(output.first_helper_id));
        }

        inline symbol_id define(source_pos pos, str name) {
            output_debug.dbgsyms[next_id] = { pos, name }; // define ID alloc
            next_id = next_id + 1; // incr ID counter
            return next_id - 1; // return prev value
        }
    };
}

