

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
constexpr std::size_t pprs              = 35;
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
    EXPECT_TRUE(gram.contains_lpr("KW_SUPPORT"));
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

    static_assert(pprs == 35);

    EXPECT_TRUE(gram.contains_ppr("Spec"));
    EXPECT_TRUE(gram.contains_ppr("Spec_SyntaxError"));

    EXPECT_TRUE(gram.contains_ppr("Clause"));
    EXPECT_TRUE(gram.contains_ppr("Clause_LexerSection"));
    EXPECT_TRUE(gram.contains_ppr("Clause_ParserSection"));
    EXPECT_TRUE(gram.contains_ppr("Clause_Rule"));
    EXPECT_TRUE(gram.contains_ppr("Clause_Rule_Qualifier"));
    EXPECT_TRUE(gram.contains_ppr("Clause_Rule_Name"));
    EXPECT_TRUE(gram.contains_ppr("Clause_Rule_Expr"));

    EXPECT_TRUE(gram.contains_ppr("Qualifier"));
    EXPECT_TRUE(gram.contains_ppr("Qualifier_Skip"));
    EXPECT_TRUE(gram.contains_ppr("Qualifier_Support"));

    EXPECT_TRUE(gram.contains_ppr("Expr"));
    EXPECT_TRUE(gram.contains_ppr("Expr_Primary"));
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

    //auto lxr = gram.lexer("KW_LEXER");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("KW_LEXER", "lexer"), taul::token(gram.lpr("KW_LEXER"), "lexer"));
    EXPECT_EQ(ctx.match("KW_LEXER", "lexer?"), taul::token(gram.lpr("KW_LEXER"), "lexer"));
    EXPECT_EQ(ctx.match("KW_LEXER", "lexer "), taul::token(gram.lpr("KW_LEXER"), "lexer"));
    EXPECT_EQ(ctx.match("KW_LEXER", "lexer\r\n"), taul::token(gram.lpr("KW_LEXER"), "lexer"));

    EXPECT_EQ(ctx.match("KW_LEXER", "lexerA"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_LEXER", "lexer1"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_LEXER", "lexer_"), taul::token::failure(""));

    EXPECT_EQ(ctx.match("KW_LEXER", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_LEXER", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_LEXER", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_LEXER", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_LEXER", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_LEXER", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_LEXER", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_LEXER", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_LEXER", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_LEXER", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_LEXER", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_LEXER", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, KW_PARSER) {

    ASSERT_TRUE(gram.contains_lpr("KW_PARSER"));

    EXPECT_EQ(gram.lpr("KW_PARSER").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("KW_PARSER");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("KW_PARSER", "parser"), taul::token(gram.lpr("KW_PARSER"), "parser"));
    EXPECT_EQ(ctx.match("KW_PARSER", "parser?"), taul::token(gram.lpr("KW_PARSER"), "parser"));
    EXPECT_EQ(ctx.match("KW_PARSER", "parser "), taul::token(gram.lpr("KW_PARSER"), "parser"));
    EXPECT_EQ(ctx.match("KW_PARSER", "parser\r\n"), taul::token(gram.lpr("KW_PARSER"), "parser"));

    EXPECT_EQ(ctx.match("KW_PARSER", "parserA"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_PARSER", "parser1"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_PARSER", "parser_"), taul::token::failure(""));

    EXPECT_EQ(ctx.match("KW_PARSER", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_PARSER", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_PARSER", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_PARSER", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_PARSER", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_PARSER", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_PARSER", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_PARSER", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_PARSER", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_PARSER", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_PARSER", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_PARSER", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, KW_SECTION) {

    ASSERT_TRUE(gram.contains_lpr("KW_SECTION"));

    EXPECT_EQ(gram.lpr("KW_SECTION").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("KW_SECTION");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("KW_SECTION", "section"), taul::token(gram.lpr("KW_SECTION"), "section"));
    EXPECT_EQ(ctx.match("KW_SECTION", "section?"), taul::token(gram.lpr("KW_SECTION"), "section"));
    EXPECT_EQ(ctx.match("KW_SECTION", "section "), taul::token(gram.lpr("KW_SECTION"), "section"));
    EXPECT_EQ(ctx.match("KW_SECTION", "section\r\n"), taul::token(gram.lpr("KW_SECTION"), "section"));

    EXPECT_EQ(ctx.match("KW_SECTION", "sectionA"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SECTION", "section1"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SECTION", "section_"), taul::token::failure(""));

    EXPECT_EQ(ctx.match("KW_SECTION", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SECTION", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SECTION", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SECTION", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SECTION", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SECTION", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SECTION", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SECTION", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SECTION", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SECTION", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SECTION", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SECTION", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, KW_SKIP) {

    ASSERT_TRUE(gram.contains_lpr("KW_SKIP"));

    EXPECT_EQ(gram.lpr("KW_SKIP").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("KW_SKIP");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("KW_SKIP", "skip"), taul::token(gram.lpr("KW_SKIP"), "skip"));
    EXPECT_EQ(ctx.match("KW_SKIP", "skip?"), taul::token(gram.lpr("KW_SKIP"), "skip"));
    EXPECT_EQ(ctx.match("KW_SKIP", "skip "), taul::token(gram.lpr("KW_SKIP"), "skip"));
    EXPECT_EQ(ctx.match("KW_SKIP", "skip\r\n"), taul::token(gram.lpr("KW_SKIP"), "skip"));

    EXPECT_EQ(ctx.match("KW_SKIP", "skipA"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SKIP", "skip1"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SKIP", "skip_"), taul::token::failure(""));

    EXPECT_EQ(ctx.match("KW_SKIP", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SKIP", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SKIP", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SKIP", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SKIP", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SKIP", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SKIP", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SKIP", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SKIP", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SKIP", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SKIP", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SKIP", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, KW_SUPPORT) {

    ASSERT_TRUE(gram.contains_lpr("KW_SUPPORT"));

    EXPECT_EQ(gram.lpr("KW_SUPPORT").qualifer, taul::qualifier::none);

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("KW_SUPPORT", "support"), taul::token(gram.lpr("KW_SUPPORT"), "support"));
    EXPECT_EQ(ctx.match("KW_SUPPORT", "support?"), taul::token(gram.lpr("KW_SUPPORT"), "support"));
    EXPECT_EQ(ctx.match("KW_SUPPORT", "support "), taul::token(gram.lpr("KW_SUPPORT"), "support"));
    EXPECT_EQ(ctx.match("KW_SUPPORT", "support\r\n"), taul::token(gram.lpr("KW_SUPPORT"), "support"));

    EXPECT_EQ(ctx.match("KW_SUPPORT", "supportA"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SUPPORT", "support1"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SUPPORT", "support_"), taul::token::failure(""));

    EXPECT_EQ(ctx.match("KW_SUPPORT", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SUPPORT", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SUPPORT", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SUPPORT", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SUPPORT", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SUPPORT", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SUPPORT", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SUPPORT", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SUPPORT", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SUPPORT", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SUPPORT", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_SUPPORT", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, KW_END) {

    ASSERT_TRUE(gram.contains_lpr("KW_END"));

    EXPECT_EQ(gram.lpr("KW_END").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("KW_END");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("KW_END", "end"), taul::token(gram.lpr("KW_END"), "end"));
    EXPECT_EQ(ctx.match("KW_END", "end?"), taul::token(gram.lpr("KW_END"), "end"));
    EXPECT_EQ(ctx.match("KW_END", "end "), taul::token(gram.lpr("KW_END"), "end"));
    EXPECT_EQ(ctx.match("KW_END", "end\r\n"), taul::token(gram.lpr("KW_END"), "end"));

    EXPECT_EQ(ctx.match("KW_END", "endA"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_END", "end1"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_END", "end_"), taul::token::failure(""));

    EXPECT_EQ(ctx.match("KW_END", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_END", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_END", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_END", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_END", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_END", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_END", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_END", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_END", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_END", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_END", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_END", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, KW_ANY) {

    ASSERT_TRUE(gram.contains_lpr("KW_ANY"));

    EXPECT_EQ(gram.lpr("KW_ANY").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("KW_ANY");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("KW_ANY", "any"), taul::token(gram.lpr("KW_ANY"), "any"));
    EXPECT_EQ(ctx.match("KW_ANY", "any?"), taul::token(gram.lpr("KW_ANY"), "any"));
    EXPECT_EQ(ctx.match("KW_ANY", "any "), taul::token(gram.lpr("KW_ANY"), "any"));
    EXPECT_EQ(ctx.match("KW_ANY", "any\r\n"), taul::token(gram.lpr("KW_ANY"), "any"));

    EXPECT_EQ(ctx.match("KW_ANY", "anyA"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_ANY", "any1"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_ANY", "any_"), taul::token::failure(""));

    EXPECT_EQ(ctx.match("KW_ANY", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_ANY", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_ANY", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_ANY", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_ANY", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_ANY", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_ANY", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_ANY", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_ANY", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_ANY", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_ANY", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_ANY", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, KW_TOKEN) {

    ASSERT_TRUE(gram.contains_lpr("KW_TOKEN"));

    EXPECT_EQ(gram.lpr("KW_TOKEN").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("KW_TOKEN");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("KW_TOKEN", "token"), taul::token(gram.lpr("KW_TOKEN"), "token"));
    EXPECT_EQ(ctx.match("KW_TOKEN", "token?"), taul::token(gram.lpr("KW_TOKEN"), "token"));
    EXPECT_EQ(ctx.match("KW_TOKEN", "token "), taul::token(gram.lpr("KW_TOKEN"), "token"));
    EXPECT_EQ(ctx.match("KW_TOKEN", "token\r\n"), taul::token(gram.lpr("KW_TOKEN"), "token"));

    EXPECT_EQ(ctx.match("KW_TOKEN", "tokenA"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_TOKEN", "token1"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_TOKEN", "token_"), taul::token::failure(""));

    EXPECT_EQ(ctx.match("KW_TOKEN", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_TOKEN", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_TOKEN", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_TOKEN", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_TOKEN", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_TOKEN", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_TOKEN", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_TOKEN", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_TOKEN", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_TOKEN", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_TOKEN", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_TOKEN", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, KW_FAILURE) {

    ASSERT_TRUE(gram.contains_lpr("KW_FAILURE"));

    EXPECT_EQ(gram.lpr("KW_FAILURE").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("KW_FAILURE");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("KW_FAILURE", "failure"), taul::token(gram.lpr("KW_FAILURE"), "failure"));
    EXPECT_EQ(ctx.match("KW_FAILURE", "failure?"), taul::token(gram.lpr("KW_FAILURE"), "failure"));
    EXPECT_EQ(ctx.match("KW_FAILURE", "failure "), taul::token(gram.lpr("KW_FAILURE"), "failure"));
    EXPECT_EQ(ctx.match("KW_FAILURE", "failure\r\n"), taul::token(gram.lpr("KW_FAILURE"), "failure"));

    EXPECT_EQ(ctx.match("KW_FAILURE", "failureA"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_FAILURE", "failure1"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_FAILURE", "failure_"), taul::token::failure(""));

    EXPECT_EQ(ctx.match("KW_FAILURE", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_FAILURE", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_FAILURE", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_FAILURE", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_FAILURE", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_FAILURE", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_FAILURE", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_FAILURE", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_FAILURE", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_FAILURE", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_FAILURE", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("KW_FAILURE", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_PERIOD) {

    ASSERT_TRUE(gram.contains_lpr("OP_PERIOD"));

    EXPECT_EQ(gram.lpr("OP_PERIOD").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("OP_PERIOD");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("OP_PERIOD", ".abc"), taul::token(gram.lpr("OP_PERIOD"), "."));
    EXPECT_EQ(ctx.match("OP_PERIOD", ".123"), taul::token(gram.lpr("OP_PERIOD"), "."));
    EXPECT_EQ(ctx.match("OP_PERIOD", ".!@#"), taul::token(gram.lpr("OP_PERIOD"), "."));
    EXPECT_EQ(ctx.match("OP_PERIOD", ". "), taul::token(gram.lpr("OP_PERIOD"), "."));
    EXPECT_EQ(ctx.match("OP_PERIOD", ".\t"), taul::token(gram.lpr("OP_PERIOD"), "."));

    EXPECT_EQ(ctx.match("OP_PERIOD", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PERIOD", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PERIOD", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PERIOD", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PERIOD", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PERIOD", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PERIOD", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PERIOD", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PERIOD", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PERIOD", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PERIOD", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PERIOD", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_COMMA) {

    ASSERT_TRUE(gram.contains_lpr("OP_COMMA"));

    EXPECT_EQ(gram.lpr("OP_COMMA").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("OP_COMMA");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("OP_COMMA", ",abc"), taul::token(gram.lpr("OP_COMMA"), ","));
    EXPECT_EQ(ctx.match("OP_COMMA", ",123"), taul::token(gram.lpr("OP_COMMA"), ","));
    EXPECT_EQ(ctx.match("OP_COMMA", ",!@#"), taul::token(gram.lpr("OP_COMMA"), ","));
    EXPECT_EQ(ctx.match("OP_COMMA", ", "), taul::token(gram.lpr("OP_COMMA"), ","));
    EXPECT_EQ(ctx.match("OP_COMMA", ",\t"), taul::token(gram.lpr("OP_COMMA"), ","));

    EXPECT_EQ(ctx.match("OP_COMMA", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COMMA", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COMMA", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COMMA", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COMMA", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COMMA", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COMMA", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COMMA", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COMMA", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COMMA", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COMMA", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COMMA", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_COLON) {

    ASSERT_TRUE(gram.contains_lpr("OP_COLON"));

    EXPECT_EQ(gram.lpr("OP_COLON").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("OP_COLON");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("OP_COLON", ":abc"), taul::token(gram.lpr("OP_COLON"), ":"));
    EXPECT_EQ(ctx.match("OP_COLON", ":123"), taul::token(gram.lpr("OP_COLON"), ":"));
    EXPECT_EQ(ctx.match("OP_COLON", ":!@#"), taul::token(gram.lpr("OP_COLON"), ":"));
    EXPECT_EQ(ctx.match("OP_COLON", ": "), taul::token(gram.lpr("OP_COLON"), ":"));
    EXPECT_EQ(ctx.match("OP_COLON", ":\t"), taul::token(gram.lpr("OP_COLON"), ":"));

    EXPECT_EQ(ctx.match("OP_COLON", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COLON", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COLON", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COLON", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COLON", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COLON", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COLON", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COLON", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COLON", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COLON", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COLON", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_COLON", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_SEMICOLON) {

    ASSERT_TRUE(gram.contains_lpr("OP_SEMICOLON"));

    EXPECT_EQ(gram.lpr("OP_SEMICOLON").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("OP_SEMICOLON");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("OP_SEMICOLON", ";abc"), taul::token(gram.lpr("OP_SEMICOLON"), ";"));
    EXPECT_EQ(ctx.match("OP_SEMICOLON", ";123"), taul::token(gram.lpr("OP_SEMICOLON"), ";"));
    EXPECT_EQ(ctx.match("OP_SEMICOLON", ";!@#"), taul::token(gram.lpr("OP_SEMICOLON"), ";"));
    EXPECT_EQ(ctx.match("OP_SEMICOLON", "; "), taul::token(gram.lpr("OP_SEMICOLON"), ";"));
    EXPECT_EQ(ctx.match("OP_SEMICOLON", ";\t"), taul::token(gram.lpr("OP_SEMICOLON"), ";"));

    EXPECT_EQ(ctx.match("OP_SEMICOLON", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_SEMICOLON", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_SEMICOLON", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_SEMICOLON", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_SEMICOLON", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_SEMICOLON", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_SEMICOLON", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_SEMICOLON", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_SEMICOLON", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_SEMICOLON", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_SEMICOLON", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_SEMICOLON", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_VBAR) {

    ASSERT_TRUE(gram.contains_lpr("OP_VBAR"));

    EXPECT_EQ(gram.lpr("OP_VBAR").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("OP_VBAR");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("OP_VBAR", "|abc"), taul::token(gram.lpr("OP_VBAR"), "|"));
    EXPECT_EQ(ctx.match("OP_VBAR", "|123"), taul::token(gram.lpr("OP_VBAR"), "|"));
    EXPECT_EQ(ctx.match("OP_VBAR", "|!@#"), taul::token(gram.lpr("OP_VBAR"), "|"));
    EXPECT_EQ(ctx.match("OP_VBAR", "| "), taul::token(gram.lpr("OP_VBAR"), "|"));
    EXPECT_EQ(ctx.match("OP_VBAR", "|\t"), taul::token(gram.lpr("OP_VBAR"), "|"));

    EXPECT_EQ(ctx.match("OP_VBAR", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_VBAR", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_VBAR", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_VBAR", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_VBAR", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_VBAR", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_VBAR", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_VBAR", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_VBAR", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_VBAR", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_VBAR", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_VBAR", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_QUESTION) {

    ASSERT_TRUE(gram.contains_lpr("OP_QUESTION"));

    EXPECT_EQ(gram.lpr("OP_QUESTION").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("OP_QUESTION");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("OP_QUESTION", "?abc"), taul::token(gram.lpr("OP_QUESTION"), "?"));
    EXPECT_EQ(ctx.match("OP_QUESTION", "?123"), taul::token(gram.lpr("OP_QUESTION"), "?"));
    EXPECT_EQ(ctx.match("OP_QUESTION", "?!@#"), taul::token(gram.lpr("OP_QUESTION"), "?"));
    EXPECT_EQ(ctx.match("OP_QUESTION", "? "), taul::token(gram.lpr("OP_QUESTION"), "?"));
    EXPECT_EQ(ctx.match("OP_QUESTION", "?\t"), taul::token(gram.lpr("OP_QUESTION"), "?"));

    EXPECT_EQ(ctx.match("OP_QUESTION", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_QUESTION", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_QUESTION", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_QUESTION", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_QUESTION", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_QUESTION", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_QUESTION", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_QUESTION", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_QUESTION", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_QUESTION", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_QUESTION", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_QUESTION", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_ASTERISK) {

    ASSERT_TRUE(gram.contains_lpr("OP_ASTERISK"));

    EXPECT_EQ(gram.lpr("OP_ASTERISK").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("OP_ASTERISK");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("OP_ASTERISK", "*abc"), taul::token(gram.lpr("OP_ASTERISK"), "*"));
    EXPECT_EQ(ctx.match("OP_ASTERISK", "*123"), taul::token(gram.lpr("OP_ASTERISK"), "*"));
    EXPECT_EQ(ctx.match("OP_ASTERISK", "*!@#"), taul::token(gram.lpr("OP_ASTERISK"), "*"));
    EXPECT_EQ(ctx.match("OP_ASTERISK", "* "), taul::token(gram.lpr("OP_ASTERISK"), "*"));
    EXPECT_EQ(ctx.match("OP_ASTERISK", "*\t"), taul::token(gram.lpr("OP_ASTERISK"), "*"));

    EXPECT_EQ(ctx.match("OP_ASTERISK", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_ASTERISK", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_ASTERISK", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_ASTERISK", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_ASTERISK", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_ASTERISK", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_ASTERISK", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_ASTERISK", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_ASTERISK", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_ASTERISK", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_ASTERISK", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_ASTERISK", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_PLUS) {

    ASSERT_TRUE(gram.contains_lpr("OP_PLUS"));

    EXPECT_EQ(gram.lpr("OP_PLUS").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("OP_PLUS");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("OP_PLUS", "+abc"), taul::token(gram.lpr("OP_PLUS"), "+"));
    EXPECT_EQ(ctx.match("OP_PLUS", "+123"), taul::token(gram.lpr("OP_PLUS"), "+"));
    EXPECT_EQ(ctx.match("OP_PLUS", "+!@#"), taul::token(gram.lpr("OP_PLUS"), "+"));
    EXPECT_EQ(ctx.match("OP_PLUS", "+ "), taul::token(gram.lpr("OP_PLUS"), "+"));
    EXPECT_EQ(ctx.match("OP_PLUS", "+\t"), taul::token(gram.lpr("OP_PLUS"), "+"));

    EXPECT_EQ(ctx.match("OP_PLUS", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PLUS", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PLUS", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PLUS", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PLUS", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PLUS", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PLUS", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PLUS", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PLUS", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PLUS", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PLUS", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_PLUS", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_TILDE) {

    ASSERT_TRUE(gram.contains_lpr("OP_TILDE"));

    EXPECT_EQ(gram.lpr("OP_TILDE").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("OP_TILDE");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("OP_TILDE", "~abc"), taul::token(gram.lpr("OP_TILDE"), "~"));
    EXPECT_EQ(ctx.match("OP_TILDE", "~123"), taul::token(gram.lpr("OP_TILDE"), "~"));
    EXPECT_EQ(ctx.match("OP_TILDE", "~!@#"), taul::token(gram.lpr("OP_TILDE"), "~"));
    EXPECT_EQ(ctx.match("OP_TILDE", "~ "), taul::token(gram.lpr("OP_TILDE"), "~"));
    EXPECT_EQ(ctx.match("OP_TILDE", "~\t"), taul::token(gram.lpr("OP_TILDE"), "~"));

    EXPECT_EQ(ctx.match("OP_TILDE", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_TILDE", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_TILDE", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_TILDE", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_TILDE", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_TILDE", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_TILDE", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_TILDE", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_TILDE", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_TILDE", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_TILDE", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_AMPERSAND) {

    ASSERT_TRUE(gram.contains_lpr("OP_AMPERSAND"));

    EXPECT_EQ(gram.lpr("OP_AMPERSAND").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("OP_AMPERSAND");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("OP_AMPERSAND", "&abc"), taul::token(gram.lpr("OP_AMPERSAND"), "&"));
    EXPECT_EQ(ctx.match("OP_AMPERSAND", "&123"), taul::token(gram.lpr("OP_AMPERSAND"), "&"));
    EXPECT_EQ(ctx.match("OP_AMPERSAND", "&!@#"), taul::token(gram.lpr("OP_AMPERSAND"), "&"));
    EXPECT_EQ(ctx.match("OP_AMPERSAND", "& "), taul::token(gram.lpr("OP_AMPERSAND"), "&"));
    EXPECT_EQ(ctx.match("OP_AMPERSAND", "&\t"), taul::token(gram.lpr("OP_AMPERSAND"), "&"));

    EXPECT_EQ(ctx.match("OP_AMPERSAND", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_AMPERSAND", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_AMPERSAND", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_AMPERSAND", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_AMPERSAND", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_AMPERSAND", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_AMPERSAND", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_AMPERSAND", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_AMPERSAND", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_AMPERSAND", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_AMPERSAND", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_AMPERSAND", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_MINUS) {

    ASSERT_TRUE(gram.contains_lpr("OP_MINUS"));

    EXPECT_EQ(gram.lpr("OP_MINUS").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("OP_MINUS");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("OP_MINUS", "-abc"), taul::token(gram.lpr("OP_MINUS"), "-"));
    EXPECT_EQ(ctx.match("OP_MINUS", "-123"), taul::token(gram.lpr("OP_MINUS"), "-"));
    EXPECT_EQ(ctx.match("OP_MINUS", "-!@#"), taul::token(gram.lpr("OP_MINUS"), "-"));
    EXPECT_EQ(ctx.match("OP_MINUS", "- "), taul::token(gram.lpr("OP_MINUS"), "-"));
    EXPECT_EQ(ctx.match("OP_MINUS", "-\t"), taul::token(gram.lpr("OP_MINUS"), "-"));

    EXPECT_EQ(ctx.match("OP_MINUS", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_MINUS", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_MINUS", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_MINUS", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_MINUS", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_MINUS", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_MINUS", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_MINUS", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_MINUS", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_MINUS", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_MINUS", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_MINUS", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_L_ROUND) {

    ASSERT_TRUE(gram.contains_lpr("OP_L_ROUND"));

    EXPECT_EQ(gram.lpr("OP_L_ROUND").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("OP_L_ROUND");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("OP_L_ROUND", "(abc"), taul::token(gram.lpr("OP_L_ROUND"), "("));
    EXPECT_EQ(ctx.match("OP_L_ROUND", "(123"), taul::token(gram.lpr("OP_L_ROUND"), "("));
    EXPECT_EQ(ctx.match("OP_L_ROUND", "(!@#"), taul::token(gram.lpr("OP_L_ROUND"), "("));
    EXPECT_EQ(ctx.match("OP_L_ROUND", "( "), taul::token(gram.lpr("OP_L_ROUND"), "("));
    EXPECT_EQ(ctx.match("OP_L_ROUND", "(\t"), taul::token(gram.lpr("OP_L_ROUND"), "("));

    EXPECT_EQ(ctx.match("OP_L_ROUND", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_L_ROUND", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_L_ROUND", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_L_ROUND", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_L_ROUND", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_L_ROUND", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_L_ROUND", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_L_ROUND", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_L_ROUND", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_L_ROUND", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_L_ROUND", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_L_ROUND", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, OP_R_ROUND) {

    ASSERT_TRUE(gram.contains_lpr("OP_R_ROUND"));

    EXPECT_EQ(gram.lpr("OP_R_ROUND").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("OP_R_ROUND");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("OP_R_ROUND", ")abc"), taul::token(gram.lpr("OP_R_ROUND"), ")"));
    EXPECT_EQ(ctx.match("OP_R_ROUND", ")123"), taul::token(gram.lpr("OP_R_ROUND"), ")"));
    EXPECT_EQ(ctx.match("OP_R_ROUND", ")!@#"), taul::token(gram.lpr("OP_R_ROUND"), ")"));
    EXPECT_EQ(ctx.match("OP_R_ROUND", ") "), taul::token(gram.lpr("OP_R_ROUND"), ")"));
    EXPECT_EQ(ctx.match("OP_R_ROUND", ")\t"), taul::token(gram.lpr("OP_R_ROUND"), ")"));

    EXPECT_EQ(ctx.match("OP_R_ROUND", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_R_ROUND", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_R_ROUND", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_R_ROUND", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_R_ROUND", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_R_ROUND", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_R_ROUND", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_R_ROUND", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_R_ROUND", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_R_ROUND", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_R_ROUND", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("OP_R_ROUND", "#abc"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, IDENTIFIER) {

    ASSERT_TRUE(gram.contains_lpr("IDENTIFIER"));

    EXPECT_EQ(gram.lpr("IDENTIFIER").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("IDENTIFIER");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("IDENTIFIER", "abc(abc"), taul::token(gram.lpr("IDENTIFIER"), "abc"));
    EXPECT_EQ(ctx.match("IDENTIFIER", "_a_b_c&abc"), taul::token(gram.lpr("IDENTIFIER"), "_a_b_c"));
    EXPECT_EQ(ctx.match("IDENTIFIER", "_123*abc"), taul::token(gram.lpr("IDENTIFIER"), "_123"));
    EXPECT_EQ(ctx.match("IDENTIFIER", "_&abc"), taul::token(gram.lpr("IDENTIFIER"), "_"));
    EXPECT_EQ(ctx.match("IDENTIFIER", "A&abc"), taul::token(gram.lpr("IDENTIFIER"), "A"));

    static_assert(keywords == 9);

    // when matching *specifically* an IDENTIFIER, not the whole grammar,
    // then the below keywords will be matched as IDENTIFIER tokens

    EXPECT_EQ(ctx.match("IDENTIFIER", "lexer"), taul::token(gram.lpr("IDENTIFIER"), "lexer"));
    EXPECT_EQ(ctx.match("IDENTIFIER", "parser"), taul::token(gram.lpr("IDENTIFIER"), "parser"));
    EXPECT_EQ(ctx.match("IDENTIFIER", "section"), taul::token(gram.lpr("IDENTIFIER"), "section"));
    EXPECT_EQ(ctx.match("IDENTIFIER", "skip"), taul::token(gram.lpr("IDENTIFIER"), "skip"));
    EXPECT_EQ(ctx.match("IDENTIFIER", "support"), taul::token(gram.lpr("IDENTIFIER"), "support"));
    EXPECT_EQ(ctx.match("IDENTIFIER", "end"), taul::token(gram.lpr("IDENTIFIER"), "end"));
    EXPECT_EQ(ctx.match("IDENTIFIER", "any"), taul::token(gram.lpr("IDENTIFIER"), "any"));
    EXPECT_EQ(ctx.match("IDENTIFIER", "token"), taul::token(gram.lpr("IDENTIFIER"), "token"));
    EXPECT_EQ(ctx.match("IDENTIFIER", "failure"), taul::token(gram.lpr("IDENTIFIER"), "failure"));

    EXPECT_EQ(ctx.match("IDENTIFIER", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("IDENTIFIER", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("IDENTIFIER", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("IDENTIFIER", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("IDENTIFIER", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("IDENTIFIER", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("IDENTIFIER", "@#&"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, STRING) {

    ASSERT_TRUE(gram.contains_lpr("STRING"));

    EXPECT_EQ(gram.lpr("STRING").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("STRING");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("STRING", "''abc"), taul::token(gram.lpr("STRING"), "''"));
    EXPECT_EQ(ctx.match("STRING", "'abc'abc"), taul::token(gram.lpr("STRING"), "'abc'"));
    EXPECT_EQ(ctx.match("STRING", "'123'abc"), taul::token(gram.lpr("STRING"), "'123'"));
    EXPECT_EQ(ctx.match("STRING", "'!@#'abc"), taul::token(gram.lpr("STRING"), "'!@#'"));
    EXPECT_EQ(ctx.match("STRING", "' \t\r\n'abc"), taul::token(gram.lpr("STRING"), "' \t\r\n'"));
    EXPECT_EQ(ctx.match("STRING", "'\\a\\b\\c'abc"), taul::token(gram.lpr("STRING"), "'\\a\\b\\c'"));
    EXPECT_EQ(ctx.match("STRING", "'\\1\\2\\3'abc"), taul::token(gram.lpr("STRING"), "'\\1\\2\\3'"));
    EXPECT_EQ(ctx.match("STRING", "'\\!\\@\\#'abc"), taul::token(gram.lpr("STRING"), "'\\!\\@\\#'"));
    EXPECT_EQ(ctx.match("STRING", "'\\ \\\t\\\r\\\n'abc"), taul::token(gram.lpr("STRING"), "'\\ \\\t\\\r\\\n'"));
    EXPECT_EQ(ctx.match("STRING", "'\\''abc"), taul::token(gram.lpr("STRING"), "'\\''"));
    EXPECT_EQ(ctx.match("STRING", "'\\\\''abc"), taul::token(gram.lpr("STRING"), "'\\\\'"));

    EXPECT_EQ(ctx.match("STRING", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("STRING", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("STRING", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("STRING", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("STRING", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("STRING", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("STRING", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("STRING", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("STRING", "\"\""), taul::token::failure(""));

    EXPECT_EQ(ctx.match("STRING", "'"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("STRING", "'\\'"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, CHARSET) {

    ASSERT_TRUE(gram.contains_lpr("CHARSET"));

    EXPECT_EQ(gram.lpr("CHARSET").qualifer, taul::qualifier::none);

    //auto lxr = gram.lexer("CHARSET");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("CHARSET", "[]abc"), taul::token(gram.lpr("CHARSET"), "[]"));
    EXPECT_EQ(ctx.match("CHARSET", "[abc]abc"), taul::token(gram.lpr("CHARSET"), "[abc]"));
    EXPECT_EQ(ctx.match("CHARSET", "[123]abc"), taul::token(gram.lpr("CHARSET"), "[123]"));
    EXPECT_EQ(ctx.match("CHARSET", "[!@#]abc"), taul::token(gram.lpr("CHARSET"), "[!@#]"));
    EXPECT_EQ(ctx.match("CHARSET", "[ \t\r\n]abc"), taul::token(gram.lpr("CHARSET"), "[ \t\r\n]"));
    EXPECT_EQ(ctx.match("CHARSET", "[\\a\\b\\c]abc"), taul::token(gram.lpr("CHARSET"), "[\\a\\b\\c]"));
    EXPECT_EQ(ctx.match("CHARSET", "[\\1\\2\\3]abc"), taul::token(gram.lpr("CHARSET"), "[\\1\\2\\3]"));
    EXPECT_EQ(ctx.match("CHARSET", "[\\!\\@\\#]abc"), taul::token(gram.lpr("CHARSET"), "[\\!\\@\\#]"));
    EXPECT_EQ(ctx.match("CHARSET", "[\\ \\\t\\\r\\\n]abc"), taul::token(gram.lpr("CHARSET"), "[\\ \\\t\\\r\\\n]"));
    EXPECT_EQ(ctx.match("CHARSET", "[\\]]abc"), taul::token(gram.lpr("CHARSET"), "[\\]]"));
    EXPECT_EQ(ctx.match("CHARSET", "[\\\\]]abc"), taul::token(gram.lpr("CHARSET"), "[\\\\]"));

    EXPECT_EQ(ctx.match("CHARSET", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("CHARSET", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("CHARSET", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("CHARSET", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("CHARSET", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("CHARSET", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("CHARSET", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("CHARSET", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("CHARSET", "\"\""), taul::token::failure(""));

    EXPECT_EQ(ctx.match("CHARSET", "["), taul::token::failure(""));
    EXPECT_EQ(ctx.match("CHARSET", "[\\]"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, WHITESPACE) {

    ASSERT_TRUE(gram.contains_lpr("WHITESPACE"));

    EXPECT_EQ(gram.lpr("WHITESPACE").qualifer, taul::qualifier::skip);

    //auto lxr = gram.lexer("WHITESPACE");

    taul::context ctx(gram, lgr);

    for (std::uint8_t i = 0x00; i < 0x80; i++) {
        // skip stray nulls for now
        if (i == 0x00) {
            continue;
        }
        std::string s = " a";
        s[0] = (char)i;
        taul::token tkn = ctx.match("WHITESPACE", s);
        if (char(i) == ' ' || char(i) == '\t') {
            EXPECT_EQ(tkn, taul::token(gram.lpr("WHITESPACE"), s.substr(0, 1))) << "std::size_t(s[0])==" << size_t(i);
        }
        else {
            EXPECT_EQ(tkn, taul::token::failure("")) << "std::size_t(s[0])==" << size_t(i);
        }
    }

    EXPECT_EQ(ctx.match("WHITESPACE", ""), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, NEWLINE) {

    ASSERT_TRUE(gram.contains_lpr("NEWLINE"));

    EXPECT_EQ(gram.lpr("NEWLINE").qualifer, taul::qualifier::skip);

    //auto lxr = gram.lexer("NEWLINE");

    taul::context ctx(gram, lgr);

    for (std::uint8_t i = 0x00; i < 0x80; i++) {
        // skip stray nulls for now
        if (i == 0x00) {
            continue;
        }
        std::string s = " a";
        s[0] = (char)i;
        taul::token tkn = ctx.match("NEWLINE", s);
        if (char(i) == '\r' || char(i) == '\n') {
            EXPECT_EQ(tkn, taul::token(gram.lpr("NEWLINE"), s.substr(0, 1))) << "std::size_t(s[0])==" << size_t(i);
        }
        else {
            EXPECT_EQ(tkn, taul::token::failure("")) << "std::size_t(s[0])==" << size_t(i);
        }
    }

    EXPECT_EQ(ctx.match("NEWLINE", "\r\na"), taul::token(gram.lpr("NEWLINE"), "\r\n"));

    EXPECT_EQ(ctx.match("NEWLINE", ""), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, SL_COMMENT) {

    ASSERT_TRUE(gram.contains_lpr("SL_COMMENT"));

    EXPECT_EQ(gram.lpr("SL_COMMENT").qualifer, taul::qualifier::skip);

    //auto lxr = gram.lexer("SL_COMMENT");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("SL_COMMENT", "#"), taul::token(gram.lpr("SL_COMMENT"), "#"));
    EXPECT_EQ(ctx.match("SL_COMMENT", "####"), taul::token(gram.lpr("SL_COMMENT"), "####"));
    EXPECT_EQ(ctx.match("SL_COMMENT", "#abc123!@#"), taul::token(gram.lpr("SL_COMMENT"), "#abc123!@#"));
    EXPECT_EQ(ctx.match("SL_COMMENT", "# abc\t123!@#"), taul::token(gram.lpr("SL_COMMENT"), "# abc\t123!@#"));
    EXPECT_EQ(ctx.match("SL_COMMENT", "#abc123!@#\r"), taul::token(gram.lpr("SL_COMMENT"), "#abc123!@#"));
    EXPECT_EQ(ctx.match("SL_COMMENT", "# abc\t123!@#\r"), taul::token(gram.lpr("SL_COMMENT"), "# abc\t123!@#"));
    EXPECT_EQ(ctx.match("SL_COMMENT", "#abc123!@#\r\n"), taul::token(gram.lpr("SL_COMMENT"), "#abc123!@#"));
    EXPECT_EQ(ctx.match("SL_COMMENT", "# abc\t123!@#\r\n"), taul::token(gram.lpr("SL_COMMENT"), "# abc\t123!@#"));
    EXPECT_EQ(ctx.match("SL_COMMENT", "#abc123!@#\n"), taul::token(gram.lpr("SL_COMMENT"), "#abc123!@#"));
    EXPECT_EQ(ctx.match("SL_COMMENT", "# abc\t123!@#\n"), taul::token(gram.lpr("SL_COMMENT"), "# abc\t123!@#"));

    EXPECT_EQ(ctx.match("SL_COMMENT", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("SL_COMMENT", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("SL_COMMENT", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("SL_COMMENT", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("SL_COMMENT", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("SL_COMMENT", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("SL_COMMENT", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("SL_COMMENT", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("SL_COMMENT", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("SL_COMMENT", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("SL_COMMENT", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("SL_COMMENT", "!##!"), taul::token::failure(""));
}

TEST_F(LoadTAULGrammarTests, ML_COMMENT) {

    ASSERT_TRUE(gram.contains_lpr("ML_COMMENT"));

    EXPECT_EQ(gram.lpr("ML_COMMENT").qualifer, taul::qualifier::skip);

    //auto lxr = gram.lexer("ML_COMMENT");

    taul::context ctx(gram, lgr);

    EXPECT_EQ(ctx.match("ML_COMMENT", "!##!abc"), taul::token(gram.lpr("ML_COMMENT"), "!##!"));
    EXPECT_EQ(ctx.match("ML_COMMENT", "!#"), taul::token(gram.lpr("ML_COMMENT"), "!#"));
    EXPECT_EQ(ctx.match("ML_COMMENT", "!# \tabc!@#123\r\n \r \n#!abc"), taul::token(gram.lpr("ML_COMMENT"), "!# \tabc!@#123\r\n \r \n#!"));
    EXPECT_EQ(ctx.match("ML_COMMENT", "!# \tabc!@#123\r\n \r \n"), taul::token(gram.lpr("ML_COMMENT"), "!# \tabc!@#123\r\n \r \n"));

    EXPECT_EQ(ctx.match("ML_COMMENT", ""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("ML_COMMENT", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("ML_COMMENT", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("ML_COMMENT", "!@#"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("ML_COMMENT", "_"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("ML_COMMENT", " "), taul::token::failure(""));
    EXPECT_EQ(ctx.match("ML_COMMENT", "\t"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("ML_COMMENT", "~~~"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("ML_COMMENT", "@#&"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("ML_COMMENT", "''"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("ML_COMMENT", "\"\""), taul::token::failure(""));
    EXPECT_EQ(ctx.match("ML_COMMENT", "#abc"), taul::token::failure(""));
    EXPECT_EQ(ctx.match("ML_COMMENT", "#!"), taul::token::failure(""));
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


    taul::context ctx(gram, lgr);


    std::string src =
        "\n";

    auto tkns = ctx.tokenize(src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 0);


    auto expected =
        taul::node_assembler()
        .begin(ctx.create(gram.ppr("Spec"), "", 0))
        .end();

    TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());


    auto actual = ctx.parse("Spec", tkns);
    
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
    EXPECT_TRUE(gram.contains_lpr("KW_SUPPORT"));
    EXPECT_TRUE(gram.contains_lpr("KW_ANY"));
    EXPECT_TRUE(gram.contains_lpr("OP_COLON"));
    EXPECT_TRUE(gram.contains_lpr("OP_SEMICOLON"));
    EXPECT_TRUE(gram.contains_lpr("IDENTIFIER"));

    ASSERT_TRUE(gram.contains_ppr("Spec"));
    ASSERT_TRUE(gram.contains_ppr("Clause"));
    ASSERT_TRUE(gram.contains_ppr("Clause_LexerSection"));
    ASSERT_TRUE(gram.contains_ppr("Clause_ParserSection"));
    EXPECT_TRUE(gram.contains_ppr("Clause_Rule"));
    EXPECT_TRUE(gram.contains_ppr("Clause_Rule_Qualifier"));
    EXPECT_TRUE(gram.contains_ppr("Clause_Rule_Name"));
    EXPECT_TRUE(gram.contains_ppr("Clause_Rule_Expr"));
    EXPECT_TRUE(gram.contains_ppr("Qualifier"));
    EXPECT_TRUE(gram.contains_ppr("Qualifier_Skip"));
    EXPECT_TRUE(gram.contains_ppr("Qualifier_Support"));
    ASSERT_TRUE(gram.contains_ppr("Expr"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Primary"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Any"));


    taul::context ctx(gram, lgr);


    std::string src =
        "lexer section:\n"
        "skip ABC : any ;\n"
        "support DEF : any ;\n"
        "parser section:\n"
        "Abc : any ;";

    auto tkns = ctx.tokenize(src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 20);


    auto expected =
        taul::node_assembler()
        .begin(ctx.create(gram.ppr("Spec"), tkns.range_str(0, 20), tkns[0].pos()))

        .enter(ctx.create(gram.ppr("Clause"), tkns.range_str(0, 3), tkns[0].pos()))
        .enter(ctx.create(gram.ppr("Clause_LexerSection"), tkns.range_str(0, 3), tkns[0].pos()))
        .attach(ctx.create(gram.lpr("KW_LEXER"), "lexer", tkns[0].pos()))
        .attach(ctx.create(gram.lpr("KW_SECTION"), "section", tkns[1].pos()))
        .attach(ctx.create(gram.lpr("OP_COLON"), ":", tkns[2].pos()))
        .exit()
        .exit()

        .enter(ctx.create(gram.ppr("Clause"), tkns.range_str(3, 5), tkns[3].pos()))
        .enter(ctx.create(gram.ppr("Clause_Rule"), tkns.range_str(3, 5), tkns[3].pos()))
        .enter(ctx.create(gram.ppr("Clause_Rule_Qualifier"), "skip", tkns[3].pos()))
        .enter(ctx.create(gram.ppr("Qualifier"), "skip", tkns[3].pos()))
        .enter(ctx.create(gram.ppr("Qualifier_Skip"), "skip", tkns[3].pos()))
        .attach(ctx.create(gram.lpr("KW_SKIP"), "skip", tkns[3].pos()))
        .exit()
        .exit()
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

        .enter(ctx.create(gram.ppr("Clause"), tkns.range_str(8, 5), tkns[8].pos()))
        .enter(ctx.create(gram.ppr("Clause_Rule"), tkns.range_str(8, 5), tkns[8].pos()))
        .enter(ctx.create(gram.ppr("Clause_Rule_Qualifier"), "support", tkns[8].pos()))
        .enter(ctx.create(gram.ppr("Qualifier"), "support", tkns[8].pos()))
        .enter(ctx.create(gram.ppr("Qualifier_Support"), "support", tkns[8].pos()))
        .attach(ctx.create(gram.lpr("KW_SUPPORT"), "support", tkns[8].pos()))
        .exit()
        .exit()
        .exit()
        .enter(ctx.create(gram.ppr("Clause_Rule_Name"), "DEF", tkns[9].pos()))
        .attach(ctx.create(gram.lpr("IDENTIFIER"), "DEF", tkns[9].pos()))
        .exit()
        .attach(ctx.create(gram.lpr("OP_COLON"), ":", tkns[10].pos()))
        .enter(ctx.create(gram.ppr("Clause_Rule_Expr"), "any", tkns[11].pos()))
        .enter(ctx.create(gram.ppr("Expr"), "any", tkns[11].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[11].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[11].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[11].pos()))
        .exit()
        .exit()
        .exit()
        .exit()
        .attach(ctx.create(gram.lpr("OP_SEMICOLON"), ";", tkns[12].pos()))
        .exit()
        .exit()
        
        .enter(ctx.create(gram.ppr("Clause"), tkns.range_str(13, 3), tkns[13].pos()))
        .enter(ctx.create(gram.ppr("Clause_ParserSection"), tkns.range_str(13, 3), tkns[13].pos()))
        .attach(ctx.create(gram.lpr("KW_PARSER"), "parser", tkns[13].pos()))
        .attach(ctx.create(gram.lpr("KW_SECTION"), "section", tkns[14].pos()))
        .attach(ctx.create(gram.lpr("OP_COLON"), ":", tkns[15].pos()))
        .exit()
        .exit()

        .enter(ctx.create(gram.ppr("Clause"), tkns.range_str(16, 4), tkns[16].pos()))
        .enter(ctx.create(gram.ppr("Clause_Rule"), tkns.range_str(16, 4), tkns[16].pos()))
        .enter(ctx.create(gram.ppr("Clause_Rule_Name"), "Abc", tkns[16].pos()))
        .attach(ctx.create(gram.lpr("IDENTIFIER"), "Abc", tkns[16].pos()))
        .exit()
        .attach(ctx.create(gram.lpr("OP_COLON"), ":", tkns[17].pos()))
        .enter(ctx.create(gram.ppr("Clause_Rule_Expr"), "any", tkns[18].pos()))
        .enter(ctx.create(gram.ppr("Expr"), "any", tkns[18].pos()))
        .enter(ctx.create(gram.ppr("Expr_Primary"), "any", tkns[18].pos()))
        .enter(ctx.create(gram.ppr("Expr_Any"), "any", tkns[18].pos()))
        .attach(ctx.create(gram.lpr("KW_ANY"), "any", tkns[18].pos()))
        .exit()
        .exit()
        .exit()
        .exit()
        .attach(ctx.create(gram.lpr("OP_SEMICOLON"), ";", tkns[19].pos()))
        .exit()
        .exit()

        .end();

    TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());


    auto actual = ctx.parse("Spec", tkns);
    
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


    taul::context ctx(gram, lgr);


    std::string src =
        "lexer section:\n"
        "skip ABC : + ;\n" // <- the '+' is a syntax error
        "parser section:\n"
        "Abc : any ;";

    auto tkns = ctx.tokenize(src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 15);


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


    auto actual = ctx.parse("Spec", tkns);
    
    TAUL_LOG(lgr, "actual.has_value() == {}", actual.has_value());
    
    ASSERT_TRUE(actual.has_value());

    TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());


    EXPECT_TRUE(expected.equiv(*actual));
}

TEST_F(LoadTAULGrammarTests, Syntax_4) {


    // this tests the syntax of primary exprs


    EXPECT_TRUE(gram.contains_lpr("KW_END"));
    EXPECT_TRUE(gram.contains_lpr("KW_ANY"));
    EXPECT_TRUE(gram.contains_lpr("KW_TOKEN"));
    EXPECT_TRUE(gram.contains_lpr("KW_FAILURE"));
    EXPECT_TRUE(gram.contains_lpr("IDENTIFIER"));
    EXPECT_TRUE(gram.contains_lpr("STRING"));
    EXPECT_TRUE(gram.contains_lpr("CHARSET"));

    ASSERT_TRUE(gram.contains_ppr("Expr_Primary"));
    ASSERT_TRUE(gram.contains_ppr("Expr_End"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Any"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Token"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Failure"));
    ASSERT_TRUE(gram.contains_ppr("Expr_String"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Charset"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Name"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Sequence"));
    ASSERT_TRUE(gram.contains_ppr("Expr_Sequence_Expr"));


    taul::context ctx(gram, lgr);


    // this is NOT semantically valid TAUL, but it is syntactic

    static_assert(primary_exprs == 8);

    std::string src =
        "end any token failure "
        "'abc'"
        "[abc]"
        "Abc";

    auto tkns = ctx.tokenize(src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 7);


    taul::node_assembler na{};

    na
        .begin(ctx.create(gram.ppr("Expr_Sequence"), tkns.range_str(0, 7), tkns[0].pos()));

    std::size_t offset = 0;

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


    auto actual = ctx.parse("Expr_Sequence", tkns);
    
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


    taul::context ctx(gram, lgr);


    std::string src =
        "( any )";

    auto tkns = ctx.tokenize(src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 3);


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


    auto actual = ctx.parse("Expr", tkns);

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


    taul::context ctx(gram, lgr);


    std::string src =
        "&any";

    auto tkns = ctx.tokenize(src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 2);


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


    auto actual = ctx.parse("Expr", tkns);

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


    taul::context ctx(gram, lgr);


    std::string src =
        "-any";

    auto tkns = ctx.tokenize(src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 2);


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


    auto actual = ctx.parse("Expr", tkns);

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


    taul::context ctx(gram, lgr);


    std::string src =
        "~any";

    auto tkns = ctx.tokenize(src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 2);


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


    auto actual = ctx.parse("Expr", tkns);

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


    taul::context ctx(gram, lgr);


    std::string src =
        "any?";

    auto tkns = ctx.tokenize(src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 2);


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


    auto actual = ctx.parse("Expr", tkns);

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


    taul::context ctx(gram, lgr);


    std::string src =
        "any*";

    auto tkns = ctx.tokenize(src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 2);


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


    auto actual = ctx.parse("Expr", tkns);

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


    taul::context ctx(gram, lgr);


    std::string src =
        "any+";

    auto tkns = ctx.tokenize(src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 2);


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


    auto actual = ctx.parse("Expr", tkns);

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


    taul::context ctx(gram, lgr);


    std::string src =
        "any any any";

    auto tkns = ctx.tokenize(src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 3);


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


    auto actual = ctx.parse("Expr", tkns);

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


    taul::context ctx(gram, lgr);


    std::string src =
        "any | any | any";

    auto tkns = ctx.tokenize(src);

    TAUL_LOG(lgr, "{}", tkns);

    ASSERT_EQ(tkns.size(), 5);


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


    auto actual = ctx.parse("Expr", tkns);

    TAUL_LOG(lgr, "actual.has_value() == {}", actual.has_value());

    ASSERT_TRUE(actual.has_value());

    TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());


    EXPECT_TRUE(expected.equiv(*actual));
}

