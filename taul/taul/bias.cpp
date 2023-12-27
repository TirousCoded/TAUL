

#include "bias.h"

#include "asserts.h"


std::string taul::fmt_bias(bias x) {
    std::string result{};
    switch (x) {
    case bias::fl:  result = "first-longest";   break;
    case bias::fs:  result = "first-shortest";  break;
    case bias::ll:  result = "last-longest";    break;
    case bias::ls:  result = "last-shortest";   break;
    case bias::f:   result = "first";           break;
    case bias::l:   result = "last";            break;
    default:        TAUL_DEADEND;               break;
    }
    return result;
}

