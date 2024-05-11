

#include <gtest/gtest.h>

#include <taul/strings.h>
#include <taul/hex.h>


TEST(HexTests, ParseHexU8) {
    for (uint8_t i = 0; i < 16; i++) {
        ASSERT_EQ(taul::parse_hex_u8(std::format("0{}", taul::hex_lowercase[i])), i * 0x01);
        ASSERT_EQ(taul::parse_hex_u8(std::format("{}0", taul::hex_lowercase[i])), i * 0x10);
        ASSERT_EQ(taul::parse_hex_u8(std::format("{0}{0}", taul::hex_lowercase[i])), i * 0x11);

        ASSERT_EQ(taul::parse_hex_u8(std::format("0{}", taul::hex_uppercase[i])), i * 0x01);
        ASSERT_EQ(taul::parse_hex_u8(std::format("{}0", taul::hex_uppercase[i])), i * 0x10);
        ASSERT_EQ(taul::parse_hex_u8(std::format("{0}{0}", taul::hex_uppercase[i])), i * 0x11);
    }
    ASSERT_EQ(taul::parse_hex_u8("af"), 0xaf);
    ASSERT_EQ(taul::parse_hex_u8("AF"), 0xaf);
    ASSERT_EQ(taul::parse_hex_u8("Af"), 0xaf);
    ASSERT_EQ(taul::parse_hex_u8("aF"), 0xaf);
}

TEST(HexTests, ParseHexU8_IgnoreExtraChars) {
    ASSERT_EQ(taul::parse_hex_u8("afjj"), 0xaf);
    ASSERT_EQ(taul::parse_hex_u8("AFjj"), 0xaf);
    ASSERT_EQ(taul::parse_hex_u8("Afjj"), 0xaf);
    ASSERT_EQ(taul::parse_hex_u8("aFjj"), 0xaf);
}

TEST(HexTests, ParseHexU8_Failure) {
    ASSERT_FALSE(taul::parse_hex_u8(""));
    ASSERT_FALSE(taul::parse_hex_u8("f"));
    ASSERT_FALSE(taul::parse_hex_u8("fj"));
}

TEST(HexTests, ParseHexU16) {
    for (uint16_t i = 0; i < 16; i++) {
        ASSERT_EQ(taul::parse_hex_u16(std::format("000{}", taul::hex_lowercase[i])), i * 0x0001);
        ASSERT_EQ(taul::parse_hex_u16(std::format("00{}0", taul::hex_lowercase[i])), i * 0x0010);
        ASSERT_EQ(taul::parse_hex_u16(std::format("0{}00", taul::hex_lowercase[i])), i * 0x0100);
        ASSERT_EQ(taul::parse_hex_u16(std::format("{}000", taul::hex_lowercase[i])), i * 0x1000);
        ASSERT_EQ(taul::parse_hex_u16(std::format("{0}{0}{0}{0}", taul::hex_lowercase[i])), i * 0x1111);

        ASSERT_EQ(taul::parse_hex_u16(std::format("000{}", taul::hex_uppercase[i])), i * 0x0001);
        ASSERT_EQ(taul::parse_hex_u16(std::format("00{}0", taul::hex_uppercase[i])), i * 0x0010);
        ASSERT_EQ(taul::parse_hex_u16(std::format("0{}00", taul::hex_uppercase[i])), i * 0x0100);
        ASSERT_EQ(taul::parse_hex_u16(std::format("{}000", taul::hex_uppercase[i])), i * 0x1000);
        ASSERT_EQ(taul::parse_hex_u16(std::format("{0}{0}{0}{0}", taul::hex_uppercase[i])), i * 0x1111);
    }
    ASSERT_EQ(taul::parse_hex_u16("ab3f"), 0xab3f);
    ASSERT_EQ(taul::parse_hex_u16("AB3F"), 0xab3f);
    ASSERT_EQ(taul::parse_hex_u16("Ab3f"), 0xab3f);
    ASSERT_EQ(taul::parse_hex_u16("aB3F"), 0xab3f);
}

TEST(HexTests, ParseHexU16_IgnoreExtraChars) {
    ASSERT_EQ(taul::parse_hex_u16("ab3fjj"), 0xab3f);
    ASSERT_EQ(taul::parse_hex_u16("AB3Fjj"), 0xab3f);
    ASSERT_EQ(taul::parse_hex_u16("Ab3fjj"), 0xab3f);
    ASSERT_EQ(taul::parse_hex_u16("aB3Fjj"), 0xab3f);
}

TEST(HexTests, ParseHexU16_Failure) {
    ASSERT_FALSE(taul::parse_hex_u16(""));
    ASSERT_FALSE(taul::parse_hex_u16("fff"));
    ASSERT_FALSE(taul::parse_hex_u16("fffj"));
}

TEST(HexTests, ParseHexU32) {
    for (uint32_t i = 0; i < 16; i++) {
        ASSERT_EQ(taul::parse_hex_u32(std::format("0000000{}", taul::hex_lowercase[i])), i * 0x00000001);
        ASSERT_EQ(taul::parse_hex_u32(std::format("000000{}0", taul::hex_lowercase[i])), i * 0x00000010);
        ASSERT_EQ(taul::parse_hex_u32(std::format("00000{}00", taul::hex_lowercase[i])), i * 0x00000100);
        ASSERT_EQ(taul::parse_hex_u32(std::format("0000{}000", taul::hex_lowercase[i])), i * 0x00001000);
        ASSERT_EQ(taul::parse_hex_u32(std::format("000{}0000", taul::hex_lowercase[i])), i * 0x00010000);
        ASSERT_EQ(taul::parse_hex_u32(std::format("00{}00000", taul::hex_lowercase[i])), i * 0x00100000);
        ASSERT_EQ(taul::parse_hex_u32(std::format("0{}000000", taul::hex_lowercase[i])), i * 0x01000000);
        ASSERT_EQ(taul::parse_hex_u32(std::format("{}0000000", taul::hex_lowercase[i])), i * 0x10000000);
        ASSERT_EQ(taul::parse_hex_u32(std::format("{0}{0}{0}{0}{0}{0}{0}{0}", taul::hex_lowercase[i])), i * 0x11111111);

        ASSERT_EQ(taul::parse_hex_u32(std::format("0000000{}", taul::hex_uppercase[i])), i * 0x00000001);
        ASSERT_EQ(taul::parse_hex_u32(std::format("000000{}0", taul::hex_uppercase[i])), i * 0x00000010);
        ASSERT_EQ(taul::parse_hex_u32(std::format("00000{}00", taul::hex_uppercase[i])), i * 0x00000100);
        ASSERT_EQ(taul::parse_hex_u32(std::format("0000{}000", taul::hex_uppercase[i])), i * 0x00001000);
        ASSERT_EQ(taul::parse_hex_u32(std::format("000{}0000", taul::hex_uppercase[i])), i * 0x00010000);
        ASSERT_EQ(taul::parse_hex_u32(std::format("00{}00000", taul::hex_uppercase[i])), i * 0x00100000);
        ASSERT_EQ(taul::parse_hex_u32(std::format("0{}000000", taul::hex_uppercase[i])), i * 0x01000000);
        ASSERT_EQ(taul::parse_hex_u32(std::format("{}0000000", taul::hex_uppercase[i])), i * 0x10000000);
        ASSERT_EQ(taul::parse_hex_u32(std::format("{0}{0}{0}{0}{0}{0}{0}{0}", taul::hex_uppercase[i])), i * 0x11111111);
    }
    ASSERT_EQ(taul::parse_hex_u32("abcde15f"), 0xabcde15f);
    ASSERT_EQ(taul::parse_hex_u32("AbCdE15f"), 0xabcde15f);
    ASSERT_EQ(taul::parse_hex_u32("aBcDe15F"), 0xabcde15f);
}

TEST(HexTests, ParseHexU32_IgnoreExtraChars) {
    ASSERT_EQ(taul::parse_hex_u32("abcde15fjj"), 0xabcde15f);
    ASSERT_EQ(taul::parse_hex_u32("AbCdE15fjj"), 0xabcde15f);
    ASSERT_EQ(taul::parse_hex_u32("aBcDe15Fjj"), 0xabcde15f);
}

TEST(HexTests, ParseHexU32_Failure) {
    ASSERT_FALSE(taul::parse_hex_u32(""));
    ASSERT_FALSE(taul::parse_hex_u32("fffffff"));
    ASSERT_FALSE(taul::parse_hex_u32("fffffffj"));
}

TEST(HexTests, FmtHexU8) {
    for (uint8_t i = 0; i < 16; i++) {
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
    for (uint16_t i = 0; i < 16; i++) {
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
    for (uint32_t i = 0; i < 16; i++) {
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

