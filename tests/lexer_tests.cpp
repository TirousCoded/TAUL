

#include <gtest/gtest.h>

#include <taul/all.h>


// each of our tests will involve calling taul::lexer::operator(), w/ us
// thusly performing tests for each ctor/assignment, but not operator() itself

class test_lexer_state final : public taul::lexer_state {
public:

    // these lexer state types *shouldn't* be mutable, but we'll make a dirty
    // little exception here for the sake of ease of testing

    // this is to make sure the lexer function actually acknowledges the lexer state,
    // nothing more

    std::string_view last = {};
};

taul::token text_lexer_fn(
    const std::shared_ptr<taul::lexer_state>& state,
    std::string_view txt,
    taul::source_pos offset,
    const std::shared_ptr<taul::logger>& lgr) {
    TAUL_LOG(lgr, "test_lexer_fn called w/ \"{}\" w/ offset {}", (std::string)txt, offset);
    assert(state);
    static_cast<test_lexer_state*>(state.get())->last = txt;
    TAUL_IN_BOUNDS(offset, 0, txt.length() + 1); // the '+ 1' is due to allowing offset == txt.length()
    return taul::token::failure(txt.substr(offset), offset);
}


class LexerTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr = nullptr;
    std::shared_ptr<test_lexer_state> state = nullptr;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
        state = std::make_shared<test_lexer_state>();
    }
};


TEST_F(LexerTests, defaultCtor) {

    taul::lexer lex{};

    EXPECT_EQ(lex("abc", lgr), taul::token{});

    EXPECT_EQ(lex("ghi", 2, lgr), taul::token{});
}

TEST_F(LexerTests, directCtor) {

    taul::lexer lex(text_lexer_fn, state);

    EXPECT_EQ(state->last, "");

    EXPECT_EQ(lex("abc", lgr), taul::token::failure("abc"));

    EXPECT_EQ(state->last, "abc");

    EXPECT_EQ(lex("ghi", 2, lgr), taul::token::failure("i", 2));

    EXPECT_EQ(state->last, "ghi");
}

TEST_F(LexerTests, copyCtor) {

    taul::lexer old_lex(text_lexer_fn, state);

    taul::lexer lex(old_lex);

    EXPECT_EQ(state->last, "");

    EXPECT_EQ(old_lex("abc", lgr), taul::token::failure("abc"));

    EXPECT_EQ(state->last, "abc");

    EXPECT_EQ(lex("def", lgr), taul::token::failure("def"));

    EXPECT_EQ(state->last, "def");

    EXPECT_EQ(lex("ghi", 2, lgr), taul::token::failure("i", 2));

    EXPECT_EQ(state->last, "ghi");
}

TEST_F(LexerTests, moveCtor) {

    taul::lexer old_lex(text_lexer_fn, state);

    taul::lexer lex(std::move(old_lex));

    EXPECT_EQ(state->last, "");

    EXPECT_EQ(lex("abc", lgr), taul::token::failure("abc"));

    EXPECT_EQ(state->last, "abc");

    EXPECT_EQ(lex("ghi", 2, lgr), taul::token::failure("i", 2));

    EXPECT_EQ(state->last, "ghi");
}

TEST_F(LexerTests, copyAssign) {

    taul::lexer old_lex(text_lexer_fn, state);

    taul::lexer lex{};

    lex = old_lex;

    EXPECT_EQ(state->last, "");

    EXPECT_EQ(old_lex("abc", lgr), taul::token::failure("abc"));

    EXPECT_EQ(state->last, "abc");

    EXPECT_EQ(lex("def", lgr), taul::token::failure("def"));

    EXPECT_EQ(state->last, "def");

    EXPECT_EQ(lex("ghi", 2, lgr), taul::token::failure("i", 2));

    EXPECT_EQ(state->last, "ghi");
}

TEST_F(LexerTests, moveAssign) {

    taul::lexer old_lex(text_lexer_fn, state);

    taul::lexer lex{};

    lex = std::move(old_lex);

    EXPECT_EQ(state->last, "");

    EXPECT_EQ(lex("abc", lgr), taul::token::failure("abc"));

    EXPECT_EQ(state->last, "abc");

    EXPECT_EQ(lex("ghi", 2, lgr), taul::token::failure("i", 2));

    EXPECT_EQ(state->last, "ghi");
}

TEST_F(LexerTests, moveAssign_ontoSelf) {

    taul::lexer lex(text_lexer_fn, state);

    lex = std::move(lex);

    EXPECT_EQ(state->last, "");

    EXPECT_EQ(lex("abc", lgr), taul::token::failure("abc"));

    EXPECT_EQ(state->last, "abc");

    EXPECT_EQ(lex("ghi", 2, lgr), taul::token::failure("i", 2));

    EXPECT_EQ(state->last, "ghi");
}

