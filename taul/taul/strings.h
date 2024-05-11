

#pragma once


#include <string>
#include <string_view>


namespace taul {


    // these are some general-purpose string analysis utils


    constexpr auto digit                = "0123456789";
    constexpr auto digit_wide           = L"0123456789";
    constexpr auto digit_u8             = u8"0123456789";
    constexpr auto digit_u16            = u"0123456789";
    constexpr auto digit_u32            = U"0123456789";

    constexpr auto hex_lowercase        = "0123456789abcdef";
    constexpr auto hex_lowercase_wide   = L"0123456789abcdef";
    constexpr auto hex_lowercase_u8     = u8"0123456789abcdef";
    constexpr auto hex_lowercase_u16    = u"0123456789abcdef";
    constexpr auto hex_lowercase_u32    = U"0123456789abcdef";

    constexpr auto hex_uppercase        = "0123456789ABCDEF";
    constexpr auto hex_uppercase_wide   = L"0123456789ABCDEF";
    constexpr auto hex_uppercase_u8     = u8"0123456789ABCDEF";
    constexpr auto hex_uppercase_u16    = u"0123456789ABCDEF";
    constexpr auto hex_uppercase_u32    = U"0123456789ABCDEF";

    constexpr auto lowercase            = "abcdefghijklmnopqrstuvwxyz";
    constexpr auto lowercase_wide       = L"abcdefghijklmnopqrstuvwxyz";
    constexpr auto lowercase_u8         = u8"abcdefghijklmnopqrstuvwxyz";
    constexpr auto lowercase_u16        = u"abcdefghijklmnopqrstuvwxyz";
    constexpr auto lowercase_u32        = U"abcdefghijklmnopqrstuvwxyz";
    
    constexpr auto uppercase            = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    constexpr auto uppercase_wide       = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    constexpr auto uppercase_u8         = u8"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    constexpr auto uppercase_u16        = u"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    constexpr auto uppercase_u32        = U"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    constexpr auto alphabet             = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    constexpr auto alphabet_wide        = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    constexpr auto alphabet_u8          = u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    constexpr auto alphabet_u16         = u"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    constexpr auto alphabet_u32         = U"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";


    // TODO: utf8_s has not been unit tested

    // utf8_s simply converts a char8_t string into an std::string
    // utf8_s is handy when unit testing TAUL's support for non-ASCII Unicode usage

    std::string utf8_s(const std::u8string& x);
    std::string utf8_s(std::u8string_view x);
    std::string utf8_s(const char8_t* x);


    // strlen is a generic version of std::strlen

    template<typename Char>
    constexpr std::size_t strlen(const Char* x, Char terminator = Char('\0')) noexcept {
        std::size_t result = 0;
        for (; x[result] != terminator; result++) {}
        return result;
    }

    // since this is constexpr, we can use static_assert instead of unit testing

    static_assert(strlen(L"abcdef123") == 9);
    static_assert(strlen(L"abcdef123", L'f') == 5);


    // where_in_set returns where chr first occurs in string s, returning
    // the length of s if chr never occurs in it

    template<typename Char>
    constexpr std::size_t where_in_set(std::basic_string_view<Char> s, Char chr) noexcept {
        for (std::size_t i = 0; i < s.length(); i++) {
            if (s[i] == chr) {
                return i;
            }
        }
        return s.length();
    }

    template<typename Char>
    constexpr std::size_t where_in_set(const std::basic_string<Char>& s, Char chr) noexcept {
        return where_in_set(std::basic_string_view<Char>(s), chr);
    }
    
    template<typename Char>
    constexpr std::size_t where_in_set(const Char* s, Char chr) noexcept {
        return where_in_set(std::basic_string_view<Char>(s, strlen(s)), chr);
    }

    // since this is constexpr, we can use static_assert instead of unit testing

    static_assert(where_in_set(L"abcdefdefabc", L'e') == 4);
    static_assert(where_in_set(L"abcdefdefabc", L'z') == 12);


    // in_set returns if chr is in string s

    template<typename Char>
    constexpr bool in_set(std::basic_string_view<Char> s, Char chr) noexcept {
        return where_in_set(s, chr) < s.length();
    }

    template<typename Char>
    constexpr bool in_set(const std::basic_string<Char>& s, Char chr) noexcept {
        return in_set(std::basic_string_view<Char>(s), chr);
    }
    
    template<typename Char>
    constexpr bool in_set(const Char* s, Char chr) noexcept {
        return in_set(std::basic_string_view<Char>(s, strlen(s)), chr);
    }

    // since this is constexpr, we can use static_assert instead of unit testing

    static_assert(in_set(L"abcdefdefabc", L'e'));
    static_assert(!in_set(L"abcdefdefabc", L'z'));
}

