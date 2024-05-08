

#include "hex.h"

#include "strings.h"


std::string taul::fmt_hex_u8(std::uint8_t x, bool uppercase) {
    const char* chrs = uppercase ? hex_uppercase : hex_lowercase;
    std::string result{};
    size_t current = size_t(x);
    for (size_t i = 0; i < 2; i++) {
        size_t remainder = current % 16;
        result = chrs[remainder] + result;
        current = (current - remainder) / 16;
    }
    return result;
}

std::string taul::fmt_hex_u16(std::uint16_t x, bool uppercase) {
    const char* chrs = uppercase ? hex_uppercase : hex_lowercase;
    std::string result{};
    size_t current = size_t(x);
    for (size_t i = 0; i < 4; i++) {
        size_t remainder = current % 16;
        result = chrs[remainder] + result;
        current = (current - remainder) / 16;
    }
    return result;
}

std::string taul::fmt_hex_u32(std::uint32_t x, bool uppercase) {
    const char* chrs = uppercase ? hex_uppercase : hex_lowercase;
    std::string result{};
    size_t current = size_t(x);
    for (size_t i = 0; i < 8; i++) {
        size_t remainder = current % 16;
        result = chrs[remainder] + result;
        current = (current - remainder) / 16;
    }
    return result;
}

