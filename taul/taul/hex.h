

#pragma once


#include <cstdint>
#include <string>
#include <optional>


namespace taul {


    // TODO: take the below and incorporate it throughout our backend during refactor


    // parse_hex_u8/16/32 parses a hex string x, returing the value resolved, if any

    // only the first 2/4/8 chars are considered, any extra are ignored

    std::optional<uint8_t> parse_hex_u8(std::string_view x) noexcept;
    std::optional<uint16_t> parse_hex_u16(std::string_view x) noexcept;
    std::optional<uint32_t> parse_hex_u32(std::string_view x) noexcept;
    
    template<typename StringLike>
    inline std::optional<uint8_t> parse_hex_u8(const StringLike& x) noexcept { return parse_hex_u8(std::string_view(x)); }
    template<typename StringLike>
    inline std::optional<uint16_t> parse_hex_u16(const StringLike& x) noexcept { return parse_hex_u16(std::string_view(x)); }
    template<typename StringLike>
    inline std::optional<uint32_t> parse_hex_u32(const StringLike& x) noexcept { return parse_hex_u32(std::string_view(x)); }

    // below shouldn't heap alloc, due to SSO

    std::string fmt_hex_u8(uint8_t x, bool uppercase = false);
    std::string fmt_hex_u16(uint16_t x, bool uppercase = false);
    std::string fmt_hex_u32(uint32_t x, bool uppercase = false);
}

