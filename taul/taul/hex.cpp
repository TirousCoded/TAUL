

#include "hex.h"

#include "strings.h"

#include "internal/util.h"


std::optional<uint8_t> taul::parse_hex_u8(std::string_view x) noexcept {
    if (x.length() < 2) return std::nullopt; // if too short, fail
    uint8_t result = 0;
    for (const auto& I : x.substr(0, 2)) {
        auto temp = (uint8_t)internal::hex_digit(I);
        if (temp == 16) return std::nullopt; // if not valid hex digit, fail
        result = result * 16 + temp;
    }
    return result;
}

std::optional<uint16_t> taul::parse_hex_u16(std::string_view x) noexcept {
    if (x.length() < 4) return std::nullopt; // if too short, fail
    uint16_t result = 0;
    for (const auto& I : x.substr(0, 4)) {
        auto temp = (uint16_t)internal::hex_digit(I);
        if (temp == 16) return std::nullopt; // if not valid hex digit, fail
        result = result * 16 + temp;
    }
    return result;
}

std::optional<uint32_t> taul::parse_hex_u32(std::string_view x) noexcept {
    if (x.length() < 8) return std::nullopt; // if too short, fail
    uint32_t result = 0;
    for (const auto& I : x.substr(0, 8)) {
        auto temp = (uint32_t)internal::hex_digit(I);
        if (temp == 16) return std::nullopt; // if not valid hex digit, fail
        result = result * 16 + temp;
    }
    return result;
}

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

