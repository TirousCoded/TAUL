

#include "polarity.h"

#include "asserts.h"


std::string taul::fmt_polarity(polarity x) {
    std::string result{};
    switch (x) {
    case polarity::positive:    result = "positive";    break;
    case polarity::negative:    result = "negative";    break;
    default:                    TAUL_DEADEND;           break;
    }
    return result;
}

