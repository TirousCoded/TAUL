

#include <gtest/gtest.h>

#include <taul/all.h>


// these unit tests assert that taul::load_taul_grammar produces a grammar with
// the power to describe TAUL syntax properly

// of course, we can't test literally every possible input for each pattern, so
// a broad *general* suite of tests will be performed, w/ these being expected
// to be *close enough* to work as proper regression tests


class LoadTAULGrammarTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr = nullptr;
    taul::grammar gram;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
        gram = taul::load_taul_grammar(lgr);
    }
};


constexpr std::size_t lprs              = 29;
constexpr std::size_t pprs              = 33;
constexpr std::size_t keywords          = 9;
constexpr std::size_t operators         = 13;
constexpr std::size_t primary_exprs     = 8;


TEST_F(LoadTAULGrammarTests, HasExpectedLPRs) {

    EXPECT_EQ(gram.lprs().size(), lprs);

    static_assert(lprs == 29);

    EXPECT_TRUE(gram.contains_lpr("IDENTIFIER"));
    EXPECT_TRUE(gram.contains_lpr("STRING"));
    EXPECT_TRUE(gram.contains_lpr("CHARSET"));

    EXPECT_TRUE(gram.contains_lpr("WHITESPACE"));
    EXPECT_TRUE(gram.contains_lpr("NEWLINE"));
    EXPECT_TRUE(gram.contains_lpr("SL_COMMENT"));
    EXPECT_TRUE(gram.contains_lpr("ML_COMMENT"));

    static_assert(keywords == 9);

    EXPECT_TRUE(gram.contains_lpr("KW_LEXER"));
    EXPECT_TRUE(gram.contains_lpr("KW_PARSER"));
    EXPECT_TRUE(gram.contains_lpr("KW_SECTION"));
    EXPECT_TRUE(gram.contains_lpr("KW_SKIP"));
    EXPECT_TRUE(gram.contains_lpr("KW_BEGIN"));
    EXPECT_TRUE(gram.contains_lpr("KW_END"));
    EXPECT_TRUE(gram.contains_lpr("KW_ANY"));
    EXPECT_TRUE(gram.contains_lpr("KW_TOKEN"));
    EXPECT_TRUE(gram.contains_lpr("KW_FAILURE"));

    static_assert(operators == 13);

    EXPECT_TRUE(gram.contains_lpr("OP_PERIOD"));
    EXPECT_TRUE(gram.contains_lpr("OP_COMMA"));
    EXPECT_TRUE(gram.contains_lpr("OP_COLON"));
    EXPECT_TRUE(gram.contains_lpr("OP_SEMICOLON"));
    EXPECT_TRUE(gram.contains_lpr("OP_VBAR"));
    EXPECT_TRUE(gram.contains_lpr("OP_QUESTION"));
    EXPECT_TRUE(gram.contains_lpr("OP_ASTERISK"));
    EXPECT_TRUE(gram.contains_lpr("OP_PLUS"));
    EXPECT_TRUE(gram.contains_lpr("OP_TILDE"));
    EXPECT_TRUE(gram.contains_lpr("OP_AMPERSAND"));
    EXPECT_TRUE(gram.contains_lpr("OP_MINUS"));
    EXPECT_TRUE(gram.contains_lpr("OP_L_ROUND"));
    EXPECT_TRUE(gram.contains_lpr("OP_R_ROUND"));
}

TEST_F(LoadTAULGrammarTests, HasExpectedPPRs) {

    EXPECT_EQ(gram.pprs().size(), pprs);

    static_assert(pprs == 33);

    EXPECT_TRUE(gram.contains_ppr("Spec"));
    EXPECT_TRUE(gram.contains_ppr("Spec_SyntaxError"));

    EXPECT_TRUE(gram.contains_ppr("Clause"));
    EXPECT_TRUE(gram.contains_ppr("Clause_LexerSection"));
    EXPECT_TRUE(gram.contains_ppr("Clause_ParserSection"));
    EXPECT_TRUE(gram.contains_ppr("Clause_Rule"));
    EXPECT_TRUE(gram.contains_ppr("Clause_Rule_Skip"));
    EXPECT_TRUE(gram.contains_ppr("Clause_Rule_Name"));
    EXPECT_TRUE(gram.contains_ppr("Clause_Rule_Expr"));

    EXPECT_TRUE(gram.contains_ppr("Expr"));
    EXPECT_TRUE(gram.contains_ppr("Expr_Primary"));
    EXPECT_TRUE(gram.contains_ppr("Expr_Begin"));
    EXPECT_TRUE(gram.contains_ppr("Expr_End"));
    EXPECT_TRUE(gram.contains_ppr("Expr_Any"));
    EXPECT_TRUE(gram.contains_ppr("Expr_Token"));
    EXPECT_TRUE(gram.contains_ppr("Expr_Failure"));
    EXPECT_TRUE(gram.contains_ppr("Expr_String"));
    EXPECT_TRUE(gram.contains_ppr("Expr_Charset"));
    EXPECT_TRUE(gram.contains_ppr("Expr_Name"));

    EXPECT_TRUE(gram.contains_ppr("Expr_Group"));
    EXPECT_TRUE(gram.contains_ppr("Expr_LookAhead"));
    EXPECT_TRUE(gram.contains_ppr("Expr_LookAheadNot"));
    EXPECT_TRUE(gram.contains_ppr("Expr_Not"));
    EXPECT_TRUE(gram.contains_ppr("Expr_Optional"));
    EXPECT_TRUE(gram.contains_ppr("Expr_Optional_Expr"));
    EXPECT_TRUE(gram.contains_ppr("Expr_KleeneStar"));
    EXPECT_TRUE(gram.contains_ppr("Expr_KleeneStar_Expr"));
    EXPECT_TRUE(gram.contains_ppr("Expr_KleenePlus"));
    EXPECT_TRUE(gram.contains_ppr("Expr_KleenePlus_Expr"));
    EXPECT_TRUE(gram.contains_ppr("Expr_Sequence"));
    EXPECT_TRUE(gram.contains_ppr("Expr_Sequence_Expr"));
    EXPECT_TRUE(gram.contains_ppr("Expr_Set"));
    EXPECT_TRUE(gram.contains_ppr("Expr_Set_Expr"));
}

TEST_F(LoadTAULGrammarTests, KW_LEXER) {

    ASSERT_TRUE(gram.contains_lpr("KW_LEXER"));

    EXPECT_EQ(gram.lpr("KW_LEXER").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_LEXER");

    EXPECT_EQ(lxr("lexer"), taul::token(gram.lpr("KW_LEXER"), "lexer"));
    EXPECT_EQ(lxr("lexer?"), taul::token(gram.lpr("KW_LEXER"), "lexer"));
    EXPECT_EQ(lxr("lexer "), taul::token(gram.lpr("KW_LEXER"), "lexer"));
    EXPECT_EQ(lxr("lexer\r\n"), taul::token(gram.lpr("KW_LEXER"), "lexer"));

    EXPECT_EQ(lxr("lexerA"), taul::token::failure(""));
    EXPECT_EQ(lxr("lexer1"), taul::token::failure(""));
    EXPECT_EQ(lxr("lexer_"), taul::token::failure(""));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, KW_PARSER) {

    ASSERT_TRUE(gram.contains_lpr("KW_PARSER"));

    EXPECT_EQ(gram.lpr("KW_PARSER").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_PARSER");

    EXPECT_EQ(lxr("parser"), taul::token(gram.lpr("KW_PARSER"), "parser"));
    EXPECT_EQ(lxr("parser?"), taul::token(gram.lpr("KW_PARSER"), "parser"));
    EXPECT_EQ(lxr("parser "), taul::token(gram.lpr("KW_PARSER"), "parser"));
    EXPECT_EQ(lxr("parser\r\n"), taul::token(gram.lpr("KW_PARSER"), "parser"));

    EXPECT_EQ(lxr("parserA"), taul::token::failure(""));
    EXPECT_EQ(lxr("parser1"), taul::token::failure(""));
    EXPECT_EQ(lxr("parser_"), taul::token::failure(""));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, KW_SECTION) {

    ASSERT_TRUE(gram.contains_lpr("KW_SECTION"));

    EXPECT_EQ(gram.lpr("KW_SECTION").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_SECTION");

    EXPECT_EQ(lxr("section"), taul::token(gram.lpr("KW_SECTION"), "section"));
    EXPECT_EQ(lxr("section?"), taul::token(gram.lpr("KW_SECTION"), "section"));
    EXPECT_EQ(lxr("section "), taul::token(gram.lpr("KW_SECTION"), "section"));
    EXPECT_EQ(lxr("section\r\n"), taul::token(gram.lpr("KW_SECTION"), "section"));

    EXPECT_EQ(lxr("sectionA"), taul::token::failure(""));
    EXPECT_EQ(lxr("section1"), taul::token::failure(""));
    EXPECT_EQ(lxr("section_"), taul::token::failure(""));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, KW_SKIP) {

    ASSERT_TRUE(gram.contains_lpr("KW_SKIP"));

    EXPECT_EQ(gram.lpr("KW_SKIP").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_SKIP");

    EXPECT_EQ(lxr("skip"), taul::token(gram.lpr("KW_SKIP"), "skip"));
    EXPECT_EQ(lxr("skip?"), taul::token(gram.lpr("KW_SKIP"), "skip"));
    EXPECT_EQ(lxr("skip "), taul::token(gram.lpr("KW_SKIP"), "skip"));
    EXPECT_EQ(lxr("skip\r\n"), taul::token(gram.lpr("KW_SKIP"), "skip"));

    EXPECT_EQ(lxr("skipA"), taul::token::failure(""));
    EXPECT_EQ(lxr("skip1"), taul::token::failure(""));
    EXPECT_EQ(lxr("skip_"), taul::token::failure(""));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, KW_BEGIN) {

    ASSERT_TRUE(gram.contains_lpr("KW_BEGIN"));

    EXPECT_EQ(gram.lpr("KW_BEGIN").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_BEGIN");

    EXPECT_EQ(lxr("begin"), taul::token(gram.lpr("KW_BEGIN"), "begin"));
    EXPECT_EQ(lxr("begin?"), taul::token(gram.lpr("KW_BEGIN"), "begin"));
    EXPECT_EQ(lxr("begin "), taul::token(gram.lpr("KW_BEGIN"), "begin"));
    EXPECT_EQ(lxr("begin\r\n"), taul::token(gram.lpr("KW_BEGIN"), "begin"));

    EXPECT_EQ(lxr("beginA"), taul::token::failure(""));
    EXPECT_EQ(lxr("begin1"), taul::token::failure(""));
    EXPECT_EQ(lxr("begin_"), taul::token::failure(""));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, KW_END) {

    ASSERT_TRUE(gram.contains_lpr("KW_END"));

    EXPECT_EQ(gram.lpr("KW_END").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_END");

    EXPECT_EQ(lxr("end"), taul::token(gram.lpr("KW_END"), "end"));
    EXPECT_EQ(lxr("end?"), taul::token(gram.lpr("KW_END"), "end"));
    EXPECT_EQ(lxr("end "), taul::token(gram.lpr("KW_END"), "end"));
    EXPECT_EQ(lxr("end\r\n"), taul::token(gram.lpr("KW_END"), "end"));

    EXPECT_EQ(lxr("endA"), taul::token::failure(""));
    EXPECT_EQ(lxr("end1"), taul::token::failure(""));
    EXPECT_EQ(lxr("end_"), taul::token::failure(""));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, KW_ANY) {

    ASSERT_TRUE(gram.contains_lpr("KW_ANY"));

    EXPECT_EQ(gram.lpr("KW_ANY").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_ANY");

    EXPECT_EQ(lxr("any"), taul::token(gram.lpr("KW_ANY"), "any"));
    EXPECT_EQ(lxr("any?"), taul::token(gram.lpr("KW_ANY"), "any"));
    EXPECT_EQ(lxr("any "), taul::token(gram.lpr("KW_ANY"), "any"));
    EXPECT_EQ(lxr("any\r\n"), taul::token(gram.lpr("KW_ANY"), "any"));

    EXPECT_EQ(lxr("anyA"), taul::token::failure(""));
    EXPECT_EQ(lxr("any1"), taul::token::failure(""));
    EXPECT_EQ(lxr("any_"), taul::token::failure(""));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, KW_TOKEN) {

    ASSERT_TRUE(gram.contains_lpr("KW_TOKEN"));

    EXPECT_EQ(gram.lpr("KW_TOKEN").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_TOKEN");

    EXPECT_EQ(lxr("token"), taul::token(gram.lpr("KW_TOKEN"), "token"));
    EXPECT_EQ(lxr("token?"), taul::token(gram.lpr("KW_TOKEN"), "token"));
    EXPECT_EQ(lxr("token "), taul::token(gram.lpr("KW_TOKEN"), "token"));
    EXPECT_EQ(lxr("token\r\n"), taul::token(gram.lpr("KW_TOKEN"), "token"));

    EXPECT_EQ(lxr("tokenA"), taul::token::failure(""));
    EXPECT_EQ(lxr("token1"), taul::token::failure(""));
    EXPECT_EQ(lxr("token_"), taul::token::failure(""));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, KW_FAILURE) {

    ASSERT_TRUE(gram.contains_lpr("KW_FAILURE"));

    EXPECT_EQ(gram.lpr("KW_FAILURE").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_FAILURE");

    EXPECT_EQ(lxr("failure"), taul::token(gram.lpr("KW_FAILURE"), "failure"));
    EXPECT_EQ(lxr("failure?"), taul::token(gram.lpr("KW_FAILURE"), "failure"));
    EXPECT_EQ(lxr("failure "), taul::token(gram.lpr("KW_FAILURE"), "failure"));
    EXPECT_EQ(lxr("failure\r\n"), taul::token(gram.lpr("KW_FAILURE"), "failure"));

    EXPECT_EQ(lxr("failureA"), taul::token::failure(""));
    EXPECT_EQ(lxr("failure1"), taul::token::failure(""));
    EXPECT_EQ(lxr("failure_"), taul::token::failure(""));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_PERIOD) {

    ASSERT_TRUE(gram.contains_lpr("OP_PERIOD"));

    EXPECT_EQ(gram.lpr("OP_PERIOD").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("OP_PERIOD");

    EXPECT_EQ(lxr(".abc"), taul::token(gram.lpr("OP_PERIOD"), "."));
    EXPECT_EQ(lxr(".123"), taul::token(gram.lpr("OP_PERIOD"), "."));
    EXPECT_EQ(lxr(".!@#"), taul::token(gram.lpr("OP_PERIOD"), "."));
    EXPECT_EQ(lxr(". "), taul::token(gram.lpr("OP_PERIOD"), "."));
    EXPECT_EQ(lxr(".\t"), taul::token(gram.lpr("OP_PERIOD"), "."));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_COMMA) {

    ASSERT_TRUE(gram.contains_lpr("OP_COMMA"));

    EXPECT_EQ(gram.lpr("OP_COMMA").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("OP_COMMA");

    EXPECT_EQ(lxr(",abc"), taul::token(gram.lpr("OP_COMMA"), ","));
    EXPECT_EQ(lxr(",123"), taul::token(gram.lpr("OP_COMMA"), ","));
    EXPECT_EQ(lxr(",!@#"), taul::token(gram.lpr("OP_COMMA"), ","));
    EXPECT_EQ(lxr(", "), taul::token(gram.lpr("OP_COMMA"), ","));
    EXPECT_EQ(lxr(",\t"), taul::token(gram.lpr("OP_COMMA"), ","));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_COLON) {

    ASSERT_TRUE(gram.contains_lpr("OP_COLON"));

    EXPECT_EQ(gram.lpr("OP_COLON").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("OP_COLON");

    EXPECT_EQ(lxr(":abc"), taul::token(gram.lpr("OP_COLON"), ":"));
    EXPECT_EQ(lxr(":123"), taul::token(gram.lpr("OP_COLON"), ":"));
    EXPECT_EQ(lxr(":!@#"), taul::token(gram.lpr("OP_COLON"), ":"));
    EXPECT_EQ(lxr(": "), taul::token(gram.lpr("OP_COLON"), ":"));
    EXPECT_EQ(lxr(":\t"), taul::token(gram.lpr("OP_COLON"), ":"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_SEMICOLON) {

    ASSERT_TRUE(gram.contains_lpr("OP_SEMICOLON"));

    EXPECT_EQ(gram.lpr("OP_SEMICOLON").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("OP_SEMICOLON");

    EXPECT_EQ(lxr(";abc"), taul::token(gram.lpr("OP_SEMICOLON"), ";"));
    EXPECT_EQ(lxr(";123"), taul::token(gram.lpr("OP_SEMICOLON"), ";"));
    EXPECT_EQ(lxr(";!@#"), taul::token(gram.lpr("OP_SEMICOLON"), ";"));
    EXPECT_EQ(lxr("; "), taul::token(gram.lpr("OP_SEMICOLON"), ";"));
    EXPECT_EQ(lxr(";\t"), taul::token(gram.lpr("OP_SEMICOLON"), ";"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_VBAR) {

    ASSERT_TRUE(gram.contains_lpr("OP_VBAR"));

    EXPECT_EQ(gram.lpr("OP_VBAR").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("OP_VBAR");

    EXPECT_EQ(lxr("|abc"), taul::token(gram.lpr("OP_VBAR"), "|"));
    EXPECT_EQ(lxr("|123"), taul::token(gram.lpr("OP_VBAR"), "|"));
    EXPECT_EQ(lxr("|!@#"), taul::token(gram.lpr("OP_VBAR"), "|"));
    EXPECT_EQ(lxr("| "), taul::token(gram.lpr("OP_VBAR"), "|"));
    EXPECT_EQ(lxr("|\t"), taul::token(gram.lpr("OP_VBAR"), "|"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_QUESTION) {

    ASSERT_TRUE(gram.contains_lpr("OP_QUESTION"));

    EXPECT_EQ(gram.lpr("OP_QUESTION").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("OP_QUESTION");

    EXPECT_EQ(lxr("?abc"), taul::token(gram.lpr("OP_QUESTION"), "?"));
    EXPECT_EQ(lxr("?123"), taul::token(gram.lpr("OP_QUESTION"), "?"));
    EXPECT_EQ(lxr("?!@#"), taul::token(gram.lpr("OP_QUESTION"), "?"));
    EXPECT_EQ(lxr("? "), taul::token(gram.lpr("OP_QUESTION"), "?"));
    EXPECT_EQ(lxr("?\t"), taul::token(gram.lpr("OP_QUESTION"), "?"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_ASTERISK) {

    ASSERT_TRUE(gram.contains_lpr("OP_ASTERISK"));

    EXPECT_EQ(gram.lpr("OP_ASTERISK").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("OP_ASTERISK");

    EXPECT_EQ(lxr("*abc"), taul::token(gram.lpr("OP_ASTERISK"), "*"));
    EXPECT_EQ(lxr("*123"), taul::token(gram.lpr("OP_ASTERISK"), "*"));
    EXPECT_EQ(lxr("*!@#"), taul::token(gram.lpr("OP_ASTERISK"), "*"));
    EXPECT_EQ(lxr("* "), taul::token(gram.lpr("OP_ASTERISK"), "*"));
    EXPECT_EQ(lxr("*\t"), taul::token(gram.lpr("OP_ASTERISK"), "*"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_PLUS) {

    ASSERT_TRUE(gram.contains_lpr("OP_PLUS"));

    EXPECT_EQ(gram.lpr("OP_PLUS").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("OP_PLUS");

    EXPECT_EQ(lxr("+abc"), taul::token(gram.lpr("OP_PLUS"), "+"));
    EXPECT_EQ(lxr("+123"), taul::token(gram.lpr("OP_PLUS"), "+"));
    EXPECT_EQ(lxr("+!@#"), taul::token(gram.lpr("OP_PLUS"), "+"));
    EXPECT_EQ(lxr("+ "), taul::token(gram.lpr("OP_PLUS"), "+"));
    EXPECT_EQ(lxr("+\t"), taul::token(gram.lpr("OP_PLUS"), "+"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_TILDE) {

    ASSERT_TRUE(gram.contains_lpr("OP_TILDE"));

    EXPECT_EQ(gram.lpr("OP_TILDE").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("OP_TILDE");

    EXPECT_EQ(lxr("~abc"), taul::token(gram.lpr("OP_TILDE"), "~"));
    EXPECT_EQ(lxr("~123"), taul::token(gram.lpr("OP_TILDE"), "~"));
    EXPECT_EQ(lxr("~!@#"), taul::token(gram.lpr("OP_TILDE"), "~"));
    EXPECT_EQ(lxr("~ "), taul::token(gram.lpr("OP_TILDE"), "~"));
    EXPECT_EQ(lxr("~\t"), taul::token(gram.lpr("OP_TILDE"), "~"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    //EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_AMPERSAND) {

    ASSERT_TRUE(gram.contains_lpr("OP_AMPERSAND"));

    EXPECT_EQ(gram.lpr("OP_AMPERSAND").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("OP_AMPERSAND");

    EXPECT_EQ(lxr("&abc"), taul::token(gram.lpr("OP_AMPERSAND"), "&"));
    EXPECT_EQ(lxr("&123"), taul::token(gram.lpr("OP_AMPERSAND"), "&"));
    EXPECT_EQ(lxr("&!@#"), taul::token(gram.lpr("OP_AMPERSAND"), "&"));
    EXPECT_EQ(lxr("& "), taul::token(gram.lpr("OP_AMPERSAND"), "&"));
    EXPECT_EQ(lxr("&\t"), taul::token(gram.lpr("OP_AMPERSAND"), "&"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_MINUS) {

    ASSERT_TRUE(gram.contains_lpr("OP_MINUS"));

    EXPECT_EQ(gram.lpr("OP_MINUS").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("OP_MINUS");

    EXPECT_EQ(lxr("-abc"), taul::token(gram.lpr("OP_MINUS"), "-"));
    EXPECT_EQ(lxr("-123"), taul::token(gram.lpr("OP_MINUS"), "-"));
    EXPECT_EQ(lxr("-!@#"), taul::token(gram.lpr("OP_MINUS"), "-"));
    EXPECT_EQ(lxr("- "), taul::token(gram.lpr("OP_MINUS"), "-"));
    EXPECT_EQ(lxr("-\t"), taul::token(gram.lpr("OP_MINUS"), "-"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_L_ROUND) {

    ASSERT_TRUE(gram.contains_lpr("OP_L_ROUND"));

    EXPECT_EQ(gram.lpr("OP_L_ROUND").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("OP_L_ROUND");

    EXPECT_EQ(lxr("(abc"), taul::token(gram.lpr("OP_L_ROUND"), "("));
    EXPECT_EQ(lxr("(123"), taul::token(gram.lpr("OP_L_ROUND"), "("));
    EXPECT_EQ(lxr("(!@#"), taul::token(gram.lpr("OP_L_ROUND"), "("));
    EXPECT_EQ(lxr("( "), taul::token(gram.lpr("OP_L_ROUND"), "("));
    EXPECT_EQ(lxr("(\t"), taul::token(gram.lpr("OP_L_ROUND"), "("));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_R_ROUND) {

    ASSERT_TRUE(gram.contains_lpr("OP_R_ROUND"));

    EXPECT_EQ(gram.lpr("OP_R_ROUND").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("OP_R_ROUND");

    EXPECT_EQ(lxr(")abc"), taul::token(gram.lpr("OP_R_ROUND"), ")"));
    EXPECT_EQ(lxr(")123"), taul::token(gram.lpr("OP_R_ROUND"), ")"));
    EXPECT_EQ(lxr(")!@#"), taul::token(gram.lpr("OP_R_ROUND"), ")"));
    EXPECT_EQ(lxr(") "), taul::token(gram.lpr("OP_R_ROUND"), ")"));
    EXPECT_EQ(lxr(")\t"), taul::token(gram.lpr("OP_R_ROUND"), ")"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, IDENTIFIER) {

    ASSERT_TRUE(gram.contains_lpr("IDENTIFIER"));

    EXPECT_EQ(gram.lpr("IDENTIFIER").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("IDENTIFIER");

    EXPECT_EQ(lxr("abc(abc"), taul::token(gram.lpr("IDENTIFIER"), "abc"));
    EXPECT_EQ(lxr("_a_b_c&abc"), taul::token(gram.lpr("IDENTIFIER"), "_a_b_c"));
    EXPECT_EQ(lxr("_123*abc"), taul::token(gram.lpr("IDENTIFIER"), "_123"));
    EXPECT_EQ(lxr("_&abc"), taul::token(gram.lpr("IDENTIFIER"), "_"));
    EXPECT_EQ(lxr("A&abc"), taul::token(gram.lpr("IDENTIFIER"), "A"));

    static_assert(keywords == 9);

    // when matching *specifically* an IDENTIFIER, not the whole grammar,
    // then the below keywords will be matched as IDENTIFIER tokens

    EXPECT_EQ(lxr("lexer"), taul::token(gram.lpr("IDENTIFIER"), "lexer"));
    EXPECT_EQ(lxr("parser"), taul::token(gram.lpr("IDENTIFIER"), "parser"));
    EXPECT_EQ(lxr("section"), taul::token(gram.lpr("IDENTIFIER"), "section"));
    EXPECT_EQ(lxr("skip"), taul::token(gram.lpr("IDENTIFIER"), "skip"));
    EXPECT_EQ(lxr("begin"), taul::token(gram.lpr("IDENTIFIER"), "begin"));
    EXPECT_EQ(lxr("end"), taul::token(gram.lpr("IDENTIFIER"), "end"));
    EXPECT_EQ(lxr("any"), taul::token(gram.lpr("IDENTIFIER"), "any"));
    EXPECT_EQ(lxr("token"), taul::token(gram.lpr("IDENTIFIER"), "token"));
    EXPECT_EQ(lxr("failure"), taul::token(gram.lpr("IDENTIFIER"), "failure"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, STRING) {

    ASSERT_TRUE(gram.contains_lpr("STRING"));

    EXPECT_EQ(gram.lpr("STRING").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("STRING");

    EXPECT_EQ(lxr("''abc"), taul::token(gram.lpr("STRING"), "''"));
    EXPECT_EQ(lxr("'abc'abc"), taul::token(gram.lpr("STRING"), "'abc'"));
    EXPECT_EQ(lxr("'123'abc"), taul::token(gram.lpr("STRING"), "'123'"));
    EXPECT_EQ(lxr("'!@#'abc"), taul::token(gram.lpr("STRING"), "'!@#'"));
    EXPECT_EQ(lxr("' \t\r\n'abc"), taul::token(gram.lpr("STRING"), "' \t\r\n'"));
    EXPECT_EQ(lxr("'\\a\\b\\c'abc"), taul::token(gram.lpr("STRING"), "'\\a\\b\\c'"));
    EXPECT_EQ(lxr("'\\1\\2\\3'abc"), taul::token(gram.lpr("STRING"), "'\\1\\2\\3'"));
    EXPECT_EQ(lxr("'\\!\\@\\#'abc"), taul::token(gram.lpr("STRING"), "'\\!\\@\\#'"));
    EXPECT_EQ(lxr("'\\ \\\t\\\r\\\n'abc"), taul::token(gram.lpr("STRING"), "'\\ \\\t\\\r\\\n'"));
    EXPECT_EQ(lxr("'\\''abc"), taul::token(gram.lpr("STRING"), "'\\''"));
    EXPECT_EQ(lxr("'\\\\''abc"), taul::token(gram.lpr("STRING"), "'\\\\'"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));

    EXPECT_EQ(lxr("'"), taul::token::failure(""));
    EXPECT_EQ(lxr("'\\'"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, CHARSET) {

    ASSERT_TRUE(gram.contains_lpr("CHARSET"));

    EXPECT_EQ(gram.lpr("CHARSET").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("CHARSET");

    EXPECT_EQ(lxr("[]abc"), taul::token(gram.lpr("CHARSET"), "[]"));
    EXPECT_EQ(lxr("[abc]abc"), taul::token(gram.lpr("CHARSET"), "[abc]"));
    EXPECT_EQ(lxr("[123]abc"), taul::token(gram.lpr("CHARSET"), "[123]"));
    EXPECT_EQ(lxr("[!@#]abc"), taul::token(gram.lpr("CHARSET"), "[!@#]"));
    EXPECT_EQ(lxr("[ \t\r\n]abc"), taul::token(gram.lpr("CHARSET"), "[ \t\r\n]"));
    EXPECT_EQ(lxr("[\\a\\b\\c]abc"), taul::token(gram.lpr("CHARSET"), "[\\a\\b\\c]"));
    EXPECT_EQ(lxr("[\\1\\2\\3]abc"), taul::token(gram.lpr("CHARSET"), "[\\1\\2\\3]"));
    EXPECT_EQ(lxr("[\\!\\@\\#]abc"), taul::token(gram.lpr("CHARSET"), "[\\!\\@\\#]"));
    EXPECT_EQ(lxr("[\\ \\\t\\\r\\\n]abc"), taul::token(gram.lpr("CHARSET"), "[\\ \\\t\\\r\\\n]"));
    EXPECT_EQ(lxr("[\\]]abc"), taul::token(gram.lpr("CHARSET"), "[\\]]"));
    EXPECT_EQ(lxr("[\\\\]]abc"), taul::token(gram.lpr("CHARSET"), "[\\\\]"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));

    EXPECT_EQ(lxr("["), taul::token::failure(""));
    EXPECT_EQ(lxr("[\\]"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, WHITESPACE) {

    ASSERT_TRUE(gram.contains_lpr("WHITESPACE"));

    EXPECT_EQ(gram.lpr("WHITESPACE").qualifer, taul::qualifier::skip);

    auto lxr = gram.lexer("WHITESPACE");

    for (std::uint8_t i = 0x00; i < 0x80; i++) {
        // skip stray nulls for now
        if (i == 0x00) {
            continue;
        }
        std::string s = " a";
        s[0] = (char)i;
        taul::token tkn = lxr(s);
        if (char(i) == ' ' || char(i) == '\t') {
            EXPECT_EQ(tkn, taul::token(gram.lpr("WHITESPACE"), s.substr(0, 1))) << "std::size_t(s[0])==" << size_t(i);
        }
        else {
            EXPECT_EQ(tkn, taul::token::failure("")) << "std::size_t(s[0])==" << size_t(i);
        }
    }

    EXPECT_EQ(lxr(""), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, NEWLINE) {

    ASSERT_TRUE(gram.contains_lpr("NEWLINE"));

    EXPECT_EQ(gram.lpr("NEWLINE").qualifer, taul::qualifier::skip);

    auto lxr = gram.lexer("NEWLINE");

    for (std::uint8_t i = 0x00; i < 0x80; i++) {
        // skip stray nulls for now
        if (i == 0x00) {
            continue;
        }
        std::string s = " a";
        s[0] = (char)i;
        taul::token tkn = lxr(s);
        if (char(i) == '\r' || char(i) == '\n') {
            EXPECT_EQ(tkn, taul::token(gram.lpr("NEWLINE"), s.substr(0, 1))) << "std::size_t(s[0])==" << size_t(i);
        }
        else {
            EXPECT_EQ(tkn, taul::token::failure("")) << "std::size_t(s[0])==" << size_t(i);
        }
    }

    EXPECT_EQ(lxr("\r\na"), taul::token(gram.lpr("NEWLINE"), "\r\n"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, SL_COMMENT) {

    ASSERT_TRUE(gram.contains_lpr("SL_COMMENT"));

    EXPECT_EQ(gram.lpr("SL_COMMENT").qualifer, taul::qualifier::skip);

    auto lxr = gram.lexer("SL_COMMENT");

    EXPECT_EQ(lxr("#"), taul::token(gram.lpr("SL_COMMENT"), "#"));
    EXPECT_EQ(lxr("####"), taul::token(gram.lpr("SL_COMMENT"), "####"));
    EXPECT_EQ(lxr("#abc123!@#"), taul::token(gram.lpr("SL_COMMENT"), "#abc123!@#"));
    EXPECT_EQ(lxr("# abc\t123!@#"), taul::token(gram.lpr("SL_COMMENT"), "# abc\t123!@#"));
    EXPECT_EQ(lxr("#abc123!@#\r"), taul::token(gram.lpr("SL_COMMENT"), "#abc123!@#"));
    EXPECT_EQ(lxr("# abc\t123!@#\r"), taul::token(gram.lpr("SL_COMMENT"), "# abc\t123!@#"));
    EXPECT_EQ(lxr("#abc123!@#\r\n"), taul::token(gram.lpr("SL_COMMENT"), "#abc123!@#"));
    EXPECT_EQ(lxr("# abc\t123!@#\r\n"), taul::token(gram.lpr("SL_COMMENT"), "# abc\t123!@#"));
    EXPECT_EQ(lxr("#abc123!@#\n"), taul::token(gram.lpr("SL_COMMENT"), "#abc123!@#"));
    EXPECT_EQ(lxr("# abc\t123!@#\n"), taul::token(gram.lpr("SL_COMMENT"), "# abc\t123!@#"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("!##!"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, ML_COMMENT) {

    ASSERT_TRUE(gram.contains_lpr("ML_COMMENT"));

    EXPECT_EQ(gram.lpr("ML_COMMENT").qualifer, taul::qualifier::skip);

    auto lxr = gram.lexer("ML_COMMENT");

    EXPECT_EQ(lxr("!##!abc"), taul::token(gram.lpr("ML_COMMENT"), "!##!"));
    EXPECT_EQ(lxr("!#"), taul::token(gram.lpr("ML_COMMENT"), "!#"));
    EXPECT_EQ(lxr("!# \tabc!@#123\r\n \r \n#!abc"), taul::token(gram.lpr("ML_COMMENT"), "!# \tabc!@#123\r\n \r \n#!"));
    EXPECT_EQ(lxr("!# \tabc!@#123\r\n \r \n"), taul::token(gram.lpr("ML_COMMENT"), "!# \tabc!@#123\r\n \r \n"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("#!"), taul::token::failure(""));
}

// we're gonna perform our syntax tests *in bulk*, w/ bulk unit tests 
// for collections of PPRs tested at once via an example node tree

// for PPRs w/ ***_Expr subpatterns used to address left-recursion,
// take note that our unit tests presume that these are implemented
// as expected, and that they work properly, w/ these tests likewise
// not testing *every single little thing* they should match

// TODO: add tests for expr precedence and (maybe) expr associativity

TEST_F(LoadTAULGrammarTests, Syntax_1) {


    // this tests an empty source file


    ASSERT_TRUE(gram.contains_ppr("Spec"));


    std::string src =
        "\n";

    auto tkns = taul::tokenize(gram, src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 0);

    taul::node_ctx ctx;


    auto expected =
        taul::node_assembler()
        .begin(ctx.create(gram.ppr("Spec"), "", 0))
        .end();

    TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());


    auto actual = gram.parser("Spec")(ctx, tkns);
    
    TAUL_LOG(lgr, "actual.has_value() == {}", actual.has_value());
    
    ASSERT_TRUE(actual.has_value());

    TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());


    EXPECT_TRUE(expected.equiv(*actual));
}

TEST_F(LoadTAULGrammarTests, Syntax_2) {


    // this tests the basics of clause syntax


    EXPECT_TRUE(gram.contains_lpr("KW_LEXER"));
    EXPECT_TRUE(gram.contains_lpr("KW_PARSER"));
    EXPECT_TRUE(gram.contains_lpr("KW_SECTION"));
    EXPECT_TRUE(gram.contains_lpr("KW_SKIP"));
    EXPECT_TRUE(gram.contains_lpr("KW_ANY"));
    EXPECT_TRUE(gram.contains_lpr("OP_COLON"));
    EXPECT_TRUE(gram.contains_lpr("OP_SEMICOLON"));
    EXPECT_TRUE(gram.contains_lpr("IDENTIFIER"));

    ASSERT_TRUE(gram.contains_ppr("Spec"));
    ASSERT_TRUE(gram.contains_ppr("Clause"));
    ASSERT_TRUE(gram.contains_ppr("Clause_LexerSection"));
    ASSERT_TRUE(gram.contains_ppr("Clause_ParserSection"));
    EXPECT_TRUE(gram.contains_ppr("Clause_Rule"));
    EXPECT_TRUE(gram.contains_ppr("Clause_Rule_Skip"));
    EXPECT_TRUE(gram.contains_ppr("Clause_Rule_Name"));
    EXPECT_TRUE(gram.contains_ppr("Clause_Rule_Expr"));
    ASSERT_TRUE(gram.contains_ppr("Expr"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Primary"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Any"));


    std::string src =
        "lexer section:\n"
        "skip ABC : any ;\n"
        "parser section:\n"
        "Abc : any ;";

    auto tkns = taul::tokenize(gram, src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 15);

    taul::node_ctx ctx;


    auto expected =
        taul::node_assembler()
        .begin(ctx.create(gram.ppr("Spec"), tkns.range_str(0, 15), tkns[0].pos()))

        .enter(ctx.create(gram.ppr("Clause"), tkns.range_str(0, 3), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Clause_LexerSection"), tkns.range_str(0, 3), tkns[0].pos()))
        .attach(ctx.create(gram.lpr("KW_LEXER"), "lexer", tkns[0].pos()))
        .attach(ctx.create(gram.lpr("KW_SECTION"), "section", tkns[1].pos()))
        .attach(ctx.create(gram.lpr("OP_COLON"), ":", tkns[2].pos()))
        .exit()
        .exit()

        .enter(ctx.create(gram.ppr("Clause"), tkns.range_str(3, 5), tkns[3].pos()))
        .enter(ctx.create(gram.ppr("Clause_Rule"), tkns.range_str(3, 5), tkns[3].pos()))
        .enter(ctx.create(gram.ppr("Clause_Rule_Skip"), "skip", tkns[3].pos()))
        .attach(ctx.create(gram.lpr("KW_SKIP"), "skip", tkns[3].pos()))
        .exit()
        .enter(ctx.create(gram.ppr("Clause_Rule_Name"), "ABC", tkns[4].pos()))
        .attach(ctx.create(gram.lpr("IDENTIFIER"), "ABC", tkns[4].pos()))
        .exit()
        .attach(ctx.create(gram.lpr("OP_COLON"), ":", tkns[5].pos()))
        .enter(ctx.create(gram.ppr("Clause_Rule_Expr"), "any", tkns[6].pos()))
        .enter(ctx.create(gram.ppr("Expr"), "any", tkns[6].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[6].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[6].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[6].pos()))
        .exit()
        .exit()
        .exit()
        .exit()
        .attach(ctx.create(gram.lpr("OP_SEMICOLON"), ";", tkns[7].pos()))
        .exit()
        .exit()
        
        .enter(ctx.create(gram.ppr("Clause"), tkns.range_str(8, 3), tkns[8].pos()))
        .enter(ctx.create(gram.ppr("Clause_ParserSection"), tkns.range_str(8, 3), tkns[8].pos()))
        .attach(ctx.create(gram.lpr("KW_PARSER"), "parser", tkns[8].pos()))
        .attach(ctx.create(gram.lpr("KW_SECTION"), "section", tkns[9].pos()))
        .attach(ctx.create(gram.lpr("OP_COLON"), ":", tkns[10].pos()))
        .exit()
        .exit()

        .enter(ctx.create(gram.ppr("Clause"), tkns.range_str(11, 4), tkns[11].pos()))
        .enter(ctx.create(gram.ppr("Clause_Rule"), tkns.range_str(11, 4), tkns[11].pos()))
        .enter(ctx.create(gram.ppr("Clause_Rule_Name"), "Abc", tkns[11].pos()))
        .attach(ctx.create(gram.lpr("IDENTIFIER"), "Abc", tkns[11].pos()))
        .exit()
        .attach(ctx.create(gram.lpr("OP_COLON"), ":", tkns[12].pos()))
        .enter(ctx.create(gram.ppr("Clause_Rule_Expr"), "any", tkns[13].pos()))
        .enter(ctx.create(gram.ppr("Expr"), "any", tkns[13].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[13].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[13].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[13].pos()))
        .exit()
        .exit()
        .exit()
        .exit()
        .attach(ctx.create(gram.lpr("OP_SEMICOLON"), ";", tkns[14].pos()))
        .exit()
        .exit()

        .end();

    TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());


    auto actual = gram.parser("Spec")(ctx, tkns);
    
    TAUL_LOG(lgr, "actual.has_value() == {}", actual.has_value());
    
    ASSERT_TRUE(actual.has_value());

    TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());


    EXPECT_TRUE(expected.equiv(*actual));
}

TEST_F(LoadTAULGrammarTests, Syntax_3) {


    // this tests the detection of syntax errors


    EXPECT_TRUE(gram.contains_lpr("KW_LEXER"));
    EXPECT_TRUE(gram.contains_lpr("KW_PARSER"));
    EXPECT_TRUE(gram.contains_lpr("KW_SECTION"));
    EXPECT_TRUE(gram.contains_lpr("KW_SKIP"));
    EXPECT_TRUE(gram.contains_lpr("KW_ANY"));
    EXPECT_TRUE(gram.contains_lpr("OP_COLON"));
    EXPECT_TRUE(gram.contains_lpr("OP_SEMICOLON"));
    EXPECT_TRUE(gram.contains_lpr("IDENTIFIER"));

    ASSERT_TRUE(gram.contains_ppr("Spec"));
    ASSERT_TRUE(gram.contains_ppr("Spec_SyntaxError"));
    ASSERT_TRUE(gram.contains_ppr("Clause"));
    ASSERT_TRUE(gram.contains_ppr("Clause_LexerSection"));
    ASSERT_TRUE(gram.contains_ppr("Expr"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Primary"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Any"));


    std::string src =
        "lexer section:\n"
        "skip ABC : + ;\n" // <- the '+' is a syntax error
        "parser section:\n"
        "Abc : any ;";

    auto tkns = taul::tokenize(gram, src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 15);

    taul::node_ctx ctx;


    auto expected =
        taul::node_assembler()
        .begin(ctx.create(gram.ppr("Spec"), tkns.range_str(0, 4), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Clause"), tkns.range_str(0, 3), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Clause_LexerSection"), tkns.range_str(0, 3), tkns[0].pos()))
        .attach(ctx.create(gram.lpr("KW_LEXER"), "lexer", tkns[0].pos()))
        .attach(ctx.create(gram.lpr("KW_SECTION"), "section", tkns[1].pos()))
        .attach(ctx.create(gram.lpr("OP_COLON"), ":", tkns[2].pos()))
        .exit()
        .exit()
        .enter(ctx.create(gram.ppr("Spec_SyntaxError"), "skip", tkns[3].pos()))
        .attach(ctx.create(gram.lpr("KW_SKIP"), "skip", tkns[3].pos()))
        .exit()
        .end();

    TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());


    auto actual = gram.parser("Spec")(ctx, tkns);
    
    TAUL_LOG(lgr, "actual.has_value() == {}", actual.has_value());
    
    ASSERT_TRUE(actual.has_value());

    TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());


    EXPECT_TRUE(expected.equiv(*actual));
}

TEST_F(LoadTAULGrammarTests, Syntax_4) {


    // this tests the syntax of primary exprs


    EXPECT_TRUE(gram.contains_lpr("KW_BEGIN"));
    EXPECT_TRUE(gram.contains_lpr("KW_END"));
    EXPECT_TRUE(gram.contains_lpr("KW_ANY"));
    EXPECT_TRUE(gram.contains_lpr("KW_TOKEN"));
    EXPECT_TRUE(gram.contains_lpr("KW_FAILURE"));
    EXPECT_TRUE(gram.contains_lpr("IDENTIFIER"));
    EXPECT_TRUE(gram.contains_lpr("STRING"));
    EXPECT_TRUE(gram.contains_lpr("CHARSET"));

    ASSERT_TRUE(gram.contains_ppr("Expr_Primary"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Begin"));
    ASSERT_TRUE(gram.contains_ppr("Expr_End"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Any"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Token"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Failure"));
    ASSERT_TRUE(gram.contains_ppr("Expr_String"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Charset"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Name"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Sequence"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Sequence_Expr"));


    // this is NOT semantically valid TAUL, but it is syntactic

    static_assert(primary_exprs == 8);

    std::string src =
        "begin end any token failure "
        "'abc'"
        "[abc]"
        "Abc";

    auto tkns = taul::tokenize(gram, src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 8);

    taul::node_ctx ctx;


    taul::node_assembler na{};

    na
        .begin(ctx.create(gram.ppr("Expr_Sequence"), tkns.range_str(0, 8), tkns[0].pos()));

    std::size_t offset = 0;

    na
        .enter(ctx.create(gram.ppr("Expr_Sequence_Expr"), tkns[offset].str(), tkns[offset].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), tkns[offset].str(), tkns[offset].pos()))
        .enter(ctx.create(gram.ppr("Expr_Begin"), tkns[offset].str(), tkns[offset].pos()))
        .attach(ctx.create(tkns[offset]))
        .exit()
        .exit()
        .exit();

    offset++;

    na
        .enter(ctx.create(gram.ppr("Expr_Sequence_Expr"), tkns[offset].str(), tkns[offset].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), tkns[offset].str(), tkns[offset].pos()))
        .enter(ctx.create(gram.ppr("Expr_End"), tkns[offset].str(), tkns[offset].pos()))
        .attach(ctx.create(tkns[offset]))
        .exit()
        .exit()
        .exit();

    offset++;

    na
        .enter(ctx.create(gram.ppr("Expr_Sequence_Expr"), tkns[offset].str(), tkns[offset].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), tkns[offset].str(), tkns[offset].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), tkns[offset].str(), tkns[offset].pos()))
        .attach(ctx.create(tkns[offset]))
        .exit()
        .exit()
        .exit();

    offset++;

    na
        .enter(ctx.create(gram.ppr("Expr_Sequence_Expr"), tkns[offset].str(), tkns[offset].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), tkns[offset].str(), tkns[offset].pos()))
        .enter(ctx.create(gram.ppr("Expr_Token"), tkns[offset].str(), tkns[offset].pos()))
        .attach(ctx.create(tkns[offset]))
        .exit()
        .exit()
        .exit();

    offset++;

    na
        .enter(ctx.create(gram.ppr("Expr_Sequence_Expr"), tkns[offset].str(), tkns[offset].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), tkns[offset].str(), tkns[offset].pos()))
        .enter(ctx.create(gram.ppr("Expr_Failure"), tkns[offset].str(), tkns[offset].pos()))
        .attach(ctx.create(tkns[offset]))
        .exit()
        .exit()
        .exit();

    offset++;

    na
        .enter(ctx.create(gram.ppr("Expr_Sequence_Expr"), tkns[offset].str(), tkns[offset].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), tkns[offset].str(), tkns[offset].pos()))
        .enter(ctx.create(gram.ppr("Expr_String"), tkns[offset].str(), tkns[offset].pos()))
        .attach(ctx.create(tkns[offset]))
        .exit()
        .exit()
        .exit();

    offset++;

    na
        .enter(ctx.create(gram.ppr("Expr_Sequence_Expr"), tkns[offset].str(), tkns[offset].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), tkns[offset].str(), tkns[offset].pos()))
        .enter(ctx.create(gram.ppr("Expr_Charset"), tkns[offset].str(), tkns[offset].pos()))
        .attach(ctx.create(tkns[offset]))
        .exit()
        .exit()
        .exit();

    offset++;

    na
        .enter(ctx.create(gram.ppr("Expr_Sequence_Expr"), tkns[offset].str(), tkns[offset].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), tkns[offset].str(), tkns[offset].pos()))
        .enter(ctx.create(gram.ppr("Expr_Name"), tkns[offset].str(), tkns[offset].pos()))
        .attach(ctx.create(tkns[offset]))
        .exit()
        .exit()
        .exit();

    auto expected = na.end();

    TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());


    auto actual = gram.parser("Expr_Sequence")(ctx, tkns);
    
    TAUL_LOG(lgr, "actual.has_value() == {}", actual.has_value());
    
    ASSERT_TRUE(actual.has_value());

    TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());

    
    EXPECT_TRUE(expected.equiv(*actual));
}

TEST_F(LoadTAULGrammarTests, Syntax_5) {


    // this tests the syntax of group exprs


    EXPECT_TRUE(gram.contains_lpr("KW_ANY"));
    EXPECT_TRUE(gram.contains_lpr("OP_L_ROUND"));
    EXPECT_TRUE(gram.contains_lpr("OP_R_ROUND"));

    ASSERT_TRUE(gram.contains_ppr("Expr"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Primary"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Any"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Group"));


    std::string src =
        "( any )";

    auto tkns = taul::tokenize(gram, src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 3);

    taul::node_ctx ctx;


    auto expected =
        taul::node_assembler()
        .begin(ctx.create(gram.ppr("Expr"), tkns.range_str(0, 3), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Group"), tkns.range_str(0, 3), tkns[0].pos()))
        .attach(ctx.create(gram.lpr("OP_L_ROUND"), "(", tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr"), "any", tkns[1].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[1].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[1].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[1].pos()))
        .exit()
        .exit()
        .exit()
        .attach(ctx.create(gram.lpr("OP_R_ROUND"), ")", tkns[2].pos()))
        .exit()
        .end();

    TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());


    auto actual = gram.parser("Expr")(ctx, tkns);

    TAUL_LOG(lgr, "actual.has_value() == {}", actual.has_value());

    ASSERT_TRUE(actual.has_value());

    TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());


    EXPECT_TRUE(expected.equiv(*actual));
}

TEST_F(LoadTAULGrammarTests, Syntax_6) {


    // this tests the syntax of lookahead exprs


    EXPECT_TRUE(gram.contains_lpr("KW_ANY"));
    EXPECT_TRUE(gram.contains_lpr("OP_AMPERSAND"));

    ASSERT_TRUE(gram.contains_ppr("Expr"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Primary"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Any"));
    ASSERT_TRUE(gram.contains_ppr("Expr_LookAhead"));


    std::string src =
        "&any";

    auto tkns = taul::tokenize(gram, src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 2);

    taul::node_ctx ctx;


    auto expected =
        taul::node_assembler()
        .begin(ctx.create(gram.ppr("Expr"), tkns.range_str(0, 2), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_LookAhead"), tkns.range_str(0, 2), tkns[0].pos()))
        .attach(ctx.create(gram.lpr("OP_AMPERSAND"), "&", tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr"), "any", tkns[1].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[1].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[1].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[1].pos()))
        .exit()
        .exit()
        .exit()
        .exit()
        .end();

    TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());


    auto actual = gram.parser("Expr")(ctx, tkns);

    TAUL_LOG(lgr, "actual.has_value() == {}", actual.has_value());

    ASSERT_TRUE(actual.has_value());

    TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());


    EXPECT_TRUE(expected.equiv(*actual));
}

TEST_F(LoadTAULGrammarTests, Syntax_7) {


    // this tests the syntax of lookahead-not exprs


    EXPECT_TRUE(gram.contains_lpr("KW_ANY"));
    EXPECT_TRUE(gram.contains_lpr("OP_MINUS"));

    ASSERT_TRUE(gram.contains_ppr("Expr"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Primary"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Any"));
    ASSERT_TRUE(gram.contains_ppr("Expr_LookAheadNot"));


    std::string src =
        "-any";

    auto tkns = taul::tokenize(gram, src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 2);

    taul::node_ctx ctx;


    auto expected =
        taul::node_assembler()
        .begin(ctx.create(gram.ppr("Expr"), tkns.range_str(0, 2), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_LookAheadNot"), tkns.range_str(0, 2), tkns[0].pos()))
        .attach(ctx.create(gram.lpr("OP_MINUS"), "-", tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr"), "any", tkns[1].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[1].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[1].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[1].pos()))
        .exit()
        .exit()
        .exit()
        .exit()
        .end();

    TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());


    auto actual = gram.parser("Expr")(ctx, tkns);

    TAUL_LOG(lgr, "actual.has_value() == {}", actual.has_value());

    ASSERT_TRUE(actual.has_value());

    TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());


    EXPECT_TRUE(expected.equiv(*actual));
}

TEST_F(LoadTAULGrammarTests, Syntax_8) {


    // this tests the syntax of not exprs


    EXPECT_TRUE(gram.contains_lpr("KW_ANY"));
    EXPECT_TRUE(gram.contains_lpr("OP_TILDE"));

    ASSERT_TRUE(gram.contains_ppr("Expr"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Primary"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Any"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Not"));


    std::string src =
        "~any";

    auto tkns = taul::tokenize(gram, src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 2);

    taul::node_ctx ctx;


    auto expected =
        taul::node_assembler()
        .begin(ctx.create(gram.ppr("Expr"), tkns.range_str(0, 2), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Not"), tkns.range_str(0, 2), tkns[0].pos()))
        .attach(ctx.create(gram.lpr("OP_TILDE"), "~", tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr"), "any", tkns[1].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[1].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[1].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[1].pos()))
        .exit()
        .exit()
        .exit()
        .exit()
        .end();

    TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());


    auto actual = gram.parser("Expr")(ctx, tkns);

    TAUL_LOG(lgr, "actual.has_value() == {}", actual.has_value());

    ASSERT_TRUE(actual.has_value());

    TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());


    EXPECT_TRUE(expected.equiv(*actual));
}

TEST_F(LoadTAULGrammarTests, Syntax_9) {


    // this tests the syntax of optional exprs


    EXPECT_TRUE(gram.contains_lpr("KW_ANY"));
    EXPECT_TRUE(gram.contains_lpr("OP_QUESTION"));

    ASSERT_TRUE(gram.contains_ppr("Expr"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Primary"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Any"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Optional"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Optional_Expr"));


    std::string src =
        "any?";

    auto tkns = taul::tokenize(gram, src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 2);

    taul::node_ctx ctx;


    auto expected =
        taul::node_assembler()
        .begin(ctx.create(gram.ppr("Expr"), tkns.range_str(0, 2), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Optional"), tkns.range_str(0, 2), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Optional_Expr"), "any", tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[0].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[0].pos()))
        .exit()
        .exit()
        .exit()
        .attach(ctx.create(gram.lpr("OP_QUESTION"), "?", tkns[1].pos()))
        .exit()
        .end();

    TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());


    auto actual = gram.parser("Expr")(ctx, tkns);

    TAUL_LOG(lgr, "actual.has_value() == {}", actual.has_value());

    ASSERT_TRUE(actual.has_value());

    TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());


    EXPECT_TRUE(expected.equiv(*actual));
}

TEST_F(LoadTAULGrammarTests, Syntax_10) {


    // this tests the syntax of kleene-star exprs


    EXPECT_TRUE(gram.contains_lpr("KW_ANY"));
    EXPECT_TRUE(gram.contains_lpr("OP_ASTERISK"));

    ASSERT_TRUE(gram.contains_ppr("Expr"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Primary"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Any"));
    ASSERT_TRUE(gram.contains_ppr("Expr_KleeneStar"));
    ASSERT_TRUE(gram.contains_ppr("Expr_KleeneStar_Expr"));


    std::string src =
        "any*";

    auto tkns = taul::tokenize(gram, src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 2);

    taul::node_ctx ctx;


    auto expected =
        taul::node_assembler()
        .begin(ctx.create(gram.ppr("Expr"), tkns.range_str(0, 2), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_KleeneStar"), tkns.range_str(0, 2), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_KleeneStar_Expr"), "any", tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[0].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[0].pos()))
        .exit()
        .exit()
        .exit()
        .attach(ctx.create(gram.lpr("OP_ASTERISK"), "*", tkns[1].pos()))
        .exit()
        .end();

    TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());


    auto actual = gram.parser("Expr")(ctx, tkns);

    TAUL_LOG(lgr, "actual.has_value() == {}", actual.has_value());

    ASSERT_TRUE(actual.has_value());

    TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());


    EXPECT_TRUE(expected.equiv(*actual));
}

TEST_F(LoadTAULGrammarTests, Syntax_11) {


    // this tests the syntax of kleene-plus exprs


    EXPECT_TRUE(gram.contains_lpr("KW_ANY"));
    EXPECT_TRUE(gram.contains_lpr("OP_PLUS"));

    ASSERT_TRUE(gram.contains_ppr("Expr"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Primary"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Any"));
    ASSERT_TRUE(gram.contains_ppr("Expr_KleenePlus"));
    ASSERT_TRUE(gram.contains_ppr("Expr_KleenePlus_Expr"));


    std::string src =
        "any+";

    auto tkns = taul::tokenize(gram, src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 2);

    taul::node_ctx ctx;


    auto expected =
        taul::node_assembler()
        .begin(ctx.create(gram.ppr("Expr"), tkns.range_str(0, 2), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_KleenePlus"), tkns.range_str(0, 2), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_KleenePlus_Expr"), "any", tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[0].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[0].pos()))
        .exit()
        .exit()
        .exit()
        .attach(ctx.create(gram.lpr("OP_PLUS"), "+", tkns[1].pos()))
        .exit()
        .end();

    TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());


    auto actual = gram.parser("Expr")(ctx, tkns);

    TAUL_LOG(lgr, "actual.has_value() == {}", actual.has_value());

    ASSERT_TRUE(actual.has_value());

    TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());


    EXPECT_TRUE(expected.equiv(*actual));
}

TEST_F(LoadTAULGrammarTests, Syntax_12) {


    // this tests the syntax of sequence exprs


    EXPECT_TRUE(gram.contains_lpr("KW_ANY"));

    ASSERT_TRUE(gram.contains_ppr("Expr"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Primary"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Any"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Sequence"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Sequence_Expr"));


    std::string src =
        "any any any";

    auto tkns = taul::tokenize(gram, src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 3);

    taul::node_ctx ctx;


    auto expected =
        taul::node_assembler()
        .begin(ctx.create(gram.ppr("Expr"), tkns.range_str(0, 3), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Sequence"), tkns.range_str(0, 3), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Sequence_Expr"), "any", tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[0].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[0].pos()))
        .exit()
        .exit()
        .exit()
        .enter(ctx.create(gram.ppr("Expr_Sequence_Expr"), "any", tkns[1].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[1].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[1].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[1].pos()))
        .exit()
        .exit()
        .exit()
        .enter(ctx.create(gram.ppr("Expr_Sequence_Expr"), "any", tkns[2].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[2].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[2].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[2].pos()))
        .exit()
        .exit()
        .exit()
        .exit()
        .end();

    TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());


    auto actual = gram.parser("Expr")(ctx, tkns);

    TAUL_LOG(lgr, "actual.has_value() == {}", actual.has_value());

    ASSERT_TRUE(actual.has_value());

    TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());


    EXPECT_TRUE(expected.equiv(*actual));
}

TEST_F(LoadTAULGrammarTests, Syntax_13) {


    // this tests the syntax of set exprs


    EXPECT_TRUE(gram.contains_lpr("KW_ANY"));
    EXPECT_TRUE(gram.contains_lpr("OP_VBAR"));

    ASSERT_TRUE(gram.contains_ppr("Expr"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Primary"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Any"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Set"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Set_Expr"));


    std::string src =
        "any | any | any";

    auto tkns = taul::tokenize(gram, src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 5);

    taul::node_ctx ctx;


    auto expected =
        taul::node_assembler()
        .begin(ctx.create(gram.ppr("Expr"), tkns.range_str(0, 5), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Set"), tkns.range_str(0, 5), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Set_Expr"), "any", tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[0].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[0].pos()))
        .exit()
        .exit()
        .exit()
        .attach(ctx.create(gram.lpr("OP_VBAR"), "|", tkns[1].pos()))
        .enter(ctx.create(gram.ppr("Expr_Set_Expr"), "any", tkns[2].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[2].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[2].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[2].pos()))
        .exit()
        .exit()
        .exit()
        .attach(ctx.create(gram.lpr("OP_VBAR"), "|", tkns[3].pos()))
        .enter(ctx.create(gram.ppr("Expr_Set_Expr"), "any", tkns[4].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[4].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[4].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[4].pos()))
        .exit()
        .exit()
        .exit()
        .exit()
        .end();

    TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());


    auto actual = gram.parser("Expr")(ctx, tkns);

    TAUL_LOG(lgr, "actual.has_value() == {}", actual.has_value());

    ASSERT_TRUE(actual.has_value());

    TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());


    EXPECT_TRUE(expected.equiv(*actual));
}

