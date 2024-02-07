

#include <gtest/gtest.h>

#include <taul/all.h>


// IMPORTANT: testing of methods 'match' and 'parse' will be performed
//            in grammar_usage_tests.cpp, rather than here

// IMPORTANT: the taul::context ctors will be presumed to work


class ContextTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr;
    taul::grammar gram;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
        auto spec =
            taul::spec_writer()
            .lpr_decl("abc")
            .lpr_decl("a")
            .lpr_decl("b")
            .lpr_decl("c")
            .lpr_decl("SPACE")
            .ppr_decl("Abc")
            .lpr("abc")
            .string("abc")
            .close()
            .lpr("a")
            .charset("aA")
            .close()
            .lpr("b")
            .charset("bB")
            .close()
            .lpr("c")
            .charset("cC")
            .close()
            .lpr("SPACE", taul::qualifier::skip)
            .string(" ")
            .close()
            .ppr("Abc")
            .name("a")
            .name("b")
            .name("c")
            .close()
            .done();
        auto loaded = taul::load(spec, lgr);
        TAUL_ASSERT(loaded);
        gram = std::move(*loaded);
    }
};


TEST_F(ContextTests, Exceptions) {
    taul::context ctx(lgr);
    
    EXPECT_THROW(ctx.match("b", ""), taul::no_grammar_bound_error);
    EXPECT_THROW(ctx.tokenize("b", ""), taul::no_grammar_bound_error);
    EXPECT_THROW(ctx.parse("Abc", taul::token_seq("")), taul::no_grammar_bound_error);
    EXPECT_THROW(ctx.parse("Abc", ""), taul::no_grammar_bound_error);

    ctx.bind(gram);
    
    EXPECT_THROW(ctx.match("h", ""), taul::lpr_not_found_error);
    EXPECT_THROW(ctx.tokenize("h", ""), taul::lpr_not_found_error);
    EXPECT_THROW(ctx.parse("Def", taul::token_seq("")), taul::ppr_not_found_error);
    EXPECT_THROW(ctx.parse("Def", ""), taul::ppr_not_found_error);
}

TEST_F(ContextTests, Tokenize_ForLPR_Basics) {
    taul::context ctx(lgr);
    ctx.bind(gram);

    auto result0 = ctx.tokenize("b", "bBbB");
    auto result1 = ctx.tokenize("b", "bBbB", 0);
    auto result2 = ctx.tokenize("b", "bBbB", 2);
    auto result3 = ctx.tokenize("b", "bBbB", 4);

    if (result0.size() == 4) {
        EXPECT_EQ(result0[0], taul::token(gram.lpr("b"), "b", 0));
        EXPECT_EQ(result0[1], taul::token(gram.lpr("b"), "B", 1));
        EXPECT_EQ(result0[2], taul::token(gram.lpr("b"), "b", 2));
        EXPECT_EQ(result0[3], taul::token(gram.lpr("b"), "B", 3));
    }
    else ADD_FAILURE();

    if (result1.size() == 4) {
        EXPECT_EQ(result1[0], taul::token(gram.lpr("b"), "b", 0));
        EXPECT_EQ(result1[1], taul::token(gram.lpr("b"), "B", 1));
        EXPECT_EQ(result1[2], taul::token(gram.lpr("b"), "b", 2));
        EXPECT_EQ(result1[3], taul::token(gram.lpr("b"), "B", 3));
    }
    else ADD_FAILURE();
    
    if (result2.size() == 2) {
        EXPECT_EQ(result2[0], taul::token(gram.lpr("b"), "b", 2));
        EXPECT_EQ(result2[1], taul::token(gram.lpr("b"), "B", 3));
    }
    else ADD_FAILURE();
    
    if (result3.size() == 0) {
        //
    }
    else ADD_FAILURE();
}

TEST_F(ContextTests, Tokenize_ForLPR_FailureTokens) {
    taul::context ctx(lgr);
    ctx.bind(gram);

    auto result0 = ctx.tokenize("b", "babc");
    auto result1 = ctx.tokenize("b", "babc", 2);
    auto result2 = ctx.tokenize("b", "bacb"); // test merging

    if (result0.size() == 4) {
        EXPECT_EQ(result0[0], taul::token(gram.lpr("b"), "b", 0));
        EXPECT_EQ(result0[1], taul::token::failure("a", 1));
        EXPECT_EQ(result0[2], taul::token(gram.lpr("b"), "b", 2));
        EXPECT_EQ(result0[3], taul::token::failure("c", 3));
    }
    else ADD_FAILURE();

    if (result1.size() == 2) {
        EXPECT_EQ(result1[0], taul::token(gram.lpr("b"), "b", 2));
        EXPECT_EQ(result1[1], taul::token::failure("c", 3));
    }
    else ADD_FAILURE();

    if (result2.size() == 3) {
        EXPECT_EQ(result2[0], taul::token(gram.lpr("b"), "b", 0));
        EXPECT_EQ(result2[1], taul::token::failure("ac", 1));
        EXPECT_EQ(result2[2], taul::token(gram.lpr("b"), "b", 3));
    }
    else ADD_FAILURE();
}

TEST_F(ContextTests, Tokenize_ForGrammar_Basics) {
    taul::context ctx(lgr);
    ctx.bind(gram);

    auto result0 = ctx.tokenize("aabc ab bc");
    auto result1 = ctx.tokenize("aabc ab bc", 0);
    auto result2 = ctx.tokenize("aabc ab bc", 6);
    auto result3 = ctx.tokenize("aabc ab bc", 0, false);
    auto result4 = ctx.tokenize("aabc ab bc", 6, false);

    if (result0.size() == 6) {
        EXPECT_EQ(result0[0], taul::token(gram.lpr("a"), "a", 0));
        EXPECT_EQ(result0[1], taul::token(gram.lpr("abc"), "abc", 1));
        EXPECT_EQ(result0[2], taul::token(gram.lpr("a"), "a", 5));
        EXPECT_EQ(result0[3], taul::token(gram.lpr("b"), "b", 6));
        EXPECT_EQ(result0[4], taul::token(gram.lpr("b"), "b", 8));
        EXPECT_EQ(result0[5], taul::token(gram.lpr("c"), "c", 9));
    }
    else ADD_FAILURE() << std::format("result0.size()==", result0.size());
    
    if (result1.size() == 6) {
        EXPECT_EQ(result1[0], taul::token(gram.lpr("a"), "a", 0));
        EXPECT_EQ(result1[1], taul::token(gram.lpr("abc"), "abc", 1));
        EXPECT_EQ(result1[2], taul::token(gram.lpr("a"), "a", 5));
        EXPECT_EQ(result1[3], taul::token(gram.lpr("b"), "b", 6));
        EXPECT_EQ(result1[4], taul::token(gram.lpr("b"), "b", 8));
        EXPECT_EQ(result1[5], taul::token(gram.lpr("c"), "c", 9));
    }
    else ADD_FAILURE() << std::format("result1.size()==", result1.size());

    if (result2.size() == 3) {
        EXPECT_EQ(result2[0], taul::token(gram.lpr("b"), "b", 6));
        EXPECT_EQ(result2[1], taul::token(gram.lpr("b"), "b", 8));
        EXPECT_EQ(result2[2], taul::token(gram.lpr("c"), "c", 9));
    }
    else ADD_FAILURE() << std::format("result2.size()==", result2.size());
    
    if (result3.size() == 8) {
        EXPECT_EQ(result3[0], taul::token(gram.lpr("a"), "a", 0));
        EXPECT_EQ(result3[1], taul::token(gram.lpr("abc"), "abc", 1));
        EXPECT_EQ(result3[2], taul::token(gram.lpr("SPACE"), " ", 4));
        EXPECT_EQ(result3[3], taul::token(gram.lpr("a"), "a", 5));
        EXPECT_EQ(result3[4], taul::token(gram.lpr("b"), "b", 6));
        EXPECT_EQ(result3[5], taul::token(gram.lpr("SPACE"), " ", 7));
        EXPECT_EQ(result3[6], taul::token(gram.lpr("b"), "b", 8));
        EXPECT_EQ(result3[7], taul::token(gram.lpr("c"), "c", 9));
    }
    else ADD_FAILURE() << std::format("result3.size()==", result3.size());

    if (result4.size() == 4) {
        EXPECT_EQ(result4[0], taul::token(gram.lpr("b"), "b", 6));
        EXPECT_EQ(result4[1], taul::token(gram.lpr("SPACE"), " ", 7));
        EXPECT_EQ(result4[2], taul::token(gram.lpr("b"), "b", 8));
        EXPECT_EQ(result4[3], taul::token(gram.lpr("c"), "c", 9));
    }
    else ADD_FAILURE() << std::format("result4.size()==", result4.size());
}

TEST_F(ContextTests, Tokenize_ForGrammar_FailureTokens) {
    taul::context ctx(lgr);
    ctx.bind(gram);

    auto result0 = ctx.tokenize("abcddbdc"); // test merging
    auto result1 = ctx.tokenize("abcddbdc", 4); // test merging

    if (result0.size() == 5) {
        EXPECT_EQ(result0[0], taul::token(gram.lpr("abc"), "abc", 0));
        EXPECT_EQ(result0[1], taul::token::failure("dd", 3));
        EXPECT_EQ(result0[2], taul::token(gram.lpr("b"), "b", 5));
        EXPECT_EQ(result0[3], taul::token::failure("d", 6));
        EXPECT_EQ(result0[4], taul::token(gram.lpr("c"), "c", 7));
    }
    else ADD_FAILURE();

    if (result1.size() == 4) {
        EXPECT_EQ(result1[0], taul::token::failure("d", 4));
        EXPECT_EQ(result1[1], taul::token(gram.lpr("b"), "b", 5));
        EXPECT_EQ(result1[2], taul::token::failure("d", 6));
        EXPECT_EQ(result1[3], taul::token(gram.lpr("c"), "c", 7));
    }
    else ADD_FAILURE();
}

TEST_F(ContextTests, Parse_WithStringInput) {
    taul::context ctx(lgr);
    ctx.bind(gram);

    auto expected0 =
        taul::node_assembler(lgr)
        .begin(ctx.create(gram.ppr("Abc"), "a B c", 0))
        .attach(ctx.create(taul::token(gram.lpr("a"), "a", 0)))
        .attach(ctx.create(taul::token(gram.lpr("b"), "B", 2)))
        .attach(ctx.create(taul::token(gram.lpr("c"), "c", 4)))
        .end();
    auto expected1 =
        taul::node_assembler(lgr)
        .begin(ctx.create(gram.ppr("Abc"), "A b C", 4))
        .attach(ctx.create(taul::token(gram.lpr("a"), "A", 4)))
        .attach(ctx.create(taul::token(gram.lpr("b"), "b", 6)))
        .attach(ctx.create(taul::token(gram.lpr("c"), "C", 8)))
        .end();

    auto result0 = ctx.parse("Abc", "a B c");
    auto result1 = ctx.parse("Abc", "a b A b C", 4);

    if (result0) {
        EXPECT_TRUE(expected0.equiv(*result0));
    }
    else ADD_FAILURE();

    if (result1) {
        EXPECT_TRUE(expected1.equiv(*result1));
    }
    else ADD_FAILURE();
}

