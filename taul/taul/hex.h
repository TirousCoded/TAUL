

#pragma once


#include <cstdint>
#include <string>


namespace taul {


    // TODO: maybe add 64-bit stuff below too?

    // TODO: take the below and incorporate it throughout our backend during refactor

    // TODO: add taul::parse_hex_*** functions


    // below shouldn't heap alloc, due to SSO

    std::string fmt_hex_u8(std::uint8_t x, bool uppercase = false);
    std::string fmt_hex_u16(std::uint16_t x, bool uppercase = false);
    std::string fmt_hex_u32(std::uint32_t x, bool uppercase = false);
}

