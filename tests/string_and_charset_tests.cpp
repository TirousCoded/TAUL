

#include <gtest/gtest.h>

#include <taul/all.h>


TEST(StringAndCharsetTests, ParseTAULChar) {
    const auto lgr = taul::make_stderr_logger();
    /*
        \0              <- Null
        \a				<- Bell (Alert)
        \b				<- Backspace
        \f				<- Form Feed
        \n				<- New Line
        \r				<- Carriage Return
        \t				<- Horizontal Tab
        \v				<- Vertical Tab
        \'				<- Single Quotation
        \]				<- Right Square Bracket
        \-				<- Minus
        \\				<- Backslash
        \xhh			<- Hex Literal (8-bit)
    */
    // test first 12 escape seqs
    constexpr std::size_t esc_num = 12;
    constexpr const char* esc_ins = "0abfnrtv']-\\";
    constexpr const char* esc_outs = "\0\a\b\f\n\r\t\v\']-\\";
    for (std::size_t i = 0; i < esc_num; i++) {
        std::string in = std::format("\\{}", esc_ins[i]);
        TAUL_LOG(lgr, "testing {}", in);
        std::size_t chars = std::size_t(-1);
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
    // test last escape seq \xhh
    constexpr std::size_t hex_digit_num = 16;
    constexpr const char* upper_hex_digits = "0123456789ABCDEF";
    constexpr const char* lower_hex_digits = "0123456789abcdef";
    for (std::size_t i = 0; i < 8; i++) {
        for (std::size_t j = 0; j < hex_digit_num; j++) {
            // these iPick and jPick vars help us test each pair of
            // hex digits being uppercase/lowercase
            for (std::size_t iPick = 0; iPick < 2; iPick++) {
                for (std::size_t jPick = 0; jPick < 2; jPick++) {
                    char charA = (iPick == 0 ? upper_hex_digits : lower_hex_digits)[i];
                    char charB = (jPick == 0 ? upper_hex_digits : lower_hex_digits)[j];
                    std::string in = std::format("\\x{}{}", charA, charB);
                    std::size_t expected_out = i * hex_digit_num + j;
                    TAUL_LOG(lgr, "testing {}", in);
                    std::size_t chars = std::size_t(-1);
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
    // test last escape seq \xhh w/ invalid form (which results in literalized x)
    {
        std::size_t chars = std::size_t(-1);
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
        std::size_t chars = std::size_t(-1);
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
        std::size_t chars = std::size_t(-1);
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
    // test *literalized* char escape sequences
    for (const auto& I : "ABCDEF123456!@#$%^") {
        std::string in = std::format("\\{}", I);
        TAUL_LOG(lgr, "testing {}", in);
        std::size_t chars = std::size_t(-1);
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
    // test regular unescaped chars
    for (const auto& I : "abfnrtv']-ABCDEF123456!@#$%^\\") {
        std::string in = std::format("{}", I);
        TAUL_LOG(lgr, "testing {}", in);
        std::size_t chars = std::size_t(-1);
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
    // test expected failures due to empty input
    {
        std::size_t chars = std::size_t(-1);
        auto result0 = taul::parse_taul_char("", &chars);
        auto result1 = taul::parse_taul_char("");
        EXPECT_FALSE(result0);
        EXPECT_FALSE(result1);
        EXPECT_EQ(chars, std::size_t(-1));
    }
}

TEST(StringAndCharsetTests, FmtTAULChar) {
    const auto lgr = taul::make_stderr_logger();
    // this broke when I did 'c <= (char)127', so ya...
    for (char c = (char)0; c < (char)127; c++) {
        TAUL_LOG(lgr, "testing (c == {})", (std::size_t)c);
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
        else if (c >= ' ' && c <= '~') {
            EXPECT_EQ(taul::fmt_taul_char(c), std::string(1, c));
            EXPECT_EQ(taul::fmt_taul_char(c, false), std::string(1, c));
        }
        else {
            constexpr const char* hex_digits = "0123456789abcdef";
            std::size_t low = std::size_t(c) % 16;
            std::size_t high = (std::size_t(c) - low) / 16;
            EXPECT_EQ(taul::fmt_taul_char(c), std::format("\\x{}{}", hex_digits[high], hex_digits[low]));
            EXPECT_EQ(taul::fmt_taul_char(c, false), std::format("\\x{}{}", hex_digits[high], hex_digits[low]));
        }
    }
}

TEST(StringAndCharsetTests, ParseTAULString) {
    EXPECT_EQ(taul::parse_taul_string(""), "");
    EXPECT_EQ(taul::parse_taul_string("abc123!@#"), "abc123!@#");
    EXPECT_EQ(taul::parse_taul_string("\\a\\b\\f\\n\\r\\t\\v\\']-\\\\"), "\a\b\f\n\r\t\v\']-\\");
    EXPECT_EQ(taul::parse_taul_string("\\A\\B\\C\\1\\2\\3\\!\\@\\#\\ \\\t"), "ABC123!@# \t");
    EXPECT_EQ(taul::parse_taul_string("\\x1e\\x1f\\x7f"), "\x1e\x1f\x7f");
    EXPECT_EQ(taul::parse_taul_string("'''"), "'''"); // <- give '\'' no special treatment
    EXPECT_EQ(taul::parse_taul_string("abc\\"), "abc\\"); // <- give trailing unescaped '\\' no special treatment
    // ensure works w/ stray nulls
    const char* txt = "abc\0def";
    EXPECT_EQ(taul::parse_taul_string("abc\\0def"), std::string(txt, 7));
}

TEST(StringAndCharsetTests, FmtTAULString) {
    EXPECT_EQ(taul::fmt_taul_string(""), "");
    EXPECT_EQ(taul::fmt_taul_string("abc123!@#"), "abc123!@#");
    EXPECT_EQ(taul::fmt_taul_string("\a\b\f\n\r\t\v\']-\\"), "\\a\\b\\f\\n\\r\\t\\v\\']-\\\\");
    EXPECT_EQ(taul::fmt_taul_string("ABC123!@# \t"), "ABC123!@# \\t");
    EXPECT_EQ(taul::fmt_taul_string("\x1e\x1f\x7f"), "\\x1e\\x1f\\x7f");
    // ensure works w/ stray nulls
    const char* txt = "abc\0def";
    EXPECT_EQ(taul::fmt_taul_string(std::string_view(txt, 7)), "abc\\0def");
}

bool char_is_expected(
    char c,
    std::string_view expected) noexcept {
    for (const auto& I : expected) {
        if (I == c) {
            return true;
        }
    }
    return false;
}

bool charset_has_only_expected(
    std::string_view charset,
    std::string_view expected) noexcept {
    // this broke when I did 'c <= (char)127', so ya...
    for (char c = (char)0; c < (char)127; c++) {
        if (char_is_expected(c, expected) != taul::in_charset_str(c, charset)) {
            return false;
        }
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
    TAUL_LOG(lgr, "charset0 == {}", charset0);
    TAUL_LOG(lgr, "charset1 == {}", charset1);
    TAUL_LOG(lgr, "charset2 == {}", charset2);
    TAUL_LOG(lgr, "charset3 == {}", charset3);
    TAUL_LOG(lgr, "charset4 == {}", charset4);
    TAUL_LOG(lgr, "charset5 == {}", charset5);
    TAUL_LOG(lgr, "charset6 == {}", charset6);
    TAUL_LOG(lgr, "charset7 == {}", charset7);
    EXPECT_TRUE(charset_has_only_expected(charset0, ""));
    EXPECT_TRUE(charset_has_only_expected(charset1, "abc"));
    EXPECT_TRUE(charset_has_only_expected(charset2, "abcdef"));
    EXPECT_TRUE(charset_has_only_expected(charset3, "123abcxyz"));
    EXPECT_TRUE(charset_has_only_expected(charset4, "abcdef"));
    EXPECT_TRUE(charset_has_only_expected(charset5, "ac-"));
    EXPECT_TRUE(charset_has_only_expected(charset6, "abc-"));
    EXPECT_TRUE(charset_has_only_expected(charset7, "abc-"));
    // test w/ escaped chars
    auto charset8 = taul::parse_taul_charset("\\A\\B\\C");
    auto charset9 = taul::parse_taul_charset("\\A-\\F");
    auto charset10 = taul::parse_taul_charset("\\F-\\A");
    auto charset11 = taul::parse_taul_charset("[-\\]");
    auto charset12 = taul::parse_taul_charset("+-\\-");
    auto charset13 = taul::parse_taul_charset("\\--/");
    auto charset14 = taul::parse_taul_charset("a+--b"); // <- right '-' exempt from needing to be escaped
    auto charset15 = taul::parse_taul_charset("a\\x2dc"); // <- \x2d does not form char range
    auto charset16 = taul::parse_taul_charset("a\\x2Dc"); // <- \x2D does not form char range
    auto charset17 = taul::parse_taul_charset("\\x1e\\x1f\\x7f");
    TAUL_LOG(lgr, "charset8 == {}", charset8);
    TAUL_LOG(lgr, "charset9 == {}", charset9);
    TAUL_LOG(lgr, "charset10 == {}", charset10);
    TAUL_LOG(lgr, "charset11 == {}", charset11);
    TAUL_LOG(lgr, "charset12 == {}", charset12);
    TAUL_LOG(lgr, "charset13 == {}", charset13);
    TAUL_LOG(lgr, "charset14 == {}", charset14);
    TAUL_LOG(lgr, "charset15 == {}", charset15);
    TAUL_LOG(lgr, "charset16 == {}", charset16);
    TAUL_LOG(lgr, "charset17 == {}", charset17);
    EXPECT_TRUE(charset_has_only_expected(charset8, "ABC"));
    EXPECT_TRUE(charset_has_only_expected(charset9, "ABCDEF"));
    EXPECT_TRUE(charset_has_only_expected(charset10, "ABCDEF"));
    EXPECT_TRUE(charset_has_only_expected(charset11, "[\\]"));
    EXPECT_TRUE(charset_has_only_expected(charset12, "+,-"));
    EXPECT_TRUE(charset_has_only_expected(charset13, "-./"));
    EXPECT_TRUE(charset_has_only_expected(charset14, "a+,-b"));
    EXPECT_TRUE(charset_has_only_expected(charset15, "a-c"));
    EXPECT_TRUE(charset_has_only_expected(charset16, "a-c"));
    EXPECT_TRUE(charset_has_only_expected(charset17, "\x1e\x1f\x7f"));
    // ensure works w/ stray nulls
    const char* txt0 = "abc\0def";
    const char* txt1 = "\0\x01\x02\x03\x04";
    auto charset18 = taul::parse_taul_charset("abc\\0def");
    auto charset19 = taul::parse_taul_charset("\\0-\\x04");
    TAUL_LOG(lgr, "charset18 == {} (length == {}; since we can't see the \\0, lol)", charset18, charset18.length());
    TAUL_LOG(lgr, "charset19 == {} (length == {}; since we can't see the \\0, lol)", charset19, charset19.length());
    EXPECT_TRUE(charset_has_only_expected(charset18, std::string_view(txt0, 7)));
    EXPECT_TRUE(charset_has_only_expected(charset19, std::string_view(txt1, 5)));
    // others
    auto charset20 = taul::parse_taul_charset("]]]"); // <- give '\'' no special treatment
    auto charset21 = taul::parse_taul_charset("abc\\"); // <- give trailing unescaped '\\' no special treatment
    TAUL_LOG(lgr, "charset20 == {}", charset20);
    TAUL_LOG(lgr, "charset21 == {}", charset21);
    EXPECT_TRUE(charset_has_only_expected(charset20, "]"));
    EXPECT_TRUE(charset_has_only_expected(charset21, "abc\\"));
}

// these tests couple the impl a bit to a specific internal behaviour
// regarding the exact appearence of the result, but this should be fine

TEST(StringAndCharsetTests, FmtTAULCharset) {
    EXPECT_EQ(taul::fmt_taul_charset(""), "");
    EXPECT_EQ(taul::fmt_taul_charset("aabbcc"), "abc");
    EXPECT_EQ(taul::fmt_taul_charset("ac11xz"), "a-c1x-z");
    EXPECT_EQ(taul::fmt_taul_charset("aa--cc"), "a\\-c");
    EXPECT_EQ(taul::fmt_taul_charset("--"), "-");
    EXPECT_EQ(taul::fmt_taul_charset("----"), "--");
    EXPECT_EQ(taul::fmt_taul_charset("--aa--"), "-a-");
    EXPECT_EQ(taul::fmt_taul_charset("\x1e\x1e\x1f\x1f\x7f\x7f"), "\\x1e\\x1f\\x7f");
    EXPECT_EQ(taul::fmt_taul_charset("\x1e\x7f"), "\\x1e-\\x7f");
    EXPECT_EQ(taul::fmt_taul_charset("\n\r"), "\\n-\\r");
    // test w/ all our main escape seqs + ensure works w/ stray nulls
    const char* txt = "\0\0\a\a\b\b\f\f\n\n\r\r\t\t\v\v'']]--\\\\";
    EXPECT_EQ(taul::fmt_taul_charset(std::string_view(txt, 24)), "\\0\\a\\b\\f\\n\\r\\t\\v'\\]\\-\\\\");
}

TEST(StringAndCharsetTests, InCharsetStr) {
    const auto lgr = taul::make_stderr_logger();
    std::string s = "afAF26xzjj\n\r";
    // this broke when I did 'c <= (char)127', so ya...
    for (char c = (char)0; c < (char)127; c++) {
        TAUL_LOG(lgr, "testing (c == {})", (std::size_t)c);
        EXPECT_EQ(
            char_is_expected(c, "abcdefABCDEF23456xyzj\n\v\f\r"),
            taul::in_charset_str(c, s));
    }
    // ensure works w/ stray nulls
    const char* txt = "\0\x04";
    const char* txt_expected = "\0\x01\x02\x03\x04";
    for (char c = (char)0; c < (char)127; c++) {
        TAUL_LOG(lgr, "testing (c == {})", (std::size_t)c);
        EXPECT_EQ(
            char_is_expected(c, std::string_view(txt_expected, 5)),
            taul::in_charset_str(c, std::string_view(txt, 2)));
    }
}

// this test depends upon taul::in_charset_str working as expected

TEST(StringAndCharsetTests, OptimizeCharsetStr) {
    // also, ensure works w/ stray nulls
    const char* txt_input = "\000\00022aa!!13bf";
    const char* txt_expected = "\000123abcdef!";
    std::string original(txt_input, 12);
    ASSERT_TRUE(charset_has_only_expected(original, std::string_view(txt_expected, 11)));
    std::string optimized = taul::optimize_charset_str(original);
    EXPECT_LE(original.length(), optimized.length());
    ASSERT_TRUE(charset_has_only_expected(optimized, std::string_view(txt_expected, 11)));
}

