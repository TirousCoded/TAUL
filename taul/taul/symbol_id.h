

#pragma once


#include <cstdint>
#include <iostream>
#include <string>
#include <format>

#include "unicode.h"


namespace taul {


    // symbol_id encapsulates codepoint/LPR/PPR symbol IDs

    // we have all three different kinds of symbols occupy one shared ID space,
    // so that we can have them liberally intermingle in usage

    // this approach comes at the cost of safety, as there's nothing stopping 
    // end_lpr_id from being used in the context of a glyph, rather than a token, 
    // w/ programmer having to manually put checks everywhere, which I definitely 
    // don't 100% approve of, but I've decided upon this nevertheless

    using symbol_id_num = std::uint32_t; // type for count of IDs

    // made this an enum type so we can't accidently implicit convert something
    // like an LPR index into a symbol_id, which wouldn't give the right ID value

    enum class symbol_id : symbol_id_num {};


    constexpr symbol_id operator+(symbol_id lhs, symbol_id_num rhs) noexcept { return symbol_id(symbol_id_num(lhs) + rhs); }
    constexpr symbol_id operator-(symbol_id lhs, symbol_id_num rhs) noexcept { return symbol_id(symbol_id_num(lhs) - rhs); }

    constexpr symbol_id operator+(symbol_id lhs, symbol_id rhs) noexcept { return lhs + symbol_id_num(rhs); }
    constexpr symbol_id operator-(symbol_id lhs, symbol_id rhs) noexcept { return lhs - symbol_id_num(rhs); }


    std::string fmt_symbol_id(symbol_id x);
}

template<>
struct std::formatter<taul::symbol_id> final : std::formatter<std::string> {
    auto format(const taul::symbol_id& x, format_context& ctx) const {
        return formatter<string>::format(taul::fmt_symbol_id(x), ctx);
    }
};

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::symbol_id& x) {
        return stream << taul::fmt_symbol_id(x);
    }
}

namespace taul {


    // we partition our ID value space into three sections for *normal* codepoint,
    // LPR, and PPR IDs, respectively

    // codepoint ID space section is large enough for all Unicode codepoints, and 
    // comes first, so all codepoint IDs are 1-to-1 w/ Unicode codepoints


    // we define the maximum number of normal IDs for a given section, and the number 
    // of special IDs for it, which added together is the total number of IDs for it

    // the below values are measures of ID counts, not IDs themselves

    constexpr symbol_id_num normal_cp_ids = 0x110000;
    constexpr symbol_id_num normal_lpr_ids = 0x10000;
    constexpr symbol_id_num normal_ppr_ids = 0x10000;

    constexpr symbol_id_num special_cp_ids = 1;
    constexpr symbol_id_num special_lpr_ids = 2;
    constexpr symbol_id_num special_ppr_ids = 0;


#define TAUL_NORMAL_IDS(section) taul::normal_ ## section ## _ids
#define TAUL_SPECIAL_IDS(section) taul::special_ ## section ## _ids

#define TAUL_TOTAL_IDS(section) (TAUL_NORMAL_IDS(section) + TAUL_SPECIAL_IDS(section))


    // notice that ranges [TAUL_FIRST_ID(cp), TAUL_LAST_ID(cp)], [TAUL_FIRST_ID(lpr), TAUL_LAST_ID(lpr)], 
    // and [TAUL_FIRST_ID(ppr), TAUL_LAST_ID(ppr)] are inclusive

    // normal (ie. non-special) symbols

    constexpr symbol_id first_cp_id = symbol_id(0);
    constexpr symbol_id first_lpr_id = symbol_id(TAUL_TOTAL_IDS(cp));
    constexpr symbol_id first_ppr_id = symbol_id(TAUL_TOTAL_IDS(cp) + TAUL_TOTAL_IDS(lpr));


    // notice how the '- 1' below is required to get last of inclusive range

#define TAUL_FIRST_ID(section) taul::first_ ## section ## _id
#define TAUL_LAST_ID(section) (TAUL_FIRST_ID(section) + TAUL_TOTAL_IDS(section) - 1)

#define TAUL_FIRST_NORMAL_ID(section) TAUL_FIRST_ID(section)
#define TAUL_LAST_NORMAL_ID(section) (TAUL_FIRST_NORMAL_ID(section) + TAUL_NORMAL_IDS(section) - 1)

#define TAUL_FIRST_SPECIAL_ID(section) (TAUL_FIRST_NORMAL_ID(section) + TAUL_NORMAL_IDS(section))
#define TAUL_LAST_SPECIAL_ID(section) (TAUL_FIRST_SPECIAL_ID(section) + TAUL_SPECIAL_IDS(section) - 1)

#define TAUL_NORMAL_ID(section, offset) (TAUL_FIRST_NORMAL_ID(section) + offset)
#define TAUL_SPECIAL_ID(section, offset) (TAUL_FIRST_SPECIAL_ID(section) + offset)


    // special symbols

    constexpr symbol_id end_cp_id = TAUL_SPECIAL_ID(cp, 0);

    constexpr symbol_id failure_lpr_id = TAUL_SPECIAL_ID(lpr, 0);
    constexpr symbol_id end_lpr_id = TAUL_SPECIAL_ID(lpr, 1);


    // TODO: add static_assert *unit tests* for below

    constexpr symbol_id cp_id(unicode_t cp) noexcept { return TAUL_NORMAL_ID(cp, taul::symbol_id_num(cp)); }
    constexpr symbol_id lpr_id(std::size_t lpr_index) noexcept { return TAUL_NORMAL_ID(lpr, taul::symbol_id_num(lpr_index)); }
    constexpr symbol_id ppr_id(std::size_t ppr_index) noexcept { return TAUL_NORMAL_ID(ppr, taul::symbol_id_num(ppr_index)); }


    // in_symbol_id_range returns if symbol ID x is within the *inclusive*
    // symbol ID value range [low, high]

    // this range is again, *inclusive*, not *exclusive*, meaning it contains
    // both low and high, and all symbol ID values in between

    // if low > high, the two will be swapped internally

    // in_symbol_id_range does not care whether x, low, or high, are valid
    // symbol ID values

    // TODO: add static_assert *unit tests* for below

    constexpr bool in_symbol_id_range(symbol_id x, symbol_id low, symbol_id high) noexcept {
        const auto _low = low <= high ? low : high;
        const auto _high = low <= high ? high : low;
        return x >= _low && x <= _high;
    }


    // TODO: add static_assert *unit tests* for below

    constexpr bool is_normal_cp_id(symbol_id id) noexcept { return in_symbol_id_range(id, TAUL_FIRST_NORMAL_ID(cp), TAUL_LAST_NORMAL_ID(cp)); }
    constexpr bool is_normal_lpr_id(symbol_id id) noexcept { return in_symbol_id_range(id, TAUL_FIRST_NORMAL_ID(lpr), TAUL_LAST_NORMAL_ID(lpr)); }
    constexpr bool is_normal_ppr_id(symbol_id id) noexcept { return in_symbol_id_range(id, TAUL_FIRST_NORMAL_ID(ppr), TAUL_LAST_NORMAL_ID(ppr)); }
    
    constexpr bool is_special_cp_id(symbol_id id) noexcept { return in_symbol_id_range(id, TAUL_FIRST_SPECIAL_ID(cp), TAUL_LAST_SPECIAL_ID(cp)); }
    constexpr bool is_special_lpr_id(symbol_id id) noexcept { return in_symbol_id_range(id, TAUL_FIRST_SPECIAL_ID(lpr), TAUL_LAST_SPECIAL_ID(lpr)); }
    constexpr bool is_special_ppr_id(symbol_id id) noexcept { return in_symbol_id_range(id, TAUL_FIRST_SPECIAL_ID(ppr), TAUL_LAST_SPECIAL_ID(ppr)); }
    
    constexpr bool is_cp_id(symbol_id id) noexcept { return in_symbol_id_range(id, TAUL_FIRST_ID(cp), TAUL_LAST_ID(cp)); }
    constexpr bool is_lpr_id(symbol_id id) noexcept { return in_symbol_id_range(id, TAUL_FIRST_ID(lpr), TAUL_LAST_ID(lpr)); }
    constexpr bool is_ppr_id(symbol_id id) noexcept { return in_symbol_id_range(id, TAUL_FIRST_ID(ppr), TAUL_LAST_ID(ppr)); }

    constexpr bool is_normal_id(symbol_id id) noexcept { return is_normal_cp_id(id) || is_normal_lpr_id(id) || is_normal_ppr_id(id); }
    constexpr bool is_end_id(symbol_id id) noexcept { return id == end_cp_id || id == end_lpr_id; }
    constexpr bool is_failure_id(symbol_id id) noexcept { return id == failure_lpr_id; }

    constexpr bool is_symbol_id(symbol_id id) noexcept { return is_cp_id(id) || is_lpr_id(id) || is_ppr_id(id); }
}

