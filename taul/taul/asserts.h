

#pragma once


#include <cassert>


// TAUL_ASSERT is here so later we can make assert behaviour configurable

#define TAUL_ASSERT(cond) assert(cond)

// TAUL_DEADEND is for asserting that a region of code should not be reached at runtime

#define TAUL_DEADEND TAUL_ASSERT(false)

// TAUL_IN_BOUNDS is for asserting that a given index is within a given range

// specifically, the below asserts: min <= x < max

#define TAUL_IN_BOUNDS(x, min, max) TAUL_ASSERT((x >= (decltype(x))min) && (x < (decltype(x))max))

// TAUL_DEREF_SAFE is a if-statement-like macro used to summarize the checking of pointer safety,
// calling TAUL_DEADEND if the safety check fails

#define TAUL_DEREF_SAFE(cond) if (!(cond)) TAUL_DEADEND; else

// taul::deref_assert wraps a pointer(-like object) deref in a function call which asserts 
// that bool(x) == true

// taul::deref_assert also helps prevent MSVC from complaining about deref of *potential
// nullptr* in scenarios where we otherwise 100% know the pointer is safe

namespace taul {
    template<typename T>
    concept deref_assert_input_type =
        requires (T v)
    {
        // TODO: how can be require *v to not return void?

        // we don't care what T derefs to
        *v;

        { (bool)v } -> std::convertible_to<bool>;
    };

    template<deref_assert_input_type T>
    inline auto&& deref_assert(T x) {
        TAUL_ASSERT(bool(x));
        return *x;
    }
}

