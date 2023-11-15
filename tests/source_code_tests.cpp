

#include <gtest/gtest.h>

#include <taul/all.h>


TEST(source_code_tests, defaultCtor) {
    
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


TEST(source_code_tests, moveCtor) {

    taul::source_code old_sc{};


    old_sc.add_str("aa", "abc");
    old_sc.add_str("bb", "def");


    taul::source_code sc(std::move(old_sc));


    EXPECT_EQ(sc.str(), "abcdef");

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
        EXPECT_EQ(sc.pages()[0].origin, "aa");
        EXPECT_EQ(sc.pages()[1].pos, 3);
        EXPECT_EQ(sc.pages()[1].length, 3);
        EXPECT_EQ(sc.pages()[1].origin, "bb");
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
        EXPECT_EQ(loc.origin, "aa");
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(1)) {
        const taul::source_location loc = *sc.location_at(1);
        EXPECT_EQ(loc.pos, 1);
        EXPECT_EQ(loc.origin, "aa");
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(2)) {
        const taul::source_location loc = *sc.location_at(2);
        EXPECT_EQ(loc.pos, 2);
        EXPECT_EQ(loc.origin, "aa");
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(3)) {
        const taul::source_location loc = *sc.location_at(3);
        EXPECT_EQ(loc.pos, 3);
        EXPECT_EQ(loc.origin, "bb");
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(4)) {
        const taul::source_location loc = *sc.location_at(4);
        EXPECT_EQ(loc.pos, 4);
        EXPECT_EQ(loc.origin, "bb");
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(5)) {
        const taul::source_location loc = *sc.location_at(5);
        EXPECT_EQ(loc.pos, 5);
        EXPECT_EQ(loc.origin, "bb");
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    EXPECT_FALSE(sc.location_at(6));
    EXPECT_FALSE(sc.location_at(taul::source_pos(-1)));
}


TEST(source_code_tests, moveAssign) {

    taul::source_code old_sc{};


    old_sc.add_str("aa", "abc");
    old_sc.add_str("bb", "def");


    taul::source_code sc{};

    sc = std::move(old_sc);


    EXPECT_EQ(sc.str(), "abcdef");

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
        EXPECT_EQ(sc.pages()[0].origin, "aa");
        EXPECT_EQ(sc.pages()[1].pos, 3);
        EXPECT_EQ(sc.pages()[1].length, 3);
        EXPECT_EQ(sc.pages()[1].origin, "bb");
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
        EXPECT_EQ(loc.origin, "aa");
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(1)) {
        const taul::source_location loc = *sc.location_at(1);
        EXPECT_EQ(loc.pos, 1);
        EXPECT_EQ(loc.origin, "aa");
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(2)) {
        const taul::source_location loc = *sc.location_at(2);
        EXPECT_EQ(loc.pos, 2);
        EXPECT_EQ(loc.origin, "aa");
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(3)) {
        const taul::source_location loc = *sc.location_at(3);
        EXPECT_EQ(loc.pos, 3);
        EXPECT_EQ(loc.origin, "bb");
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(4)) {
        const taul::source_location loc = *sc.location_at(4);
        EXPECT_EQ(loc.pos, 4);
        EXPECT_EQ(loc.origin, "bb");
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(5)) {
        const taul::source_location loc = *sc.location_at(5);
        EXPECT_EQ(loc.pos, 5);
        EXPECT_EQ(loc.origin, "bb");
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    EXPECT_FALSE(sc.location_at(6));
    EXPECT_FALSE(sc.location_at(taul::source_pos(-1)));
}


TEST(source_code_tests, moveAssign_ontoSelf) {

    taul::source_code sc{};


    sc.add_str("aa", "abc");
    sc.add_str("bb", "def");


    sc = std::move(sc); // <- self-assignment


    EXPECT_EQ(sc.str(), "abcdef");

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
        EXPECT_EQ(sc.pages()[0].origin, "aa");
        EXPECT_EQ(sc.pages()[1].pos, 3);
        EXPECT_EQ(sc.pages()[1].length, 3);
        EXPECT_EQ(sc.pages()[1].origin, "bb");
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
        EXPECT_EQ(loc.origin, "aa");
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(1)) {
        const taul::source_location loc = *sc.location_at(1);
        EXPECT_EQ(loc.pos, 1);
        EXPECT_EQ(loc.origin, "aa");
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(2)) {
        const taul::source_location loc = *sc.location_at(2);
        EXPECT_EQ(loc.pos, 2);
        EXPECT_EQ(loc.origin, "aa");
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(3)) {
        const taul::source_location loc = *sc.location_at(3);
        EXPECT_EQ(loc.pos, 3);
        EXPECT_EQ(loc.origin, "bb");
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(4)) {
        const taul::source_location loc = *sc.location_at(4);
        EXPECT_EQ(loc.pos, 4);
        EXPECT_EQ(loc.origin, "bb");
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(5)) {
        const taul::source_location loc = *sc.location_at(5);
        EXPECT_EQ(loc.pos, 5);
        EXPECT_EQ(loc.origin, "bb");
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    EXPECT_FALSE(sc.location_at(6));
    EXPECT_FALSE(sc.location_at(taul::source_pos(-1)));
}


// newlines within source string w/ them being tested seperately

TEST(source_code_tests, add_str) {

    taul::source_code sc{};


    // test all overloads

    sc.add_str("aa", std::string("abc"));
    sc.add_str("bb", std::string_view("def"));
    sc.add_str("cc", "ghi");


    EXPECT_EQ(sc.str(), "abcdefghi");

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
        EXPECT_EQ(sc.pages()[0].origin, "aa");
        EXPECT_EQ(sc.pages()[1].pos, 3);
        EXPECT_EQ(sc.pages()[1].length, 3);
        EXPECT_EQ(sc.pages()[1].origin, "bb");
        EXPECT_EQ(sc.pages()[2].pos, 6);
        EXPECT_EQ(sc.pages()[2].length, 3);
        EXPECT_EQ(sc.pages()[2].origin, "cc");
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
        EXPECT_EQ(loc.origin, "aa");
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(1)) {
        const taul::source_location loc = *sc.location_at(1);
        EXPECT_EQ(loc.pos, 1);
        EXPECT_EQ(loc.origin, "aa");
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(2)) {
        const taul::source_location loc = *sc.location_at(2);
        EXPECT_EQ(loc.pos, 2);
        EXPECT_EQ(loc.origin, "aa");
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(3)) {
        const taul::source_location loc = *sc.location_at(3);
        EXPECT_EQ(loc.pos, 3);
        EXPECT_EQ(loc.origin, "bb");
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(4)) {
        const taul::source_location loc = *sc.location_at(4);
        EXPECT_EQ(loc.pos, 4);
        EXPECT_EQ(loc.origin, "bb");
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(5)) {
        const taul::source_location loc = *sc.location_at(5);
        EXPECT_EQ(loc.pos, 5);
        EXPECT_EQ(loc.origin, "bb");
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(6)) {
        const taul::source_location loc = *sc.location_at(6);
        EXPECT_EQ(loc.pos, 6);
        EXPECT_EQ(loc.origin, "cc");
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(7)) {
        const taul::source_location loc = *sc.location_at(7);
        EXPECT_EQ(loc.pos, 7);
        EXPECT_EQ(loc.origin, "cc");
        EXPECT_EQ(loc.chr, 2);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(8)) {
        const taul::source_location loc = *sc.location_at(8);
        EXPECT_EQ(loc.pos, 8);
        EXPECT_EQ(loc.origin, "cc");
        EXPECT_EQ(loc.chr, 3);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    EXPECT_FALSE(sc.location_at(9));
    EXPECT_FALSE(sc.location_at(taul::source_pos(-1)));
}


TEST(source_code_tests, add_str_withNewlines) {

    taul::source_code sc{};


    sc.add_str("aa", "ab\ncd\nef");
    sc.add_str("bb", "g\nhi");


    ASSERT_EQ(sc.str(), "ab\ncd\nefg\nhi");
    ASSERT_EQ(sc.pages().size(), 2);


    {
        taul::source_pos pos = 0;
        if (sc.location_at(pos)) {
            const taul::source_location loc = *sc.location_at(pos);
            EXPECT_EQ(loc.pos, pos);
            EXPECT_EQ(loc.origin, "aa");
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
            EXPECT_EQ(loc.origin, "aa");
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
            EXPECT_EQ(loc.origin, "aa");
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
            EXPECT_EQ(loc.origin, "aa");
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
            EXPECT_EQ(loc.origin, "aa");
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
            EXPECT_EQ(loc.origin, "aa");
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
            EXPECT_EQ(loc.origin, "aa");
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
            EXPECT_EQ(loc.origin, "aa");
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
            EXPECT_EQ(loc.origin, "bb");
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
            EXPECT_EQ(loc.origin, "bb");
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
            EXPECT_EQ(loc.origin, "bb");
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
            EXPECT_EQ(loc.origin, "bb");
            EXPECT_EQ(loc.chr, 2);
            EXPECT_EQ(loc.line, 2);
        }
        else ADD_FAILURE();
    }
}


TEST(source_code_tests, add_file_success) {

    taul::source_code sc{};


    const std::filesystem::path fp = std::filesystem::current_path() / "support\\add_file_helper.txt";

    const auto result = sc.add_file(fp, taul::make_stderr_logger());


    ASSERT_TRUE(result);


    sc.add_str("aa", "0");


    EXPECT_EQ(sc.str(), "this is a test text file!0");

    EXPECT_TRUE(sc.pos_in_bounds(0));
    EXPECT_TRUE(sc.pos_in_bounds(25));
    EXPECT_FALSE(sc.pos_in_bounds(26));
    EXPECT_FALSE(sc.pos_in_bounds(taul::source_pos(-1)));

    if (sc.pages().size() == 2) {
        EXPECT_EQ(sc.pages()[0].pos, 0);
        EXPECT_EQ(sc.pages()[0].length, 25);
        EXPECT_EQ(sc.pages()[0].origin, fp);
        EXPECT_EQ(sc.pages()[1].pos, 25);
        EXPECT_EQ(sc.pages()[1].length, 1);
        EXPECT_EQ(sc.pages()[1].origin, "aa");
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
        EXPECT_EQ(loc.origin, fp);
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(24)) {
        const taul::source_location loc = *sc.location_at(24);
        EXPECT_EQ(loc.pos, 24);
        EXPECT_EQ(loc.origin, fp);
        EXPECT_EQ(loc.chr, 25);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    if (sc.location_at(25)) {
        const taul::source_location loc = *sc.location_at(25);
        EXPECT_EQ(loc.pos, 25);
        EXPECT_EQ(loc.origin, "aa");
        EXPECT_EQ(loc.chr, 1);
        EXPECT_EQ(loc.line, 1);
    }
    else ADD_FAILURE();
    EXPECT_FALSE(sc.location_at(26));
    EXPECT_FALSE(sc.location_at(taul::source_pos(-1)));
}


TEST(source_code_tests, add_file_failure) {

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


TEST(source_code_tests, reset) {

    taul::source_code sc{};


    sc.add_str("aa", "abc");
    sc.add_str("bb", "def");

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

