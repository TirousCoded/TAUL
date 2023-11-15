

#include "bias.h"

#include "asserts.h"


std::string taul::fmt_bias(bias x) {
    std::string result{};
    switch (x) {
    case bias::first_longest:   result = "first-longest";   break;
    case bias::first_shortest:  result = "first-shortest";  break;
    case bias::last_longest:    result = "last-longest";    break;
    case bias::last_shortest:   result = "last-shortest";   break;
    case bias::first:           result = "first";           break;
    case bias::last:            result = "last";            break;
    default:                    TAUL_DEADEND;		        break;
    }
    return result;
}

