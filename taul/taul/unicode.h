

#pragma once


#include <cstddef>

#include <string>


namespace taul {


    // unicode_t represents a single Unicode codepoint

    using unicode_t = char32_t;


    std::string fmt_unicode(unicode_t x, bool uppercase_hex = false);


    // in_codepoint_range returns if codepoint x is within the *inclusive*
    // Unicode codepoint range [low, high]

    // this range is again, *inclusive*, not *exclusive*, meaning it contains
    // both low and high, and all codepoints in between

    // if low > high, the two will be swapped internally

    // in_codepoint_range does not care whether x, low, or high, are valid
    // Unicode codepoints

    constexpr bool in_codepoint_range(unicode_t x, unicode_t low, unicode_t high) noexcept {
        const auto _low = low <= high ? low : high;
        const auto _high = low <= high ? high : low;
        return x >= _low && x <= _high;
    }

    static_assert(in_codepoint_range(0x0000, 0x0000, 0x0000) == true);
    static_assert(in_codepoint_range(0x0000, 0x0000, 0x1000) == true);
    static_assert(in_codepoint_range(0x1000, 0x0000, 0x1000) == true);
    static_assert(in_codepoint_range(0x0f00, 0x0000, 0x0fff) == true);
    static_assert(in_codepoint_range(0x0f00, 0x0fff, 0x0000) == true);

    static_assert(in_codepoint_range(0xf000, 0x0000, 0x0fff) == false);
    static_assert(in_codepoint_range(0x0f00, 0x0fff, 0x1000) == false);
    static_assert(in_codepoint_range(0xf000, 0x0fff, 0x0000) == false);
    static_assert(in_codepoint_range(0x0f00, 0x1000, 0x0fff) == false);


    // is_ascii returns if codepoint x is in Unicode's ASCII block

    constexpr bool is_ascii(unicode_t x) noexcept {
        return in_codepoint_range(x, 0x0000, 0x007f);
    }

    static_assert(is_ascii(unicode_t(0)) == true);
    static_assert(is_ascii(unicode_t(61)) == true);
    static_assert(is_ascii(unicode_t(127)) == true);
    static_assert(is_ascii(unicode_t(128)) == false);
    static_assert(is_ascii(unicode_t(-1)) == false);


    // is_visible_ascii returns if codepoint x is in Unicode's ASCII 
    // block, and is a *visible* ASCII character

    constexpr bool is_visible_ascii(unicode_t x) noexcept {
        return in_codepoint_range(x, 0x0020, 0x007e);
    }

    static_assert(is_visible_ascii(unicode_t(0)) == false);
    static_assert(is_visible_ascii(unicode_t(31)) == false);
    static_assert(is_visible_ascii(unicode_t(32)) == true);
    static_assert(is_visible_ascii(unicode_t(61)) == true);
    static_assert(is_visible_ascii(unicode_t(126)) == true);
    static_assert(is_visible_ascii(unicode_t(127)) == false);
    static_assert(is_visible_ascii(unicode_t(-1)) == false);


    // is_unicode returns if codepoint x is a legal Unicode codepoint

    // legal codepoints are those which exist in the inclusive ranges
    // U+0000-U+D7FF and U+E000-U+10FFFF

    constexpr bool is_unicode(unicode_t x) noexcept {
        return 
            in_codepoint_range(x, 0x0000, 0xd7ff) || 
            in_codepoint_range(x, 0xe000, 0x10ffff);
    }

    static_assert(is_unicode(unicode_t(0x0000)) == true);
    static_assert(is_unicode(unicode_t(0x0061)) == true);
    static_assert(is_unicode(unicode_t(0x0062)) == true);
    static_assert(is_unicode(unicode_t(0x0063)) == true);
    static_assert(is_unicode(unicode_t(0xd7ff)) == true);

    static_assert(is_unicode(unicode_t(0xd800)) == false);
    static_assert(is_unicode(unicode_t(0xd8c5)) == false);
    static_assert(is_unicode(unicode_t(0xdf00)) == false);
    static_assert(is_unicode(unicode_t(0xdfa7)) == false);
    static_assert(is_unicode(unicode_t(0xdfff)) == false);

    static_assert(is_unicode(unicode_t(0xe000)) == true);
    static_assert(is_unicode(unicode_t(0xe123)) == true);
    static_assert(is_unicode(unicode_t(0xeabc)) == true);
    static_assert(is_unicode(unicode_t(0xffff)) == true);
    static_assert(is_unicode(unicode_t(0x1ffff)) == true);
    static_assert(is_unicode(unicode_t(0x5ffff)) == true);
    static_assert(is_unicode(unicode_t(0x10ffff)) == true);

    static_assert(is_unicode(unicode_t(0x110000)) == false);
    static_assert(is_unicode(unicode_t(-1)) == false);


    constexpr unicode_t first_surrogate = 0xd800;
    constexpr unicode_t last_surrogate = 0xdfff;

    constexpr unicode_t first_leading_surrogate = first_surrogate;
    constexpr unicode_t last_leading_surrogate = 0xdbff;
    constexpr unicode_t first_trailing_surrogate = 0xdc00;
    constexpr unicode_t last_trailing_surrogate = last_surrogate;

    // is_surrogate returns if codepoint x is within the range of Unicode's
    // special UTF-16 surrogate codepoints which are unavailable for normal
    // use as Unicode codepoints

    // is_leading_surrogate and is_trailing_surrogate do the same, but for
    // the leading and trailing surrogate halves of the full range

    constexpr bool is_surrogate(unicode_t x) noexcept {
        return in_codepoint_range(x, first_surrogate, last_surrogate);
    }

    static_assert(is_surrogate(unicode_t(0x0000)) == false);
    static_assert(is_surrogate(unicode_t(0x0061)) == false);
    static_assert(is_surrogate(unicode_t(0xd7ff)) == false);

    static_assert(is_surrogate(unicode_t(first_leading_surrogate)) == true);
    static_assert(is_surrogate(unicode_t(0xd8c5)) == true);
    static_assert(is_surrogate(unicode_t(last_leading_surrogate)) == true);
    static_assert(is_surrogate(unicode_t(first_trailing_surrogate)) == true);
    static_assert(is_surrogate(unicode_t(0xdfa7)) == true);
    static_assert(is_surrogate(unicode_t(last_trailing_surrogate)) == true);

    static_assert(is_surrogate(unicode_t(0xe000)) == false);
    static_assert(is_surrogate(unicode_t(0x10ffff)) == false);
    static_assert(is_surrogate(unicode_t(0x110000)) == false);
    static_assert(is_surrogate(unicode_t(-1)) == false);

    constexpr bool is_leading_surrogate(unicode_t x) noexcept {
        return in_codepoint_range(x, first_leading_surrogate, last_leading_surrogate);
    }

    static_assert(is_leading_surrogate(unicode_t(0x0000)) == false);
    static_assert(is_leading_surrogate(unicode_t(0x0061)) == false);
    static_assert(is_leading_surrogate(unicode_t(0xd7ff)) == false);

    static_assert(is_leading_surrogate(unicode_t(first_leading_surrogate)) == true);
    static_assert(is_leading_surrogate(unicode_t(0xd8c5)) == true);
    static_assert(is_leading_surrogate(unicode_t(last_leading_surrogate)) == true);
    static_assert(is_leading_surrogate(unicode_t(first_trailing_surrogate)) == false);
    static_assert(is_leading_surrogate(unicode_t(0xdfa7)) == false);
    static_assert(is_leading_surrogate(unicode_t(last_trailing_surrogate)) == false);

    static_assert(is_leading_surrogate(unicode_t(0xe000)) == false);
    static_assert(is_leading_surrogate(unicode_t(0x10ffff)) == false);
    static_assert(is_leading_surrogate(unicode_t(0x110000)) == false);
    static_assert(is_leading_surrogate(unicode_t(-1)) == false);

    constexpr bool is_trailing_surrogate(unicode_t x) noexcept {
        return in_codepoint_range(x, first_trailing_surrogate, last_trailing_surrogate);
    }

    static_assert(is_trailing_surrogate(unicode_t(0x0000)) == false);
    static_assert(is_trailing_surrogate(unicode_t(0x0061)) == false);
    static_assert(is_trailing_surrogate(unicode_t(0xd7ff)) == false);

    static_assert(is_trailing_surrogate(unicode_t(first_leading_surrogate)) == false);
    static_assert(is_trailing_surrogate(unicode_t(0xd8c5)) == false);
    static_assert(is_trailing_surrogate(unicode_t(last_leading_surrogate)) == false);
    static_assert(is_trailing_surrogate(unicode_t(first_trailing_surrogate)) == true);
    static_assert(is_trailing_surrogate(unicode_t(0xdfa7)) == true);
    static_assert(is_trailing_surrogate(unicode_t(last_trailing_surrogate)) == true);

    static_assert(is_trailing_surrogate(unicode_t(0xe000)) == false);
    static_assert(is_trailing_surrogate(unicode_t(0x10ffff)) == false);
    static_assert(is_trailing_surrogate(unicode_t(0x110000)) == false);
    static_assert(is_trailing_surrogate(unicode_t(-1)) == false);


    // U+FEFF is the codepoint who's encoded form defines UTF-8/16/32 BOMs

    constexpr unicode_t bom_codepoint = unicode_t(0xfeff);

    constexpr bool is_bom(unicode_t x) noexcept {
        return x == bom_codepoint;
    }

    static_assert(is_bom(unicode_t(0)) == false);
    static_assert(is_bom(unicode_t(0xfefe)) == false);

    static_assert(is_bom(bom_codepoint) == true);

    static_assert(is_bom(unicode_t(0xff00)) == false);
    static_assert(is_bom(unicode_t(0x10ffff)) == false);
    static_assert(is_bom(unicode_t(-1)) == false);
}

