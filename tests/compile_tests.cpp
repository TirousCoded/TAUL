

#include <gtest/gtest.h>

#include <taul/all.h>


#include "helpers/test_spec_interpreter.h"


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


// these tests test that taul::compile produces the expected spec for a given input

// originally I though we'd be best to test taul::compile by compiling a spec, then loading
// a grammar w/ it, then testing that grammar's behaviour, to decouple our tests from the
// *specific* spec generated

// however, I've come to realize now that doing that would require us to essentially create
// an entire parallel set of semantic tests for taul::compile, so that, alongside other
// similar reasons, has convinced me to test it w/ the below instead

TEST_F(CompileTests, TopLevel) {

    std::filesystem::path src_path = std::filesystem::current_path() / "support\\compile_test_spec_1.taul";

    ASSERT_TRUE(std::filesystem::exists(src_path)) << "file compile_test_spec_1.taul not found!";

    auto src = std::make_shared<taul::source_code>();
    src->add_file(src_path, lgr);


    auto result = taul::compile(ctx, src, ec, lgr);

    ASSERT_TRUE((bool)result);

    EXPECT_TRUE(result->associated(src));

    EXPECT_EQ(ec.total(), 0);


    test_spec_interpreter tsi{};

    std::string expected{};


    tsi.interpret(*result);

    expected += "startup\n";
    expected += "lpr-decl \"LPR0\"\n";
    expected += "lpr-decl \"LPR1\"\n";
    expected += "lpr-decl \"LPR2\"\n";
    expected += "lpr-decl \"LPR3\"\n";
    expected += "ppr-decl \"PPR0\"\n";
    expected += "ppr-decl \"PPR1\"\n";
    expected += "ppr-decl \"PPR2\"\n";
    expected += "lpr-decl \"LPR4\"\n";
    expected += "ppr-decl \"PPR3\"\n";
    expected += "lpr \"LPR0\" none\n";
    expected += "any\n";
    expected += "close\n";
    expected += "lpr \"LPR1\" none\n";
    expected += "any\n";
    expected += "close\n";
    expected += "lpr \"LPR2\" skip\n";
    expected += "any\n";
    expected += "close\n";
    expected += "lpr \"LPR3\" support\n";
    expected += "any\n";
    expected += "close\n";
    expected += "ppr \"PPR0\" none\n";
    expected += "any\n";
    expected += "close\n";
    expected += "ppr \"PPR1\" skip\n";
    expected += "any\n";
    expected += "close\n";
    expected += "ppr \"PPR2\" support\n";
    expected += "any\n";
    expected += "close\n";
    expected += "lpr \"LPR4\" none\n";
    expected += "any\n";
    expected += "close\n";
    expected += "ppr \"PPR3\" none\n";
    expected += "any\n";
    expected += "close\n";
    expected += "shutdown\n";

    ASSERT_EQ(tsi.output, expected);

    TAUL_LOG(lgr, "{}", tsi.output);
}

TEST_F(CompileTests, PrimaryExprs) {

    std::filesystem::path src_path = std::filesystem::current_path() / "support\\compile_test_spec_2.taul";

    ASSERT_TRUE(std::filesystem::exists(src_path)) << "file compile_test_spec_2.taul not found!";

    auto src = std::make_shared<taul::source_code>();
    src->add_file(src_path, lgr);


    auto result = taul::compile(ctx, src, ec, lgr);

    ASSERT_TRUE((bool)result);

    EXPECT_TRUE(result->associated(src));

    EXPECT_EQ(ec.total(), 0);


    test_spec_interpreter tsi{};

    std::string expected{};


    tsi.interpret(*result);

    expected += "startup\n";
    expected += "lpr-decl \"LPR0\"\n";
    expected += "ppr-decl \"PPR0\"\n";
    expected += "lpr \"LPR0\" none\n";
    expected += "sequence\n";
    expected += "end\n";
    expected += "any\n";
    expected += "string \"abc\\n\\fdef\"\n";
    expected += "charset \"-abc1-49-6A\\-Z-\"\n";
    expected += "token\n";
    expected += "failure\n";
    expected += "name \"LPR0\"\n";
    expected += "name \"PPR0\"\n";
    expected += "close\n";
    expected += "close\n";
    expected += "ppr \"PPR0\" none\n";
    expected += "sequence\n";
    expected += "end\n";
    expected += "any\n";
    expected += "string \"abc\\n\\fdef\"\n";
    expected += "charset \"-abc1-49-6A\\-Z-\"\n";
    expected += "token\n";
    expected += "failure\n";
    expected += "name \"LPR0\"\n";
    expected += "name \"PPR0\"\n";
    expected += "close\n";
    expected += "close\n";
    expected += "shutdown\n";

    ASSERT_EQ(tsi.output, expected);

    TAUL_LOG(lgr, "{}", tsi.output);
}

TEST_F(CompileTests, GroupExprs) {

    std::filesystem::path src_path = std::filesystem::current_path() / "support\\compile_test_spec_3.taul";

    ASSERT_TRUE(std::filesystem::exists(src_path)) << "file compile_test_spec_3.taul not found!";

    auto src = std::make_shared<taul::source_code>();
    src->add_file(src_path, lgr);


    auto result = taul::compile(ctx, src, ec, lgr);

    ASSERT_TRUE((bool)result);

    EXPECT_TRUE(result->associated(src));

    EXPECT_EQ(ec.total(), 0);


    test_spec_interpreter tsi{};

    std::string expected{};


    tsi.interpret(*result);

    expected += "startup\n";
    expected += "lpr-decl \"LPR0\"\n";
    expected += "ppr-decl \"PPR0\"\n";
    expected += "lpr \"LPR0\" none\n";
    expected += "any\n";
    expected += "close\n";
    expected += "ppr \"PPR0\" none\n";
    expected += "any\n";
    expected += "close\n";
    expected += "shutdown\n";

    ASSERT_EQ(tsi.output, expected);

    TAUL_LOG(lgr, "{}", tsi.output);
}

TEST_F(CompileTests, LookAheadExprs) {

    std::filesystem::path src_path = std::filesystem::current_path() / "support\\compile_test_spec_4.taul";

    ASSERT_TRUE(std::filesystem::exists(src_path)) << "file compile_test_spec_4.taul not found!";

    auto src = std::make_shared<taul::source_code>();
    src->add_file(src_path, lgr);


    auto result = taul::compile(ctx, src, ec, lgr);

    ASSERT_TRUE((bool)result);

    EXPECT_TRUE(result->associated(src));

    EXPECT_EQ(ec.total(), 0);


    test_spec_interpreter tsi{};

    std::string expected{};


    tsi.interpret(*result);

    expected += "startup\n";
    expected += "lpr-decl \"LPR0\"\n";
    expected += "ppr-decl \"PPR0\"\n";
    expected += "lpr \"LPR0\" none\n";
    expected += "lookahead\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "ppr \"PPR0\" none\n";
    expected += "lookahead\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "shutdown\n";

    ASSERT_EQ(tsi.output, expected);

    TAUL_LOG(lgr, "{}", tsi.output);
}

TEST_F(CompileTests, LookAheadNotExprs) {

    std::filesystem::path src_path = std::filesystem::current_path() / "support\\compile_test_spec_5.taul";

    ASSERT_TRUE(std::filesystem::exists(src_path)) << "file compile_test_spec_5.taul not found!";

    auto src = std::make_shared<taul::source_code>();
    src->add_file(src_path, lgr);


    auto result = taul::compile(ctx, src, ec, lgr);

    ASSERT_TRUE((bool)result);

    EXPECT_TRUE(result->associated(src));

    EXPECT_EQ(ec.total(), 0);


    test_spec_interpreter tsi{};

    std::string expected{};


    tsi.interpret(*result);

    expected += "startup\n";
    expected += "lpr-decl \"LPR0\"\n";
    expected += "ppr-decl \"PPR0\"\n";
    expected += "lpr \"LPR0\" none\n";
    expected += "lookahead-not\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "ppr \"PPR0\" none\n";
    expected += "lookahead-not\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "shutdown\n";

    ASSERT_EQ(tsi.output, expected);

    TAUL_LOG(lgr, "{}", tsi.output);
}

TEST_F(CompileTests, NotExprs) {

    std::filesystem::path src_path = std::filesystem::current_path() / "support\\compile_test_spec_6.taul";

    ASSERT_TRUE(std::filesystem::exists(src_path)) << "file compile_test_spec_6.taul not found!";

    auto src = std::make_shared<taul::source_code>();
    src->add_file(src_path, lgr);


    auto result = taul::compile(ctx, src, ec, lgr);

    ASSERT_TRUE((bool)result);

    EXPECT_TRUE(result->associated(src));

    EXPECT_EQ(ec.total(), 0);


    test_spec_interpreter tsi{};

    std::string expected{};


    tsi.interpret(*result);

    expected += "startup\n";
    expected += "lpr-decl \"LPR0\"\n";
    expected += "ppr-decl \"PPR0\"\n";
    expected += "lpr \"LPR0\" none\n";
    expected += "not\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "ppr \"PPR0\" none\n";
    expected += "not\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "shutdown\n";

    ASSERT_EQ(tsi.output, expected);

    TAUL_LOG(lgr, "{}", tsi.output);
}

TEST_F(CompileTests, OptionalExprs) {

    std::filesystem::path src_path = std::filesystem::current_path() / "support\\compile_test_spec_7.taul";

    ASSERT_TRUE(std::filesystem::exists(src_path)) << "file compile_test_spec_7.taul not found!";

    auto src = std::make_shared<taul::source_code>();
    src->add_file(src_path, lgr);


    auto result = taul::compile(ctx, src, ec, lgr);

    ASSERT_TRUE((bool)result);

    EXPECT_TRUE(result->associated(src));

    EXPECT_EQ(ec.total(), 0);


    test_spec_interpreter tsi{};

    std::string expected{};


    tsi.interpret(*result);

    expected += "startup\n";
    expected += "lpr-decl \"LPR0\"\n";
    expected += "ppr-decl \"PPR0\"\n";
    expected += "lpr \"LPR0\" none\n";
    expected += "optional\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "ppr \"PPR0\" none\n";
    expected += "optional\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "shutdown\n";

    ASSERT_EQ(tsi.output, expected);

    TAUL_LOG(lgr, "{}", tsi.output);
}

TEST_F(CompileTests, KleeneStarExprs) {

    std::filesystem::path src_path = std::filesystem::current_path() / "support\\compile_test_spec_8.taul";

    ASSERT_TRUE(std::filesystem::exists(src_path)) << "file compile_test_spec_8.taul not found!";

    auto src = std::make_shared<taul::source_code>();
    src->add_file(src_path, lgr);


    auto result = taul::compile(ctx, src, ec, lgr);

    ASSERT_TRUE((bool)result);

    EXPECT_TRUE(result->associated(src));

    EXPECT_EQ(ec.total(), 0);


    test_spec_interpreter tsi{};

    std::string expected{};


    tsi.interpret(*result);

    expected += "startup\n";
    expected += "lpr-decl \"LPR0\"\n";
    expected += "ppr-decl \"PPR0\"\n";
    expected += "lpr \"LPR0\" none\n";
    expected += "kleene-star\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "ppr \"PPR0\" none\n";
    expected += "kleene-star\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "shutdown\n";

    ASSERT_EQ(tsi.output, expected);

    TAUL_LOG(lgr, "{}", tsi.output);
}

TEST_F(CompileTests, KleenePlusExprs) {

    std::filesystem::path src_path = std::filesystem::current_path() / "support\\compile_test_spec_9.taul";

    ASSERT_TRUE(std::filesystem::exists(src_path)) << "file compile_test_spec_9.taul not found!";

    auto src = std::make_shared<taul::source_code>();
    src->add_file(src_path, lgr);


    auto result = taul::compile(ctx, src, ec, lgr);

    ASSERT_TRUE((bool)result);

    EXPECT_TRUE(result->associated(src));

    EXPECT_EQ(ec.total(), 0);


    test_spec_interpreter tsi{};

    std::string expected{};


    tsi.interpret(*result);

    expected += "startup\n";
    expected += "lpr-decl \"LPR0\"\n";
    expected += "ppr-decl \"PPR0\"\n";
    expected += "lpr \"LPR0\" none\n";
    expected += "kleene-plus\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "ppr \"PPR0\" none\n";
    expected += "kleene-plus\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "shutdown\n";

    ASSERT_EQ(tsi.output, expected);

    TAUL_LOG(lgr, "{}", tsi.output);
}

TEST_F(CompileTests, SequenceExprs) {

    std::filesystem::path src_path = std::filesystem::current_path() / "support\\compile_test_spec_10.taul";

    ASSERT_TRUE(std::filesystem::exists(src_path)) << "file compile_test_spec_10.taul not found!";

    auto src = std::make_shared<taul::source_code>();
    src->add_file(src_path, lgr);


    auto result = taul::compile(ctx, src, ec, lgr);

    ASSERT_TRUE((bool)result);

    EXPECT_TRUE(result->associated(src));

    EXPECT_EQ(ec.total(), 0);


    test_spec_interpreter tsi{};

    std::string expected{};


    tsi.interpret(*result);

    expected += "startup\n";
    expected += "lpr-decl \"LPR0\"\n";
    expected += "ppr-decl \"PPR0\"\n";
    expected += "lpr \"LPR0\" none\n";
    expected += "sequence\n";
    expected += "any\n";
    expected += "sequence\n";
    expected += "any\n";
    expected += "any\n";
    expected += "close\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "ppr \"PPR0\" none\n";
    expected += "sequence\n";
    expected += "any\n";
    expected += "sequence\n";
    expected += "any\n";
    expected += "any\n";
    expected += "close\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "shutdown\n";

    ASSERT_EQ(tsi.output, expected);

    TAUL_LOG(lgr, "{}", tsi.output);
}

TEST_F(CompileTests, SetExprs) {

    std::filesystem::path src_path = std::filesystem::current_path() / "support\\compile_test_spec_11.taul";

    ASSERT_TRUE(std::filesystem::exists(src_path)) << "file compile_test_spec_11.taul not found!";

    auto src = std::make_shared<taul::source_code>();
    src->add_file(src_path, lgr);


    auto result = taul::compile(ctx, src, ec, lgr);

    ASSERT_TRUE((bool)result);

    EXPECT_TRUE(result->associated(src));

    EXPECT_EQ(ec.total(), 0);


    test_spec_interpreter tsi{};

    std::string expected{};


    tsi.interpret(*result);

    expected += "startup\n";
    expected += "lpr-decl \"LPR0\"\n";
    expected += "ppr-decl \"PPR0\"\n";
    expected += "lpr \"LPR0\" none\n";
    expected += "set\n";
    expected += "sequence\n";
    expected += "any\n";
    expected += "any\n";
    expected += "close\n";
    expected += "any\n";
    expected += "set\n";
    expected += "sequence\n";
    expected += "any\n";
    expected += "any\n";
    expected += "close\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "close\n";
    expected += "ppr \"PPR0\" none\n";
    expected += "set\n";
    expected += "sequence\n";
    expected += "any\n";
    expected += "any\n";
    expected += "close\n";
    expected += "any\n";
    expected += "set\n";
    expected += "sequence\n";
    expected += "any\n";
    expected += "any\n";
    expected += "close\n";
    expected += "any\n";
    expected += "close\n";
    expected += "close\n";
    expected += "close\n";
    expected += "shutdown\n";

    ASSERT_EQ(tsi.output, expected);

    TAUL_LOG(lgr, "{}", tsi.output);
}


TEST_F(CompileTests, SyntaxError) {

    std::filesystem::path src_path = std::filesystem::current_path() / "support\\compile_test_spec_err.taul";

    ASSERT_TRUE(std::filesystem::exists(src_path)) << "file compile_test_spec_err.taul not found!";

    auto src = std::make_shared<taul::source_code>();
    src->add_file(src_path, lgr);


    auto result = taul::compile(ctx, src, ec, lgr);

    ASSERT_FALSE((bool)result);

    EXPECT_TRUE(ec.count(taul::spec_error::syntax_error) >= 1);
}

