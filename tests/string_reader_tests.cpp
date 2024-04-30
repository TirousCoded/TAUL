

#include <gtest/gtest.h>

#include <taul/string_reader.h>

#include "helpers/test_glyph_observer.h"
#include "parameterized_tests/glyph_stream_tests.h"


using namespace taul::string_literals;


// test w/ non-empty input

GlyphStreamParam _make_param_1() {
    auto stream = std::make_shared<taul::string_reader>("abcdef"_str);
    std::size_t n_after_done = 6;
    return GlyphStreamParam::init(stream, n_after_done);
}

INSTANTIATE_TEST_SUITE_P(
    StringReader_NonEmptyInput,
    GlyphStreamTests, 
    testing::Values(_make_param_1()));


// test w/ empty input

GlyphStreamParam _make_param_2() {
    auto stream = std::make_shared<taul::string_reader>(""_str);
    std::size_t n_after_done = 0;
    return GlyphStreamParam::init(stream, n_after_done);
}

INSTANTIATE_TEST_SUITE_P(
    StringReader_EmptyInput,
    GlyphStreamTests, 
    testing::Values(_make_param_2()));


TEST(StringReaderTests, Observers) {
    taul::string_reader rdr("abcdef"_str);
    test_glyph_observer obsvr;

    rdr.bind_observer(&obsvr);

    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'a', 0));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'b', 1));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'c', 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'd', 3));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'e', 4));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'f', 5));
    ASSERT_TRUE(rdr.done());
    ASSERT_EQ(rdr.next(), taul::glyph::end(6));

    std::string expected{};
    expected += std::format("\n{}", taul::glyph::normal(U'a', 0));
    expected += std::format("\n{}", taul::glyph::normal(U'b', 1));
    expected += std::format("\n{}", taul::glyph::normal(U'c', 2));
    expected += std::format("\n{}", taul::glyph::normal(U'd', 3));
    expected += std::format("\n{}", taul::glyph::normal(U'e', 4));
    expected += std::format("\n{}", taul::glyph::normal(U'f', 5));
    expected += std::format("\n{}", taul::glyph::end(6));

    EXPECT_EQ(obsvr.output, expected);
}

TEST(StringReaderTests, PeekNextAndDone) {
    taul::string_reader rdr("abcdef"_str);

    ASSERT_FALSE(rdr.done());

    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'a', 0));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'a', 0));

    ASSERT_FALSE(rdr.done());

    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'b', 1));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'b', 1));

    ASSERT_FALSE(rdr.done());

    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'c', 2));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'c', 2));

    ASSERT_FALSE(rdr.done());

    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'd', 3));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'd', 3));

    ASSERT_FALSE(rdr.done());

    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'e', 4));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'e', 4));

    ASSERT_FALSE(rdr.done());

    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'f', 5));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'f', 5));

    ASSERT_TRUE(rdr.done());

    EXPECT_EQ(rdr.peek(), taul::glyph::end(6));
    EXPECT_EQ(rdr.next(), taul::glyph::end(6));

    ASSERT_TRUE(rdr.done());
}

TEST(StringReaderTests, Reset) {
    taul::string_reader rdr("abcdef"_str);

    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'a', 0));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'b', 1));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'c', 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'd', 3));
    ASSERT_FALSE(rdr.done());

    rdr.reset(); // reset progress made

    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'a', 0));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'b', 1));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'c', 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'd', 3));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'e', 4));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'f', 5));
    ASSERT_TRUE(rdr.done());
    ASSERT_EQ(rdr.next(), taul::glyph::end(6));
}

TEST(StringReaderTests, ChangeInput) {
    taul::string_reader rdr("abc"_str);

    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'a', 0));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'b', 1));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'c', 2));
    ASSERT_TRUE(rdr.done());
    ASSERT_EQ(rdr.next(), taul::glyph::end(3));

    rdr.change_input("1234"_str);
    rdr.reset();

    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'1', 0));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'2', 1));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'3', 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'4', 3));
    ASSERT_TRUE(rdr.done());
    ASSERT_EQ(rdr.next(), taul::glyph::end(4));
}

