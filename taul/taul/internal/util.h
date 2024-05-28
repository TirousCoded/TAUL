

#pragma once


#include <utility>
#include <optional>

#include "../logger.h"
#include "../source_code.h"
#include "../spec_error.h"


namespace taul::internal {


    struct for_internal_use_tag final {};


    template<typename... Args>
    inline void raise_error(
        std::shared_ptr<logger> lgr,
        spec_error_counter* ec,
        const source_code* src,
        source_pos pos,
        spec_error err,
        std::format_string<Args...> fmt,
        Args&&... args) {
        if (ec) ec->raise(err);
        if (src) {
            TAUL_LOG(lgr,
                "TAUL error: {} {} ({})",
                src->location_at(pos),
                std::format(fmt, std::forward<Args&&>(args)...),
                err);
        }
        else {
            TAUL_LOG(lgr,
                "TAUL error: {} ({})",
                std::format(fmt, std::forward<Args&&>(args)...),
                err);
        }
    }


    // returns 16 if char is not hex digit

    template<typename Char>
    inline std::size_t hex_digit(Char x) noexcept {
        switch (x) {
        case Char('0'): return 0;   break;
        case Char('1'): return 1;   break;
        case Char('2'): return 2;   break;
        case Char('3'): return 3;   break;
        case Char('4'): return 4;   break;
        case Char('5'): return 5;   break;
        case Char('6'): return 6;   break;
        case Char('7'): return 7;   break;
        case Char('8'): return 8;   break;
        case Char('9'): return 9;   break;
        case Char('a'): return 10;  break;
        case Char('b'): return 11;  break;
        case Char('c'): return 12;  break;
        case Char('d'): return 13;  break;
        case Char('e'): return 14;  break;
        case Char('f'): return 15;  break;
        case Char('A'): return 10;  break;
        case Char('B'): return 11;  break;
        case Char('C'): return 12;  break;
        case Char('D'): return 13;  break;
        case Char('E'): return 14;  break;
        case Char('F'): return 15;  break;
        default:        return 16;  break;
        }
    }


    // below are exclusive ranges [a_first, a_last) and [b_first, b_last)

    // range_overlap returns false if the two ranges are 0 units apart, but not overlapping

    template<typename T>
    inline bool range_overlap(T a_first, T a_last, T b_first, T b_last) noexcept {
        TAUL_ASSERT(a_first <= a_last);
        TAUL_ASSERT(b_first <= b_last);
        //return !(a_first >= b_last || a_last <= b_first);
        return a_first < b_last && b_first < a_last;
    }

    // range_overlap_details is like range_overlap, except that it returns an exclusive
    // range of the exact overlap between [a_first, a_last) and [b_first, b_last)

    // range_overlap_details returns std::nullopt if there is no overlap

    template<typename T>
    inline std::optional<std::pair<T, T>> range_overlap_details(T a_first, T a_last, T b_first, T b_last) noexcept {
        TAUL_ASSERT(a_first <= a_last);
        TAUL_ASSERT(b_first <= b_last);
        if (range_overlap(a_first, a_last, b_first, b_last)) {
            std::pair<T, T> result{};
            result.first = std::max(a_first, b_first);
            result.second = std::min(a_last, b_last);
            return std::make_optional(std::move(result));
        }
        else return std::nullopt;
    }
}

