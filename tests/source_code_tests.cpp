

#include <gtest/gtest.h>

#include <fstream>

#include <taul/logger.h>
#include <taul/str.h>
#include <taul/source_code.h>


using namespace taul::string_literals;


TEST(SourceCodeTests, DefaultCtor) {
    
    taul::source_code sc{};


    EXPECT_TRUE(sc.str().empty());

    EXPECT_FALSE(sc.pos_in_bounds(0));
    EXPECT_FALSE(sc.pos_in_bounds(1));
    EXPECT_FALSE(sc.pos_in_bounds(taul::source_pos(-1)));

    EXPECT_TRUE(sc.pages().empty());

    EXPECT_FALSE(sc.page_at(0));
    EXPECT_FALSE(sc.page_at(1));
    EXPECT_FALSE(sc.page_at(taul::source_pos(-1)));

    EXPECT_FALSE(sc.location_at(0));
    EXPECT_FALSE(sc.location_at(1));
    EXPECT_FALSE(sc.location_at(taul::source_pos(-1)));
}

TEST(SourceCodeTests, MoveCtor) {

    taul::source_code old_sc{};


    old_sc.add_str("aa"_str, "abc"_str);
    old_sc.add_str("bb"_str, "def"_str);


    taul::source_code sc(std::move(old_sc));


    EXPECT_EQ(sc.str(), "abcdef"_str);

    EXPECT_TRUE(sc.pos_in_bounds(0));
    EXPECT_TRUE(sc.pos_in_bounds(1));
    EXPECT_TRUE(sc.pos_in_bounds(2));
    EXPECT_TRUE(sc.pos_in_bounds(3));
    EXPECT_TRUE(sc.pos_in_bounds(4));
    EXPECT_TRUE(sc.pos_in_bounds(5));
    EXPECT_FALSE(sc.pos_in_bounds(6));
    EXPECT_FALSE(sc.pos_in_bounds(taul::source_pos(-1)));

    if (sc.pages().size() == 2) {
        EXPECT_EQ(sc.pages()[0].pos, 0);
        EXPECT_EQ(sc.pages()[0].length, 3);
        EXPECT_EQ(sc.pages()[0].origin, "aa"_str);
        EXPECT_EQ(sc.pages()[1].pos, 3);
        EXPECT_EQ(sc.pages()[1].length, 3);
        EXPECT_EQ(sc.pages()[1].origin, "bb"_str);
    }
    else ADD_FAILURE();

    if (sc.page_at(0)) { EXPECT_EQ(*sc.page_at(0), 0); }
    else ADD_FAILURE();
    if (sc.page_at(1)) { EXPECT_EQ(*sc.page_at(1), 0); }
    else ADD_FAILURE();
    if (sc.page_at(2)) { EXPECT_EQ(*sc.page_at(2), 0); }
    else ADD_FAILURE();
    if (sc.page_at(3)) { EXPECT_EQ(*sc.page_at(3), 1); }
    else ADD_FAILURE();
    if (sc.page_at(4)) { EXPECT_EQ(*sc.page_at(4), 1); }
    else ADD_FAILURE();
    if (sc.page_at(5)) { EXPECT_EQ(*sc.page_at(5), 1); }
    else ADD_FAILURE();
    EXPECT_FALSE(sc.page_at(6));
    EXPECT_FALSE(sc.page_at(taul::source_pos(-1)));

    if (sc.location_at(0)) {
        const taul::source_location loc = *sc.location_at(0);
        EXPECT_EQ(loc.pos, 0);
        EXPECT_EQ(loc.origin, "aa"_str);
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(1)) {
        const taul::source_location loc = *sc.location_at(1);
        EXPECT_EQ(loc.pos, 1);
        EXPECT_EQ(loc.origin, "aa"_str);
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(2)) {
        const taul::source_location loc = *sc.location_at(2);
        EXPECT_EQ(loc.pos, 2);
        EXPECT_EQ(loc.origin, "aa"_str);
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(3)) {
        const taul::source_location loc = *sc.location_at(3);
        EXPECT_EQ(loc.pos, 3);
        EXPECT_EQ(loc.origin, "bb"_str);
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(4)) {
        const taul::source_location loc = *sc.location_at(4);
        EXPECT_EQ(loc.pos, 4);
        EXPECT_EQ(loc.origin, "bb"_str);
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(5)) {
        const taul::source_location loc = *sc.location_at(5);
        EXPECT_EQ(loc.pos, 5);
        EXPECT_EQ(loc.origin, "bb"_str);
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    EXPECT_FALSE(sc.location_at(6));
    EXPECT_FALSE(sc.location_at(taul::source_pos(-1)));
}

TEST(SourceCodeTests, MoveAssign) {

    taul::source_code old_sc{};


    old_sc.add_str("aa"_str, "abc"_str);
    old_sc.add_str("bb"_str, "def"_str);


    taul::source_code sc{};

    sc = std::move(old_sc);


    EXPECT_EQ(sc.str(), "abcdef"_str);

    EXPECT_TRUE(sc.pos_in_bounds(0));
    EXPECT_TRUE(sc.pos_in_bounds(1));
    EXPECT_TRUE(sc.pos_in_bounds(2));
    EXPECT_TRUE(sc.pos_in_bounds(3));
    EXPECT_TRUE(sc.pos_in_bounds(4));
    EXPECT_TRUE(sc.pos_in_bounds(5));
    EXPECT_FALSE(sc.pos_in_bounds(6));
    EXPECT_FALSE(sc.pos_in_bounds(taul::source_pos(-1)));

    if (sc.pages().size() == 2) {
        EXPECT_EQ(sc.pages()[0].pos, 0);
        EXPECT_EQ(sc.pages()[0].length, 3);
        EXPECT_EQ(sc.pages()[0].origin, "aa"_str);
        EXPECT_EQ(sc.pages()[1].pos, 3);
        EXPECT_EQ(sc.pages()[1].length, 3);
        EXPECT_EQ(sc.pages()[1].origin, "bb"_str);
    }
    else ADD_FAILURE();

    if (sc.page_at(0)) { EXPECT_EQ(*sc.page_at(0), 0); }
    else ADD_FAILURE();
    if (sc.page_at(1)) { EXPECT_EQ(*sc.page_at(1), 0); }
    else ADD_FAILURE();
    if (sc.page_at(2)) { EXPECT_EQ(*sc.page_at(2), 0); }
    else ADD_FAILURE();
    if (sc.page_at(3)) { EXPECT_EQ(*sc.page_at(3), 1); }
    else ADD_FAILURE();
    if (sc.page_at(4)) { EXPECT_EQ(*sc.page_at(4), 1); }
    else ADD_FAILURE();
    if (sc.page_at(5)) { EXPECT_EQ(*sc.page_at(5), 1); }
    else ADD_FAILURE();
    EXPECT_FALSE(sc.page_at(6));
    EXPECT_FALSE(sc.page_at(taul::source_pos(-1)));

    if (sc.location_at(0)) {
        const taul::source_location loc = *sc.location_at(0);
        EXPECT_EQ(loc.pos, 0);
        EXPECT_EQ(loc.origin, "aa"_str);
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(1)) {
        const taul::source_location loc = *sc.location_at(1);
        EXPECT_EQ(loc.pos, 1);
        EXPECT_EQ(loc.origin, "aa"_str);
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(2)) {
        const taul::source_location loc = *sc.location_at(2);
        EXPECT_EQ(loc.pos, 2);
        EXPECT_EQ(loc.origin, "aa"_str);
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(3)) {
        const taul::source_location loc = *sc.location_at(3);
        EXPECT_EQ(loc.pos, 3);
        EXPECT_EQ(loc.origin, "bb"_str);
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(4)) {
        const taul::source_location loc = *sc.location_at(4);
        EXPECT_EQ(loc.pos, 4);
        EXPECT_EQ(loc.origin, "bb"_str);
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(5)) {
        const taul::source_location loc = *sc.location_at(5);
        EXPECT_EQ(loc.pos, 5);
        EXPECT_EQ(loc.origin, "bb"_str);
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    EXPECT_FALSE(sc.location_at(6));
    EXPECT_FALSE(sc.location_at(taul::source_pos(-1)));
}

TEST(SourceCodeTests, MoveAssignOntoSelf) {

    taul::source_code sc{};


    sc.add_str("aa"_str, "abc"_str);
    sc.add_str("bb"_str, "def"_str);


    sc = std::move(sc); // <- self-assignment


    EXPECT_EQ(sc.str(), "abcdef"_str);

    EXPECT_TRUE(sc.pos_in_bounds(0));
    EXPECT_TRUE(sc.pos_in_bounds(1));
    EXPECT_TRUE(sc.pos_in_bounds(2));
    EXPECT_TRUE(sc.pos_in_bounds(3));
    EXPECT_TRUE(sc.pos_in_bounds(4));
    EXPECT_TRUE(sc.pos_in_bounds(5));
    EXPECT_FALSE(sc.pos_in_bounds(6));
    EXPECT_FALSE(sc.pos_in_bounds(taul::source_pos(-1)));

    if (sc.pages().size() == 2) {
        EXPECT_EQ(sc.pages()[0].pos, 0);
        EXPECT_EQ(sc.pages()[0].length, 3);
        EXPECT_EQ(sc.pages()[0].origin, "aa"_str);
        EXPECT_EQ(sc.pages()[1].pos, 3);
        EXPECT_EQ(sc.pages()[1].length, 3);
        EXPECT_EQ(sc.pages()[1].origin, "bb"_str);
    }
    else ADD_FAILURE();

    if (sc.page_at(0)) { EXPECT_EQ(*sc.page_at(0), 0); }
    else ADD_FAILURE();
    if (sc.page_at(1)) { EXPECT_EQ(*sc.page_at(1), 0); }
    else ADD_FAILURE();
    if (sc.page_at(2)) { EXPECT_EQ(*sc.page_at(2), 0); }
    else ADD_FAILURE();
    if (sc.page_at(3)) { EXPECT_EQ(*sc.page_at(3), 1); }
    else ADD_FAILURE();
    if (sc.page_at(4)) { EXPECT_EQ(*sc.page_at(4), 1); }
    else ADD_FAILURE();
    if (sc.page_at(5)) { EXPECT_EQ(*sc.page_at(5), 1); }
    else ADD_FAILURE();
    EXPECT_FALSE(sc.page_at(6));
    EXPECT_FALSE(sc.page_at(taul::source_pos(-1)));

    if (sc.location_at(0)) {
        const taul::source_location loc = *sc.location_at(0);
        EXPECT_EQ(loc.pos, 0);
        EXPECT_EQ(loc.origin, "aa"_str);
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(1)) {
        const taul::source_location loc = *sc.location_at(1);
        EXPECT_EQ(loc.pos, 1);
        EXPECT_EQ(loc.origin, "aa"_str);
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(2)) {
        const taul::source_location loc = *sc.location_at(2);
        EXPECT_EQ(loc.pos, 2);
        EXPECT_EQ(loc.origin, "aa"_str);
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(3)) {
        const taul::source_location loc = *sc.location_at(3);
        EXPECT_EQ(loc.pos, 3);
        EXPECT_EQ(loc.origin, "bb"_str);
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(4)) {
        const taul::source_location loc = *sc.location_at(4);
        EXPECT_EQ(loc.pos, 4);
        EXPECT_EQ(loc.origin, "bb"_str);
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(5)) {
        const taul::source_location loc = *sc.location_at(5);
        EXPECT_EQ(loc.pos, 5);
        EXPECT_EQ(loc.origin, "bb"_str);
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    EXPECT_FALSE(sc.location_at(6));
    EXPECT_FALSE(sc.location_at(taul::source_pos(-1)));
}

TEST(SourceCodeTests, ToFile) {
    auto path = std::filesystem::current_path() / "outputs\\source_code_to_file_output.txt";

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

// newlines within source string w/ them being tested seperately

TEST(SourceCodeTests, AddStr) {
    taul::source_code sc{};
    // test all overloads
    sc.add_str("aa"_str, "abc"_str);
    sc.add_str("bb"_str, "def"_str);
    sc.add_str("cc"_str, "ghi"_str);

    EXPECT_EQ(sc.str(), "abcdefghi"_str);

    EXPECT_TRUE(sc.pos_in_bounds(0));
    EXPECT_TRUE(sc.pos_in_bounds(1));
    EXPECT_TRUE(sc.pos_in_bounds(2));
    EXPECT_TRUE(sc.pos_in_bounds(3));
    EXPECT_TRUE(sc.pos_in_bounds(4));
    EXPECT_TRUE(sc.pos_in_bounds(5));
    EXPECT_TRUE(sc.pos_in_bounds(6));
    EXPECT_TRUE(sc.pos_in_bounds(7));
    EXPECT_TRUE(sc.pos_in_bounds(8));
    EXPECT_FALSE(sc.pos_in_bounds(9));
    EXPECT_FALSE(sc.pos_in_bounds(taul::source_pos(-1)));

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

    if (sc.page_at(0)) { EXPECT_EQ(*sc.page_at(0), 0); } else ADD_FAILURE();
    if (sc.page_at(1)) { EXPECT_EQ(*sc.page_at(1), 0); } else ADD_FAILURE();
    if (sc.page_at(2)) { EXPECT_EQ(*sc.page_at(2), 0); } else ADD_FAILURE();
    if (sc.page_at(3)) { EXPECT_EQ(*sc.page_at(3), 1); } else ADD_FAILURE();
    if (sc.page_at(4)) { EXPECT_EQ(*sc.page_at(4), 1); } else ADD_FAILURE();
    if (sc.page_at(5)) { EXPECT_EQ(*sc.page_at(5), 1); } else ADD_FAILURE();
    if (sc.page_at(6)) { EXPECT_EQ(*sc.page_at(6), 2); } else ADD_FAILURE();
    if (sc.page_at(7)) { EXPECT_EQ(*sc.page_at(7), 2); } else ADD_FAILURE();
    if (sc.page_at(8)) { EXPECT_EQ(*sc.page_at(8), 2); } else ADD_FAILURE();
    EXPECT_FALSE(sc.page_at(9));
    EXPECT_FALSE(sc.page_at(taul::source_pos(-1)));

    if (sc.location_at(0)) {
        const taul::source_location loc = *sc.location_at(0);
        EXPECT_EQ(loc.pos, 0);
        EXPECT_EQ(loc.origin, "aa"_str);
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(1)) {
        const taul::source_location loc = *sc.location_at(1);
        EXPECT_EQ(loc.pos, 1);
        EXPECT_EQ(loc.origin, "aa"_str);
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(2)) {
        const taul::source_location loc = *sc.location_at(2);
        EXPECT_EQ(loc.pos, 2);
        EXPECT_EQ(loc.origin, "aa"_str);
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(3)) {
        const taul::source_location loc = *sc.location_at(3);
        EXPECT_EQ(loc.pos, 3);
        EXPECT_EQ(loc.origin, "bb"_str);
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(4)) {
        const taul::source_location loc = *sc.location_at(4);
        EXPECT_EQ(loc.pos, 4);
        EXPECT_EQ(loc.origin, "bb"_str);
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(5)) {
        const taul::source_location loc = *sc.location_at(5);
        EXPECT_EQ(loc.pos, 5);
        EXPECT_EQ(loc.origin, "bb"_str);
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(6)) {
        const taul::source_location loc = *sc.location_at(6);
        EXPECT_EQ(loc.pos, 6);
        EXPECT_EQ(loc.origin, "cc"_str);
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(7)) {
        const taul::source_location loc = *sc.location_at(7);
        EXPECT_EQ(loc.pos, 7);
        EXPECT_EQ(loc.origin, "cc"_str);
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(8)) {
        const taul::source_location loc = *sc.location_at(8);
        EXPECT_EQ(loc.pos, 8);
        EXPECT_EQ(loc.origin, "cc"_str);
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    EXPECT_FALSE(sc.location_at(9));
    EXPECT_FALSE(sc.location_at(taul::source_pos(-1)));
}

TEST(SourceCodeTests, AddStr_WithNewlines) {
    taul::source_code sc{};
    sc.add_str("aa"_str, "ab\ncd\nef"_str);
    sc.add_str("bb"_str, "g\nhi"_str);

    ASSERT_EQ(sc.str(), "ab\ncd\nefg\nhi"_str);
    ASSERT_EQ(sc.pages().size(), 2);

    {
        taul::source_pos pos = 0;
        if (sc.location_at(pos)) {
            const taul::source_location loc = *sc.location_at(pos);
            EXPECT_EQ(loc.pos, pos);
            EXPECT_EQ(loc.origin, "aa"_str);
            EXPECT_EQ(loc.chr, 1);
            EXPECT_EQ(loc.line, 1);
        }
        else ADD_FAILURE();
    }
    {
        taul::source_pos pos = 1;
        if (sc.location_at(pos)) {
            const taul::source_location loc = *sc.location_at(pos);
            EXPECT_EQ(loc.pos, pos);
            EXPECT_EQ(loc.origin, "aa"_str);
            EXPECT_EQ(loc.chr, 2);
            EXPECT_EQ(loc.line, 1);
        }
        else ADD_FAILURE();
    }
    {
        taul::source_pos pos = 2;
        if (sc.location_at(pos)) {
            const taul::source_location loc = *sc.location_at(pos);
            EXPECT_EQ(loc.pos, pos);
            EXPECT_EQ(loc.origin, "aa"_str);
            EXPECT_EQ(loc.chr, 3);
            EXPECT_EQ(loc.line, 1);
        }
        else ADD_FAILURE();
    }
    {
        taul::source_pos pos = 3;
        if (sc.location_at(pos)) {
            const taul::source_location loc = *sc.location_at(pos);
            EXPECT_EQ(loc.pos, pos);
            EXPECT_EQ(loc.origin, "aa"_str);
            EXPECT_EQ(loc.chr, 1);
            EXPECT_EQ(loc.line, 2);
        }
        else ADD_FAILURE();
    }
    {
        taul::source_pos pos = 4;
        if (sc.location_at(pos)) {
            const taul::source_location loc = *sc.location_at(pos);
            EXPECT_EQ(loc.pos, pos);
            EXPECT_EQ(loc.origin, "aa"_str);
            EXPECT_EQ(loc.chr, 2);
            EXPECT_EQ(loc.line, 2);
        }
        else ADD_FAILURE();
    }
    {
        taul::source_pos pos = 5;
        if (sc.location_at(pos)) {
            const taul::source_location loc = *sc.location_at(pos);
            EXPECT_EQ(loc.pos, pos);
            EXPECT_EQ(loc.origin, "aa"_str);
            EXPECT_EQ(loc.chr, 3);
            EXPECT_EQ(loc.line, 2);
        }
        else ADD_FAILURE();
    }
    {
        taul::source_pos pos = 6;
        if (sc.location_at(pos)) {
            const taul::source_location loc = *sc.location_at(pos);
            EXPECT_EQ(loc.pos, pos);
            EXPECT_EQ(loc.origin, "aa"_str);
            EXPECT_EQ(loc.chr, 1);
            EXPECT_EQ(loc.line, 3);
        }
        else ADD_FAILURE();
    }
    {
        taul::source_pos pos = 7;
        if (sc.location_at(pos)) {
            const taul::source_location loc = *sc.location_at(pos);
            EXPECT_EQ(loc.pos, pos);
            EXPECT_EQ(loc.origin, "aa"_str);
            EXPECT_EQ(loc.chr, 2);
            EXPECT_EQ(loc.line, 3);
        }
        else ADD_FAILURE();
    }
    {
        taul::source_pos pos = 8;
        if (sc.location_at(pos)) {
            const taul::source_location loc = *sc.location_at(pos);
            EXPECT_EQ(loc.pos, pos);
            EXPECT_EQ(loc.origin, "bb"_str);
            EXPECT_EQ(loc.chr, 1);
            EXPECT_EQ(loc.line, 1);
        }
        else ADD_FAILURE();
    }
    {
        taul::source_pos pos = 9;
        if (sc.location_at(pos)) {
            const taul::source_location loc = *sc.location_at(pos);
            EXPECT_EQ(loc.pos, pos);
            EXPECT_EQ(loc.origin, "bb"_str);
            EXPECT_EQ(loc.chr, 2);
            EXPECT_EQ(loc.line, 1);
        }
        else ADD_FAILURE();
    }
    {
        taul::source_pos pos = 10;
        if (sc.location_at(pos)) {
            const taul::source_location loc = *sc.location_at(pos);
            EXPECT_EQ(loc.pos, pos);
            EXPECT_EQ(loc.origin, "bb"_str);
            EXPECT_EQ(loc.chr, 1);
            EXPECT_EQ(loc.line, 2);
        }
        else ADD_FAILURE();
    }
    {
        taul::source_pos pos = 11;
        if (sc.location_at(pos)) {
            const taul::source_location loc = *sc.location_at(pos);
            EXPECT_EQ(loc.pos, pos);
            EXPECT_EQ(loc.origin, "bb"_str);
            EXPECT_EQ(loc.chr, 2);
            EXPECT_EQ(loc.line, 2);
        }
        else ADD_FAILURE();
    }
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

    auto expected_output_s = taul::convert_encoding<char, char8_t>(taul::utf8, taul::utf8, u8"abc123Δ魂💩abc123Δ魂💩abc123Δ魂💩"_str);
    ASSERT_TRUE(expected_output_s);
    auto expected_output = taul::str(expected_output_s.value());

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

    taul::source_code sc{};


    const std::filesystem::path fp = std::filesystem::current_path() / "support\\add_file_helper.txt";

    const auto result = sc.add_file(fp, taul::make_stderr_logger());


    ASSERT_TRUE(result);


    sc.add_str("aa"_str, "0"_str);


    EXPECT_EQ(sc.str(), "this is a test text file!0");

    EXPECT_TRUE(sc.pos_in_bounds(0));
    EXPECT_TRUE(sc.pos_in_bounds(25));
    EXPECT_FALSE(sc.pos_in_bounds(26));
    EXPECT_FALSE(sc.pos_in_bounds(taul::source_pos(-1)));

    if (sc.pages().size() == 2) {
        EXPECT_EQ(sc.pages()[0].pos, 0);
        EXPECT_EQ(sc.pages()[0].length, 25);
        EXPECT_EQ(sc.pages()[0].origin, taul::str(std::filesystem::proximate(fp, std::filesystem::current_path()).string()));
        EXPECT_EQ(sc.pages()[1].pos, 25);
        EXPECT_EQ(sc.pages()[1].length, 1);
        EXPECT_EQ(sc.pages()[1].origin, "aa"_str);
    }
    else ADD_FAILURE();

    if (sc.page_at(0)) { EXPECT_EQ(*sc.page_at(0), 0); } else ADD_FAILURE();
    if (sc.page_at(24)) { EXPECT_EQ(*sc.page_at(24), 0); } else ADD_FAILURE();
    if (sc.page_at(25)) { EXPECT_EQ(*sc.page_at(25), 1); } else ADD_FAILURE();
    EXPECT_FALSE(sc.page_at(26));
    EXPECT_FALSE(sc.page_at(taul::source_pos(-1)));
    
    if (sc.location_at(0)) {
        const taul::source_location loc = *sc.location_at(0);
        EXPECT_EQ(loc.pos, 0);
        EXPECT_EQ(loc.origin, taul::str(std::filesystem::proximate(fp, std::filesystem::current_path()).string()));
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(24)) {
        const taul::source_location loc = *sc.location_at(24);
        EXPECT_EQ(loc.pos, 24);
        EXPECT_EQ(loc.origin, taul::str(std::filesystem::proximate(fp, std::filesystem::current_path()).string()));
        EXPECT_EQ(loc.chr, 25);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(25)) {
        const taul::source_location loc = *sc.location_at(25);
        EXPECT_EQ(loc.pos, 25);
        EXPECT_EQ(loc.origin, "aa"_str);
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    EXPECT_FALSE(sc.location_at(26));
    EXPECT_FALSE(sc.location_at(taul::source_pos(-1)));
}

TEST(SourceCodeTests, AddFile_Failure) {

    taul::source_code sc{};


    const std::filesystem::path fp = "some_nonexistent_file.txt";

    const auto result = sc.add_file(fp, taul::make_stderr_logger());


    EXPECT_FALSE(result);


    EXPECT_TRUE(sc.str().empty());

    EXPECT_FALSE(sc.pos_in_bounds(0));
    EXPECT_FALSE(sc.pos_in_bounds(1));
    EXPECT_FALSE(sc.pos_in_bounds(taul::source_pos(-1)));

    EXPECT_TRUE(sc.pages().empty());

    EXPECT_FALSE(sc.page_at(0));
    EXPECT_FALSE(sc.page_at(1));
    EXPECT_FALSE(sc.page_at(taul::source_pos(-1)));

    EXPECT_FALSE(sc.location_at(0));
    EXPECT_FALSE(sc.location_at(1));
    EXPECT_FALSE(sc.location_at(taul::source_pos(-1)));
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

    EXPECT_FALSE(sc.pos_in_bounds(0));
    EXPECT_FALSE(sc.pos_in_bounds(1));
    EXPECT_FALSE(sc.pos_in_bounds(taul::source_pos(-1)));

    EXPECT_TRUE(sc.pages().empty());

    EXPECT_FALSE(sc.page_at(0));
    EXPECT_FALSE(sc.page_at(1));
    EXPECT_FALSE(sc.page_at(taul::source_pos(-1)));

    EXPECT_FALSE(sc.location_at(0));
    EXPECT_FALSE(sc.location_at(1));
    EXPECT_FALSE(sc.location_at(taul::source_pos(-1)));
}

