

#include "qualifier.h"

#include "asserts.h"


std::string taul::fmt_qualifier(qualifier x) {
    std::string result{};
    switch (x) {
    case qualifier::none:       result = "none";    break;
    case qualifier::skip:       result = "skip";    break;
    case qualifier::support:    result = "support"; break;
    default:                    TAUL_DEADEND;       break;
    }
    return result;
}

