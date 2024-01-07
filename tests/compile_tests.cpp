

#include <gtest/gtest.h>

#include <taul/all.h>


class CompileTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr = nullptr;

    taul::node_ctx ctx;
    taul::spec_error_counter ec;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
    }
};


TEST_F(CompileTests, FailWithSourceCodeNotFoundSpecError_DueToSrcSharedPtrBeingNullptr) {

    std::shared_ptr<taul::source_code> src = nullptr;

    auto result = taul::compile(ctx, src, ec, lgr);


    EXPECT_FALSE(result);

    EXPECT_EQ(ec.count(taul::spec_error::source_code_not_found), 1);
}

TEST_F(CompileTests, FailWithSourceCodeNotFoundSpecError_DueToSrcFileNotFound) {

    std::filesystem::path src_path = std::filesystem::current_path() / "does-not-exist.taul";

    ASSERT_FALSE(std::filesystem::exists(src_path)) << "src_path file incorrectly exists!";

    auto result = taul::compile(ctx, src_path, ec, lgr);


    EXPECT_FALSE(result);

    EXPECT_EQ(ec.count(taul::spec_error::source_code_not_found), 1);
}

// this tests the basics of top-level TAUL clause syntax compiling correctly, and producing
// a grammar w/ the expected LPRs/PPRs

// this test does not test the usage of LPRs/PPRs, only that the top-level stuff compiles

TEST_F(CompileTests, TopLevel_Compile_Success) {

    std::filesystem::path src_path = std::filesystem::current_path() / "support\\compile_test_spec_1.taul";

    ASSERT_TRUE(std::filesystem::exists(src_path)) << "file compile_test_spec_1.taul not found!";

    auto src = std::make_shared<taul::source_code>();
    src->add_file(src_path, lgr);
    

    auto result = taul::compile(ctx, src, ec, lgr);

    ASSERT_TRUE((bool)result);

    EXPECT_TRUE(result->associated(src));


    auto gram = taul::load(*result, lgr);

    ASSERT_TRUE((bool)gram);

    TAUL_LOG(lgr, "{}", *gram);

    EXPECT_EQ(gram->bias(), taul::bias::f);

    EXPECT_EQ(gram->lprs().size(), 2);
    EXPECT_EQ(gram->pprs().size(), 2);

    EXPECT_TRUE(gram->contains_lpr("ABC"));
    EXPECT_TRUE(gram->contains_lpr("DEF"));

    EXPECT_TRUE(gram->contains_ppr("Abc"));
    EXPECT_TRUE(gram->contains_ppr("Def"));

    if (gram->contains_lpr("ABC")) {
        EXPECT_EQ(gram->lpr("ABC").qualifer, taul::qualifier::skip);
    }
    if (gram->contains_lpr("DEF")) {
        EXPECT_EQ(gram->lpr("DEF").qualifer, taul::qualifier::none);
    }
}

// this tests the usage of LPRs/PPRs of primary exprs

TEST_F(CompileTests, PrimaryExprs_Compile_Success) {

    std::filesystem::path src_path = std::filesystem::current_path() / "support\\compile_test_spec_2.taul";

    ASSERT_TRUE(std::filesystem::exists(src_path)) << "file compile_test_spec_2.taul not found!";

    auto src = std::make_shared<taul::source_code>();
    src->add_file(src_path, lgr);
    

    auto result = taul::compile(ctx, src, ec, lgr);

    ASSERT_TRUE((bool)result);

    EXPECT_TRUE(result->associated(src));


    auto gram = taul::load(*result, lgr);

    ASSERT_TRUE((bool)gram);

    TAUL_LOG(lgr, "{}", *gram);

    ASSERT_TRUE(gram->contains_lpr("LPR0"));
    ASSERT_TRUE(gram->contains_lpr("LPR1"));
    ASSERT_TRUE(gram->contains_lpr("LPR2"));
    ASSERT_TRUE(gram->contains_lpr("LPR3"));

    ASSERT_TRUE(gram->contains_ppr("PPR0"));
    ASSERT_TRUE(gram->contains_ppr("PPR1"));
    ASSERT_TRUE(gram->contains_ppr("PPR2"));
    ASSERT_TRUE(gram->contains_ppr("PPR3"));
    ASSERT_TRUE(gram->contains_ppr("PPR4"));

    const auto& lpr0 = gram->lpr("LPR0");
    const auto& lpr1 = gram->lpr("LPR1");
    const auto& lpr2 = gram->lpr("LPR2");
    const auto& lpr3 = gram->lpr("LPR3");

    const auto& ppr0 = gram->ppr("PPR0");
    const auto& ppr1 = gram->ppr("PPR1");
    const auto& ppr2 = gram->ppr("PPR2");
    const auto& ppr3 = gram->ppr("PPR3");
    const auto& ppr4 = gram->ppr("PPR4");

    auto lxr0 = gram->lexer("LPR0");
    auto lxr1 = gram->lexer("LPR1");
    auto lxr2 = gram->lexer("LPR2");
    auto lxr3 = gram->lexer("LPR3");

    auto psr0 = gram->parser("PPR0");
    auto psr1 = gram->parser("PPR1");
    auto psr2 = gram->parser("PPR2");
    auto psr3 = gram->parser("PPR3");
    auto psr4 = gram->parser("PPR4");

    // LPR0 : LPR3 LPR3 LPR3 ;

    EXPECT_EQ(lxr0("abcadcadcabc"), taul::token(lpr0, "abcadcadc"));
    EXPECT_EQ(lxr0("abcadcadCabc"), taul::token::failure(""));
    EXPECT_EQ(lxr0("abcadcad"), taul::token::failure(""));

    EXPECT_EQ(lxr0("___abcadcadcabc", 3), taul::token(lpr0, "abcadcadc", 3));
    EXPECT_EQ(lxr0("___abcadcadCabc", 3), taul::token::failure("", 3));
    EXPECT_EQ(lxr0("___abcadcad", 3), taul::token::failure("", 3));
    
    // LPR1 : begin 'abc' end ;

    EXPECT_EQ(lxr1("abc"), taul::token(lpr1, "abc"));
    EXPECT_EQ(lxr1("ab"), taul::token::failure(""));
    EXPECT_EQ(lxr1("abcd"), taul::token::failure(""));
    EXPECT_EQ(lxr1("123"), taul::token::failure(""));

    EXPECT_EQ(lxr1("___abc", 3), taul::token::failure("", 3));
    EXPECT_EQ(lxr1("___ab", 3), taul::token::failure("", 3));
    EXPECT_EQ(lxr1("___abcd", 3), taul::token::failure("", 3));
    EXPECT_EQ(lxr1("___123", 3), taul::token::failure("", 3));
    
    // LPR2 : any any any ;

    EXPECT_EQ(lxr2("abc"), taul::token(lpr2, "abc"));
    EXPECT_EQ(lxr2("abcd"), taul::token(lpr2, "abc"));
    EXPECT_EQ(lxr2("123"), taul::token(lpr2, "123"));
    EXPECT_EQ(lxr2("1234"), taul::token(lpr2, "123"));
    EXPECT_EQ(lxr2("!@#"), taul::token(lpr2, "!@#"));
    EXPECT_EQ(lxr2("!@#$"), taul::token(lpr2, "!@#"));
    EXPECT_EQ(lxr2("ab"), taul::token::failure(""));
    EXPECT_EQ(lxr2("12"), taul::token::failure(""));
    EXPECT_EQ(lxr2("!@"), taul::token::failure(""));

    EXPECT_EQ(lxr2("___abc", 3), taul::token(lpr2, "abc", 3));
    EXPECT_EQ(lxr2("___abcd", 3), taul::token(lpr2, "abc", 3));
    EXPECT_EQ(lxr2("___123", 3), taul::token(lpr2, "123", 3));
    EXPECT_EQ(lxr2("___1234", 3), taul::token(lpr2, "123", 3));
    EXPECT_EQ(lxr2("___!@#", 3), taul::token(lpr2, "!@#", 3));
    EXPECT_EQ(lxr2("___!@#$", 3), taul::token(lpr2, "!@#", 3));
    EXPECT_EQ(lxr2("___ab", 3), taul::token::failure("", 3));
    EXPECT_EQ(lxr2("___12", 3), taul::token::failure("", 3));
    EXPECT_EQ(lxr2("___!@", 3), taul::token::failure("", 3));
    
    // LPR3 : 'a' [bd] 'c' ;

    EXPECT_EQ(lxr3("abc"), taul::token(lpr3, "abc"));
    EXPECT_EQ(lxr3("adc"), taul::token(lpr3, "adc"));
    EXPECT_EQ(lxr3("abcd"), taul::token(lpr3, "abc"));
    EXPECT_EQ(lxr3("adcd"), taul::token(lpr3, "adc"));
    EXPECT_EQ(lxr3("abC"), taul::token::failure(""));
    EXPECT_EQ(lxr3("adC"), taul::token::failure(""));
    EXPECT_EQ(lxr3("abCd"), taul::token::failure(""));
    EXPECT_EQ(lxr3("adCd"), taul::token::failure(""));

    EXPECT_EQ(lxr3("___abc", 3), taul::token(lpr3, "abc", 3));
    EXPECT_EQ(lxr3("___adc", 3), taul::token(lpr3, "adc", 3));
    EXPECT_EQ(lxr3("___abcd", 3), taul::token(lpr3, "abc", 3));
    EXPECT_EQ(lxr3("___adcd", 3), taul::token(lpr3, "adc", 3));
    EXPECT_EQ(lxr3("___abC", 3), taul::token::failure("", 3));
    EXPECT_EQ(lxr3("___adC", 3), taul::token::failure("", 3));
    EXPECT_EQ(lxr3("___abCd", 3), taul::token::failure("", 3));
    EXPECT_EQ(lxr3("___adCd", 3), taul::token::failure("", 3));

    // PPR0 : PPR3 PPR4 PPR3 ;

    {
        std::string txt = "abcdefadc";
        taul::token_seq tkns(txt);
        // just gonna *manually* make some tokens to test w/
        tkns.push(lpr3, 3);
        tkns.push_failure(3);
        tkns.push(lpr3, 3);

        auto expected =
            taul::node_assembler()
            .begin(ctx.create(ppr0, tkns.range_str(0, 3), 0))
            .enter(ctx.create(ppr3, "abc", 0))
            .attach(ctx.create(tkns[0]))
            .exit()
            .enter(ctx.create(ppr4, "def", 3))
            .attach(ctx.create(tkns[1]))
            .exit()
            .enter(ctx.create(ppr3, "adc", 6))
            .attach(ctx.create(tkns[2]))
            .exit()
            .end();

        TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());

        auto actual = psr0(ctx, tkns);

        if (actual) {
            TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());

            EXPECT_TRUE(expected.equiv(*actual));
        }
        else ADD_FAILURE();
    }

    {
        std::string txt = "abcabcadc";
        taul::token_seq tkns(txt);
        // just gonna *manually* make some tokens to test w/
        tkns.push(lpr3, 3);
        tkns.push(lpr3, 3);
        tkns.push(lpr3, 3);

        EXPECT_FALSE((bool)psr0(ctx, tkns));
    }

    {
        std::string txt = "abCdefadc";
        taul::token_seq tkns(txt);
        // just gonna *manually* make some tokens to test w/
        // these *back-to-back* failure tokens aren't how TAUL works, but whatever...
        tkns.push_failure(3);
        tkns.push_failure(3);
        tkns.push(lpr3, 3);

        EXPECT_FALSE((bool)psr0(ctx, tkns));
    }
    
    // PPR1 : begin 'abc' end ;

    {
        std::string txt = "abc";
        taul::token_seq tkns(txt);
        // just gonna *manually* make some tokens to test w/
        tkns.push(lpr3, 3);

        auto expected =
            taul::node_assembler()
            .begin(ctx.create(ppr1, "abc", 0))
            .attach(ctx.create(tkns[0]))
            .end();

        TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());

        auto actual = psr1(ctx, tkns);

        if (actual) {
            TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());

            EXPECT_TRUE(expected.equiv(*actual));
        }
        else ADD_FAILURE();
    }

    {
        std::string txt = "abcabc";
        taul::token_seq tkns(txt);
        // just gonna *manually* make some tokens to test w/
        tkns.push(lpr3, 3);
        tkns.push(lpr3, 3);

        EXPECT_FALSE((bool)psr1(ctx, tkns));
    }

    {
        std::string txt = "abcabc";
        taul::token_seq tkns(txt);
        // just gonna *manually* make some tokens to test w/
        tkns.push(lpr3, 3);
        tkns.push(lpr3, 3);

        EXPECT_FALSE((bool)psr1(ctx, tkns, 1));
    }
    
    // PPR2 : any any any ;

    {
        std::string txt = "abcabcdefabc";
        taul::token_seq tkns(txt);
        // just gonna *manually* make some tokens to test w/
        tkns.push(lpr3, 3);
        tkns.push(lpr3, 3);
        tkns.push_failure(3);
        tkns.push(lpr3, 3);

        auto expected =
            taul::node_assembler()
            .begin(ctx.create(ppr2, tkns.range_str(0, 3), 0))
            .attach(ctx.create(tkns[0]))
            .attach(ctx.create(tkns[1]))
            .attach(ctx.create(tkns[2]))
            .end();

        TAUL_LOG(lgr, "expected:\n{}", expected.fmt_tree());

        auto actual = psr2(ctx, tkns);

        if (actual) {
            TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());

            EXPECT_TRUE(expected.equiv(*actual));
        }
        else ADD_FAILURE();
    }

    {
        std::string txt = "abcabc";
        taul::token_seq tkns(txt);
        // just gonna *manually* make some tokens to test w/
        tkns.push(lpr3, 3);
        tkns.push(lpr3, 3);

        EXPECT_FALSE((bool)psr2(ctx, tkns));
    }

    {
        std::string txt = "abcabcabcabc";
        taul::token_seq tkns(txt);
        // just gonna *manually* make some tokens to test w/
        tkns.push(lpr3, 3);
        tkns.push(lpr3, 3);
        tkns.push(lpr3, 3);
        tkns.push(lpr3, 3);

        EXPECT_FALSE((bool)psr2(ctx, tkns, 2));
    }
    
    // PPR3 : token ; <- these should be implicitly tested w/ PPR0
    
    // PPR4 : failure ; <- these should be implicitly tested w/ PPR0
}

