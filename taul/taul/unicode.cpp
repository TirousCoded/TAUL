

#include "unicode.h"

#include "asserts.h"
#include "strings.h"


std::string taul::fmt_unicode(unicode_t x, bool uppercase_hex) {
    std::string result{};
    const auto hex_digits = uppercase_hex ? hex_uppercase : hex_lowercase;
    if (hex_digits) {
        do {
            const auto n = x % unicode_t(16);
            result = hex_digits[n] + result;
            x = (x - n) / 16;
        } while (x > unicode_t(0));
        if (result.length() < 4) result = std::string(4 - result.length(), '0') + result;
    }
    else TAUL_DEADEND;
    return "U+" + result;
}

