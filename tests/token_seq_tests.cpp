

#include <gtest/gtest.h>

#include <taul/all.h>


class TokenSeqTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr = nullptr;
    taul::grammar gram;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
        auto spec =
            taul::spec_writer()
            .lpr_decl("a")
            .lpr_decl("b")
            .lpr("a")
            .close()
            .lpr("b")
            .close()
            .done();
        gram = std::move(*taul::load(spec));
    }
};


TEST_F(TokenSeqTests, PrimaryCtor) {

    const char* txt = "abc def  ghi";

    taul::token_seq ts(txt);


    EXPECT_EQ(ts.str(), "abc def  ghi");

    EXPECT_EQ(ts.size(), 0);
    EXPECT_FALSE(ts.has_size());
    EXPECT_TRUE(ts.empty());

    EXPECT_EQ(ts.data(), nullptr);
    EXPECT_THROW(ts.at(0), std::out_of_range);
    EXPECT_THROW(ts[0], std::out_of_range);
    EXPECT_THROW(ts.back(), std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 0);
    EXPECT_EQ(ts.end_pos(), 12);
    EXPECT_FALSE(ts.at_end());
}

TEST_F(TokenSeqTests, CopyCtor) {

    const char* txt = "abc def  ghi";

    taul::token_seq ts(txt);

    ts.push(gram.lpr("a"), 3);
    ts.skip(1);
    ts.push(gram.lpr("b"), 3);
    ts.skip(2);

    taul::token_seq ts0(ts);


    EXPECT_EQ(ts0.str(), "abc def  ghi");

    EXPECT_EQ(ts0.size(), 2);
    EXPECT_TRUE(ts0.has_size());
    EXPECT_FALSE(ts0.empty());

    EXPECT_EQ(ts0.data(), &(ts0[0]));
    EXPECT_EQ(ts0.at(0), taul::token(gram.lpr("a"), "abc", 0));
    EXPECT_EQ(ts0.at(1), taul::token(gram.lpr("b"), "def", 4));
    EXPECT_EQ(ts0[0], taul::token(gram.lpr("a"), "abc", 0));
    EXPECT_EQ(ts0[1], taul::token(gram.lpr("b"), "def", 4));
    EXPECT_THROW(ts0.at(2), std::out_of_range);
    EXPECT_THROW(ts0[2], std::out_of_range);

    EXPECT_EQ(ts0.read_pos(), 9);
    EXPECT_EQ(ts0.end_pos(), 12);
    EXPECT_FALSE(ts0.at_end());
}

TEST_F(TokenSeqTests, MoveCtor) {

    const char* txt = "abc def  ghi";

    taul::token_seq ts(txt);

    ts.push(gram.lpr("a"), 3);
    ts.skip(1);
    ts.push(gram.lpr("b"), 3);
    ts.skip(2);

    taul::token_seq ts0(std::move(ts));


    EXPECT_EQ(ts0.str(), "abc def  ghi");

    EXPECT_EQ(ts0.size(), 2);
    EXPECT_TRUE(ts0.has_size());
    EXPECT_FALSE(ts0.empty());

    EXPECT_EQ(ts0.data(), &(ts0[0]));
    EXPECT_EQ(ts0.at(0), taul::token(gram.lpr("a"), "abc", 0));
    EXPECT_EQ(ts0.at(1), taul::token(gram.lpr("b"), "def", 4));
    EXPECT_EQ(ts0[0], taul::token(gram.lpr("a"), "abc", 0));
    EXPECT_EQ(ts0[1], taul::token(gram.lpr("b"), "def", 4));
    EXPECT_THROW(ts0.at(2), std::out_of_range);
    EXPECT_THROW(ts0[2], std::out_of_range);

    EXPECT_EQ(ts0.read_pos(), 9);
    EXPECT_EQ(ts0.end_pos(), 12);
    EXPECT_FALSE(ts0.at_end());
}

TEST_F(TokenSeqTests, CopyAssign) {

    const char* txt = "abc def  ghi";

    taul::token_seq ts(txt);

    ts.push(gram.lpr("a"), 3);
    ts.skip(1);
    ts.push(gram.lpr("b"), 3);
    ts.skip(2);

    taul::token_seq ts0("some other source string");

    ts0 = ts;


    EXPECT_EQ(ts0.str(), "abc def  ghi");

    EXPECT_EQ(ts0.size(), 2);
    EXPECT_TRUE(ts0.has_size());
    EXPECT_FALSE(ts0.empty());

    EXPECT_EQ(ts0.data(), &(ts0[0]));
    EXPECT_EQ(ts0.at(0), taul::token(gram.lpr("a"), "abc", 0));
    EXPECT_EQ(ts0.at(1), taul::token(gram.lpr("b"), "def", 4));
    EXPECT_EQ(ts0[0], taul::token(gram.lpr("a"), "abc", 0));
    EXPECT_EQ(ts0[1], taul::token(gram.lpr("b"), "def", 4));
    EXPECT_THROW(ts0.at(2), std::out_of_range);
    EXPECT_THROW(ts0[2], std::out_of_range);

    EXPECT_EQ(ts0.read_pos(), 9);
    EXPECT_EQ(ts0.end_pos(), 12);
    EXPECT_FALSE(ts0.at_end());
}

TEST_F(TokenSeqTests, MoveAssign) {

    const char* txt = "abc def  ghi";

    taul::token_seq ts(txt);

    ts.push(gram.lpr("a"), 3);
    ts.skip(1);
    ts.push(gram.lpr("b"), 3);
    ts.skip(2);

    taul::token_seq ts0("some other source string");

    ts0 = std::move(ts);


    EXPECT_EQ(ts0.str(), "abc def  ghi");

    EXPECT_EQ(ts0.size(), 2);
    EXPECT_TRUE(ts0.has_size());
    EXPECT_FALSE(ts0.empty());

    EXPECT_EQ(ts0.data(), &(ts0[0]));
    EXPECT_EQ(ts0.at(0), taul::token(gram.lpr("a"), "abc", 0));
    EXPECT_EQ(ts0.at(1), taul::token(gram.lpr("b"), "def", 4));
    EXPECT_EQ(ts0[0], taul::token(gram.lpr("a"), "abc", 0));
    EXPECT_EQ(ts0[1], taul::token(gram.lpr("b"), "def", 4));
    EXPECT_THROW(ts0.at(2), std::out_of_range);
    EXPECT_THROW(ts0[2], std::out_of_range);

    EXPECT_EQ(ts0.read_pos(), 9);
    EXPECT_EQ(ts0.end_pos(), 12);
    EXPECT_FALSE(ts0.at_end());
}

TEST_F(TokenSeqTests, Skip) {

    const char* txt = "abcdef";

    taul::token_seq ts(txt);


    EXPECT_EQ(ts.size(), 0);

    EXPECT_EQ(ts.read_pos(), 0);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    ts.skip(2);


    EXPECT_EQ(ts.size(), 0);

    EXPECT_EQ(ts.read_pos(), 2);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    ts.skip(0);


    EXPECT_EQ(ts.size(), 0);

    EXPECT_EQ(ts.read_pos(), 2);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    ts.push(gram.lpr("a"), 2);


    EXPECT_EQ(ts.size(), 1);

    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "cd", 2));

    EXPECT_EQ(ts.read_pos(), 4);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    ts.skip(2);


    EXPECT_EQ(ts.size(), 1);

    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "cd", 2));

    EXPECT_EQ(ts.read_pos(), 6);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_TRUE(ts.at_end());
}

TEST_F(TokenSeqTests, Skip_StrongGuaranteeOnThrow) {

    const char* txt = "abcdef";

    taul::token_seq ts(txt);

    ts.skip(2);
    ts.push(gram.lpr("a"), 2);


    EXPECT_EQ(ts.str(), "abcdef");

    EXPECT_EQ(ts.size(), 1);
    EXPECT_TRUE(ts.has_size());
    EXPECT_FALSE(ts.empty());

    EXPECT_EQ(ts.data(), &(ts[0]));
    EXPECT_EQ(ts.at(0), taul::token(gram.lpr("a"), "cd", 2));
    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "cd", 2));
    EXPECT_THROW(ts.at(1), std::out_of_range);
    EXPECT_THROW(ts[1], std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 4);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    EXPECT_THROW(ts.skip(3), std::out_of_range);


    // expect strong guarantee

    EXPECT_EQ(ts.str(), "abcdef");

    EXPECT_EQ(ts.size(), 1);
    EXPECT_TRUE(ts.has_size());
    EXPECT_FALSE(ts.empty());

    EXPECT_EQ(ts.data(), &(ts[0]));
    EXPECT_EQ(ts.at(0), taul::token(gram.lpr("a"), "cd", 2));
    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "cd", 2));
    EXPECT_THROW(ts.at(1), std::out_of_range);
    EXPECT_THROW(ts[1], std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 4);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());
}

TEST_F(TokenSeqTests, UndoSkips) {

    const char* txt = "abcdef";

    taul::token_seq ts(txt);


    ts.skip(2);
    ts.skip(3);


    EXPECT_EQ(ts.read_pos(), 5);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    ts.undo_skips();


    EXPECT_EQ(ts.read_pos(), 0);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    ts.push(gram.lpr("a"), 2);
    ts.skip(3);


    EXPECT_EQ(ts.read_pos(), 5);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    ts.undo_skips();


    EXPECT_EQ(ts.read_pos(), 2);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());
}

TEST_F(TokenSeqTests, Push) {

    const char* txt = "abcdef";

    taul::token_seq ts(txt);


    EXPECT_EQ(ts.size(), 0);

    EXPECT_EQ(ts.read_pos(), 0);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    ts.push(gram.lpr("a"), 2);


    EXPECT_EQ(ts.size(), 1);

    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "ab", 0));

    EXPECT_EQ(ts.read_pos(), 2);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    ts.skip(2);


    EXPECT_EQ(ts.size(), 1);

    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "ab", 0));

    EXPECT_EQ(ts.read_pos(), 4);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    ts.push(gram.lpr("a"), 0);


    EXPECT_EQ(ts.size(), 2);

    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "ab", 0));
    EXPECT_EQ(ts[1], taul::token(gram.lpr("a"), "", 4));

    EXPECT_EQ(ts.read_pos(), 4);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    ts.push(gram.lpr("a"), 2);


    EXPECT_EQ(ts.size(), 3);

    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "ab", 0));
    EXPECT_EQ(ts[1], taul::token(gram.lpr("a"), "", 4));
    EXPECT_EQ(ts[2], taul::token(gram.lpr("a"), "ef", 4));

    EXPECT_EQ(ts.read_pos(), 6);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_TRUE(ts.at_end());


    ts.push(gram.lpr("a"), 0);


    EXPECT_EQ(ts.size(), 4);

    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "ab", 0));
    EXPECT_EQ(ts[1], taul::token(gram.lpr("a"), "", 4));
    EXPECT_EQ(ts[2], taul::token(gram.lpr("a"), "ef", 4));
    EXPECT_EQ(ts[3], taul::token(gram.lpr("a"), "", 6));

    EXPECT_EQ(ts.read_pos(), 6);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_TRUE(ts.at_end());
}

TEST_F(TokenSeqTests, Push_StrongGuaranteeOnThrow) {

    const char* txt = "abcdef";

    taul::token_seq ts(txt);

    ts.push(gram.lpr("a"), 2);
    ts.skip(2);
    ts.push(gram.lpr("a"), 1);


    EXPECT_EQ(ts.str(), "abcdef");

    EXPECT_EQ(ts.size(), 2);
    EXPECT_TRUE(ts.has_size());
    EXPECT_FALSE(ts.empty());

    EXPECT_EQ(ts.data(), &(ts[0]));
    EXPECT_EQ(ts.at(0), taul::token(gram.lpr("a"), "ab", 0));
    EXPECT_EQ(ts.at(1), taul::token(gram.lpr("a"), "e", 4));
    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "ab", 0));
    EXPECT_EQ(ts[1], taul::token(gram.lpr("a"), "e", 4));
    EXPECT_THROW(ts.at(2), std::out_of_range);
    EXPECT_THROW(ts[2], std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 5);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    EXPECT_THROW(ts.push(gram.lpr("a"), 2), std::out_of_range);


    // expect strong guarantee

    EXPECT_EQ(ts.str(), "abcdef");

    EXPECT_EQ(ts.size(), 2);
    EXPECT_TRUE(ts.has_size());
    EXPECT_FALSE(ts.empty());

    EXPECT_EQ(ts.data(), &(ts[0]));
    EXPECT_EQ(ts.at(0), taul::token(gram.lpr("a"), "ab", 0));
    EXPECT_EQ(ts.at(1), taul::token(gram.lpr("a"), "e", 4));
    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "ab", 0));
    EXPECT_EQ(ts[1], taul::token(gram.lpr("a"), "e", 4));
    EXPECT_THROW(ts.at(2), std::out_of_range);
    EXPECT_THROW(ts[2], std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 5);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());
}

TEST_F(TokenSeqTests, PushFailure) {

    const char* txt = "abcdef";

    taul::token_seq ts(txt);


    EXPECT_EQ(ts.size(), 0);

    EXPECT_EQ(ts.read_pos(), 0);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    ts.push_failure(2);


    EXPECT_EQ(ts.size(), 1);

    EXPECT_EQ(ts[0], taul::token::failure("ab", 0));

    EXPECT_EQ(ts.read_pos(), 2);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    ts.skip(2);


    EXPECT_EQ(ts.size(), 1);

    EXPECT_EQ(ts[0], taul::token::failure("ab", 0));

    EXPECT_EQ(ts.read_pos(), 4);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    ts.push_failure(0);


    EXPECT_EQ(ts.size(), 2);

    EXPECT_EQ(ts[0], taul::token::failure("ab", 0));
    EXPECT_EQ(ts[1], taul::token::failure("", 4));

    EXPECT_EQ(ts.read_pos(), 4);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    ts.push_failure(2);


    EXPECT_EQ(ts.size(), 3);

    EXPECT_EQ(ts[0], taul::token::failure("ab", 0));
    EXPECT_EQ(ts[1], taul::token::failure("", 4));
    EXPECT_EQ(ts[2], taul::token::failure("ef", 4));

    EXPECT_EQ(ts.read_pos(), 6);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_TRUE(ts.at_end());


    ts.push_failure(0);


    EXPECT_EQ(ts.size(), 4);

    EXPECT_EQ(ts[0], taul::token::failure("ab", 0));
    EXPECT_EQ(ts[1], taul::token::failure("", 4));
    EXPECT_EQ(ts[2], taul::token::failure("ef", 4));
    EXPECT_EQ(ts[3], taul::token::failure("", 6));

    EXPECT_EQ(ts.read_pos(), 6);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_TRUE(ts.at_end());
}

TEST_F(TokenSeqTests, PushFailure_StrongGuaranteeOnThrow) {

    const char* txt = "abcdef";

    taul::token_seq ts(txt);

    ts.push_failure(2);
    ts.skip(2);
    ts.push_failure(1);


    EXPECT_EQ(ts.str(), "abcdef");

    EXPECT_EQ(ts.size(), 2);
    EXPECT_TRUE(ts.has_size());
    EXPECT_FALSE(ts.empty());

    EXPECT_EQ(ts.data(), &(ts[0]));
    EXPECT_EQ(ts.at(0), taul::token::failure("ab", 0));
    EXPECT_EQ(ts.at(1), taul::token::failure("e", 4));
    EXPECT_EQ(ts[0], taul::token::failure("ab", 0));
    EXPECT_EQ(ts[1], taul::token::failure("e", 4));
    EXPECT_THROW(ts.at(2), std::out_of_range);
    EXPECT_THROW(ts[2], std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 5);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());


    EXPECT_THROW(ts.push_failure(2), std::out_of_range);


    // expect strong guarantee

    EXPECT_EQ(ts.str(), "abcdef");

    EXPECT_EQ(ts.size(), 2);
    EXPECT_TRUE(ts.has_size());
    EXPECT_FALSE(ts.empty());

    EXPECT_EQ(ts.data(), &(ts[0]));
    EXPECT_EQ(ts.at(0), taul::token::failure("ab", 0));
    EXPECT_EQ(ts.at(1), taul::token::failure("e", 4));
    EXPECT_EQ(ts[0], taul::token::failure("ab", 0));
    EXPECT_EQ(ts[1], taul::token::failure("e", 4));
    EXPECT_THROW(ts.at(2), std::out_of_range);
    EXPECT_THROW(ts[2], std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 5);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());
}

TEST_F(TokenSeqTests, Pop) {

    const char* txt = "a b c d";

    taul::token_seq ts(txt);

    ts.push(gram.lpr("a"), 1);
    ts.skip(1);
    ts.push(gram.lpr("a"), 1);
    ts.skip(1);
    ts.push(gram.lpr("a"), 1);
    ts.skip(1);
    ts.push(gram.lpr("a"), 1);


    EXPECT_EQ(ts.size(), 4);

    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "a", 0));
    EXPECT_EQ(ts[1], taul::token(gram.lpr("a"), "b", 2));
    EXPECT_EQ(ts[2], taul::token(gram.lpr("a"), "c", 4));
    EXPECT_EQ(ts[3], taul::token(gram.lpr("a"), "d", 6));
    EXPECT_THROW(ts[4], std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 7);
    EXPECT_EQ(ts.end_pos(), 7);
    EXPECT_TRUE(ts.at_end());


    // test pop w/ default n == 1

    ts.pop();


    EXPECT_EQ(ts.size(), 3);

    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "a", 0));
    EXPECT_EQ(ts[1], taul::token(gram.lpr("a"), "b", 2));
    EXPECT_EQ(ts[2], taul::token(gram.lpr("a"), "c", 4));
    EXPECT_THROW(ts[3], std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 6);
    EXPECT_EQ(ts.end_pos(), 7);
    EXPECT_FALSE(ts.at_end());


    // test pop w/ n == 0

    ts.pop(0);


    EXPECT_EQ(ts.size(), 3);

    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "a", 0));
    EXPECT_EQ(ts[1], taul::token(gram.lpr("a"), "b", 2));
    EXPECT_EQ(ts[2], taul::token(gram.lpr("a"), "c", 4));
    EXPECT_THROW(ts[3], std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 6);
    EXPECT_EQ(ts.end_pos(), 7);
    EXPECT_FALSE(ts.at_end());


    // test pop w/ n > 1

    ts.pop(2);


    EXPECT_EQ(ts.size(), 1);

    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "a", 0));
    EXPECT_THROW(ts[1], std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 2);
    EXPECT_EQ(ts.end_pos(), 7);
    EXPECT_FALSE(ts.at_end());


    // test pop w/ n > size()

    ts.pop(2);


    EXPECT_EQ(ts.size(), 0);

    EXPECT_THROW(ts[0], std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 0);
    EXPECT_EQ(ts.end_pos(), 7);
    EXPECT_FALSE(ts.at_end());
}

TEST_F(TokenSeqTests, PopAll) {

    const char* txt = "a b c d";

    taul::token_seq ts(txt);

    ts.push(gram.lpr("a"), 1);
    ts.skip(1);
    ts.push(gram.lpr("a"), 1);
    ts.skip(1);
    ts.push(gram.lpr("a"), 1);
    ts.skip(1);
    ts.push(gram.lpr("a"), 1);


    EXPECT_EQ(ts.size(), 4);

    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "a", 0));
    EXPECT_EQ(ts[1], taul::token(gram.lpr("a"), "b", 2));
    EXPECT_EQ(ts[2], taul::token(gram.lpr("a"), "c", 4));
    EXPECT_EQ(ts[3], taul::token(gram.lpr("a"), "d", 6));
    EXPECT_THROW(ts[4], std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 7);
    EXPECT_EQ(ts.end_pos(), 7);
    EXPECT_TRUE(ts.at_end());


    ts.pop_all();


    EXPECT_EQ(ts.size(), 0);

    EXPECT_THROW(ts[0], std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 0);
    EXPECT_EQ(ts.end_pos(), 7);
    EXPECT_FALSE(ts.at_end());
}

TEST_F(TokenSeqTests, Reset) {

    const char* txt = "abc def  ghi";

    taul::token_seq ts(txt);

    ts.push(gram.lpr("a"), 3);
    ts.skip(1);
    ts.push(gram.lpr("b"), 3);
    ts.skip(2);


    EXPECT_EQ(ts.str(), "abc def  ghi");

    EXPECT_EQ(ts.size(), 2);
    EXPECT_TRUE(ts.has_size());
    EXPECT_FALSE(ts.empty());

    EXPECT_EQ(ts.data(), &(ts[0]));
    EXPECT_EQ(ts.at(0), taul::token(gram.lpr("a"), "abc", 0));
    EXPECT_EQ(ts.at(1), taul::token(gram.lpr("b"), "def", 4));
    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "abc", 0));
    EXPECT_EQ(ts[1], taul::token(gram.lpr("b"), "def", 4));
    EXPECT_THROW(ts.at(2), std::out_of_range);
    EXPECT_THROW(ts[2], std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 9);
    EXPECT_EQ(ts.end_pos(), 12);
    EXPECT_FALSE(ts.at_end());


    ts.reset("abcdef");


    EXPECT_EQ(ts.str(), "abcdef");

    EXPECT_EQ(ts.size(), 0);
    EXPECT_FALSE(ts.has_size());
    EXPECT_TRUE(ts.empty());

    EXPECT_EQ(ts.data(), nullptr);
    EXPECT_THROW(ts.at(0), std::out_of_range);
    EXPECT_THROW(ts[0], std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 0);
    EXPECT_EQ(ts.end_pos(), 6);
    EXPECT_FALSE(ts.at_end());
}

TEST_F(TokenSeqTests, Iterators) {

    const char* txt = "a b c d";

    taul::token_seq ts(txt);
    taul::token_seq ts0(txt); // <- for iterating over no tokens

    ts.push(gram.lpr("a"), 1);
    ts.skip(1);
    ts.push(gram.lpr("a"), 1);
    ts.skip(1);
    ts.push(gram.lpr("a"), 1);
    ts.skip(1);
    ts.push(gram.lpr("a"), 1);


    EXPECT_EQ(ts.size(), 4);

    EXPECT_EQ(ts[0], taul::token(gram.lpr("a"), "a", 0));
    EXPECT_EQ(ts[1], taul::token(gram.lpr("a"), "b", 2));
    EXPECT_EQ(ts[2], taul::token(gram.lpr("a"), "c", 4));
    EXPECT_EQ(ts[3], taul::token(gram.lpr("a"), "d", 6));
    EXPECT_THROW(ts[4], std::out_of_range);

    EXPECT_EQ(ts.read_pos(), 7);
    EXPECT_EQ(ts.end_pos(), 7);
    EXPECT_TRUE(ts.at_end());


    {
        auto it = ts.begin();
        if (it != ts.end()) EXPECT_EQ(*it, ts[0]); else ADD_FAILURE();
        std::advance(it, 1);
        if (it != ts.end()) EXPECT_EQ(*it, ts[1]); else ADD_FAILURE();
        std::advance(it, 1);
        if (it != ts.end()) EXPECT_EQ(*it, ts[2]); else ADD_FAILURE();
        std::advance(it, 1);
        if (it != ts.end()) EXPECT_EQ(*it, ts[3]); else ADD_FAILURE();
        std::advance(it, 1);
        EXPECT_EQ(it, ts.end());
    }

    {
        auto it = ts.cbegin();
        if (it != ts.cend()) EXPECT_EQ(*it, ts[0]); else ADD_FAILURE();
        std::advance(it, 1);
        if (it != ts.cend()) EXPECT_EQ(*it, ts[1]); else ADD_FAILURE();
        std::advance(it, 1);
        if (it != ts.cend()) EXPECT_EQ(*it, ts[2]); else ADD_FAILURE();
        std::advance(it, 1);
        if (it != ts.cend()) EXPECT_EQ(*it, ts[3]); else ADD_FAILURE();
        std::advance(it, 1);
        EXPECT_EQ(it, ts.cend());
    }

    {
        auto it = ts.rbegin();
        if (it != ts.rend()) EXPECT_EQ(*it, ts[3]); else ADD_FAILURE();
        std::advance(it, 1);
        if (it != ts.rend()) EXPECT_EQ(*it, ts[2]); else ADD_FAILURE();
        std::advance(it, 1);
        if (it != ts.rend()) EXPECT_EQ(*it, ts[1]); else ADD_FAILURE();
        std::advance(it, 1);
        if (it != ts.rend()) EXPECT_EQ(*it, ts[0]); else ADD_FAILURE();
        std::advance(it, 1);
        EXPECT_EQ(it, ts.rend());
    }

    {
        auto it = ts.crbegin();
        if (it != ts.crend()) EXPECT_EQ(*it, ts[3]); else ADD_FAILURE();
        std::advance(it, 1);
        if (it != ts.crend()) EXPECT_EQ(*it, ts[2]); else ADD_FAILURE();
        std::advance(it, 1);
        if (it != ts.crend()) EXPECT_EQ(*it, ts[1]); else ADD_FAILURE();
        std::advance(it, 1);
        if (it != ts.crend()) EXPECT_EQ(*it, ts[0]); else ADD_FAILURE();
        std::advance(it, 1);
        EXPECT_EQ(it, ts.crend());
    }


    {
        EXPECT_EQ(ts0.begin(), ts0.end());
        EXPECT_EQ(ts0.cbegin(), ts0.cend());
        EXPECT_EQ(ts0.rbegin(), ts0.rend());
        EXPECT_EQ(ts0.crbegin(), ts0.crend());
    }
}

TEST_F(TokenSeqTests, Pointers) {

    const char* txt = "a b c d";

    taul::token_seq ts(txt);
    taul::token_seq ts0(txt); // <- for iterating over no tokens

    ts.push(gram.lpr("a"), 1);
    ts.skip(1);
    ts.push(gram.lpr("a"), 1);
    ts.skip(1);
    ts.push(gram.lpr("a"), 1);
    ts.skip(1);
    ts.push(gram.lpr("a"), 1);
    

    ASSERT_TRUE(ts.data());


    // test pointers w/ regards to tokens

    EXPECT_EQ(ts.data() + 0, &(ts.at(0)));
    EXPECT_EQ(ts.data() + 1, &(ts.at(1)));
    EXPECT_EQ(ts.data() + 2, &(ts.at(2)));
    EXPECT_EQ(ts.data() + 3, &(ts.at(3)));

    EXPECT_EQ(ts.data() + 0, &(ts[0]));
    EXPECT_EQ(ts.data() + 1, &(ts[1]));
    EXPECT_EQ(ts.data() + 2, &(ts[2]));
    EXPECT_EQ(ts.data() + 3, &(ts[3]));

    EXPECT_EQ(ts.data() + 3, &(ts.back()));

    EXPECT_EQ(ts.data() + 0, &(*std::next(ts.begin(), 0)));
    EXPECT_EQ(ts.data() + 1, &(*std::next(ts.begin(), 1)));
    EXPECT_EQ(ts.data() + 2, &(*std::next(ts.begin(), 2)));
    EXPECT_EQ(ts.data() + 3, &(*std::next(ts.begin(), 3)));


    // test pointers w/ regards to chars

    EXPECT_EQ(ts[0].str().data(), txt + 0);
    EXPECT_EQ(ts[1].str().data(), txt + 2);
    EXPECT_EQ(ts[2].str().data(), txt + 4);
    EXPECT_EQ(ts[3].str().data(), txt + 6);
}

TEST_F(TokenSeqTests, RangeStr) {

    taul::token_seq ts("abc def ghi jkl mno");

    ts.push(gram.lpr("a"), 3);
    ts.skip(1);
    ts.push(gram.lpr("a"), 3);
    ts.skip(1);
    ts.push(gram.lpr("a"), 3);
    ts.skip(1);
    ts.push(gram.lpr("a"), 3);
    ts.skip(1);
    ts.push(gram.lpr("a"), 3);

    
    ASSERT_EQ(ts.size(), 5);


    // TODO: in theory we should *maybe* also test that the pointed-to memory
    //       is of the source string... or is this even needed?


    EXPECT_EQ(ts.range_str(0, 5), "abc def ghi jkl mno");

    EXPECT_EQ(ts.range_str(0, 4), "abc def ghi jkl");
    EXPECT_EQ(ts.range_str(1, 4), "def ghi jkl mno");

    EXPECT_EQ(ts.range_str(0, 3), "abc def ghi");
    EXPECT_EQ(ts.range_str(1, 3), "def ghi jkl");
    EXPECT_EQ(ts.range_str(2, 3), "ghi jkl mno");

    EXPECT_EQ(ts.range_str(0, 2), "abc def");
    EXPECT_EQ(ts.range_str(1, 2), "def ghi");
    EXPECT_EQ(ts.range_str(2, 2), "ghi jkl");
    EXPECT_EQ(ts.range_str(3, 2), "jkl mno");

    EXPECT_EQ(ts.range_str(0, 1), "abc");
    EXPECT_EQ(ts.range_str(1, 1), "def");
    EXPECT_EQ(ts.range_str(2, 1), "ghi");
    EXPECT_EQ(ts.range_str(3, 1), "jkl");
    EXPECT_EQ(ts.range_str(4, 1), "mno");

    EXPECT_EQ(ts.range_str(0, 0), "");
    EXPECT_EQ(ts.range_str(1, 0), "");
    EXPECT_EQ(ts.range_str(2, 0), "");
    EXPECT_EQ(ts.range_str(3, 0), "");
    EXPECT_EQ(ts.range_str(4, 0), "");


    EXPECT_THROW(ts.range_str(5, 1), std::out_of_range);
    EXPECT_THROW(ts.range_str(0, 6), std::out_of_range);
    EXPECT_THROW(ts.range_str(2, 4), std::out_of_range);
    EXPECT_THROW(ts.range_str(10, 1), std::out_of_range);
}

