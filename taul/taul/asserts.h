

#pragma once


#include <cassert>


// this is here so later we can make assert behaviour configurable

#define TAUL_ASSERT(cond) assert(cond)


// this is for asserting that a region of code should not be reached at runtime

#define TAUL_DEADEND TAUL_ASSERT(false)

// this is for asserting that a given index is within a given range

// specifically, the below asserts: min <= x < max

#define TAUL_IN_BOUNDS(x, min, max) TAUL_ASSERT((x >= (decltype(x))min) && (x < (decltype(x))max))

