

#pragma once


#include "source_code.h"


namespace taul {


    // TODO: this has not been unit tested

    // TODO: if we haven't already, add a code example for end-users to learn from,
    //       and make note here pointing this out to them

    // source_pos_counter is a helper used to ease the burden of having to
    // keep track of source_pos values when assembling parse_tree(s) and
    // parse_tree_pattern(s)

    struct source_pos_counter final {
        source_pos pos = 0; // current pos


        // next adds len + after_len to pos, and then returns old value of pos

        // len describes the amount of chars to count which are a part of the
        // parse_tree(_pattern) feature who's pos is being described

        // after_len describes the amount of chars to count which are AFTER the
        // parse_tree(_pattern) feature who's pos is being described

        inline source_pos next(source_len len = 0, source_len after_len = 0) {
            const auto result = pos;
            pos += len + after_len;
            return result;
        }

        inline source_pos operator()(source_len len = 0, source_len after_len = 0) {
            return next(len, after_len);
        }
    };
}

