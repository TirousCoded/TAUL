

#include "encoding.h"

#include "asserts.h"


std::string taul::fmt_encoding(encoding x) {
    std::string result{};
    switch (x) {
    case encoding::utf8:        result = "UTF-8";       break;
    case encoding::utf16:       result = "UTF-16";      break;
    case encoding::utf32:       result = "UTF-32";      break;
    case encoding::utf16_be:    result = "UTF-16 BE";   break;
    case encoding::utf16_le:    result = "UTF-16 LE";   break;
    case encoding::utf32_be:    result = "UTF-32 BE";   break;
    case encoding::utf32_le:    result = "UTF-32 LE";   break;
    case encoding::utf8_bom:    result = "UTF-8 BOM";   break;
    case encoding::utf16_bom:   result = "UTF-16 BOM";  break;
    case encoding::utf32_bom:   result = "UTF-32 BOM";  break;
    default:                    TAUL_DEADEND;           break;
    }
    return result;
}

