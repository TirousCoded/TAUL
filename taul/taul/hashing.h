

#pragma once


#include <functional>
#include <concepts>


namespace taul {


    // this is just a header file of hashing-related utilities


    // TODO: hash_combine has not been unit tested

    // hash_combine combines two hashes into one

    inline size_t hash_combine(size_t a, size_t b) noexcept {
        // got this from cppreference
        return a ^ (b << 1);
    }

    template<typename T>
    concept hashable_type =
        requires (T v)
    {
        { std::hash<T>{}(v) } noexcept -> std::convertible_to<size_t>;
    };

    template<hashable_type T>
    inline size_t hash(const T& x) noexcept {
        return std::hash<T>{}(x);
    }

    // the below overloads of hash sequentially hash_combine the
    // hash codes of a number of inputs

    template<hashable_type Arg, hashable_type... Args>
    inline size_t hash(const Arg& arg, const Args&... args) noexcept {
        return hash_combine(hash(arg), hash(args...));
    }
}

