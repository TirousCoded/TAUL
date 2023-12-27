

#include <gtest/gtest.h>

#include <taul/all.h>


// these unit tests assert that taul::load_taul_grammar produces a grammar with
// the power to describe TAUL syntax properly

// of course, we can't test literally every possible input for each pattern, so
// a broad *general* suite of tests will be performed, w/ these being expected
// to be *close enough* to work as proper regression tests


class LoadTaulGrammarTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr = nullptr;
    taul::grammar gram;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
        gram = taul::load_taul_grammar(lgr);
    }
};


constexpr std::size_t nonsupport_lprs = 42;
constexpr std::size_t keywords = 18;
constexpr std::size_t operators = 16;


TEST_F(LoadTaulGrammarTests, HasExpectedGrammarWideBias) {

    EXPECT_EQ(gram.bias(), taul::bias::fl);
}

TEST_F(LoadTaulGrammarTests, HasExpectedLPRs) {

    EXPECT_EQ(gram.nonsupport_lprs(), nonsupport_lprs);

    //EXPECT_TRUE(gram.contains_lpr("VIS_ASCII"));
    //EXPECT_TRUE(gram.contains_lpr("LETTER"));
    //EXPECT_TRUE(gram.contains_lpr("DIGIT"));
    //EXPECT_TRUE(gram.contains_lpr("ANY_IDENTIFIER"));

    EXPECT_TRUE(gram.contains_lpr("IDENTIFIER"));
    EXPECT_TRUE(gram.contains_lpr("INTEGER"));
    EXPECT_TRUE(gram.contains_lpr("STRING"));
    EXPECT_TRUE(gram.contains_lpr("CHARSET"));

    EXPECT_TRUE(gram.contains_lpr("WHITESPACE"));
    EXPECT_TRUE(gram.contains_lpr("NEWLINE"));
    EXPECT_TRUE(gram.contains_lpr("SL_COMMENT"));
    EXPECT_TRUE(gram.contains_lpr("ML_COMMENT"));

    //EXPECT_TRUE(gram.contains_lpr("KEYWORD"));
    //EXPECT_TRUE(gram.contains_lpr("EOK"));

    static_assert(keywords == 18);

    EXPECT_TRUE(gram.contains_lpr("KW_OPTIONS"));
    EXPECT_TRUE(gram.contains_lpr("KW_LEXER"));
    EXPECT_TRUE(gram.contains_lpr("KW_PARSER"));
    EXPECT_TRUE(gram.contains_lpr("KW_BIAS"));
    EXPECT_TRUE(gram.contains_lpr("KW_SKIP"));
    EXPECT_TRUE(gram.contains_lpr("KW_SUPPORT"));
    EXPECT_TRUE(gram.contains_lpr("KW_FL"));
    EXPECT_TRUE(gram.contains_lpr("KW_FS"));
    EXPECT_TRUE(gram.contains_lpr("KW_LL"));
    EXPECT_TRUE(gram.contains_lpr("KW_LS"));
    EXPECT_TRUE(gram.contains_lpr("KW_F"));
    EXPECT_TRUE(gram.contains_lpr("KW_L"));
    EXPECT_TRUE(gram.contains_lpr("KW_BEGIN"));
    EXPECT_TRUE(gram.contains_lpr("KW_END"));
    EXPECT_TRUE(gram.contains_lpr("KW_ANY"));
    EXPECT_TRUE(gram.contains_lpr("KW_TOKEN"));
    EXPECT_TRUE(gram.contains_lpr("KW_FAILURE"));
    EXPECT_TRUE(gram.contains_lpr("KW_LOCALEND"));

    //EXPECT_TRUE(gram.contains_lpr("OPERATORS"));

    static_assert(operators == 16);

    EXPECT_TRUE(gram.contains_lpr("OP_PERIOD"));
    EXPECT_TRUE(gram.contains_lpr("OP_COMMA"));
    EXPECT_TRUE(gram.contains_lpr("OP_COLON"));
    EXPECT_TRUE(gram.contains_lpr("OP_SEMICOLON"));
    EXPECT_TRUE(gram.contains_lpr("OP_VBAR"));
    EXPECT_TRUE(gram.contains_lpr("OP_QUESTION"));
    EXPECT_TRUE(gram.contains_lpr("OP_ASTERISK"));
    EXPECT_TRUE(gram.contains_lpr("OP_PLUS"));
    EXPECT_TRUE(gram.contains_lpr("OP_EXCLAM"));
    EXPECT_TRUE(gram.contains_lpr("OP_TILDE"));
    EXPECT_TRUE(gram.contains_lpr("OP_AMPERSAND"));
    EXPECT_TRUE(gram.contains_lpr("OP_MINUS"));
    EXPECT_TRUE(gram.contains_lpr("OP_L_ROUND"));
    EXPECT_TRUE(gram.contains_lpr("OP_R_ROUND"));
    EXPECT_TRUE(gram.contains_lpr("OP_L_CURLY"));
    EXPECT_TRUE(gram.contains_lpr("OP_R_CURLY"));
}

TEST_F(LoadTaulGrammarTests, HasExpectedPPRs) {

    //
}

/*TEST_F(LoadTaulGrammarTests, VIS_ASCII) {

    ASSERT_TRUE(gram.contains_lpr("VIS_ASCII"));

    EXPECT_EQ(gram.lpr("VIS_ASCII").qualifer, taul::qualifier::support);
    
    auto lxr = gram.lexer("VIS_ASCII");

    for (std::uint8_t i = 0x00; i < 0x80; i++) {
        // skip stray nulls for now
        if (i == 0x00) {
            continue;
        }
        std::string s = " a";
        s[0] = (char)i;
        taul::token tkn = lxr(s);
        if ((char(i) >= '!') && (char(i) <= '~')) {
            EXPECT_EQ(tkn, taul::token(gram.lpr("VIS_ASCII"), s.substr(0, 1))) << "std::size_t(s[0])==" << size_t(i);
        }
        else {
            EXPECT_EQ(tkn, taul::token::failure("")) << "std::size_t(s[0])==" << size_t(i);
        }
    }

    EXPECT_EQ(lxr(""), taul::token::failure(""));
}*/

/*TEST_F(LoadTaulGrammarTests, LETTER) {

    ASSERT_TRUE(gram.contains_lpr("LETTER"));

    EXPECT_EQ(gram.lpr("LETTER").qualifer, taul::qualifier::support);
    
    auto lxr = gram.lexer("LETTER");

    for (std::uint8_t i = 0x00; i < 0x80; i++) {
        // skip stray nulls for now
        if (i == 0x00) {
            continue;
        }
        std::string s = " a";
        s[0] = (char)i;
        taul::token tkn = lxr(s);
        if (((char(i) >= 'a') && (char(i) <= 'z')) || ((char(i) >= 'A') && (char(i) <= 'Z'))) {
            EXPECT_EQ(tkn, taul::token(gram.lpr("LETTER"), s.substr(0, 1))) << "std::size_t(s[0])==" << size_t(i);
        }
        else {
            EXPECT_EQ(tkn, taul::token::failure("")) << "std::size_t(s[0])==" << size_t(i);
        }
    }

    EXPECT_EQ(lxr(""), taul::token::failure(""));
}*/

/*TEST_F(LoadTaulGrammarTests, DIGIT) {

    ASSERT_TRUE(gram.contains_lpr("DIGIT"));

    EXPECT_EQ(gram.lpr("DIGIT").qualifer, taul::qualifier::support);
    
    auto lxr = gram.lexer("DIGIT");

    for (std::uint8_t i = 0x00; i < 0x80; i++) {
        // skip stray nulls for now
        if (i == 0x00) {
            continue;
        }
        std::string s = " a";
        s[0] = (char)i;
        taul::token tkn = lxr(s);
        if ((char(i) >= '0') && (char(i) <= '9')) {
            EXPECT_EQ(tkn, taul::token(gram.lpr("DIGIT"), s.substr(0, 1))) << "std::size_t(s[0])==" << size_t(i);
        }
        else {
            EXPECT_EQ(tkn, taul::token::failure("")) << "std::size_t(s[0])==" << size_t(i);
        }
    }

    EXPECT_EQ(lxr(""), taul::token::failure(""));
}*/

/*TEST_F(LoadTaulGrammarTests, ANY_IDENTIFIER) {

    ASSERT_TRUE(gram.contains_lpr("ANY_IDENTIFIER"));

    EXPECT_EQ(gram.lpr("ANY_IDENTIFIER").qualifer, taul::qualifier::support);

    auto lxr = gram.lexer("ANY_IDENTIFIER");

    EXPECT_EQ(lxr("abc(abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "abc"));
    EXPECT_EQ(lxr("_a_b_c&abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "_a_b_c"));
    EXPECT_EQ(lxr("_123*abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "_123"));
    EXPECT_EQ(lxr("_&abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "_"));
    EXPECT_EQ(lxr("A&abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "A"));

    static_assert(keywords == 18);

    EXPECT_EQ(lxr("options)abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "options"));
    EXPECT_EQ(lxr("lexer!abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "lexer"));
    EXPECT_EQ(lxr("parser(abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "parser"));
    EXPECT_EQ(lxr("bias&abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "bias"));
    EXPECT_EQ(lxr("skip+abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "skip"));
    EXPECT_EQ(lxr("support-abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "support"));
    EXPECT_EQ(lxr("fl abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "fl"));
    EXPECT_EQ(lxr("fs\tabc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "fs"));
    EXPECT_EQ(lxr("ll*abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "ll"));
    EXPECT_EQ(lxr("ls?abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "ls"));
    EXPECT_EQ(lxr("f:abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "f"));
    EXPECT_EQ(lxr("l;abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "l"));
    EXPECT_EQ(lxr("begin)abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "begin"));
    EXPECT_EQ(lxr("end(abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "end"));
    EXPECT_EQ(lxr("any&abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "any"));
    EXPECT_EQ(lxr("token&abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "token"));
    EXPECT_EQ(lxr("failure~abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "failure"));
    EXPECT_EQ(lxr("localend*abc"), taul::token(gram.lpr("ANY_IDENTIFIER"), "localend"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
}*/

TEST_F(LoadTaulGrammarTests, IDENTIFIER) {

    ASSERT_TRUE(gram.contains_lpr("IDENTIFIER"));

    EXPECT_EQ(gram.lpr("IDENTIFIER").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("IDENTIFIER");

    EXPECT_EQ(lxr("abc(abc"), taul::token(gram.lpr("IDENTIFIER"), "abc"));
    EXPECT_EQ(lxr("_a_b_c&abc"), taul::token(gram.lpr("IDENTIFIER"), "_a_b_c"));
    EXPECT_EQ(lxr("_123*abc"), taul::token(gram.lpr("IDENTIFIER"), "_123"));
    EXPECT_EQ(lxr("_&abc"), taul::token(gram.lpr("IDENTIFIER"), "_"));
    EXPECT_EQ(lxr("A&abc"), taul::token(gram.lpr("IDENTIFIER"), "A"));

    static_assert(keywords == 18);

    EXPECT_EQ(lxr("optionsA"), taul::token(gram.lpr("IDENTIFIER"), "optionsA"));
    EXPECT_EQ(lxr("lexerA"), taul::token(gram.lpr("IDENTIFIER"), "lexerA"));
    EXPECT_EQ(lxr("parserA"), taul::token(gram.lpr("IDENTIFIER"), "parserA"));
    EXPECT_EQ(lxr("biasA"), taul::token(gram.lpr("IDENTIFIER"), "biasA"));
    EXPECT_EQ(lxr("skipA"), taul::token(gram.lpr("IDENTIFIER"), "skipA"));
    EXPECT_EQ(lxr("supportA"), taul::token(gram.lpr("IDENTIFIER"), "supportA"));
    EXPECT_EQ(lxr("flA"), taul::token(gram.lpr("IDENTIFIER"), "flA"));
    EXPECT_EQ(lxr("fsA"), taul::token(gram.lpr("IDENTIFIER"), "fsA"));
    EXPECT_EQ(lxr("llA"), taul::token(gram.lpr("IDENTIFIER"), "llA"));
    EXPECT_EQ(lxr("lsA"), taul::token(gram.lpr("IDENTIFIER"), "lsA"));
    EXPECT_EQ(lxr("fA"), taul::token(gram.lpr("IDENTIFIER"), "fA"));
    EXPECT_EQ(lxr("lA"), taul::token(gram.lpr("IDENTIFIER"), "lA"));
    EXPECT_EQ(lxr("beginA"), taul::token(gram.lpr("IDENTIFIER"), "beginA"));
    EXPECT_EQ(lxr("endA"), taul::token(gram.lpr("IDENTIFIER"), "endA"));
    EXPECT_EQ(lxr("anyA"), taul::token(gram.lpr("IDENTIFIER"), "anyA"));
    EXPECT_EQ(lxr("tokenA"), taul::token(gram.lpr("IDENTIFIER"), "tokenA"));
    EXPECT_EQ(lxr("failureA"), taul::token(gram.lpr("IDENTIFIER"), "failureA"));
    EXPECT_EQ(lxr("localendA"), taul::token(gram.lpr("IDENTIFIER"), "localendA"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));

    static_assert(keywords == 18);

    EXPECT_EQ(lxr("options"), taul::token::failure(""));
    EXPECT_EQ(lxr("lexer"), taul::token::failure(""));
    EXPECT_EQ(lxr("parser"), taul::token::failure(""));
    EXPECT_EQ(lxr("bias"), taul::token::failure(""));
    EXPECT_EQ(lxr("skip"), taul::token::failure(""));
    EXPECT_EQ(lxr("support"), taul::token::failure(""));
    EXPECT_EQ(lxr("fl"), taul::token::failure(""));
    EXPECT_EQ(lxr("fs"), taul::token::failure(""));
    EXPECT_EQ(lxr("ll"), taul::token::failure(""));
    EXPECT_EQ(lxr("ls"), taul::token::failure(""));
    EXPECT_EQ(lxr("f"), taul::token::failure(""));
    EXPECT_EQ(lxr("l"), taul::token::failure(""));
    EXPECT_EQ(lxr("begin"), taul::token::failure(""));
    EXPECT_EQ(lxr("end"), taul::token::failure(""));
    EXPECT_EQ(lxr("any"), taul::token::failure(""));
    EXPECT_EQ(lxr("token"), taul::token::failure(""));
    EXPECT_EQ(lxr("failure"), taul::token::failure(""));
    EXPECT_EQ(lxr("localend"), taul::token::failure(""));
}

TEST_F(LoadTaulGrammarTests, INTEGER) {

    ASSERT_TRUE(gram.contains_lpr("INTEGER"));

    EXPECT_EQ(gram.lpr("INTEGER").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("INTEGER");

    EXPECT_EQ(lxr("123(abc"), taul::token(gram.lpr("INTEGER"), "123"));
    EXPECT_EQ(lxr("1_2_3(abc"), taul::token(gram.lpr("INTEGER"), "1_2_3"));
    EXPECT_EQ(lxr("102_203_300_489_009+abc"), taul::token(gram.lpr("INTEGER"), "102_203_300_489_009"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));

    EXPECT_EQ(lxr("123a"), taul::token::failure(""));
    EXPECT_EQ(lxr("1_2_3A"), taul::token::failure(""));
    EXPECT_EQ(lxr("102_203_300_489_009abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123_"), taul::token::failure(""));
    EXPECT_EQ(lxr("1_2_3_"), taul::token::failure(""));
    EXPECT_EQ(lxr("102_203_300_489_009_"), taul::token::failure(""));

    EXPECT_EQ(lxr("1__23"), taul::token::failure(""));
    EXPECT_EQ(lxr("12__3"), taul::token::failure(""));
}

TEST_F(LoadTaulGrammarTests, STRING) {

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

TEST_F(LoadTaulGrammarTests, CHARSET) {

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

TEST_F(LoadTaulGrammarTests, WHITESPACE) {

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

TEST_F(LoadTaulGrammarTests, NEWLINE) {

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

TEST_F(LoadTaulGrammarTests, SL_COMMENT) {

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

TEST_F(LoadTaulGrammarTests, ML_COMMENT) {

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

TEST_F(LoadTaulGrammarTests, KW_OPTIONS) {

    ASSERT_TRUE(gram.contains_lpr("KW_OPTIONS"));

    EXPECT_EQ(gram.lpr("KW_OPTIONS").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_OPTIONS");

    EXPECT_EQ(lxr("options"), taul::token(gram.lpr("KW_OPTIONS"), "options"));
    EXPECT_EQ(lxr("options?"), taul::token(gram.lpr("KW_OPTIONS"), "options"));
    EXPECT_EQ(lxr("options "), taul::token(gram.lpr("KW_OPTIONS"), "options"));
    EXPECT_EQ(lxr("options\r\n"), taul::token(gram.lpr("KW_OPTIONS"), "options"));

    EXPECT_EQ(lxr("optionsA"), taul::token::failure(""));
    EXPECT_EQ(lxr("options1"), taul::token::failure(""));
    EXPECT_EQ(lxr("options_"), taul::token::failure(""));

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

TEST_F(LoadTaulGrammarTests, KW_LEXER) {

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

TEST_F(LoadTaulGrammarTests, KW_PARSER) {

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

TEST_F(LoadTaulGrammarTests, KW_BIAS) {

    ASSERT_TRUE(gram.contains_lpr("KW_BIAS"));

    EXPECT_EQ(gram.lpr("KW_BIAS").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_BIAS");

    EXPECT_EQ(lxr("bias"), taul::token(gram.lpr("KW_BIAS"), "bias"));
    EXPECT_EQ(lxr("bias?"), taul::token(gram.lpr("KW_BIAS"), "bias"));
    EXPECT_EQ(lxr("bias "), taul::token(gram.lpr("KW_BIAS"), "bias"));
    EXPECT_EQ(lxr("bias\r\n"), taul::token(gram.lpr("KW_BIAS"), "bias"));

    EXPECT_EQ(lxr("biasA"), taul::token::failure(""));
    EXPECT_EQ(lxr("bias1"), taul::token::failure(""));
    EXPECT_EQ(lxr("bias_"), taul::token::failure(""));

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

TEST_F(LoadTaulGrammarTests, KW_SKIP) {

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

TEST_F(LoadTaulGrammarTests, KW_SUPPORT) {

    ASSERT_TRUE(gram.contains_lpr("KW_SUPPORT"));

    EXPECT_EQ(gram.lpr("KW_SUPPORT").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_SUPPORT");

    EXPECT_EQ(lxr("support"), taul::token(gram.lpr("KW_SUPPORT"), "support"));
    EXPECT_EQ(lxr("support?"), taul::token(gram.lpr("KW_SUPPORT"), "support"));
    EXPECT_EQ(lxr("support "), taul::token(gram.lpr("KW_SUPPORT"), "support"));
    EXPECT_EQ(lxr("support\r\n"), taul::token(gram.lpr("KW_SUPPORT"), "support"));

    EXPECT_EQ(lxr("supportA"), taul::token::failure(""));
    EXPECT_EQ(lxr("support1"), taul::token::failure(""));
    EXPECT_EQ(lxr("support_"), taul::token::failure(""));

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

TEST_F(LoadTaulGrammarTests, KW_FL) {

    ASSERT_TRUE(gram.contains_lpr("KW_FL"));

    EXPECT_EQ(gram.lpr("KW_FL").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_FL");

    EXPECT_EQ(lxr("fl"), taul::token(gram.lpr("KW_FL"), "fl"));
    EXPECT_EQ(lxr("fl?"), taul::token(gram.lpr("KW_FL"), "fl"));
    EXPECT_EQ(lxr("fl "), taul::token(gram.lpr("KW_FL"), "fl"));
    EXPECT_EQ(lxr("fl\r\n"), taul::token(gram.lpr("KW_FL"), "fl"));

    EXPECT_EQ(lxr("flA"), taul::token::failure(""));
    EXPECT_EQ(lxr("fl1"), taul::token::failure(""));
    EXPECT_EQ(lxr("fl_"), taul::token::failure(""));

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

TEST_F(LoadTaulGrammarTests, KW_FS) {

    ASSERT_TRUE(gram.contains_lpr("KW_FS"));

    EXPECT_EQ(gram.lpr("KW_FS").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_FS");

    EXPECT_EQ(lxr("fs"), taul::token(gram.lpr("KW_FS"), "fs"));
    EXPECT_EQ(lxr("fs?"), taul::token(gram.lpr("KW_FS"), "fs"));
    EXPECT_EQ(lxr("fs "), taul::token(gram.lpr("KW_FS"), "fs"));
    EXPECT_EQ(lxr("fs\r\n"), taul::token(gram.lpr("KW_FS"), "fs"));

    EXPECT_EQ(lxr("fsA"), taul::token::failure(""));
    EXPECT_EQ(lxr("fs1"), taul::token::failure(""));
    EXPECT_EQ(lxr("fs_"), taul::token::failure(""));

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

TEST_F(LoadTaulGrammarTests, KW_LL) {

    ASSERT_TRUE(gram.contains_lpr("KW_LL"));

    EXPECT_EQ(gram.lpr("KW_LL").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_LL");

    EXPECT_EQ(lxr("ll"), taul::token(gram.lpr("KW_LL"), "ll"));
    EXPECT_EQ(lxr("ll?"), taul::token(gram.lpr("KW_LL"), "ll"));
    EXPECT_EQ(lxr("ll "), taul::token(gram.lpr("KW_LL"), "ll"));
    EXPECT_EQ(lxr("ll\r\n"), taul::token(gram.lpr("KW_LL"), "ll"));

    EXPECT_EQ(lxr("llA"), taul::token::failure(""));
    EXPECT_EQ(lxr("ll1"), taul::token::failure(""));
    EXPECT_EQ(lxr("ll_"), taul::token::failure(""));

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

TEST_F(LoadTaulGrammarTests, KW_LS) {

    ASSERT_TRUE(gram.contains_lpr("KW_LS"));

    EXPECT_EQ(gram.lpr("KW_LS").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_LS");

    EXPECT_EQ(lxr("ls"), taul::token(gram.lpr("KW_LS"), "ls"));
    EXPECT_EQ(lxr("ls?"), taul::token(gram.lpr("KW_LS"), "ls"));
    EXPECT_EQ(lxr("ls "), taul::token(gram.lpr("KW_LS"), "ls"));
    EXPECT_EQ(lxr("ls\r\n"), taul::token(gram.lpr("KW_LS"), "ls"));

    EXPECT_EQ(lxr("lsA"), taul::token::failure(""));
    EXPECT_EQ(lxr("ls1"), taul::token::failure(""));
    EXPECT_EQ(lxr("ls_"), taul::token::failure(""));

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

TEST_F(LoadTaulGrammarTests, KW_F) {

    ASSERT_TRUE(gram.contains_lpr("KW_F"));

    EXPECT_EQ(gram.lpr("KW_F").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_F");

    EXPECT_EQ(lxr("f"), taul::token(gram.lpr("KW_F"), "f"));
    EXPECT_EQ(lxr("f?"), taul::token(gram.lpr("KW_F"), "f"));
    EXPECT_EQ(lxr("f "), taul::token(gram.lpr("KW_F"), "f"));
    EXPECT_EQ(lxr("f\r\n"), taul::token(gram.lpr("KW_F"), "f"));

    EXPECT_EQ(lxr("fA"), taul::token::failure(""));
    EXPECT_EQ(lxr("f1"), taul::token::failure(""));
    EXPECT_EQ(lxr("f_"), taul::token::failure(""));

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

TEST_F(LoadTaulGrammarTests, KW_L) {

    ASSERT_TRUE(gram.contains_lpr("KW_L"));

    EXPECT_EQ(gram.lpr("KW_L").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_L");

    EXPECT_EQ(lxr("l"), taul::token(gram.lpr("KW_L"), "l"));
    EXPECT_EQ(lxr("l?"), taul::token(gram.lpr("KW_L"), "l"));
    EXPECT_EQ(lxr("l "), taul::token(gram.lpr("KW_L"), "l"));
    EXPECT_EQ(lxr("l\r\n"), taul::token(gram.lpr("KW_L"), "l"));

    EXPECT_EQ(lxr("lA"), taul::token::failure(""));
    EXPECT_EQ(lxr("l1"), taul::token::failure(""));
    EXPECT_EQ(lxr("l_"), taul::token::failure(""));

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

TEST_F(LoadTaulGrammarTests, KW_BEGIN) {

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

TEST_F(LoadTaulGrammarTests, KW_END) {

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

TEST_F(LoadTaulGrammarTests, KW_ANY) {

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

TEST_F(LoadTaulGrammarTests, KW_TOKEN) {

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

TEST_F(LoadTaulGrammarTests, KW_FAILURE) {

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

TEST_F(LoadTaulGrammarTests, KW_LOCALEND) {

    ASSERT_TRUE(gram.contains_lpr("KW_LOCALEND"));

    EXPECT_EQ(gram.lpr("KW_LOCALEND").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("KW_LOCALEND");

    EXPECT_EQ(lxr("localend"), taul::token(gram.lpr("KW_LOCALEND"), "localend"));
    EXPECT_EQ(lxr("localend?"), taul::token(gram.lpr("KW_LOCALEND"), "localend"));
    EXPECT_EQ(lxr("localend "), taul::token(gram.lpr("KW_LOCALEND"), "localend"));
    EXPECT_EQ(lxr("localend\r\n"), taul::token(gram.lpr("KW_LOCALEND"), "localend"));

    EXPECT_EQ(lxr("localendA"), taul::token::failure(""));
    EXPECT_EQ(lxr("localend1"), taul::token::failure(""));
    EXPECT_EQ(lxr("localend_"), taul::token::failure(""));

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

TEST_F(LoadTaulGrammarTests, OP_PERIOD) {

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

TEST_F(LoadTaulGrammarTests, OP_COMMA) {

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

TEST_F(LoadTaulGrammarTests, OP_COLON) {

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

TEST_F(LoadTaulGrammarTests, OP_SEMICOLON) {

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

TEST_F(LoadTaulGrammarTests, OP_VBAR) {

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

TEST_F(LoadTaulGrammarTests, OP_QUESTION) {

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

TEST_F(LoadTaulGrammarTests, OP_ASTERISK) {

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

TEST_F(LoadTaulGrammarTests, OP_PLUS) {

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

TEST_F(LoadTaulGrammarTests, OP_EXCLAM) {

    ASSERT_TRUE(gram.contains_lpr("OP_EXCLAM"));

    EXPECT_EQ(gram.lpr("OP_EXCLAM").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("OP_EXCLAM");

    EXPECT_EQ(lxr("!abc"), taul::token(gram.lpr("OP_EXCLAM"), "!"));
    EXPECT_EQ(lxr("!123"), taul::token(gram.lpr("OP_EXCLAM"), "!"));
    EXPECT_EQ(lxr("!!@#"), taul::token(gram.lpr("OP_EXCLAM"), "!"));
    EXPECT_EQ(lxr("! "), taul::token(gram.lpr("OP_EXCLAM"), "!"));
    EXPECT_EQ(lxr("!\t"), taul::token(gram.lpr("OP_EXCLAM"), "!"));

    EXPECT_EQ(lxr(""), taul::token::failure(""));
    EXPECT_EQ(lxr("abc"), taul::token::failure(""));
    EXPECT_EQ(lxr("123"), taul::token::failure(""));
    //EXPECT_EQ(lxr("!@#"), taul::token::failure(""));
    EXPECT_EQ(lxr("_"), taul::token::failure(""));
    EXPECT_EQ(lxr(" "), taul::token::failure(""));
    EXPECT_EQ(lxr("\t"), taul::token::failure(""));
    EXPECT_EQ(lxr("~~~"), taul::token::failure(""));
    EXPECT_EQ(lxr("@#&"), taul::token::failure(""));
    EXPECT_EQ(lxr("''"), taul::token::failure(""));
    EXPECT_EQ(lxr("\"\""), taul::token::failure(""));
    EXPECT_EQ(lxr("#abc"), taul::token::failure(""));
}

TEST_F(LoadTaulGrammarTests, OP_TILDE) {

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

TEST_F(LoadTaulGrammarTests, OP_AMPERSAND) {

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

TEST_F(LoadTaulGrammarTests, OP_MINUS) {

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

TEST_F(LoadTaulGrammarTests, OP_L_ROUND) {

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

TEST_F(LoadTaulGrammarTests, OP_R_ROUND) {

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

TEST_F(LoadTaulGrammarTests, OP_L_CURLY) {

    ASSERT_TRUE(gram.contains_lpr("OP_L_CURLY"));

    EXPECT_EQ(gram.lpr("OP_L_CURLY").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("OP_L_CURLY");

    EXPECT_EQ(lxr("{abc"), taul::token(gram.lpr("OP_L_CURLY"), "{"));
    EXPECT_EQ(lxr("{123"), taul::token(gram.lpr("OP_L_CURLY"), "{"));
    EXPECT_EQ(lxr("{!@#"), taul::token(gram.lpr("OP_L_CURLY"), "{"));
    EXPECT_EQ(lxr("{ "), taul::token(gram.lpr("OP_L_CURLY"), "{"));
    EXPECT_EQ(lxr("{\t"), taul::token(gram.lpr("OP_L_CURLY"), "{"));

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

TEST_F(LoadTaulGrammarTests, OP_R_CURLY) {

    ASSERT_TRUE(gram.contains_lpr("OP_R_CURLY"));

    EXPECT_EQ(gram.lpr("OP_R_CURLY").qualifer, taul::qualifier::none);

    auto lxr = gram.lexer("OP_R_CURLY");

    EXPECT_EQ(lxr("}abc"), taul::token(gram.lpr("OP_R_CURLY"), "}"));
    EXPECT_EQ(lxr("}123"), taul::token(gram.lpr("OP_R_CURLY"), "}"));
    EXPECT_EQ(lxr("}!@#"), taul::token(gram.lpr("OP_R_CURLY"), "}"));
    EXPECT_EQ(lxr("} "), taul::token(gram.lpr("OP_R_CURLY"), "}"));
    EXPECT_EQ(lxr("}\t"), taul::token(gram.lpr("OP_R_CURLY"), "}"));

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

