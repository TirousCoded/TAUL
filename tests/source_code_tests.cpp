

#include <gtest/gtest.h>

#include <fstream>

#include <taul/logger.h>
#include <taul/str.h>
#include <taul/source_code.h>


using namespace taul::string_literals;


TEST(SourceCodeTests, DefaultCtor) {
    taul::source_code sc{};

    EXPECT_TRUE(sc.str().empty());
    EXPECT_TRUE(sc.pages().empty());

    EXPECT_EQ(sc.page_at(0), 0);
    EXPECT_EQ(sc.page_at(1), 0);
    EXPECT_EQ(sc.page_at(taul::source_pos(-1)), 0);

    const auto loc0 = taul::source_location{ 0, ""_str, 1, 1 };

    EXPECT_EQ(sc.location_at(0).fmt(), loc0.fmt());
    EXPECT_EQ(sc.location_at(1).fmt(), loc0.fmt());
    EXPECT_EQ(sc.location_at(taul::source_pos(-1)).fmt(), loc0.fmt());
}

TEST(SourceCodeTests, MoveCtor) {
    taul::source_code old_sc{};
    old_sc.add_str("aa"_str, "abc"_str);
    old_sc.add_str("bb"_str, "def"_str);

    taul::source_code sc(std::move(old_sc));

    EXPECT_EQ(sc.str(), "abcdef"_str);

    if (sc.pages().size() == 2) {
        EXPECT_EQ(sc.pages()[0].pos, 0);
        EXPECT_EQ(sc.pages()[0].length, 3);
        EXPECT_EQ(sc.pages()[0].origin, "aa"_str);
        EXPECT_EQ(sc.pages()[1].pos, 3);
        EXPECT_EQ(sc.pages()[1].length, 3);
        EXPECT_EQ(sc.pages()[1].origin, "bb"_str);
    }
    else ADD_FAILURE();

    EXPECT_EQ(sc.page_at(0), 0);
    EXPECT_EQ(sc.page_at(1), 0);
    EXPECT_EQ(sc.page_at(2), 0);
    EXPECT_EQ(sc.page_at(3), 1);
    EXPECT_EQ(sc.page_at(4), 1);
    EXPECT_EQ(sc.page_at(5), 1);
    EXPECT_EQ(sc.page_at(6), 1);
    EXPECT_EQ(sc.page_at(taul::source_pos(-1)), 1);

    const auto loc0 = taul::source_location{ 0, "aa"_str, 1, 1 };
    const auto loc1 = taul::source_location{ 1, "aa"_str, 2, 1 };
    const auto loc2 = taul::source_location{ 2, "aa"_str, 3, 1 };
    const auto loc3 = taul::source_location{ 3, "bb"_str, 1, 1 };
    const auto loc4 = taul::source_location{ 4, "bb"_str, 2, 1 };
    const auto loc5 = taul::source_location{ 5, "bb"_str, 3, 1 };
    const auto loc6 = taul::source_location{ 6, "bb"_str, 4, 1 };

    EXPECT_EQ(sc.location_at(0).fmt(), loc0.fmt());
    EXPECT_EQ(sc.location_at(1).fmt(), loc1.fmt());
    EXPECT_EQ(sc.location_at(2).fmt(), loc2.fmt());
    EXPECT_EQ(sc.location_at(3).fmt(), loc3.fmt());
    EXPECT_EQ(sc.location_at(4).fmt(), loc4.fmt());
    EXPECT_EQ(sc.location_at(5).fmt(), loc5.fmt());
    EXPECT_EQ(sc.location_at(6).fmt(), loc6.fmt());
    EXPECT_EQ(sc.location_at(taul::source_pos(-1)).fmt(), loc6.fmt());
}

TEST(SourceCodeTests, MoveAssign) {
    taul::source_code old_sc{};
    old_sc.add_str("aa"_str, "abc"_str);
    old_sc.add_str("bb"_str, "def"_str);

    taul::source_code sc{};
    sc = std::move(old_sc);

    EXPECT_EQ(sc.str(), "abcdef"_str);

    if (sc.pages().size() == 2) {
        EXPECT_EQ(sc.pages()[0].pos, 0);
        EXPECT_EQ(sc.pages()[0].length, 3);
        EXPECT_EQ(sc.pages()[0].origin, "aa"_str);
        EXPECT_EQ(sc.pages()[1].pos, 3);
        EXPECT_EQ(sc.pages()[1].length, 3);
        EXPECT_EQ(sc.pages()[1].origin, "bb"_str);
    }
    else ADD_FAILURE();

    EXPECT_EQ(sc.page_at(0), 0);
    EXPECT_EQ(sc.page_at(1), 0);
    EXPECT_EQ(sc.page_at(2), 0);
    EXPECT_EQ(sc.page_at(3), 1);
    EXPECT_EQ(sc.page_at(4), 1);
    EXPECT_EQ(sc.page_at(5), 1);
    EXPECT_EQ(sc.page_at(6), 1);
    EXPECT_EQ(sc.page_at(taul::source_pos(-1)), 1);

    const auto loc0 = taul::source_location{ 0, "aa"_str, 1, 1 };
    const auto loc1 = taul::source_location{ 1, "aa"_str, 2, 1 };
    const auto loc2 = taul::source_location{ 2, "aa"_str, 3, 1 };
    const auto loc3 = taul::source_location{ 3, "bb"_str, 1, 1 };
    const auto loc4 = taul::source_location{ 4, "bb"_str, 2, 1 };
    const auto loc5 = taul::source_location{ 5, "bb"_str, 3, 1 };
    const auto loc6 = taul::source_location{ 6, "bb"_str, 4, 1 };

    EXPECT_EQ(sc.location_at(0).fmt(), loc0.fmt());
    EXPECT_EQ(sc.location_at(1).fmt(), loc1.fmt());
    EXPECT_EQ(sc.location_at(2).fmt(), loc2.fmt());
    EXPECT_EQ(sc.location_at(3).fmt(), loc3.fmt());
    EXPECT_EQ(sc.location_at(4).fmt(), loc4.fmt());
    EXPECT_EQ(sc.location_at(5).fmt(), loc5.fmt());
    EXPECT_EQ(sc.location_at(6).fmt(), loc6.fmt());
    EXPECT_EQ(sc.location_at(taul::source_pos(-1)).fmt(), loc6.fmt());
}

TEST(SourceCodeTests, MoveAssignOntoSelf) {
    taul::source_code sc{};
    sc.add_str("aa"_str, "abc"_str);
    sc.add_str("bb"_str, "def"_str);

    sc = std::move(sc); // <- self-assignment

    EXPECT_EQ(sc.str(), "abcdef"_str);

    if (sc.pages().size() == 2) {
        EXPECT_EQ(sc.pages()[0].pos, 0);
        EXPECT_EQ(sc.pages()[0].length, 3);
        EXPECT_EQ(sc.pages()[0].origin, "aa"_str);
        EXPECT_EQ(sc.pages()[1].pos, 3);
        EXPECT_EQ(sc.pages()[1].length, 3);
        EXPECT_EQ(sc.pages()[1].origin, "bb"_str);
    }
    else ADD_FAILURE();

    EXPECT_EQ(sc.page_at(0), 0);
    EXPECT_EQ(sc.page_at(1), 0);
    EXPECT_EQ(sc.page_at(2), 0);
    EXPECT_EQ(sc.page_at(3), 1);
    EXPECT_EQ(sc.page_at(4), 1);
    EXPECT_EQ(sc.page_at(5), 1);
    EXPECT_EQ(sc.page_at(6), 1);
    EXPECT_EQ(sc.page_at(taul::source_pos(-1)), 1);

    const auto loc0 = taul::source_location{ 0, "aa"_str, 1, 1 };
    const auto loc1 = taul::source_location{ 1, "aa"_str, 2, 1 };
    const auto loc2 = taul::source_location{ 2, "aa"_str, 3, 1 };
    const auto loc3 = taul::source_location{ 3, "bb"_str, 1, 1 };
    const auto loc4 = taul::source_location{ 4, "bb"_str, 2, 1 };
    const auto loc5 = taul::source_location{ 5, "bb"_str, 3, 1 };
    const auto loc6 = taul::source_location{ 6, "bb"_str, 4, 1 };

    EXPECT_EQ(sc.location_at(0).fmt(), loc0.fmt());
    EXPECT_EQ(sc.location_at(1).fmt(), loc1.fmt());
    EXPECT_EQ(sc.location_at(2).fmt(), loc2.fmt());
    EXPECT_EQ(sc.location_at(3).fmt(), loc3.fmt());
    EXPECT_EQ(sc.location_at(4).fmt(), loc4.fmt());
    EXPECT_EQ(sc.location_at(5).fmt(), loc5.fmt());
    EXPECT_EQ(sc.location_at(6).fmt(), loc6.fmt());
    EXPECT_EQ(sc.location_at(taul::source_pos(-1)).fmt(), loc6.fmt());
}

TEST(SourceCodeTests, ToFile) {
    auto path = std::filesystem::current_path() / "outputs\\to_file_helper_1.txt";

    std::filesystem::remove(path);
    ASSERT_FALSE(std::filesystem::exists(path));
    
    auto txt_1 = "abc123Δ魂💩"_str;
    auto txt_2 = "new text contents!"_str;

    taul::source_code src_1{};
    src_1.add_str("a"_str, txt_1);

    EXPECT_TRUE(src_1.to_file(path)); // creation of new file
    EXPECT_TRUE(std::filesystem::exists(path));

    // sample contents to see if it worked
    taul::source_code sample_1{};
    EXPECT_TRUE(sample_1.add_file(path));
    EXPECT_EQ(sample_1.str(), txt_1);

    taul::source_code src_2{};
    src_2.add_str("a"_str, txt_2);

    EXPECT_TRUE(src_2.to_file(path)); // overwriting of existing file
    EXPECT_TRUE(std::filesystem::exists(path));

    // sample contents to see if it worked
    taul::source_code sample_2{};
    EXPECT_TRUE(sample_2.add_file(path));
    EXPECT_EQ(sample_2.str(), txt_2);
}

TEST(SourceCodeTests, ToFile_FailDueToNoFilename) {
    auto path = std::filesystem::current_path() / "outputs\\";
    ASSERT_FALSE(path.has_filename());

    taul::source_code src{};
    EXPECT_FALSE(src.to_file(path));
}

TEST(SourceCodeTests, ToFile_DoNotTransformLFsIntoNativeNewlines) {
    // this is for making sure that, especially on Windows and macOS, which don't
    // use LF as their native newline, that the to_file impl doesn't mistakenly
    // output in a way that mangles the result due to the impl failing to output
    // as binary, rather than as text
    auto path = std::filesystem::current_path() / "outputs\\to_file_output_helper_2.txt";

    auto txt = "ab\rcd\r\nef\ngh"_str;

    std::filesystem::remove(path);
    ASSERT_FALSE(std::filesystem::exists(path));
    taul::source_code src{};
    src.add_str("a"_str, txt);
    EXPECT_TRUE(src.to_file(path)); // creation of new file
    EXPECT_TRUE(std::filesystem::exists(path));

    // sample contents to see if it worked
    taul::source_code sample{};
    EXPECT_TRUE(sample.add_file(path));
    EXPECT_EQ(sample.str(), txt);
}

// newlines within source string w/ them being tested seperately

TEST(SourceCodeTests, AddStr) {
    taul::source_code sc{};
    sc.add_str("aa"_str, "abc"_str);
    sc.add_str("bb"_str, "def"_str);
    sc.add_str("cc"_str, "ghi"_str);

    EXPECT_EQ(sc.str(), "abcdefghi"_str);

    if (sc.pages().size() == 3) {
        EXPECT_EQ(sc.pages()[0].pos, 0);
        EXPECT_EQ(sc.pages()[0].length, 3);
        EXPECT_EQ(sc.pages()[0].origin, "aa"_str);
        EXPECT_EQ(sc.pages()[1].pos, 3);
        EXPECT_EQ(sc.pages()[1].length, 3);
        EXPECT_EQ(sc.pages()[1].origin, "bb"_str);
        EXPECT_EQ(sc.pages()[2].pos, 6);
        EXPECT_EQ(sc.pages()[2].length, 3);
        EXPECT_EQ(sc.pages()[2].origin, "cc"_str);
    }
    else ADD_FAILURE();

    EXPECT_EQ(sc.page_at(0), 0);
    EXPECT_EQ(sc.page_at(1), 0);
    EXPECT_EQ(sc.page_at(2), 0);
    EXPECT_EQ(sc.page_at(3), 1);
    EXPECT_EQ(sc.page_at(4), 1);
    EXPECT_EQ(sc.page_at(5), 1);
    EXPECT_EQ(sc.page_at(6), 2);
    EXPECT_EQ(sc.page_at(7), 2);
    EXPECT_EQ(sc.page_at(8), 2);
    EXPECT_EQ(sc.page_at(9), 2);
    EXPECT_EQ(sc.page_at(taul::source_pos(-1)), 2);

    const auto loc0 = taul::source_location{ 0, "aa"_str, 1, 1 };
    const auto loc1 = taul::source_location{ 1, "aa"_str, 2, 1 };
    const auto loc2 = taul::source_location{ 2, "aa"_str, 3, 1 };
    const auto loc3 = taul::source_location{ 3, "bb"_str, 1, 1 };
    const auto loc4 = taul::source_location{ 4, "bb"_str, 2, 1 };
    const auto loc5 = taul::source_location{ 5, "bb"_str, 3, 1 };
    const auto loc6 = taul::source_location{ 6, "cc"_str, 1, 1 };
    const auto loc7 = taul::source_location{ 7, "cc"_str, 2, 1 };
    const auto loc8 = taul::source_location{ 8, "cc"_str, 3, 1 };
    const auto loc9 = taul::source_location{ 9, "cc"_str, 4, 1 };

    EXPECT_EQ(sc.location_at(0).fmt(), loc0.fmt());
    EXPECT_EQ(sc.location_at(1).fmt(), loc1.fmt());
    EXPECT_EQ(sc.location_at(2).fmt(), loc2.fmt());
    EXPECT_EQ(sc.location_at(3).fmt(), loc3.fmt());
    EXPECT_EQ(sc.location_at(4).fmt(), loc4.fmt());
    EXPECT_EQ(sc.location_at(5).fmt(), loc5.fmt());
    EXPECT_EQ(sc.location_at(6).fmt(), loc6.fmt());
    EXPECT_EQ(sc.location_at(7).fmt(), loc7.fmt());
    EXPECT_EQ(sc.location_at(8).fmt(), loc8.fmt());
    EXPECT_EQ(sc.location_at(9).fmt(), loc9.fmt());
    EXPECT_EQ(sc.location_at(taul::source_pos(-1)).fmt(), loc9.fmt());
}

TEST(SourceCodeTests, AddStr_WithNewlines) {
    taul::source_code sc{};
    sc.add_str("aa"_str, "ab\ncd\nef"_str);
    sc.add_str("bb"_str, "g\nhi"_str);
    // test that impl can properly handle mix of LF, CR, and CRLF newlines
    sc.add_str("cc"_str, "j\rkl\r\nmn\n\ro"_str);

    ASSERT_EQ(sc.str(), "ab\ncd\nefg\nhij\rkl\r\nmn\n\ro"_str);
    ASSERT_EQ(sc.pages().size(), 3);

    // page #1
    // ab\n
    const auto loc0 = taul::source_location{ 0, "aa"_str, 1, 1 };
    const auto loc1 = taul::source_location{ 1, "aa"_str, 2, 1 };
    const auto loc2 = taul::source_location{ 2, "aa"_str, 3, 1 };
    // cd\n
    const auto loc3 = taul::source_location{ 3, "aa"_str, 1, 2 };
    const auto loc4 = taul::source_location{ 4, "aa"_str, 2, 2 };
    const auto loc5 = taul::source_location{ 5, "aa"_str, 3, 2 };
    // ef
    const auto loc6 = taul::source_location{ 6, "aa"_str, 1, 3 };
    const auto loc7 = taul::source_location{ 7, "aa"_str, 2, 3 };
    // page #2
    // g\n
    const auto loc8 = taul::source_location{ 8, "bb"_str, 1, 1 };
    const auto loc9 = taul::source_location{ 9, "bb"_str, 2, 1 };
    // hi
    const auto loc10 = taul::source_location{ 10, "bb"_str, 1, 2 };
    const auto loc11 = taul::source_location{ 11, "bb"_str, 2, 2 };
    // page #3
    // j\r
    const auto loc12 = taul::source_location{ 12, "cc"_str, 1, 1 };
    const auto loc13 = taul::source_location{ 13, "cc"_str, 2, 1 };
    // kl\r\n
    const auto loc14 = taul::source_location{ 14, "cc"_str, 1, 2 };
    const auto loc15 = taul::source_location{ 15, "cc"_str, 2, 2 };
    const auto loc16 = taul::source_location{ 16, "cc"_str, 3, 2 };
    const auto loc17 = taul::source_location{ 17, "cc"_str, 4, 2 };
    // mn\n
    const auto loc18 = taul::source_location{ 18, "cc"_str, 1, 3 };
    const auto loc19 = taul::source_location{ 19, "cc"_str, 2, 3 };
    const auto loc20 = taul::source_location{ 20, "cc"_str, 3, 3 };
    // \r
    const auto loc21 = taul::source_location{ 21, "cc"_str, 1, 4 };
    // o
    const auto loc22 = taul::source_location{ 22, "cc"_str, 1, 5 };
    const auto loc23 = taul::source_location{ 23, "cc"_str, 2, 5 };

    EXPECT_EQ(sc.location_at(0).fmt(), loc0.fmt());
    EXPECT_EQ(sc.location_at(1).fmt(), loc1.fmt());
    EXPECT_EQ(sc.location_at(2).fmt(), loc2.fmt());
    EXPECT_EQ(sc.location_at(3).fmt(), loc3.fmt());
    EXPECT_EQ(sc.location_at(4).fmt(), loc4.fmt());
    EXPECT_EQ(sc.location_at(5).fmt(), loc5.fmt());
    EXPECT_EQ(sc.location_at(6).fmt(), loc6.fmt());
    EXPECT_EQ(sc.location_at(7).fmt(), loc7.fmt());
    EXPECT_EQ(sc.location_at(8).fmt(), loc8.fmt());
    EXPECT_EQ(sc.location_at(9).fmt(), loc9.fmt());
    EXPECT_EQ(sc.location_at(10).fmt(), loc10.fmt());
    EXPECT_EQ(sc.location_at(11).fmt(), loc11.fmt());
    EXPECT_EQ(sc.location_at(12).fmt(), loc12.fmt());
    EXPECT_EQ(sc.location_at(13).fmt(), loc13.fmt());
    EXPECT_EQ(sc.location_at(14).fmt(), loc14.fmt());
    EXPECT_EQ(sc.location_at(15).fmt(), loc15.fmt());
    EXPECT_EQ(sc.location_at(16).fmt(), loc16.fmt());
    EXPECT_EQ(sc.location_at(17).fmt(), loc17.fmt());
    EXPECT_EQ(sc.location_at(18).fmt(), loc18.fmt());
    EXPECT_EQ(sc.location_at(19).fmt(), loc19.fmt());
    EXPECT_EQ(sc.location_at(20).fmt(), loc20.fmt());
    EXPECT_EQ(sc.location_at(21).fmt(), loc21.fmt());
    EXPECT_EQ(sc.location_at(22).fmt(), loc22.fmt());
    EXPECT_EQ(sc.location_at(23).fmt(), loc23.fmt());
    EXPECT_EQ(sc.location_at(taul::source_pos(-1)).fmt(), loc23.fmt());
}

TEST(SourceCodeTests, AddStr_WithMultiByteUTF8) {
    // encountered an issues where our impl counted each UTF-8 code
    // segment as a seperate char when computing char positions, so
    // this is meant to address that issue

    taul::source_code sc{};
    sc.add_str("aa"_str, taul::str(taul::utf8_s(u8"元気\r\nですか")));

    ASSERT_EQ(sc.str(), taul::str(taul::utf8_s(u8"元気\r\nですか")));
    ASSERT_EQ(sc.pages().size(), 1);

    // 元気\r\n
    const auto loc0 = taul::source_location{ 0, "aa"_str, 1, 1 };
    const auto loc1 = taul::source_location{ 1, "aa"_str, 1, 1 };
    const auto loc2 = taul::source_location{ 2, "aa"_str, 1, 1 };
    const auto loc3 = taul::source_location{ 3, "aa"_str, 2, 1 };
    const auto loc4 = taul::source_location{ 4, "aa"_str, 2, 1 };
    const auto loc5 = taul::source_location{ 5, "aa"_str, 2, 1 };
    const auto loc6 = taul::source_location{ 6, "aa"_str, 3, 1 };
    const auto loc7 = taul::source_location{ 7, "aa"_str, 4, 1 };
    // ですか
    const auto loc8 = taul::source_location{ 8, "aa"_str, 1, 2 };
    const auto loc9 = taul::source_location{ 9, "aa"_str, 1, 2 };
    const auto loc10 = taul::source_location{ 10, "aa"_str, 1, 2 };
    const auto loc11 = taul::source_location{ 11, "aa"_str, 2, 2 };
    const auto loc12 = taul::source_location{ 12, "aa"_str, 2, 2 };
    const auto loc13 = taul::source_location{ 13, "aa"_str, 2, 2 };
    const auto loc14 = taul::source_location{ 14, "aa"_str, 3, 2 };
    const auto loc15 = taul::source_location{ 15, "aa"_str, 3, 2 };
    const auto loc16 = taul::source_location{ 16, "aa"_str, 3, 2 };
    const auto loc17 = taul::source_location{ 17, "aa"_str, 4, 2 };

    EXPECT_EQ(sc.location_at(0).fmt(), loc0.fmt());
    EXPECT_EQ(sc.location_at(1).fmt(), loc1.fmt());
    EXPECT_EQ(sc.location_at(2).fmt(), loc2.fmt());
    EXPECT_EQ(sc.location_at(3).fmt(), loc3.fmt());
    EXPECT_EQ(sc.location_at(4).fmt(), loc4.fmt());
    EXPECT_EQ(sc.location_at(5).fmt(), loc5.fmt());
    EXPECT_EQ(sc.location_at(6).fmt(), loc6.fmt());
    EXPECT_EQ(sc.location_at(7).fmt(), loc7.fmt());
    EXPECT_EQ(sc.location_at(8).fmt(), loc8.fmt());
    EXPECT_EQ(sc.location_at(9).fmt(), loc9.fmt());
    EXPECT_EQ(sc.location_at(10).fmt(), loc10.fmt());
    EXPECT_EQ(sc.location_at(11).fmt(), loc11.fmt());
    EXPECT_EQ(sc.location_at(12).fmt(), loc12.fmt());
    EXPECT_EQ(sc.location_at(13).fmt(), loc13.fmt());
    EXPECT_EQ(sc.location_at(14).fmt(), loc14.fmt());
    EXPECT_EQ(sc.location_at(15).fmt(), loc15.fmt());
    EXPECT_EQ(sc.location_at(16).fmt(), loc16.fmt());
    EXPECT_EQ(sc.location_at(17).fmt(), loc17.fmt());
    EXPECT_EQ(sc.location_at(taul::source_pos(-1)).fmt(), loc17.fmt());
}

TEST(SourceCodeTests, AddStr_InputEncodingNotUTF8) {
    // these tests largely presume that TAUL's encoding conversion stuff
    // is used under the hood

    auto input_utf32_bom = taul::convert_encoding<char, char8_t>(taul::utf8, taul::utf32_bom, u8"abc123Δ魂💩");
    auto input_utf16_be = taul::convert_encoding<char, char8_t>(taul::utf8, taul::utf16_be, u8"abc123Δ魂💩");
    auto input_utf16_le = taul::convert_encoding<char, char8_t>(taul::utf8, taul::utf16_le, u8"abc123Δ魂💩");
    ASSERT_TRUE(input_utf32_bom);
    ASSERT_TRUE(input_utf16_be);
    ASSERT_TRUE(input_utf16_le);

    auto expected_output = taul::str(taul::utf8_s(u8"abc123Δ魂💩abc123Δ魂💩abc123Δ魂💩"));

    taul::source_code src{};
    src.add_str("a"_str, taul::str(input_utf32_bom.value()), taul::utf32_bom);
    src.add_str("b"_str, taul::str(input_utf16_be.value()), taul::utf16_be);
    src.add_str("c"_str, taul::str(input_utf16_le.value()), taul::utf16_le);

    TAUL_LOG(taul::make_stderr_logger(), "expected_output==\"{}\"", expected_output);
    TAUL_LOG(taul::make_stderr_logger(), "src.str()==\"{}\"", src.str());

    ASSERT_EQ(src.str(), expected_output);
    ASSERT_EQ(src.pages().size(), 3);
}

TEST(SourceCodeTests, AddFile_Success) {
    const std::filesystem::path fp = std::filesystem::current_path() / "support\\add_file_helper_1.txt";
    taul::source_code sc{};
    ASSERT_TRUE(sc.add_file(fp, taul::make_stderr_logger()));
    sc.add_str("aa"_str, "0"_str);

    EXPECT_EQ(sc.str(), "this is a test text file!0");

    if (sc.pages().size() == 2) {
        EXPECT_EQ(sc.pages()[0].pos, 0);
        EXPECT_EQ(sc.pages()[0].length, 25);
        EXPECT_EQ(sc.pages()[0].origin, taul::str(std::filesystem::proximate(fp, std::filesystem::current_path()).string()));
        EXPECT_EQ(sc.pages()[1].pos, 25);
        EXPECT_EQ(sc.pages()[1].length, 1);
        EXPECT_EQ(sc.pages()[1].origin, "aa"_str);
    }
    else ADD_FAILURE();

    EXPECT_EQ(sc.page_at(0), 0);
    EXPECT_EQ(sc.page_at(24), 0);
    EXPECT_EQ(sc.page_at(25), 1);
    EXPECT_EQ(sc.page_at(26), 1);
    EXPECT_EQ(sc.page_at(taul::source_pos(-1)), 1);

    const auto origin = taul::str(std::filesystem::proximate(fp, std::filesystem::current_path()).string());

    const auto loc0 = taul::source_location{ 0, origin, 1, 1 };
    const auto loc24 = taul::source_location{ 24, origin, 25, 1 };
    const auto loc25 = taul::source_location{ 25, "aa"_str, 1, 1 };
    const auto loc26 = taul::source_location{ 26, "aa"_str, 2, 1 };

    EXPECT_EQ(sc.location_at(0).fmt(), loc0.fmt());
    EXPECT_EQ(sc.location_at(24).fmt(), loc24.fmt());
    EXPECT_EQ(sc.location_at(25).fmt(), loc25.fmt());
    EXPECT_EQ(sc.location_at(26).fmt(), loc26.fmt());
    EXPECT_EQ(sc.location_at(taul::source_pos(-1)).fmt(), loc26.fmt());
}

TEST(SourceCodeTests, AddFile_Failure) {
    const std::filesystem::path fp = "some_nonexistent_file.txt";
    taul::source_code sc{};
    EXPECT_FALSE(sc.add_file(fp, taul::make_stderr_logger()));

    EXPECT_TRUE(sc.str().empty());
    EXPECT_TRUE(sc.pages().empty());

    EXPECT_EQ(sc.page_at(0), 0);
    EXPECT_EQ(sc.page_at(1), 0);
    EXPECT_EQ(sc.page_at(taul::source_pos(-1)), 0);

    const auto loc0 = taul::source_location{ 0, ""_str, 1, 1 };

    EXPECT_EQ(sc.location_at(0).fmt(), loc0.fmt());
    EXPECT_EQ(sc.location_at(1).fmt(), loc0.fmt());
    EXPECT_EQ(sc.location_at(taul::source_pos(-1)).fmt(), loc0.fmt());
}

TEST(SourceCodeTests, AddFile_WithNewlines) {
    const std::filesystem::path fp = std::filesystem::current_path() / "outputs\\add_file_output_helper_1.txt";
    std::filesystem::remove(fp);
    ASSERT_FALSE(std::filesystem::exists(fp));
    taul::source_code out_src{};
    // test that impl can properly handle mix of LF, CR, and CRLF newlines
    out_src.add_str("a"_str, "ab\rcd\r\nef\ngh"_str);
    ASSERT_TRUE(out_src.to_file(fp)); // <- output file we'll then load in via add_file
    ASSERT_TRUE(std::filesystem::exists(fp));

    taul::source_code src{};
    ASSERT_TRUE(src.add_file(fp)); // <- load in the file we exported above

    auto expected_origin = taul::str(std::filesystem::proximate(fp, std::filesystem::current_path()).string());

    // ab\r
    const auto loc0 = taul::source_location{ 0, expected_origin, 1, 1 };
    const auto loc1 = taul::source_location{ 1, expected_origin, 2, 1 };
    const auto loc2 = taul::source_location{ 2, expected_origin, 3, 1 };
    // cd\r\n
    const auto loc3 = taul::source_location{ 3, expected_origin, 1, 2 };
    const auto loc4 = taul::source_location{ 4, expected_origin, 2, 2 };
    const auto loc5 = taul::source_location{ 5, expected_origin, 3, 2 };
    const auto loc6 = taul::source_location{ 6, expected_origin, 4, 2 };
    // ef\n
    const auto loc7 = taul::source_location{ 7, expected_origin, 1, 3 };
    const auto loc8 = taul::source_location{ 8, expected_origin, 2, 3 };
    const auto loc9 = taul::source_location{ 9, expected_origin, 3, 3 };
    // gh
    const auto loc10 = taul::source_location{ 10, expected_origin, 1, 4 };
    const auto loc11 = taul::source_location{ 11, expected_origin, 2, 4 };
    const auto loc12 = taul::source_location{ 12, expected_origin, 3, 4 };

    EXPECT_EQ(src.location_at(0).fmt(), loc0.fmt());
    EXPECT_EQ(src.location_at(1).fmt(), loc1.fmt());
    EXPECT_EQ(src.location_at(2).fmt(), loc2.fmt());
    EXPECT_EQ(src.location_at(3).fmt(), loc3.fmt());
    EXPECT_EQ(src.location_at(4).fmt(), loc4.fmt());
    EXPECT_EQ(src.location_at(5).fmt(), loc5.fmt());
    EXPECT_EQ(src.location_at(6).fmt(), loc6.fmt());
    EXPECT_EQ(src.location_at(7).fmt(), loc7.fmt());
    EXPECT_EQ(src.location_at(8).fmt(), loc8.fmt());
    EXPECT_EQ(src.location_at(9).fmt(), loc9.fmt());
    EXPECT_EQ(src.location_at(10).fmt(), loc10.fmt());
    EXPECT_EQ(src.location_at(11).fmt(), loc11.fmt());
    EXPECT_EQ(src.location_at(12).fmt(), loc12.fmt());
    EXPECT_EQ(src.location_at(taul::source_pos(-1)).fmt(), loc12.fmt());
}

TEST(SourceCodeTests, AddFile_WithMultiByteUTF8) {
    // TODO: I'm not 100% sure on this, but I think we could potentially get
    //       some minor portability issues w/ regards to different OSes using
    //       different newlines (ie. LF, CR, and CRLF), w/ add_file_helper_2.txt
    //       using CRLF, since we made it on Windows
    //
    //       that said, it also could be fine, as the file won't change on
    //       macOS or Linux... in which case we'd instead get the portability
    //       issue of those using different filesystems altogether, lol, which
    //       may or may not be an issue, I really don't know...

    // encountered an issues where our impl counted each UTF-8 code
    // segment as a seperate char when computing char positions, so
    // this is meant to address that issue

    const std::filesystem::path fp = std::filesystem::current_path() / "support\\add_file_helper_2.txt";
    taul::source_code sc{};
    ASSERT_TRUE(sc.add_file(fp, taul::make_stderr_logger()));

    ASSERT_EQ(sc.str(), taul::str(taul::utf8_s(u8"元気\r\nですか")));
    ASSERT_EQ(sc.pages().size(), 1);

    auto expected_origin = taul::str(std::filesystem::proximate(fp, std::filesystem::current_path()).string());

    /*for (size_t i = 0; i <= 20; i++) {
        TAUL_LOG(taul::make_stderr_logger(), "-> (i=={}) {}", i, sc.location_at(taul::source_pos(i)));
    }*/

    // 元気\r\n
    const auto loc0 = taul::source_location{ 0, expected_origin, 1, 1 };
    const auto loc1 = taul::source_location{ 1, expected_origin, 1, 1 };
    const auto loc2 = taul::source_location{ 2, expected_origin, 1, 1 };
    const auto loc3 = taul::source_location{ 3, expected_origin, 2, 1 };
    const auto loc4 = taul::source_location{ 4, expected_origin, 2, 1 };
    const auto loc5 = taul::source_location{ 5, expected_origin, 2, 1 };
    const auto loc6 = taul::source_location{ 6, expected_origin, 3, 1 };
    const auto loc7 = taul::source_location{ 7, expected_origin, 4, 1 };
    // ですか
    const auto loc8 = taul::source_location{ 8, expected_origin, 1, 2 };
    const auto loc9 = taul::source_location{ 9, expected_origin, 1, 2 };
    const auto loc10 = taul::source_location{ 10, expected_origin, 1, 2 };
    const auto loc11 = taul::source_location{ 11, expected_origin, 2, 2 };
    const auto loc12 = taul::source_location{ 12, expected_origin, 2, 2 };
    const auto loc13 = taul::source_location{ 13, expected_origin, 2, 2 };
    const auto loc14 = taul::source_location{ 14, expected_origin, 3, 2 };
    const auto loc15 = taul::source_location{ 15, expected_origin, 3, 2 };
    const auto loc16 = taul::source_location{ 16, expected_origin, 3, 2 };
    const auto loc17 = taul::source_location{ 17, expected_origin, 4, 2 };

    EXPECT_EQ(sc.location_at(0).fmt(), loc0.fmt());
    EXPECT_EQ(sc.location_at(1).fmt(), loc1.fmt());
    EXPECT_EQ(sc.location_at(2).fmt(), loc2.fmt());
    EXPECT_EQ(sc.location_at(3).fmt(), loc3.fmt());
    EXPECT_EQ(sc.location_at(4).fmt(), loc4.fmt());
    EXPECT_EQ(sc.location_at(5).fmt(), loc5.fmt());
    EXPECT_EQ(sc.location_at(6).fmt(), loc6.fmt());
    EXPECT_EQ(sc.location_at(7).fmt(), loc7.fmt());
    EXPECT_EQ(sc.location_at(8).fmt(), loc8.fmt());
    EXPECT_EQ(sc.location_at(9).fmt(), loc9.fmt());
    EXPECT_EQ(sc.location_at(10).fmt(), loc10.fmt());
    EXPECT_EQ(sc.location_at(11).fmt(), loc11.fmt());
    EXPECT_EQ(sc.location_at(12).fmt(), loc12.fmt());
    EXPECT_EQ(sc.location_at(13).fmt(), loc13.fmt());
    EXPECT_EQ(sc.location_at(14).fmt(), loc14.fmt());
    EXPECT_EQ(sc.location_at(15).fmt(), loc15.fmt());
    EXPECT_EQ(sc.location_at(16).fmt(), loc16.fmt());
    EXPECT_EQ(sc.location_at(17).fmt(), loc17.fmt());
    EXPECT_EQ(sc.location_at(taul::source_pos(-1)).fmt(), loc17.fmt());
}

TEST(SourceCodeTests, AddFile_FileIsEncodedAsUTF8BOM) {
    auto path = std::filesystem::current_path() / "support\\txt_file_encoded_as_utf8_bom.txt";
    ASSERT_TRUE(std::filesystem::exists(path));

    // confirm that txt_file_encoded_as_utf8_bom.txt is actually UTF-8 BOM encoded
    {
        std::ifstream ifs(path, std::ios::binary | std::ios::ate);//元気ですか
        ASSERT_TRUE(ifs.is_open());
        const size_t file_size = (size_t)ifs.tellg();
        ifs.seekg(0);
        std::string buff{};
        buff.resize(file_size, '\0');
        ifs.read(buff.data(), file_size);

        auto status = taul::check_bom(taul::utf8, std::string_view(buff).substr(0, ifs.gcount()));

        ASSERT_EQ(status, taul::bom_status::bom);
    }

    taul::source_code src{};
    src.add_file(path);

    ASSERT_EQ(taul::check_bom(taul::utf8, std::string_view(src.str())), taul::bom_status::no_bom);
}

TEST(SourceCodeTests, Reset) {
    taul::source_code sc{};
    sc.add_str("aa"_str, "abc"_str);
    sc.add_str("bb"_str, "def"_str);

    sc.reset();

    EXPECT_TRUE(sc.str().empty());
    EXPECT_TRUE(sc.pages().empty());

    EXPECT_EQ(sc.page_at(0), 0);
    EXPECT_EQ(sc.page_at(1), 0);
    EXPECT_EQ(sc.page_at(taul::source_pos(-1)), 0);

    const auto loc0 = taul::source_location{ 0, ""_str, 1, 1 };

    EXPECT_EQ(sc.location_at(0).fmt(), loc0.fmt());
    EXPECT_EQ(sc.location_at(1).fmt(), loc0.fmt());
    EXPECT_EQ(sc.location_at(taul::source_pos(-1)).fmt(), loc0.fmt());
}

