

#include <gtest/gtest.h>

#include <taul/strings.h>
#include <taul/hex.h>


TEST(HexTests, FmtHexU8) {
    for (std::uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(taul::fmt_hex_u8(i * 0x01), std::format("0{}", taul::hex_lowercase[i]));
        ASSERT_EQ(taul::fmt_hex_u8(i * 0x10), std::format("{}0", taul::hex_lowercase[i]));
        ASSERT_EQ(taul::fmt_hex_u8(i * 0x11), std::format("{0}{0}", taul::hex_lowercase[i]));

        ASSERT_EQ(taul::fmt_hex_u8(i * 0x01, true), std::format("0{}", taul::hex_uppercase[i]));
        ASSERT_EQ(taul::fmt_hex_u8(i * 0x10, true), std::format("{}0", taul::hex_uppercase[i]));
        ASSERT_EQ(taul::fmt_hex_u8(i * 0x11, true), std::format("{0}{0}", taul::hex_uppercase[i]));
    }
    ASSERT_EQ(taul::fmt_hex_u8(0xaf), "af");
    ASSERT_EQ(taul::fmt_hex_u8(0xaf, true), "AF");
}

TEST(HexTests, FmtHexU16) {
    for (std::uint16_t i = 0; i < 16; i++) {
        ASSERT_EQ(taul::fmt_hex_u16(i * 0x0001), std::format("000{}", taul::hex_lowercase[i]));
        ASSERT_EQ(taul::fmt_hex_u16(i * 0x0010), std::format("00{}0", taul::hex_lowercase[i]));
        ASSERT_EQ(taul::fmt_hex_u16(i * 0x0100), std::format("0{}00", taul::hex_lowercase[i]));
        ASSERT_EQ(taul::fmt_hex_u16(i * 0x1000), std::format("{}000", taul::hex_lowercase[i]));
        ASSERT_EQ(taul::fmt_hex_u16(i * 0x1111), std::format("{0}{0}{0}{0}", taul::hex_lowercase[i]));

        ASSERT_EQ(taul::fmt_hex_u16(i * 0x0001, true), std::format("000{}", taul::hex_uppercase[i]));
        ASSERT_EQ(taul::fmt_hex_u16(i * 0x0010, true), std::format("00{}0", taul::hex_uppercase[i]));
        ASSERT_EQ(taul::fmt_hex_u16(i * 0x0100, true), std::format("0{}00", taul::hex_uppercase[i]));
        ASSERT_EQ(taul::fmt_hex_u16(i * 0x1000, true), std::format("{}000", taul::hex_uppercase[i]));
        ASSERT_EQ(taul::fmt_hex_u16(i * 0x1111, true), std::format("{0}{0}{0}{0}", taul::hex_uppercase[i]));
    }
    ASSERT_EQ(taul::fmt_hex_u16(0xab3f), "ab3f");
    ASSERT_EQ(taul::fmt_hex_u16(0xab3f, true), "AB3F");
}

TEST(HexTests, FmtHexU32) {
    for (std::uint32_t i = 0; i < 16; i++) {
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x00000001), std::format("0000000{}", taul::hex_lowercase[i]));
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x00000010), std::format("000000{}0", taul::hex_lowercase[i]));
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x00000100), std::format("00000{}00", taul::hex_lowercase[i]));
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x00001000), std::format("0000{}000", taul::hex_lowercase[i]));
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x00010000), std::format("000{}0000", taul::hex_lowercase[i]));
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x00100000), std::format("00{}00000", taul::hex_lowercase[i]));
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x01000000), std::format("0{}000000", taul::hex_lowercase[i]));
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x10000000), std::format("{}0000000", taul::hex_lowercase[i]));
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x11111111), std::format("{0}{0}{0}{0}{0}{0}{0}{0}", taul::hex_lowercase[i]));

        ASSERT_EQ(taul::fmt_hex_u32(i * 0x00000001, true), std::format("0000000{}", taul::hex_uppercase[i]));
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x00000010, true), std::format("000000{}0", taul::hex_uppercase[i]));
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x00000100, true), std::format("00000{}00", taul::hex_uppercase[i]));
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x00001000, true), std::format("0000{}000", taul::hex_uppercase[i]));
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x00010000, true), std::format("000{}0000", taul::hex_uppercase[i]));
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x00100000, true), std::format("00{}00000", taul::hex_uppercase[i]));
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x01000000, true), std::format("0{}000000", taul::hex_uppercase[i]));
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x10000000, true), std::format("{}0000000", taul::hex_uppercase[i]));
        ASSERT_EQ(taul::fmt_hex_u32(i * 0x11111111, true), std::format("{0}{0}{0}{0}{0}{0}{0}{0}", taul::hex_uppercase[i]));
    }
    ASSERT_EQ(taul::fmt_hex_u32(0xabcde15f), "abcde15f");
    ASSERT_EQ(taul::fmt_hex_u32(0xabcde15f, true), "ABCDE15F");
}

