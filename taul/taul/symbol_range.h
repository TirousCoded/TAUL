

#pragma once


#include "symbol_id.h"
#include "symbol_traits.h"

#include "internal/util.h"


namespace taul {


    struct glyph;
    struct token;

    template<typename Symbol, typename Traits = symbol_traits<Symbol>>
    struct symbol_range;


    using glyph_range = symbol_range<glyph>;
    using token_range = symbol_range<token>;


    // IMPORTANT: symbol_range describes an *inclusive* range, not an *exclusive* one!

    template<typename Symbol, typename Traits>
    struct symbol_range final {

        using symbol_t = Symbol;
        using traits_t = Traits;

        using this_t = symbol_range<symbol_t, traits_t>;

        using preferred_type = typename traits_t::preferred_type;


        symbol_id low, high;


        inline std::size_t count() const noexcept { return std::size_t(high - low) + 1; }
        inline bool contains(symbol_id x) const noexcept { return x >= low && x <= high; }

        // TODO: overlaps has not been unit tested

        inline bool overlaps(const this_t& other) const noexcept {
            // IMPORTANT: internal::range_overlap accepts *exclusive* ranges, not *inclusive* ranges
            return internal::range_overlap(low, high + 1, other.low, other.high + 1);
        }


        inline bool equal(const this_t& other) const noexcept {
            return low == other.low && high == other.high;
        }

        inline bool operator==(const this_t& other) const noexcept { return equal(other); }
        inline bool operator!=(const this_t& other) const noexcept { return !equal(other); }


        inline std::string fmt() const {
            return
                count() == 1
                ? std::format("({})", low)
                : std::format("({}-{})", low, high);
        }


        inline static this_t create(symbol_id low, symbol_id high) noexcept {
            TAUL_ASSERT(traits_t::legal_id(low));
            TAUL_ASSERT(traits_t::legal_id(high));
            if (low > high) std::swap(low, high);
            return { low, high };
        }

        inline static this_t create(const preferred_type& low, const preferred_type& high) noexcept {
            return create(traits_t::id(low), traits_t::id(high));
        }

        inline static this_t create(const symbol_t& low, const symbol_t& high) noexcept {
            return create(traits_t::id(low), traits_t::id(high));
        }
    };
}

template<typename Symbol>
struct std::formatter<taul::symbol_range<Symbol>> final : std::formatter<std::string> {
    auto format(const taul::symbol_range<Symbol>& x, format_context& ctx) const {
        return formatter<string>::format(x.fmt(), ctx);
    }
};

namespace std {
    template<typename Symbol>
    inline std::ostream& operator<<(std::ostream& stream, const taul::symbol_range<Symbol>& x) {
        return stream << x.fmt();
    }
}

