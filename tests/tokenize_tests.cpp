

#include <gtest/gtest.h>

#include <taul/all.h>


class test_lexer_state final : public taul::lexer_state {
public:

    taul::grammar gram;


    inline test_lexer_state(taul::grammar x) 
        : lexer_state(), 
        gram(std::move(x)) {}
};

// we'll have it be that test_lexer_fn_1 will match 'a' w/ a token of it, while
// matching 'b' with a 0 length token, and failing everything else

taul::token test_lexer_fn_1(
    const std::shared_ptr<taul::lexer_state>& state,
    std::string_view txt,
    taul::source_pos offset,
    const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(state);
    TAUL_IN_BOUNDS(offset, 0, txt.length() + 1);
    const auto& gram = static_cast<test_lexer_state*>(state.get())->gram;
    TAUL_ASSERT(gram.contains_lpr("lpr0"));
    if (txt.length() > offset && txt[offset] == 'a') {
        return taul::token(gram.lpr("lpr0"), txt.substr(offset, 1), offset);
    }
    else if (txt.length() > offset && txt[offset] == 'b') {
        return taul::token(gram.lpr("lpr0"), "", offset);
    }
    else {
        return taul::token::failure("", offset);
    }
}

// we'll have it be that test_lexer_fn_2 will match 0 length tokens for any
// character, but will fail if said character is 'b'

taul::token test_lexer_fn_2(
    const std::shared_ptr<taul::lexer_state>& state,
    std::string_view txt,
    taul::source_pos offset,
    const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(state);
    TAUL_IN_BOUNDS(offset, 0, txt.length() + 1);
    const auto& gram = static_cast<test_lexer_state*>(state.get())->gram;
    TAUL_ASSERT(gram.contains_lpr("lpr1"));
    if (txt.length() <= offset || txt[offset] != 'b') {
        return taul::token(gram.lpr("lpr1"), "", offset);
    }
    else {
        return taul::token::failure("", offset);
    }
}

class TokenizeTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr;
    taul::grammar gram;
    taul::lexer lex1, lex2;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
        const auto spec =
            taul::spec_writer()
            .lpr_decl("lpr0")
            .lpr_decl("lpr1")
            .lpr("lpr0")
            .close()
            .lpr("lpr1")
            .close()
            .done();
        gram = *taul::load(spec, lgr);
        TAUL_LOG(lgr, "{}", gram);
        lex1 = taul::lexer(test_lexer_fn_1, std::make_shared<test_lexer_state>(gram));
        lex2 = taul::lexer(test_lexer_fn_2, std::make_shared<test_lexer_state>(gram));
    }
};


TEST_F(TokenizeTests, tokenize_into) {

    // the below tests the semantics of when match failures should and
    // shouldn't produce failure tokens (ie. there's no failure token
    // at the end of the below) and tests the merging of contiguous
    // failure tokens that are output

    std::vector<taul::token> tkns0, tkns1, tkns2, tkns3, tkns4, tkns5;

    // test w/out offset

    tkns0.push_back(taul::token::failure("abc", 10));

    taul::tokenize_into(tkns0, lex1, "aaa00aa", lgr);

    if (tkns0.size() == 7) {
        EXPECT_EQ(tkns0[0], taul::token::failure("abc", 10));
        EXPECT_EQ(tkns0[1], taul::token(gram.lpr("lpr0"), "a", 0));
        EXPECT_EQ(tkns0[2], taul::token(gram.lpr("lpr0"), "a", 1));
        EXPECT_EQ(tkns0[3], taul::token(gram.lpr("lpr0"), "a", 2));
        EXPECT_EQ(tkns0[4], taul::token::failure("00", 3));
        EXPECT_EQ(tkns0[5], taul::token(gram.lpr("lpr0"), "a", 5));
        EXPECT_EQ(tkns0[6], taul::token(gram.lpr("lpr0"), "a", 6));
    }
    else ADD_FAILURE();

    // test w/ offset

    tkns1.push_back(taul::token::failure("abc", 10));

    taul::tokenize_into(tkns1, lex1, "00a00aa", 2, lgr);

    if (tkns1.size() == 5) {
        EXPECT_EQ(tkns1[0], taul::token::failure("abc", 10));
        EXPECT_EQ(tkns1[1], taul::token(gram.lpr("lpr0"), "a", 2));
        EXPECT_EQ(tkns1[2], taul::token::failure("00", 3));
        EXPECT_EQ(tkns1[3], taul::token(gram.lpr("lpr0"), "a", 5));
        EXPECT_EQ(tkns1[4], taul::token(gram.lpr("lpr0"), "a", 6));
    }
    else ADD_FAILURE();

    // test 0 length tokens w/out offset

    tkns2.push_back(taul::token::failure("abc", 10));

    taul::tokenize_into(tkns2, lex2, "000b", lgr);

    if (tkns2.size() == 8) {
        EXPECT_EQ(tkns2[0], taul::token::failure("abc", 10));
        EXPECT_EQ(tkns2[1], taul::token(gram.lpr("lpr1"), "", 0));
        EXPECT_EQ(tkns2[2], taul::token::failure("0", 0));
        EXPECT_EQ(tkns2[3], taul::token(gram.lpr("lpr1"), "", 1));
        EXPECT_EQ(tkns2[4], taul::token::failure("0", 1));
        EXPECT_EQ(tkns2[5], taul::token(gram.lpr("lpr1"), "", 2));
        EXPECT_EQ(tkns2[6], taul::token::failure("0b", 2));
        EXPECT_EQ(tkns2[7], taul::token(gram.lpr("lpr1"), "", 4));
    }
    else ADD_FAILURE();

    // test 0 length tokens w/ offset

    tkns3.push_back(taul::token::failure("abc", 10));

    taul::tokenize_into(tkns3, lex2, "000b", 2, lgr);

    if (tkns3.size() == 4) {
        EXPECT_EQ(tkns3[0], taul::token::failure("abc", 10));
        EXPECT_EQ(tkns3[1], taul::token(gram.lpr("lpr1"), "", 2));
        EXPECT_EQ(tkns3[2], taul::token::failure("0b", 2));
        EXPECT_EQ(tkns3[3], taul::token(gram.lpr("lpr1"), "", 4));
    }
    else ADD_FAILURE();

    // test empty string w/out token at end w/out offset

    tkns4.push_back(taul::token::failure("abc", 10));

    taul::tokenize_into(tkns4, lex1, "", lgr);

    if (tkns4.size() == 1) {
        EXPECT_EQ(tkns4[0], taul::token::failure("abc", 10));
    }
    else ADD_FAILURE();

    // test empty string (section) w/out token at end w/ offset

    tkns5.push_back(taul::token::failure("abc", 10));

    taul::tokenize_into(tkns5, lex1, "aa", 2, lgr);

    if (tkns5.size() == 1) {
        EXPECT_EQ(tkns5[0], taul::token::failure("abc", 10));
    }
    else ADD_FAILURE();
}

TEST_F(TokenizeTests, tokenize) {

    // the below tests the semantics of when match failures should and
    // shouldn't produce failure tokens (ie. there's no failure token
    // at the end of the below) and tests the merging of contiguous
    // failure tokens that are output

    // test w/out offset

    const auto tkns0 = taul::tokenize(lex1, "aaa00aa", lgr);

    if (tkns0.size() == 6) {
        EXPECT_EQ(tkns0[0], taul::token(gram.lpr("lpr0"), "a", 0));
        EXPECT_EQ(tkns0[1], taul::token(gram.lpr("lpr0"), "a", 1));
        EXPECT_EQ(tkns0[2], taul::token(gram.lpr("lpr0"), "a", 2));
        EXPECT_EQ(tkns0[3], taul::token::failure("00", 3));
        EXPECT_EQ(tkns0[4], taul::token(gram.lpr("lpr0"), "a", 5));
        EXPECT_EQ(tkns0[5], taul::token(gram.lpr("lpr0"), "a", 6));
    }
    else ADD_FAILURE();

    // test w/ offset

    const auto tkns1 = taul::tokenize(lex1, "00a00aa", 2, lgr);

    if (tkns1.size() == 4) {
        EXPECT_EQ(tkns1[0], taul::token(gram.lpr("lpr0"), "a", 2));
        EXPECT_EQ(tkns1[1], taul::token::failure("00", 3));
        EXPECT_EQ(tkns1[2], taul::token(gram.lpr("lpr0"), "a", 5));
        EXPECT_EQ(tkns1[3], taul::token(gram.lpr("lpr0"), "a", 6));
    }
    else ADD_FAILURE();

    // test 0 length tokens w/out offset

    const auto tkns2 = taul::tokenize(lex2, "000b", lgr);

    if (tkns2.size() == 7) {
        EXPECT_EQ(tkns2[0], taul::token(gram.lpr("lpr1"), "", 0));
        EXPECT_EQ(tkns2[1], taul::token::failure("0", 0));
        EXPECT_EQ(tkns2[2], taul::token(gram.lpr("lpr1"), "", 1));
        EXPECT_EQ(tkns2[3], taul::token::failure("0", 1));
        EXPECT_EQ(tkns2[4], taul::token(gram.lpr("lpr1"), "", 2));
        EXPECT_EQ(tkns2[5], taul::token::failure("0b", 2));
        EXPECT_EQ(tkns2[6], taul::token(gram.lpr("lpr1"), "", 4));
    }
    else ADD_FAILURE();

    // test 0 length tokens w/ offset

    const auto tkns3 = taul::tokenize(lex2, "000b", 2, lgr);

    if (tkns3.size() == 3) {
        EXPECT_EQ(tkns3[0], taul::token(gram.lpr("lpr1"), "", 2));
        EXPECT_EQ(tkns3[1], taul::token::failure("0b", 2));
        EXPECT_EQ(tkns3[2], taul::token(gram.lpr("lpr1"), "", 4));
    }
    else ADD_FAILURE() << std::format("tkns3.size() == {}", tkns3.size());

    // test empty string w/out token at end w/out offset

    const auto tkns4 = taul::tokenize(lex1, "", lgr);

    EXPECT_TRUE(tkns4.empty());

    // test empty string (section) w/out token at end w/ offset

    const auto tkns5 = taul::tokenize(lex1, "aa", 2, lgr);

    EXPECT_TRUE(tkns5.empty());
}

