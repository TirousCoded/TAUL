

#pragma once


#include <functional>


namespace taul {


    // this is just a header file of hashing-related utilities


    template<typename T>
    inline std::size_t hash(const T& x) noexcept {
        return std::hash<T>{}(x);
    }

    inline std::size_t hash_combine(std::size_t a, std::size_t b) noexcept {
        // got this from cppreference
        return a ^ (b << 1);
    }
}

