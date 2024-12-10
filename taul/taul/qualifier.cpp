

#include "qualifier.h"

#include "asserts.h"


std::string taul::fmt_qualifier(qualifier x) {
    std::string result{};
    static_assert(qualifiers == 4);
    switch (x) {
    case qualifier::none:       result = "none";        break;
    case qualifier::skip:       result = "skip";        break;
    case qualifier::support:    result = "support";     break;
    case qualifier::precedence: result = "precedence";  break;
    default:                    TAUL_DEADEND;           break;
    }
    return result;
}

