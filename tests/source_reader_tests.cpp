

#include <gtest/gtest.h>

#include <taul/source_reader.h>

#include "helpers/test_glyph_observer.h"
#include "parameterized_tests/glyph_stream_tests.h"


using namespace taul::string_literals;


// test w/ non-empty input

GlyphStreamParam _make_param_1() {
    auto stream = std::make_shared<taul::source_reader>("abcdef"_str);
    std::size_t n_after_done = 6;
    return GlyphStreamParam::init(stream, n_after_done);
}

INSTANTIATE_TEST_SUITE_P(
    SourceReader_NonEmptyInput,
    GlyphStreamTests, 
    testing::Values(_make_param_1()));


// test w/ empty input

GlyphStreamParam _make_param_2() {
    auto stream = std::make_shared<taul::source_reader>(""_str);
    std::size_t n_after_done = 0;
    return GlyphStreamParam::init(stream, n_after_done);
}

INSTANTIATE_TEST_SUITE_P(
    SourceReader_EmptyInput,
    GlyphStreamTests, 
    testing::Values(_make_param_2()));


TEST(SourceReaderTests, Observers) {
    taul::source_reader rdr(taul::str(taul::convert_encoding<char>(taul::utf8, taul::utf8, u8"abc123Δ魂💩").value()));
    test_glyph_observer obsvr;

    rdr.bind_observer(&obsvr);

    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'a', 0));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'b', 1));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'c', 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'1', 3));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'2', 4));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'3', 5));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'Δ', 6, 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'魂', 8, 3));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'💩', 11, 4));
    ASSERT_TRUE(rdr.done());
    ASSERT_EQ(rdr.next(), taul::glyph::end(15));

    test_glyph_observer expected;
    expected.observe(taul::glyph::normal(U'a', 0));
    expected.observe(taul::glyph::normal(U'b', 1));
    expected.observe(taul::glyph::normal(U'c', 2));
    expected.observe(taul::glyph::normal(U'1', 3));
    expected.observe(taul::glyph::normal(U'2', 4));
    expected.observe(taul::glyph::normal(U'3', 5));
    expected.observe(taul::glyph::normal(U'Δ', 6, 2));
    expected.observe(taul::glyph::normal(U'魂', 8, 3));
    expected.observe(taul::glyph::normal(U'💩', 11, 4));
    expected.observe(taul::glyph::end(15));

    EXPECT_EQ(obsvr.output, expected.output);
}

TEST(SourceReaderTests, Observers_InputEncodingNotUTF8) {
    taul::source_reader rdr(
        taul::str(taul::convert_encoding<char>(taul::utf8, taul::utf16, u8"abc123Δ魂💩").value()),
        taul::utf16);
    test_glyph_observer obsvr;

    rdr.bind_observer(&obsvr);

    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'a', 0, 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'b', 2, 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'c', 4, 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'1', 6, 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'2', 8, 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'3', 10, 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'Δ', 12, 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'魂', 14, 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'💩', 16, 4));
    ASSERT_TRUE(rdr.done());
    ASSERT_EQ(rdr.next(), taul::glyph::end(20));

    test_glyph_observer expected;
    expected.observe(taul::glyph::normal(U'a', 0, 2));
    expected.observe(taul::glyph::normal(U'b', 2, 2));
    expected.observe(taul::glyph::normal(U'c', 4, 2));
    expected.observe(taul::glyph::normal(U'1', 6, 2));
    expected.observe(taul::glyph::normal(U'2', 8, 2));
    expected.observe(taul::glyph::normal(U'3', 10, 2));
    expected.observe(taul::glyph::normal(U'Δ', 12, 2));
    expected.observe(taul::glyph::normal(U'魂', 14, 2));
    expected.observe(taul::glyph::normal(U'💩', 16, 4));
    expected.observe(taul::glyph::end(20));

    EXPECT_EQ(obsvr.output, expected.output);
}

TEST(SourceReaderTests, PeekNextAndDone) {
    taul::source_reader rdr(taul::str(taul::convert_encoding<char>(taul::utf8, taul::utf8, u8"abc123Δ魂💩").value()));

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
    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'1', 3));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'1', 3));

    ASSERT_FALSE(rdr.done());
    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'2', 4));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'2', 4));

    ASSERT_FALSE(rdr.done());
    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'3', 5));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'3', 5));

    ASSERT_FALSE(rdr.done());
    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'Δ', 6, 2));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'Δ', 6, 2));

    ASSERT_FALSE(rdr.done());
    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'魂', 8, 3));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'魂', 8, 3));

    ASSERT_FALSE(rdr.done());
    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'💩', 11, 4));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'💩', 11, 4));

    ASSERT_TRUE(rdr.done());
    EXPECT_EQ(rdr.peek(), taul::glyph::end(15));
    EXPECT_EQ(rdr.next(), taul::glyph::end(15));

    ASSERT_TRUE(rdr.done());
}

TEST(SourceReaderTests, PeekNextAndDone_InputEncodingNotUTF8) {
    taul::source_reader rdr(
        taul::str(taul::convert_encoding<char>(taul::utf8, taul::utf16, u8"abc123Δ魂💩").value()),
        taul::utf16);

    ASSERT_FALSE(rdr.done());
    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'a', 0, 2));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'a', 0, 2));

    ASSERT_FALSE(rdr.done());
    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'b', 2, 2));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'b', 2, 2));

    ASSERT_FALSE(rdr.done());
    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'c', 4, 2));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'c', 4, 2));

    ASSERT_FALSE(rdr.done());
    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'1', 6, 2));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'1', 6, 2));

    ASSERT_FALSE(rdr.done());
    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'2', 8, 2));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'2', 8, 2));

    ASSERT_FALSE(rdr.done());
    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'3', 10, 2));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'3', 10, 2));

    ASSERT_FALSE(rdr.done());
    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'Δ', 12, 2));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'Δ', 12, 2));

    ASSERT_FALSE(rdr.done());
    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'魂', 14, 2));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'魂', 14, 2));

    ASSERT_FALSE(rdr.done());
    EXPECT_EQ(rdr.peek(), taul::glyph::normal(U'💩', 16, 4));
    EXPECT_EQ(rdr.next(), taul::glyph::normal(U'💩', 16, 4));

    ASSERT_TRUE(rdr.done());
    EXPECT_EQ(rdr.peek(), taul::glyph::end(20));
    EXPECT_EQ(rdr.next(), taul::glyph::end(20));

    ASSERT_TRUE(rdr.done());
}

TEST(SourceReaderTests, Reset) {
    taul::source_reader rdr("abcdef"_str);

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

TEST(SourceReaderTests, ChangeInput) {
    taul::source_reader rdr("abc"_str);

    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'a', 0));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'b', 1));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'c', 2));
    ASSERT_TRUE(rdr.done());
    ASSERT_EQ(rdr.next(), taul::glyph::end(3));

    rdr.change_input(taul::str(taul::convert_encoding<char>(taul::utf8, taul::utf8, u8"123Δ魂💩").value()));
    rdr.reset();

    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'1', 0));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'2', 1));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'3', 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'Δ', 3, 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'魂', 5, 3));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'💩', 8, 4));
    ASSERT_TRUE(rdr.done());
    ASSERT_EQ(rdr.next(), taul::glyph::end(12));
}

TEST(SourceReaderTests, ChangeInput_InputEncodingNotUTF8) {
    taul::source_reader rdr("abc"_str); // <- take note that we ALSO change encoding below too!

    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'a', 0));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'b', 1));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'c', 2));
    ASSERT_TRUE(rdr.done());
    ASSERT_EQ(rdr.next(), taul::glyph::end(3));

    rdr.change_input(
        taul::str(taul::convert_encoding<char>(taul::utf8, taul::utf16, u8"123Δ魂💩").value()),
        taul::utf16);
    rdr.reset();

    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'1', 0, 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'2', 2, 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'3', 4, 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'Δ', 6, 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'魂', 8, 2));
    ASSERT_EQ(rdr.next(), taul::glyph::normal(U'💩', 10, 4));
    ASSERT_TRUE(rdr.done());
    ASSERT_EQ(rdr.next(), taul::glyph::end(14));
}

