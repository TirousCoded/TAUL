

#pragma once


#include <bit>


namespace taul {


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


    // TODO: below functions have not yet been unit tested
    
    template<typename T>
    inline T flip_bytes(T x) noexcept {
        static_assert(std::is_trivially_copyable_v<T>);
        if (sizeof(T) == 1) {
            return x;
        }
        auto ptr = (std::uint8_t*)&x;
        if (ptr) {
            for (std::size_t i = 0; i < sizeof(T) / 2; i++) {
                std::swap(ptr[i], ptr[sizeof(T) - 1 - i]);
            }
        }
        else TAUL_DEADEND;
        return x;
    }

    template<typename T>
    inline T ensure_big_endian(T x) noexcept {
        static_assert(std::is_trivially_copyable_v<T>);
        return
            is_big_endian
            ? x
            : flip_bytes(x);
    }

    template<typename T>
    inline T ensure_little_endian(T x) noexcept {
        static_assert(std::is_trivially_copyable_v<T>);
        return
            is_little_endian
            ? x
            : flip_bytes(x);
    }

    template<typename T>
    inline T ensure_endian(std::endian e, T x) noexcept {
        static_assert(std::is_trivially_copyable_v<T>);
        return
            e == std::endian::big
            ? ensure_big_endian(x)
            : ensure_little_endian(x);
    }
}

