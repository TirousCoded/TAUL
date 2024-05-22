

#include <gtest/gtest.h>

#include <taul/logger.h>
#include <taul/strings.h>
#include <taul/hex.h>
#include <taul/encoding.h>
#include <taul/string_and_charset.h>
#include <taul/symbol_set.h>


TEST(StringAndCharsetTests, ParseTAULChar_FailDueToEmptyInput) {
    size_t chars = size_t(-1);
    auto result0 = taul::parse_taul_char("", &chars);
    auto result1 = taul::parse_taul_char("");
    EXPECT_FALSE(result0);
    EXPECT_FALSE(result1);
    EXPECT_EQ(chars, size_t(-1));
}

TEST(StringAndCharsetTests, ParseTAULChar_UnescapedChars) {
    const auto lgr = taul::make_stderr_logger();
    for (const auto& I : "abfnrtv']-ABCDEF123456!@#$%^\\") {
        std::string in = std::format("{}", I);
        TAUL_LOG(lgr, "testing {}", in);
        size_t chars = size_t(-1);
        auto result0 = taul::parse_taul_char(in, &chars);
        auto result1 = taul::parse_taul_char(in);
        if (result0) {
            EXPECT_EQ(*result0, I);
            EXPECT_EQ(chars, 1);
        }
        else ADD_FAILURE();
        if (result1) {
            EXPECT_EQ(*result1, I);
        }
        else ADD_FAILURE();
    }
}

TEST(StringAndCharsetTests, ParseTAULChar_BasicEscapeSeqs) {
    const auto lgr = taul::make_stderr_logger();
    // test first 12 escape seqs
    constexpr size_t esc_num = 12;
    constexpr const char* esc_ins = "0abfnrtv']-\\";
    constexpr const taul::unicode_t* esc_outs = U"\0\a\b\f\n\r\t\v\']-\\";
    for (size_t i = 0; i < esc_num; i++) {
        std::string in = std::format("\\{}", esc_ins[i]);
        TAUL_LOG(lgr, "testing {}", in);
        size_t chars = size_t(-1);
        auto result0 = taul::parse_taul_char(in, &chars);
        auto result1 = taul::parse_taul_char(in);
        if (result0) {
            EXPECT_EQ(*result0, esc_outs[i]);
            EXPECT_EQ(chars, 2);
        }
        else ADD_FAILURE();
        if (result1) {
            EXPECT_EQ(*result1, esc_outs[i]);
        }
        else ADD_FAILURE();
    }
}

TEST(StringAndCharsetTests, ParseTAULChar_8BitHexEscapeSeq) {
    const auto lgr = taul::make_stderr_logger();
    // test escape seq \xhh
    constexpr size_t hex_digit_num = 16;
    for (size_t i = 0; i < 8; i++) {
        for (size_t j = 0; j < hex_digit_num; j++) {
            // these iPick and jPick vars help us test each pair of
            // hex digits being uppercase/lowercase
            for (size_t iPick = 0; iPick < 2; iPick++) {
                for (size_t jPick = 0; jPick < 2; jPick++) {
                    char charA = (iPick == 0 ? taul::hex_uppercase : taul::hex_lowercase)[i];
                    char charB = (jPick == 0 ? taul::hex_uppercase : taul::hex_lowercase)[j];
                    std::string in = std::format("\\x{}{}", charA, charB);
                    size_t expected_out = i * hex_digit_num + j;
                    TAUL_LOG(lgr, "testing {}", in);
                    size_t chars = size_t(-1);
                    auto result0 = taul::parse_taul_char(in, &chars);
                    auto result1 = taul::parse_taul_char(in);
                    if (result0) {
                        EXPECT_EQ(*result0, expected_out);
                        EXPECT_EQ(chars, 4);
                    }
                    else ADD_FAILURE();
                    if (result1) {
                        EXPECT_EQ(*result1, expected_out);
                    }
                    else ADD_FAILURE();
                }
            }
        }
    }
}

TEST(StringAndCharsetTests, ParseTAULChar_8BitHexEscapeSeq_InvalidFormResultsInLiteralizationInstead) {
    const auto lgr = taul::make_stderr_logger();
    {
        // too short
        size_t chars = size_t(-1);
        auto result0 = taul::parse_taul_char("\\x", &chars);
        auto result1 = taul::parse_taul_char("\\x");
        if (result0) {
            EXPECT_EQ(*result0, 'x');
            EXPECT_EQ(chars, 2);
        }
        else ADD_FAILURE();
        if (result1) {
            EXPECT_EQ(*result1, 'x');
        }
        else ADD_FAILURE();
    }
    {
        // too short
        size_t chars = size_t(-1);
        auto result0 = taul::parse_taul_char("\\xf", &chars);
        auto result1 = taul::parse_taul_char("\\xf");
        if (result0) {
            EXPECT_EQ(*result0, 'x');
            EXPECT_EQ(chars, 2);
        }
        else ADD_FAILURE();
        if (result1) {
            EXPECT_EQ(*result1, 'x');
        }
        else ADD_FAILURE();
    }
    {
        // invalid char encountered
        size_t chars = size_t(-1);
        auto result0 = taul::parse_taul_char("\\xfj", &chars);
        auto result1 = taul::parse_taul_char("\\xfj");
        if (result0) {
            EXPECT_EQ(*result0, 'x');
            EXPECT_EQ(chars, 2);
        }
        else ADD_FAILURE();
        if (result1) {
            EXPECT_EQ(*result1, 'x');
        }
        else ADD_FAILURE();
    }
}

TEST(StringAndCharsetTests, ParseTAULChar_16BitHexEscapeSeq) {
    const auto lgr = taul::make_stderr_logger();
    // test escape seq \uhhhh
    constexpr size_t hex_digit_num = 16;
    for (size_t i = 0; i < 8; i++) {
        for (size_t j = 0; j < hex_digit_num; j++) {
            // these iPick and jPick vars help us test each pair of
            // hex digits being uppercase/lowercase
            for (size_t iPick = 0; iPick < 2; iPick++) {
                for (size_t jPick = 0; jPick < 2; jPick++) {
                    char charA = (iPick == 0 ? taul::hex_uppercase : taul::hex_lowercase)[i];
                    char charB = (jPick == 0 ? taul::hex_uppercase : taul::hex_lowercase)[j];
                    std::string in = std::format("\\u{0}{1}{0}{1}", charA, charB);
                    size_t expected_out =
                        i * hex_digit_num * hex_digit_num * hex_digit_num +
                        j * hex_digit_num * hex_digit_num +
                        i * hex_digit_num +
                        j;
                    TAUL_LOG(lgr, "testing {}", in);
                    size_t chars = size_t(-1);
                    auto result0 = taul::parse_taul_char(in, &chars);
                    auto result1 = taul::parse_taul_char(in);
                    if (result0) {
                        EXPECT_EQ(*result0, expected_out);
                        EXPECT_EQ(chars, 6);
                    }
                    else ADD_FAILURE();
                    if (result1) {
                        EXPECT_EQ(*result1, expected_out);
                    }
                    else ADD_FAILURE();
                }
            }
        }
    }
}

TEST(StringAndCharsetTests, ParseTAULChar_16BitHexEscapeSeq_InvalidFormResultsInLiteralizationInstead) {
    const auto lgr = taul::make_stderr_logger();
    {
        // too short
        size_t chars = size_t(-1);
        auto result0 = taul::parse_taul_char("\\u", &chars);
        auto result1 = taul::parse_taul_char("\\u");
        if (result0) {
            EXPECT_EQ(*result0, 'u');
            EXPECT_EQ(chars, 2);
        }
        else ADD_FAILURE();
        if (result1) {
            EXPECT_EQ(*result1, 'u');
        }
        else ADD_FAILURE();
    }
    {
        // too short
        size_t chars = size_t(-1);
        auto result0 = taul::parse_taul_char("\\ufff", &chars);
        auto result1 = taul::parse_taul_char("\\ufff");
        if (result0) {
            EXPECT_EQ(*result0, 'u');
            EXPECT_EQ(chars, 2);
        }
        else ADD_FAILURE();
        if (result1) {
            EXPECT_EQ(*result1, 'u');
        }
        else ADD_FAILURE();
    }
    {
        // invalid char encountered
        size_t chars = size_t(-1);
        auto result0 = taul::parse_taul_char("\\ufffj", &chars);
        auto result1 = taul::parse_taul_char("\\ufffj");
        if (result0) {
            EXPECT_EQ(*result0, 'u');
            EXPECT_EQ(chars, 2);
        }
        else ADD_FAILURE();
        if (result1) {
            EXPECT_EQ(*result1, 'u');
        }
        else ADD_FAILURE();
    }
}

TEST(StringAndCharsetTests, ParseTAULChar_32BitHexEscapeSeq) {
    const auto lgr = taul::make_stderr_logger();
    // test escape seq \Uhhhhhhhh
    constexpr size_t hex_digit_num = 16;
    for (size_t i = 0; i < 8; i++) {
        for (size_t j = 0; j < hex_digit_num; j++) {
            // these iPick and jPick vars help us test each pair of
            // hex digits being uppercase/lowercase
            for (size_t iPick = 0; iPick < 2; iPick++) {
                for (size_t jPick = 0; jPick < 2; jPick++) {
                    char charA = (iPick == 0 ? taul::hex_uppercase : taul::hex_lowercase)[i];
                    char charB = (jPick == 0 ? taul::hex_uppercase : taul::hex_lowercase)[j];
                    // illegal Unicode includes anything above 
                    std::string in = std::format("\\U0010{0}{1}{0}{1}", charA, charB);
                    size_t expected_out =
                        0x0010'0000 +
                        i * hex_digit_num * hex_digit_num * hex_digit_num +
                        j * hex_digit_num * hex_digit_num +
                        i * hex_digit_num +
                        j;
                    TAUL_LOG(lgr, "testing {}", in);
                    size_t chars = size_t(-1);
                    auto result0 = taul::parse_taul_char(in, &chars);
                    auto result1 = taul::parse_taul_char(in);
                    if (result0) {
                        EXPECT_EQ(*result0, expected_out);
                        EXPECT_EQ(chars, 10);
                    }
                    else ADD_FAILURE();
                    if (result1) {
                        EXPECT_EQ(*result1, expected_out);
                    }
                    else ADD_FAILURE();
                }
            }
        }
    }
}

TEST(StringAndCharsetTests, ParseTAULChar_32BitHexEscapeSeq_InvalidFormResultsInLiteralizationInstead) {
    const auto lgr = taul::make_stderr_logger();
    {
        // too short
        size_t chars = size_t(-1);
        auto result0 = taul::parse_taul_char("\\U", &chars);
        auto result1 = taul::parse_taul_char("\\U");
        if (result0) {
            EXPECT_EQ(*result0, 'U');
            EXPECT_EQ(chars, 2);
        }
        else ADD_FAILURE();
        if (result1) {
            EXPECT_EQ(*result1, 'U');
        }
        else ADD_FAILURE();
    }
    {
        // too short
        size_t chars = size_t(-1);
        auto result0 = taul::parse_taul_char("\\U0010fff", &chars);
        auto result1 = taul::parse_taul_char("\\U0010fff");
        if (result0) {
            EXPECT_EQ(*result0, 'U');
            EXPECT_EQ(chars, 2);
        }
        else ADD_FAILURE();
        if (result1) {
            EXPECT_EQ(*result1, 'U');
        }
        else ADD_FAILURE();
    }
    {
        // invalid char encountered
        size_t chars = size_t(-1);
        auto result0 = taul::parse_taul_char("\\U0010fffj", &chars);
        auto result1 = taul::parse_taul_char("\\U0010fffj");
        if (result0) {
            EXPECT_EQ(*result0, 'U');
            EXPECT_EQ(chars, 2);
        }
        else ADD_FAILURE();
        if (result1) {
            EXPECT_EQ(*result1, 'U');
        }
        else ADD_FAILURE();
    }
}

TEST(StringAndCharsetTests, ParseTAULChar_LiteralizedCharEscapeSeqs) {
    const auto lgr = taul::make_stderr_logger();
    for (const auto& I : "ABCDEF123456!@#$%^") {
        std::string in = std::format("\\{}", I);
        TAUL_LOG(lgr, "testing {}", in);
        size_t chars = size_t(-1);
        auto result0 = taul::parse_taul_char(in, &chars);
        auto result1 = taul::parse_taul_char(in);
        if (result0) {
            EXPECT_EQ(*result0, I);
            EXPECT_EQ(chars, 2);
        }
        else ADD_FAILURE();
        if (result1) {
            EXPECT_EQ(*result1, I);
        }
        else ADD_FAILURE();
    }
}

TEST(StringAndCharsetTests, ParseTAULChar_MultiByteUTF8) {
    const auto lgr = taul::make_stderr_logger();

    std::string input0 = taul::utf8_s(u8"Δa");
    std::string input1 = taul::utf8_s(u8"魂a");
    std::string input2 = taul::utf8_s(u8"💩a");
    std::string input3 = taul::utf8_s(u8"\\Δa");
    std::string input4 = taul::utf8_s(u8"\\魂a");
    std::string input5 = taul::utf8_s(u8"\\💩a");

    size_t chars0 = size_t(-1);
    size_t chars1 = size_t(-1);
    size_t chars2 = size_t(-1);
    size_t chars3 = size_t(-1);
    size_t chars4 = size_t(-1);
    size_t chars5 = size_t(-1);
    auto output0 = taul::parse_taul_char(input0, &chars0);
    auto output1 = taul::parse_taul_char(input1, &chars1);
    auto output2 = taul::parse_taul_char(input2, &chars2);
    auto output3 = taul::parse_taul_char(input3, &chars3);
    auto output4 = taul::parse_taul_char(input4, &chars4);
    auto output5 = taul::parse_taul_char(input5, &chars5);

    EXPECT_EQ(chars0, 2);
    EXPECT_EQ(chars1, 3);
    EXPECT_EQ(chars2, 4);
    EXPECT_EQ(chars3, 3);
    EXPECT_EQ(chars4, 4);
    EXPECT_EQ(chars5, 5);

    EXPECT_TRUE(output0);
    EXPECT_TRUE(output1);
    EXPECT_TRUE(output2);
    EXPECT_TRUE(output3);
    EXPECT_TRUE(output4);
    EXPECT_TRUE(output5);

    if (output0) EXPECT_EQ(output0.value(), U'Δ');
    if (output1) EXPECT_EQ(output1.value(), U'魂');
    if (output2) EXPECT_EQ(output2.value(), U'💩');
    if (output3) EXPECT_EQ(output3.value(), U'Δ');
    if (output4) EXPECT_EQ(output4.value(), U'魂');
    if (output5) EXPECT_EQ(output5.value(), U'💩');
}

TEST(StringAndCharsetTests, ParseTAULChar_AllowIllegalUnicode) {
    const auto lgr = taul::make_stderr_logger();

    std::string input0 = taul::utf8_s(u8"\\ud800");
    std::string input1 = taul::utf8_s(u8"\\udfff");
    std::string input2 = taul::utf8_s(u8"\\U00110000");
    std::string input3 = taul::utf8_s(u8"\\Uffffffff");

    size_t chars0 = size_t(-1);
    size_t chars1 = size_t(-1);
    size_t chars2 = size_t(-1);
    size_t chars3 = size_t(-1);
    auto output0 = taul::parse_taul_char(input0, &chars0);
    auto output1 = taul::parse_taul_char(input1, &chars1);
    auto output2 = taul::parse_taul_char(input2, &chars2);
    auto output3 = taul::parse_taul_char(input3, &chars3);

    EXPECT_EQ(chars0, 6);
    EXPECT_EQ(chars1, 6);
    EXPECT_EQ(chars2, 10);
    EXPECT_EQ(chars3, 10);

    EXPECT_TRUE(output0);
    EXPECT_TRUE(output1);
    EXPECT_TRUE(output2);
    EXPECT_TRUE(output3);

    if (output0) EXPECT_EQ(output0.value(), taul::unicode_t(0xd800));
    if (output1) EXPECT_EQ(output1.value(), taul::unicode_t(0xdfff));
    if (output2) EXPECT_EQ(output2.value(), taul::unicode_t(0x0011'0000));
    if (output3) EXPECT_EQ(output3.value(), taul::unicode_t(0xffff'ffff));
}

TEST(StringAndCharsetTests, FmtTAULChar) {
    const auto lgr = taul::make_stderr_logger();
    // this broke when I did 'c <= (char)127', so ya...
    for (taul::unicode_t c = (taul::unicode_t)0; c < (taul::unicode_t)127; c++) {
        TAUL_LOG(lgr, "testing (c == {})", (size_t)c);
        TAUL_LOG(lgr, "taul::fmt_taul_char(c) == {}", taul::fmt_taul_char(c));
        TAUL_LOG(lgr, "taul::fmt_taul_char(c, false) == {}", taul::fmt_taul_char(c, false));
        if (c == '\0') {
            EXPECT_EQ(taul::fmt_taul_char(c), "\\0");
            EXPECT_EQ(taul::fmt_taul_char(c, false), "\\0");
        }
        else if (c == '\a') {
            EXPECT_EQ(taul::fmt_taul_char(c), "\\a");
            EXPECT_EQ(taul::fmt_taul_char(c, false), "\\a");
        }
        else if (c == '\b') {
            EXPECT_EQ(taul::fmt_taul_char(c), "\\b");
            EXPECT_EQ(taul::fmt_taul_char(c, false), "\\b");
        }
        else if (c == '\f') {
            EXPECT_EQ(taul::fmt_taul_char(c), "\\f");
            EXPECT_EQ(taul::fmt_taul_char(c, false), "\\f");
        }
        else if (c == '\n') {
            EXPECT_EQ(taul::fmt_taul_char(c), "\\n");
            EXPECT_EQ(taul::fmt_taul_char(c, false), "\\n");
        }
        else if (c == '\r') {
            EXPECT_EQ(taul::fmt_taul_char(c), "\\r");
            EXPECT_EQ(taul::fmt_taul_char(c, false), "\\r");
        }
        else if (c == '\t') {
            EXPECT_EQ(taul::fmt_taul_char(c), "\\t");
            EXPECT_EQ(taul::fmt_taul_char(c, false), "\\t");
        }
        else if (c == '\v') {
            EXPECT_EQ(taul::fmt_taul_char(c), "\\v");
            EXPECT_EQ(taul::fmt_taul_char(c, false), "\\v");
        }
        else if (c == '\'') {
            EXPECT_EQ(taul::fmt_taul_char(c), "\\'");
            EXPECT_EQ(taul::fmt_taul_char(c, false), "'");
        }
        else if (c == ']') {
            EXPECT_EQ(taul::fmt_taul_char(c), "]");
            EXPECT_EQ(taul::fmt_taul_char(c, false), "\\]");
        }
        else if (c == '-') {
            EXPECT_EQ(taul::fmt_taul_char(c), "-");
            EXPECT_EQ(taul::fmt_taul_char(c, false), "\\-");
        }
        else if (c == '\\') {
            EXPECT_EQ(taul::fmt_taul_char(c), "\\\\");
            EXPECT_EQ(taul::fmt_taul_char(c, false), "\\\\");
        }
        else if (taul::is_visible_ascii(c)) {
            EXPECT_EQ(taul::fmt_taul_char(c), std::string(1, char(c)));
            EXPECT_EQ(taul::fmt_taul_char(c, false), std::string(1, char(c)));
        }
        else {
            EXPECT_EQ(taul::fmt_taul_char(c), std::format("\\x{}", taul::fmt_hex_u8(uint8_t(c))));
            EXPECT_EQ(taul::fmt_taul_char(c, false), std::format("\\x{}", taul::fmt_hex_u8(uint8_t(c))));
        }
    }
}

TEST(StringAndCharsetTests, FmtTAULChar_MultiByteUTF8) {
    const auto lgr = taul::make_stderr_logger();
    // w/ codepoints which fit in 16-bit range
    for (size_t c = 1; c < 127; c++) { // <- take note that we *skip* c == 0, as that's not in desired range
        taul::unicode_t cc = taul::unicode_t(c * 0x0100);
        TAUL_LOG(lgr, "testing (cc == {})", (size_t)cc);
        TAUL_LOG(lgr, "taul::fmt_taul_char(cc) == {}", taul::fmt_taul_char(cc));
        TAUL_LOG(lgr, "taul::fmt_taul_char(cc, false) == {}", taul::fmt_taul_char(cc, false));
        EXPECT_EQ(taul::fmt_taul_char(cc), std::format("\\u{}", taul::fmt_hex_u16(uint16_t(cc))));
        EXPECT_EQ(taul::fmt_taul_char(cc, false), std::format("\\u{}", taul::fmt_hex_u16(uint16_t(cc))));
    }
    // w/ codepoints which fit in 32-bit range
    for (size_t c = 1; c < 127; c++) { // <- take note that we *skip* c == 0, as that's not in desired range
        taul::unicode_t cc = taul::unicode_t(c * 0x0101'0000);
        TAUL_LOG(lgr, "testing (cc == {})", (size_t)cc);
        TAUL_LOG(lgr, "taul::fmt_taul_char(cc) == {}", taul::fmt_taul_char(cc));
        TAUL_LOG(lgr, "taul::fmt_taul_char(cc, false) == {}", taul::fmt_taul_char(cc, false));
        EXPECT_EQ(taul::fmt_taul_char(cc), std::format("\\U{}", taul::fmt_hex_u32(uint32_t(cc))));
        EXPECT_EQ(taul::fmt_taul_char(cc, false), std::format("\\U{}", taul::fmt_hex_u32(uint32_t(cc))));
    }
    // test 16-bit and 32-bit w/ specific codepoints
    // 16-bit
    EXPECT_EQ(taul::fmt_taul_char(U'Δ'), "\\u0394");
    EXPECT_EQ(taul::fmt_taul_char(U'Δ', false), "\\u0394");
    EXPECT_EQ(taul::fmt_taul_char(U'魂'), "\\u9b42");
    EXPECT_EQ(taul::fmt_taul_char(U'魂', false), "\\u9b42");
    // 32-bit
    EXPECT_EQ(taul::fmt_taul_char(U'💩'), "\\U0001f4a9");
    EXPECT_EQ(taul::fmt_taul_char(U'💩', false), "\\U0001f4a9");
}

TEST(StringAndCharsetTests, FmtTAULChar_AllowIllegalUnicode) {
    const auto lgr = taul::make_stderr_logger();
    EXPECT_EQ(taul::fmt_taul_char(taul::unicode_t(0xd800)), "\\ud800");
    EXPECT_EQ(taul::fmt_taul_char(taul::unicode_t(0xd800), false), "\\ud800");
    EXPECT_EQ(taul::fmt_taul_char(taul::unicode_t(0xdfff)), "\\udfff");
    EXPECT_EQ(taul::fmt_taul_char(taul::unicode_t(0xdfff), false), "\\udfff");
    EXPECT_EQ(taul::fmt_taul_char(taul::unicode_t(0xffff'ffff)), "\\Uffffffff");
    EXPECT_EQ(taul::fmt_taul_char(taul::unicode_t(0xffff'ffff), false), "\\Uffffffff");
    EXPECT_EQ(taul::fmt_taul_char(taul::unicode_t(0xffff'ffff)), "\\Uffffffff");
    EXPECT_EQ(taul::fmt_taul_char(taul::unicode_t(0xffff'ffff), false), "\\Uffffffff");
}

TEST(StringAndCharsetTests, ParseTAULString_BasicEscapeSeqs) {
    EXPECT_EQ(taul::parse_taul_string(""), U"");
    EXPECT_EQ(taul::parse_taul_string("abc123!@#"), U"abc123!@#");
    EXPECT_EQ(taul::parse_taul_string("\\a\\b\\f\\n\\r\\t\\v\\']-\\\\"), U"\a\b\f\n\r\t\v\']-\\");
    EXPECT_EQ(taul::parse_taul_string("\\A\\B\\C\\1\\2\\3\\!\\@\\#\\ \\\t"), U"ABC123!@# \t");
    EXPECT_EQ(taul::parse_taul_string("\\x1e\\x1f\\x7f"), U"\x1e\x1f\x7f");
    EXPECT_EQ(taul::parse_taul_string("'''"), U"'''"); // <- give '\'' no special treatment
    EXPECT_EQ(taul::parse_taul_string("abc\\"), U"abc\\"); // <- give trailing unescaped '\\' no special treatment
}

TEST(StringAndCharsetTests, ParseTAULString_StrayNulls) {
    const char32_t* txt = U"abc\0def";
    EXPECT_EQ(taul::parse_taul_string("abc\\0def"), std::u32string(txt, 7));
}

TEST(StringAndCharsetTests, ParseTAULString_MultiByteUTF8) {
    std::string input = taul::utf8_s(u8"abcΔ魂💩\\Δ\\魂\\💩");
    std::u32string output = U"abcΔ魂💩Δ魂💩";
    EXPECT_EQ(taul::parse_taul_string(input), output);
}

TEST(StringAndCharsetTests, ParseTAULString_AllowIllegalUnicode) {
    std::string input0 = taul::utf8_s(u8"ab\\ud800cd");
    std::string input1 = taul::utf8_s(u8"ab\\udfffcd");
    std::string input2 = taul::utf8_s(u8"ab\\U00110000cd");
    std::string input3 = taul::utf8_s(u8"ab\\Uffffffffcd");
    
    std::u32string output0 = std::u32string(U"ab") + taul::unicode_t(0xd800) + U"cd";
    std::u32string output1 = std::u32string(U"ab") + taul::unicode_t(0xdfff) + U"cd";
    std::u32string output2 = std::u32string(U"ab") + taul::unicode_t(0x0011'0000) + U"cd";
    std::u32string output3 = std::u32string(U"ab") + taul::unicode_t(0xffff'ffff) + U"cd";

    EXPECT_EQ(taul::parse_taul_string(input0), output0);
    EXPECT_EQ(taul::parse_taul_string(input1), output1);
    EXPECT_EQ(taul::parse_taul_string(input2), output2);
    EXPECT_EQ(taul::parse_taul_string(input3), output3);
}

TEST(StringAndCharsetTests, FmtTAULString) {
    EXPECT_EQ(taul::fmt_taul_string(U""), "");
    EXPECT_EQ(taul::fmt_taul_string(U"abc123!@#"), "abc123!@#");
    EXPECT_EQ(taul::fmt_taul_string(U"\a\b\f\n\r\t\v\']-\\"), "\\a\\b\\f\\n\\r\\t\\v\\']-\\\\");
    EXPECT_EQ(taul::fmt_taul_string(U"ABC123!@# \t"), "ABC123!@# \\t");
    EXPECT_EQ(taul::fmt_taul_string(U"\x1e\x1f\x7f"), "\\x1e\\x1f\\x7f");
}

TEST(StringAndCharsetTests, FmtTAULString_StrayNulls) {
    const char32_t* txt = U"abc\0def";
    EXPECT_EQ(taul::fmt_taul_string(std::u32string_view(txt, 7)), "abc\\0def");
}

TEST(StringAndCharsetTests, FmtTAULString_MultiByteUTF8) {
    EXPECT_EQ(taul::fmt_taul_string(U"Δ魂💩"), "\\u0394\\u9b42\\U0001f4a9");
}

TEST(StringAndCharsetTests, FmtTAULString_AllowIllegalUnicode) {
    std::u32string input0 = std::u32string(U"ab") + taul::unicode_t(0xd800) + U"cd";
    std::u32string input1 = std::u32string(U"ab") + taul::unicode_t(0xdfff) + U"cd";
    std::u32string input2 = std::u32string(U"ab") + taul::unicode_t(0x0011'0000) + U"cd";
    std::u32string input3 = std::u32string(U"ab") + taul::unicode_t(0xffff'ffff) + U"cd";

    std::string output0 = taul::utf8_s(u8"ab\\ud800cd");
    std::string output1 = taul::utf8_s(u8"ab\\udfffcd");
    std::string output2 = taul::utf8_s(u8"ab\\U00110000cd");
    std::string output3 = taul::utf8_s(u8"ab\\Uffffffffcd");

    EXPECT_EQ(taul::fmt_taul_string(input0), output0);
    EXPECT_EQ(taul::fmt_taul_string(input1), output1);
    EXPECT_EQ(taul::fmt_taul_string(input2), output2);
    EXPECT_EQ(taul::fmt_taul_string(input3), output3);
}

// IMPORTANT: charset_has_only_expected will give potential false-negatives for 
//            situations where charset contains multiple entries which describe
//            the same value over-and-over
//
//            this shouldn't get in the way of testing though, but be sure to
//            thusly write tests such that these false-negatives are avoided

bool charset_has_only_expected(
    std::u32string_view charset,
    std::u32string_view expected_char_list) noexcept {
    // measure exact size of charset w/ regards to how many codepoints it contains
    size_t codepoints = 0;
    for (size_t i = 0; i < charset.size(); i += 2) {
        codepoints += (charset[i + 1] - charset[i]) + 1;
    }
    // assert that charset must contain exactly the number expected of it
    if (codepoints != expected_char_list.length()) return false;
    // check that charset's contents is EXACTLY as expected
    for (const auto& I : expected_char_list) {
        if (!taul::in_charset_str(I, charset)) return false;
    }
    return true;
}

// this test depends upon taul::in_charset_str working as expected

TEST(StringAndCharsetTests, ParseTAULCharset) {
    const auto lgr = taul::make_stderr_logger();
    auto charset0 = taul::parse_taul_charset("");
    auto charset1 = taul::parse_taul_charset("abc");
    auto charset2 = taul::parse_taul_charset("a-f");
    auto charset3 = taul::parse_taul_charset("1a-c2x-z3");
    auto charset4 = taul::parse_taul_charset("f-a");
    auto charset5 = taul::parse_taul_charset("a\\-c");
    auto charset6 = taul::parse_taul_charset("-a-c");
    auto charset7 = taul::parse_taul_charset("a-c-");
    TAUL_LOG(lgr, "charset0 == {}", taul::fmt_taul_charset(charset0));
    TAUL_LOG(lgr, "charset1 == {}", taul::fmt_taul_charset(charset1));
    TAUL_LOG(lgr, "charset2 == {}", taul::fmt_taul_charset(charset2));
    TAUL_LOG(lgr, "charset3 == {}", taul::fmt_taul_charset(charset3));
    TAUL_LOG(lgr, "charset4 == {}", taul::fmt_taul_charset(charset4));
    TAUL_LOG(lgr, "charset5 == {}", taul::fmt_taul_charset(charset5));
    TAUL_LOG(lgr, "charset6 == {}", taul::fmt_taul_charset(charset6));
    TAUL_LOG(lgr, "charset7 == {}", taul::fmt_taul_charset(charset7));
    EXPECT_TRUE(charset_has_only_expected(charset0, U""));
    EXPECT_TRUE(charset_has_only_expected(charset1, U"abc"));
    EXPECT_TRUE(charset_has_only_expected(charset2, U"abcdef"));
    EXPECT_TRUE(charset_has_only_expected(charset3, U"123abcxyz"));
    EXPECT_TRUE(charset_has_only_expected(charset4, U"abcdef"));
    EXPECT_TRUE(charset_has_only_expected(charset5, U"ac-"));
    EXPECT_TRUE(charset_has_only_expected(charset6, U"abc-"));
    EXPECT_TRUE(charset_has_only_expected(charset7, U"abc-"));
    // test w/ escaped chars
    auto charset8 = taul::parse_taul_charset("\\A\\B\\C");
    auto charset9 = taul::parse_taul_charset("\\A-\\F");
    auto charset10 = taul::parse_taul_charset("\\F-\\A");
    auto charset11 = taul::parse_taul_charset("[-\\]");
    auto charset12 = taul::parse_taul_charset("+-\\-");
    auto charset13 = taul::parse_taul_charset("\\--/");
    auto charset14 = taul::parse_taul_charset("a+--b"); // <- right '-' exempt from needing to be escaped
    auto charset15 = taul::parse_taul_charset("a\\x2dc"); // <- \x2d (ie. '-') does not form char range
    auto charset16 = taul::parse_taul_charset("a\\x2Dc"); // <- \x2D (ie. '-') does not form char range
    auto charset17 = taul::parse_taul_charset("\\x1e\\x1f\\x7f");
    TAUL_LOG(lgr, "charset8 == {}", taul::fmt_taul_charset(charset8));
    TAUL_LOG(lgr, "charset9 == {}", taul::fmt_taul_charset(charset9));
    TAUL_LOG(lgr, "charset10 == {}", taul::fmt_taul_charset(charset10));
    TAUL_LOG(lgr, "charset11 == {}", taul::fmt_taul_charset(charset11));
    TAUL_LOG(lgr, "charset12 == {}", taul::fmt_taul_charset(charset12));
    TAUL_LOG(lgr, "charset13 == {}", taul::fmt_taul_charset(charset13));
    TAUL_LOG(lgr, "charset14 == {}", taul::fmt_taul_charset(charset14));
    TAUL_LOG(lgr, "charset15 == {}", taul::fmt_taul_charset(charset15));
    TAUL_LOG(lgr, "charset16 == {}", taul::fmt_taul_charset(charset16));
    TAUL_LOG(lgr, "charset17 == {}", taul::fmt_taul_charset(charset17));
    EXPECT_TRUE(charset_has_only_expected(charset8, U"ABC"));
    EXPECT_TRUE(charset_has_only_expected(charset9, U"ABCDEF"));
    EXPECT_TRUE(charset_has_only_expected(charset10, U"ABCDEF"));
    EXPECT_TRUE(charset_has_only_expected(charset11, U"[\\]"));
    EXPECT_TRUE(charset_has_only_expected(charset12, U"+,-"));
    EXPECT_TRUE(charset_has_only_expected(charset13, U"-./"));
    EXPECT_TRUE(charset_has_only_expected(charset14, U"a+,-b"));
    EXPECT_TRUE(charset_has_only_expected(charset15, U"a-c"));
    EXPECT_TRUE(charset_has_only_expected(charset16, U"a-c"));
    EXPECT_TRUE(charset_has_only_expected(charset17, U"\x1e\x1f\x7f"));
}

TEST(StringAndCharsetTests, ParseTAULCharset_StrayNulls) {
    const auto lgr = taul::make_stderr_logger();
    const char32_t* txt0 = U"abc\0def";
    const char32_t* txt1 = U"\0\x01\x02\x03\x04";
    auto charset0 = taul::parse_taul_charset("abc\\0def");
    auto charset1 = taul::parse_taul_charset("\\0-\\x04");
    TAUL_LOG(lgr, "charset0 == {} (length == {}; since we can't see the \\0, lol)", taul::fmt_taul_charset(charset0), charset0.length());
    TAUL_LOG(lgr, "charset1 == {} (length == {}; since we can't see the \\0, lol)", taul::fmt_taul_charset(charset1), charset1.length());
    EXPECT_TRUE(charset_has_only_expected(charset0, std::u32string_view(txt0, 7)));
    EXPECT_TRUE(charset_has_only_expected(charset1, std::u32string_view(txt1, 5)));
}

TEST(StringAndCharsetTests, ParseTAULCharset_MultiByteUTF8) {
    const auto lgr = taul::make_stderr_logger();
    auto charset0 = taul::parse_taul_charset(taul::utf8_s(u8"\\α-\\δ\\魂\\か-\\く"));
    auto charset1 = taul::parse_taul_charset(taul::utf8_s(u8"α-δ魂か-く"));
    TAUL_LOG(lgr, "charset0 == {}", taul::fmt_taul_charset(charset0));
    TAUL_LOG(lgr, "charset1 == {}", taul::fmt_taul_charset(charset1));
    EXPECT_TRUE(charset_has_only_expected(charset0, U"αβγδ魂かがきぎく"));
    EXPECT_TRUE(charset_has_only_expected(charset1, U"αβγδ魂かがきぎく"));
}

TEST(StringAndCharsetTests, ParseTAULCharset_AllowIllegalUnicode) {
    const auto lgr = taul::make_stderr_logger();

    std::u32string charset = taul::parse_taul_charset(taul::utf8_s(u8"a-f\\ud800-\\ud803\\udfff\\Uffffffff123"));

    std::u32string expected =
        std::u32string(U"abcdef") +
        taul::unicode_t(0xd800) +
        taul::unicode_t(0xd801) +
        taul::unicode_t(0xd802) +
        taul::unicode_t(0xd803) +
        taul::unicode_t(0xdfff) +
        taul::unicode_t(0xffff'ffff) +
        U"123";

    TAUL_LOG(lgr, "charset == {}", taul::fmt_taul_charset(charset));
    TAUL_LOG(lgr, "charset == {} (as string)", taul::fmt_taul_string(charset));
    TAUL_LOG(lgr, "expected == {}", taul::fmt_taul_string(expected));

    EXPECT_TRUE(charset_has_only_expected(charset, expected));
}

TEST(StringAndCharsetTests, ParseTAULCharset_Other) {
    const auto lgr = taul::make_stderr_logger();
    auto charset0 = taul::parse_taul_charset("]"); // <- give '\'' no special treatment
    auto charset1 = taul::parse_taul_charset("abc\\"); // <- give trailing unescaped '\\' no special treatment
    TAUL_LOG(lgr, "charset0 == {}", taul::fmt_taul_charset(charset0));
    TAUL_LOG(lgr, "charset1 == {}", taul::fmt_taul_charset(charset1));
    EXPECT_TRUE(charset_has_only_expected(charset0, U"]"));
    EXPECT_TRUE(charset_has_only_expected(charset1, U"abc\\"));
}

TEST(StringAndCharsetTests, FmtTAULCharset) {
    EXPECT_EQ(taul::fmt_taul_charset(U""), "");
    EXPECT_EQ(taul::fmt_taul_charset(U"aabbcc"), "abc");
    EXPECT_EQ(taul::fmt_taul_charset(U"ac11xz"), "a-c1x-z");
    EXPECT_EQ(taul::fmt_taul_charset(U"aa--cc"), "a\\-c");
    EXPECT_EQ(taul::fmt_taul_charset(U"--"), "-");
    EXPECT_EQ(taul::fmt_taul_charset(U"----"), "--");
    EXPECT_EQ(taul::fmt_taul_charset(U"--aa--"), "-a-");
    EXPECT_EQ(taul::fmt_taul_charset(U"\x1e\x1e\x1f\x1f\x7f\x7f"), "\\x1e\\x1f\\x7f");
    EXPECT_EQ(taul::fmt_taul_charset(U"\x1e\x7f"), "\\x1e-\\x7f");
    EXPECT_EQ(taul::fmt_taul_charset(U"\n\r"), "\\n-\\r");
}

TEST(StringAndCharsetTests, FmtTAULCharset_AllBasicEscapeSeqs_AndStrayNulls) {
    const char32_t* txt = U"\0\0\a\a\b\b\f\f\n\n\r\r\t\t\v\v'']]--\\\\";
    EXPECT_EQ(taul::fmt_taul_charset(std::u32string_view(txt, 24)), "\\0\\a\\b\\f\\n\\r\\t\\v'\\]\\-\\\\");
}

TEST(StringAndCharsetTests, FmtTAULCharset_MultiByteUTF8) {
    EXPECT_EQ(taul::fmt_taul_charset(U"afαδ魂魂かく"), "a-f\\u03b1-\\u03b4\\u9b42\\u304b-\\u304f");
}

TEST(StringAndCharsetTests, FmtTAULCharset_AllowIllegalUnicode) {
    std::u32string input =
        std::u32string(U"az") +
        taul::unicode_t(0xd800) +
        taul::unicode_t(0xdfff) +
        taul::unicode_t(0xffff'ffff) +
        taul::unicode_t(0xffff'ffff) +
        U"112233";

    std::string output = taul::utf8_s(u8"a-z\\ud800-\\udfff\\Uffffffff123");

    EXPECT_EQ(taul::fmt_taul_charset(input), output);
}

