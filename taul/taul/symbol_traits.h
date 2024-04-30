

#pragma once


#include <string>
#include <format>

#include "symbol_id.h"


#include "internal/util.h"


namespace taul {


    template<typename Symbol>
    struct symbol_traits final {
        // expects:
        //      using preferred_type = ...;
        //          * preferred integer-like type to specify IDs instead of symbol_id
        //          * these values DO NOT need to equal their corresponding IDs
        //          * for example, preferred_type being LPR indices, where LPR indices != LPR IDs
        //      static constexpr symbol_id first_id = ...;
        //      static constexpr symbol_id last_id = ...;
        //          * first_id and last_id define the range of valid IDs [first_id, last_id]
        //          * note that [first_id, last_id] is an inclusive (ie. not exclusive!) range
        //      static constexpr symbol_id end_of_input_id = ...;
        //          * end_of_input_id defines the end-of-input sentinel ID
        //      static constexpr symbol_id first_nonterminal_id = ...;
        //      static constexpr symbol_id last_nonterminal_id = ...;
        //          * these are the equiv of first_id/last_id, but for the non-terminals
        //      static bool legal_id(symbol_id x) noexcept
        //          * returns if x is in the range of IDs legal for use w/ symbol type
        //          * for glyphs, only codepoint IDs are allowed
        //          * for tokens, only LPR IDs are allowed
        //      static bool legal_nonterminal_id(symbol_id x) noexcept
        //          * these are the equiv of legal_id, but for the non-terminals
        //      static symbol_id id(const preferred_type& x) noexcept
        //      static symbol_id id(const symbol& x) noexcept
        //          * extracts the symbol ID value of x
        //      static std::optional<preferred_type> preferred(symbol_id x) noexcept
        //          * attempts to convert symbol ID x to its corresponding preferred_type value
    };
}

