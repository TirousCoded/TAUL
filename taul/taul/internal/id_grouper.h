

#pragma once


#include <vector>

#include "../asserts.h"
#include "../symbol_id.h"
#include "../symbol_traits.h"
#include "../symbol_range.h"
#include "../symbols.h"


#define _DUMP_ID_GROUPER_USE_CASE_ADDING_LOG 0


namespace taul::internal {


    // these 'ID groupers' exist to solve a specific problem: if we
    // impl our lexer w/ parse tables which have each entry map using
    // a single Unicode codepoint symbol ID as its terminal, we'd
    // encounter a scaling issue where HUGE ranges of Unicode codepoints
    // would correspond to needing HUGE amounts of parse table entries,
    // one for EACH INDIVIDUAL CODEPOINT

    // to solve this, we'll group together glyph symbol IDs into 'groups',
    // and use group IDs for our lexer parse table rather than IDs for
    // individual Unicode codepoint-based symbols

    // the ID grouper must define a set of ID groups such that, given
    // some set of IDs being used (ie. for FIRST/FOLLOW sets), the ID
    // grouper must be able to provide a set of ID groups which together
    // perfectly encapsulate all the symbol IDs of the use case, w/
    // none being excluded, and no additional symbol IDs being included


    using group_id = symbol_id_num;


    template<typename Symbol>
    class id_grouper final {
    public:
        // expects:
        //      void add_use_case(symbol_id low, symbol_id high)
        //          * adds inclusive (ie. not exclusive) ID range [low, high] as
        //            a use case the ID grouper must account for
        //          * invalidates all current group IDs
        //          * all use cases should be specified before proper usage of
        //            the group ID space may begin
        //      group_id get_group_id(symbol_id x) const noexcept
        //          * returns the group ID x is mapped to
        //          * all values of x must be mappable
        //      symbol_range<Symbol> get_symbol_range(group_id x) const noexcept
        //          * returns the symbol range which maps to x
        //          * not all values of x may be mappable
    };


    template<>
    class id_grouper<glyph> final {
    public:

        using traits_t = symbol_traits<glyph>;

        using symbol_range_t = symbol_range<glyph, traits_t>;


        std::vector<symbol_range_t> ranges;


        inline id_grouper() {
            ranges.push_back(symbol_range_t{ traits_t::first_id, traits_t::last_id });
        }


        inline void add_use_case(symbol_id low, symbol_id high) {
            TAUL_ASSERT(traits_t::legal_id(low));
            TAUL_ASSERT(traits_t::legal_id(high));
            TAUL_ASSERT(low <= high);
            // find the ranges inside which low and high reside
            for (std::size_t i = 0; i < ranges.size(); i++) {
                const auto r = ranges[i];
                const auto has_low = r.contains(low) && low != r.low;
                const auto has_high = r.contains(high) && high != r.high;
                const auto may_exit = r.contains(high);
                if (has_low && has_high) {
                    ranges.insert(ranges.begin() + i + 1, symbol_range_t{});
                    ranges.insert(ranges.begin() + i + 1, symbol_range_t{});
                    ranges[i + 0] = symbol_range_t{ r.low, low - 1 };
                    ranges[i + 1] = symbol_range_t{ low, high };
                    ranges[i + 2] = symbol_range_t{ high + 1, r.high };
                }
                else if (has_low && !has_high) {
                    ranges.insert(ranges.begin() + i + 1, symbol_range_t{});
                    ranges[i + 0] = symbol_range_t{ r.low, low - 1 };
                    ranges[i + 1] = symbol_range_t{ low, r.high };
                }
                else if (!has_low && has_high) {
                    ranges.insert(ranges.begin() + i + 1, symbol_range_t{});
                    ranges[i + 0] = symbol_range_t{ r.low, high };
                    ranges[i + 1] = symbol_range_t{ high + 1, r.high };
                }
                if (may_exit) break;
            }
        }

        inline group_id get_group_id(symbol_id x) const noexcept {
            TAUL_ASSERT(traits_t::legal_id(x));
            return _find(x, 0, ranges.size());
        }

        inline group_id operator()(symbol_id x) const noexcept { return get_group_id(x); }

        inline symbol_range_t get_symbol_range(group_id x) const noexcept {
            TAUL_ASSERT(x < ranges.size());
            return ranges[x];
        }


    private:

        inline void _split(symbol_id x, std::size_t index) {
            TAUL_ASSERT(traits_t::legal_id(x));
            TAUL_ASSERT(index < ranges.size());
            TAUL_ASSERT(ranges[index].contains(x));
            const auto v = ranges[index];
            ranges.insert(ranges.begin() + index + 1, symbol_range_t{});
            ranges[index] = symbol_range_t{ v.low, x - 1 };
            ranges[index + 1] = symbol_range_t{ x, v.high };
            TAUL_ASSERT(ranges[index].low <= ranges[index].high);
            TAUL_ASSERT(ranges[index + 1].low <= ranges[index + 1].high);
        }

        // the range [first, last) is exclusive

        inline group_id _find(symbol_id x, std::size_t first, std::size_t last) const noexcept {
            TAUL_ASSERT(traits_t::legal_id(x));
            TAUL_ASSERT((last - first) >= 1);
            const auto middle = first + (last - first) / 2;
            if (ranges[middle].contains(x)) return group_id(middle);
            if (x < ranges[middle].low) return _find(x, first, middle);
            if (x > ranges[middle].high) return _find(x, middle + 1, last);
            TAUL_DEADEND;
            return group_id{};
        }
    };


    template<>
    class id_grouper<token> final {
    public:

        using traits_t = symbol_traits<token>;

        using symbol_range_t = symbol_range<token, traits_t>;


        std::vector<symbol_range_t> ranges;


        inline id_grouper() {
            ranges.push_back(symbol_range_t{ traits_t::first_id, traits_t::last_id });
        }


        inline void add_use_case(symbol_id low, symbol_id high) {
#if _DUMP_ID_GROUPER_USE_CASE_ADDING_LOG
            TAUL_LOG(make_stderr_logger(), "    > low=={}, high=={}", low, high);
#endif
            TAUL_ASSERT(traits_t::legal_id(low));
            TAUL_ASSERT(traits_t::legal_id(high));
            TAUL_ASSERT(low <= high);
            // find the ranges inside which low and high reside
            for (std::size_t i = 0; i < ranges.size(); i++) {
                const auto r = ranges[i];
                const auto has_low = r.contains(low) && low != r.low;
                const auto has_high = r.contains(high) && high != r.high;
                const auto may_exit = r.contains(high);
                if (has_low && has_high) {
                    ranges.insert(ranges.begin() + i + 1, symbol_range_t{});
                    ranges.insert(ranges.begin() + i + 1, symbol_range_t{});
                    ranges[i + 0] = symbol_range_t{ r.low, low - 1 };
                    ranges[i + 1] = symbol_range_t{ low, high };
                    ranges[i + 2] = symbol_range_t{ high + 1, r.high };
                }
                else if (has_low && !has_high) {
                    ranges.insert(ranges.begin() + i + 1, symbol_range_t{});
                    ranges[i + 0] = symbol_range_t{ r.low, low - 1 };
                    ranges[i + 1] = symbol_range_t{ low, r.high };
                }
                else if (!has_low && has_high) {
                    ranges.insert(ranges.begin() + i + 1, symbol_range_t{});
                    ranges[i + 0] = symbol_range_t{ r.low, high };
                    ranges[i + 1] = symbol_range_t{ high + 1, r.high };
                }
                if (may_exit) break;
            }
        }

        inline group_id get_group_id(symbol_id x) const noexcept {
            TAUL_ASSERT(traits_t::legal_id(x));
            return _find(x, 0, ranges.size());
        }

        inline group_id operator()(symbol_id x) const noexcept { return get_group_id(x); }

        inline symbol_range_t get_symbol_range(group_id x) const noexcept {
            TAUL_ASSERT(x < ranges.size());
            return ranges[x];
        }


    private:

        inline void _split(symbol_id x, std::size_t index) {
            TAUL_ASSERT(traits_t::legal_id(x));
            TAUL_ASSERT(index < ranges.size());
            TAUL_ASSERT(ranges[index].contains(x));
            const auto v = ranges[index];
            ranges.insert(ranges.begin() + index + 1, symbol_range_t{});
            ranges[index] = symbol_range_t{ v.low, x - 1 };
            ranges[index + 1] = symbol_range_t{ x, v.high };
            TAUL_ASSERT(ranges[index].low <= ranges[index].high);
            TAUL_ASSERT(ranges[index + 1].low <= ranges[index + 1].high);
        }

        // the range [first, last) is exclusive

        inline group_id _find(symbol_id x, std::size_t first, std::size_t last) const noexcept {
            TAUL_ASSERT(traits_t::legal_id(x));
            TAUL_ASSERT((last - first) >= 1);
            const auto middle = first + (last - first) / 2;
            if (ranges[middle].contains(x)) return group_id(middle);
            if (x < ranges[middle].low) return _find(x, first, middle);
            if (x > ranges[middle].high) return _find(x, middle + 1, last);
            TAUL_DEADEND;
            return group_id{};
        }
    };


    // IMPORTANT: keep this if it's ever useful, but I've decided that it's better to
    //            use the above semantics for both, as I got HUGE parse tables generated
    //            for our PPRs w/ the below, which was suboptimal...

    // to keep our ID groupers reasonable w/ regards to coupling, we'll have a 
    // specialization for tokens which simply defines a set of groups each of
    // which corresponds to a particular symbol

    //template<>
    //class id_grouper<token> final {
    //public:
    //    using symbol_range_t = symbol_range<token>;
    //    inline void add_use_case(symbol_id, symbol_id) {}
    //    inline group_id get_group_id(symbol_id x) const noexcept { return group_id(x - TAUL_FIRST_ID(lpr)); }
    //    inline group_id operator()(symbol_id x) const noexcept { return get_group_id(x); }
    //    inline symbol_range_t get_symbol_range(group_id x) const noexcept {
    //        TAUL_ASSERT(x < TAUL_TOTAL_IDS(lpr));
    //        return symbol_range_t{ TAUL_FIRST_ID(lpr) + x, TAUL_FIRST_ID(lpr) + x };
    //    }
    //};
}

