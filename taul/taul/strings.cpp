

#include "strings.h"

#include "asserts.h"


std::string taul::utf8_s(const std::u8string& x) {
    return utf8_s(std::u8string_view(x));
}

std::string taul::utf8_s(std::u8string_view x) {
    static_assert(sizeof(char8_t) == sizeof(char));
    // below should be fine as the only real difference in at compile time
    return std::string((const char*)x.data(), x.size());
}

std::string taul::utf8_s(const char8_t* x) {
    TAUL_ASSERT(x);
    return utf8_s(std::u8string_view(x));
}

