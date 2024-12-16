

#pragma once


#include <bit>

#include "general.h"
#include "asserts.h"


namespace taul {


    // TODO: this code has not been unit tested yet


    constexpr bool is_native_endian(std::endian x) noexcept {
        return x == std::endian::native;
    }

    constexpr bool is_big_endian    = is_native_endian(std::endian::big);
    constexpr bool is_little_endian = is_native_endian(std::endian::little);
    constexpr bool is_mixed_endian  = !is_big_endian && !is_little_endian;


    constexpr std::endian opposite_endian(std::endian x) noexcept {
        return 
            x == std::endian::big
            ? std::endian::little
            : std::endian::big;
    }


    template<trivially_copyable_type T>
    inline T flip_bytes(T x) noexcept {
        if constexpr (sizeof(T) == 1) return x;
        auto ptr = (uint8_t*)&x;
        for (size_t i = 0; i < sizeof(T) / 2; i++) {
            auto& low = deref_assert(ptr + i);
            auto& high = deref_assert(ptr + (sizeof(T) - 1 - i));
            std::swap(low, high);
        }
        return x;
    }

    // NOTE: while I'm gonna have them be seperate for readability, I find it really
    //       interesting how the to_### and from_### fns below are *literally identical*

    // given some x using native endianness, these return the value
    // in the desired endianness

    template<trivially_copyable_type T>
    inline T to_big_endian(T x) noexcept { return is_big_endian ? x : flip_bytes(x); }
    template<trivially_copyable_type T>
    inline T to_little_endian(T x) noexcept { return is_little_endian ? x : flip_bytes(x); }
    template<trivially_copyable_type T>
    inline T to_endian(std::endian e, T x) noexcept {
        return e == std::endian::big ? to_big_endian(x) : to_little_endian(x);
    }

    // given some x using the specified endianness, these return the
    // value in native endianness

    template<trivially_copyable_type T>
    inline T from_big_endian(T x) noexcept { return is_big_endian ? x : flip_bytes(x); }
    template<trivially_copyable_type T>
    inline T from_little_endian(T x) noexcept { return is_little_endian ? x : flip_bytes(x); }
    template<trivially_copyable_type T>
    inline T from_endian(std::endian e, T x) noexcept {
        return e == std::endian::big ? from_big_endian(x) : from_little_endian(x);
    }
}

