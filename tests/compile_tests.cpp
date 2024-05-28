

#include <gtest/gtest.h>

#include <taul/disassemble_spec.h>
#include <taul/compile.h>


using namespace taul::string_literals;


// taul::compile will be tested by asserting the expected spec generated

// originally I thought to test it by asserting the expected behaviour
// of a grammar loaded by the generated spec, but I realized that doing
// so would be in many ways equivalent to having to maintain two copies
// of code testing taul::taul_spec, so I decided against it


// like w/ our load tests, we'll just assert that the impl needs to raise
// the expected error at least once, but is free to raise it multiple
// times, and alongside whatever other errors it may choose to raise


class CompileTests : public testing::Test {
protected:
    std::shared_ptr<taul::logger> lgr = nullptr;
    taul::spec_error_counter ec;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
    }
};


TEST_F(CompileTests, SourceCodeNotFoundError_DueToSrcBeingNullptr) {
    auto actual = taul::compile(nullptr, ec, lgr, false);

    EXPECT_GE(ec.count(taul::spec_error::source_code_not_found), 1);
    EXPECT_FALSE(actual);
}

TEST_F(CompileTests, SourceCodeNotFoundError_DueToSrcFileNotFound) {
    auto actual = taul::compile(std::filesystem::current_path() / "nonexistent-taul-script.taul", ec, lgr, false);

    EXPECT_GE(ec.count(taul::spec_error::source_code_not_found), 1);
    EXPECT_FALSE(actual);
}

TEST_F(CompileTests, SyntaxError) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_err_1.taul"));

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_GE(ec.count(taul::spec_error::syntax_error), 1);
    EXPECT_FALSE(actual);
}

TEST_F(CompileTests, IllegalMultipleQualifiers_ForLPRs) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_err_2.taul"));

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_GE(ec.count(taul::spec_error::illegal_multiple_qualifiers), 1);
    EXPECT_FALSE(actual);
}

TEST_F(CompileTests, IllegalMultipleQualifiers_ForPPRs) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_err_3.taul"));

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_GE(ec.count(taul::spec_error::illegal_multiple_qualifiers), 1);
    EXPECT_FALSE(actual);
}

TEST_F(CompileTests, IllegalStringLiteral_DueToNoClosingSingleQuote_ForLPRs) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_err_4a.taul"));

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_GE(ec.count(taul::spec_error::illegal_string_literal), 1);
    EXPECT_FALSE(actual);
}

TEST_F(CompileTests, IllegalStringLiteral_DueToNoClosingSingleQuote_ForPPRs) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_err_4b.taul"));

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_GE(ec.count(taul::spec_error::illegal_string_literal), 1);
    EXPECT_FALSE(actual);
}

TEST_F(CompileTests, IllegalCharsetLiteral_DueToNoClosingSquareBracket_ForLPRs) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_err_5a.taul"));

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_GE(ec.count(taul::spec_error::illegal_charset_literal), 1);
    EXPECT_FALSE(actual);
}

TEST_F(CompileTests, IllegalCharsetLiteral_DueToNoClosingSquareBracket_ForPPRs) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_err_5b.taul"));

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_GE(ec.count(taul::spec_error::illegal_charset_literal), 1);
    EXPECT_FALSE(actual);
}


TEST_F(CompileTests, ReturnedSpecIsImbuedWithSrc) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_1.taul"));

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_EQ(ec.total(), 0);
    ASSERT_TRUE(actual);

    EXPECT_TRUE(actual->associated(src));
}

TEST_F(CompileTests, TopLevel) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_2.taul"));

    auto expected =
        taul::spec_writer()
        .lpr_decl("LPR0"_str)
        .lpr_decl("LPR1"_str)
        .lpr_decl("LPR2"_str)
        .ppr_decl("PPR0"_str)
        .ppr_decl("PPR1"_str)
        .ppr_decl("PPR2"_str)
        .lpr("LPR0"_str)
        .close()
        .lpr("LPR1"_str, taul::skip)
        .any()
        .close()
        .lpr("LPR2"_str, taul::support)
        .any()
        .any()
        .alternative()
        .any()
        .alternative()
        .alternative()
        .close()
        .ppr("PPR0"_str)
        .close()
        .ppr("PPR1"_str)
        .any()
        .close()
        .ppr("PPR2"_str)
        .any()
        .any()
        .alternative()
        .any()
        .alternative()
        .alternative()
        .close()
        .done();

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_EQ(ec.total(), 0);
    ASSERT_TRUE(actual);

    EXPECT_EQ(taul::disassemble_spec(expected), taul::disassemble_spec(actual.value()));
}

TEST_F(CompileTests, TopLevel_Empty) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_3.taul"));

    auto expected =
        taul::spec_writer()
        .done();

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_EQ(ec.total(), 0);
    ASSERT_TRUE(actual);

    EXPECT_EQ(taul::disassemble_spec(expected), taul::disassemble_spec(actual.value()));
}

TEST_F(CompileTests, TopLevel_ImplicitLexerSectionIfNoExplicitSectionIsDeclared) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_4.taul"));

    auto expected =
        taul::spec_writer()
        .lpr_decl("LPR0"_str)
        .lpr_decl("LPR1"_str)
        .lpr_decl("LPR2"_str)
        .lpr("LPR0"_str)
        .close()
        .lpr("LPR1"_str, taul::skip)
        .any()
        .close()
        .lpr("LPR2"_str, taul::support)
        .any()
        .any()
        .alternative()
        .any()
        .alternative()
        .alternative()
        .close()
        .done();

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_EQ(ec.total(), 0);
    ASSERT_TRUE(actual);

    EXPECT_EQ(taul::disassemble_spec(expected), taul::disassemble_spec(actual.value()));
}

TEST_F(CompileTests, PrimaryExprs) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_5a.taul"));

    auto expected =
        taul::spec_writer()
        .lpr_decl("LPR0"_str)
        .lpr_decl("LPR1"_str)
        .ppr_decl("PPR0"_str)
        .ppr_decl("PPR1"_str)
        .lpr("LPR0"_str)
        .end()
        .any()
        .string("abc"_str)
        .charset("0-9a-zA-Z_"_str)
        .name("LPR1"_str)
        .close()
        .lpr("LPR1"_str)
        .close()
        .ppr("PPR0"_str)
        .end()
        .any()
        .token()
        .failure()
        .name("LPR1"_str)
        .name("PPR1"_str)
        .close()
        .ppr("PPR1"_str)
        .close()
        .done();

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_EQ(ec.total(), 0);
    ASSERT_TRUE(actual);

    TAUL_LOG(lgr, "expected:\n{}", taul::disassemble_spec(expected));
    TAUL_LOG(lgr, "actual:\n{}", taul::disassemble_spec(actual.value()));

    EXPECT_EQ(taul::disassemble_spec(expected), taul::disassemble_spec(actual.value()));
}

TEST_F(CompileTests, PrimaryExprs_WithNonASCIIUnicode) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_5b.taul"));

    auto expected =
        taul::spec_writer()
        .lpr_decl("LPR0"_str)
        .lpr("LPR0"_str)
        .string(taul::convert_encoding<char>(taul::utf8, taul::utf8, u8"abc123Δ魂💩").value())
        .charset(taul::convert_encoding<char>(taul::utf8, taul::utf8, u8"a-fα-δ魂か-く").value())
        .close()
        .done();

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_EQ(ec.total(), 0);
    ASSERT_TRUE(actual);

    TAUL_LOG(lgr, "expected:\n{}", taul::disassemble_spec(expected));
    TAUL_LOG(lgr, "actual:\n{}", taul::disassemble_spec(actual.value()));

    EXPECT_EQ(taul::disassemble_spec(expected), taul::disassemble_spec(actual.value()));
}

TEST_F(CompileTests, SequenceExprs) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_6.taul"));

    auto expected =
        taul::spec_writer()
        .lpr_decl("LPR0"_str)
        .ppr_decl("PPR0"_str)
        .lpr("LPR0"_str)
        .sequence()
        .any()
        .alternative()
        .any()
        .any()
        .alternative()
        .alternative()
        .close()
        .close()
        .ppr("PPR0"_str)
        .sequence()
        .any()
        .alternative()
        .any()
        .any()
        .alternative()
        .alternative()
        .close()
        .close()
        .done();

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_EQ(ec.total(), 0);
    ASSERT_TRUE(actual);

    EXPECT_EQ(taul::disassemble_spec(expected), taul::disassemble_spec(actual.value()));
}

TEST_F(CompileTests, LookAheadExprs) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_7.taul"));

    auto expected =
        taul::spec_writer()
        .lpr_decl("LPR0"_str)
        .ppr_decl("PPR0"_str)
        .lpr("LPR0"_str)
        .lookahead()
        .any()
        .close()
        .close()
        .ppr("PPR0"_str)
        .lookahead()
        .any()
        .close()
        .close()
        .done();

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_EQ(ec.total(), 0);
    ASSERT_TRUE(actual);

    EXPECT_EQ(taul::disassemble_spec(expected), taul::disassemble_spec(actual.value()));
}

TEST_F(CompileTests, LookAheadNotExprs) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_8.taul"));

    auto expected =
        taul::spec_writer()
        .lpr_decl("LPR0"_str)
        .ppr_decl("PPR0"_str)
        .lpr("LPR0"_str)
        .lookahead_not()
        .any()
        .close()
        .close()
        .ppr("PPR0"_str)
        .lookahead_not()
        .any()
        .close()
        .close()
        .done();

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_EQ(ec.total(), 0);
    ASSERT_TRUE(actual);

    EXPECT_EQ(taul::disassemble_spec(expected), taul::disassemble_spec(actual.value()));
}

TEST_F(CompileTests, NotExprs) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_9.taul"));

    auto expected =
        taul::spec_writer()
        .lpr_decl("LPR0"_str)
        .ppr_decl("PPR0"_str)
        .lpr("LPR0"_str)
        .not0()
        .any()
        .close()
        .close()
        .ppr("PPR0"_str)
        .not0()
        .any()
        .close()
        .close()
        .done();

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_EQ(ec.total(), 0);
    ASSERT_TRUE(actual);

    EXPECT_EQ(taul::disassemble_spec(expected), taul::disassemble_spec(actual.value()));
}

TEST_F(CompileTests, OptionalExprs) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_10.taul"));

    auto expected =
        taul::spec_writer()
        .lpr_decl("LPR0"_str)
        .ppr_decl("PPR0"_str)
        .lpr("LPR0"_str)
        .optional()
        .any()
        .close()
        .close()
        .ppr("PPR0"_str)
        .optional()
        .any()
        .close()
        .close()
        .done();

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_EQ(ec.total(), 0);
    ASSERT_TRUE(actual);

    TAUL_LOG(lgr, "expected:\n{}", taul::disassemble_spec(expected));
    TAUL_LOG(lgr, "actual:\n{}", taul::disassemble_spec(actual.value()));

    EXPECT_EQ(taul::disassemble_spec(expected), taul::disassemble_spec(actual.value()));
}

TEST_F(CompileTests, KleeneStarExprs) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_11.taul"));

    auto expected =
        taul::spec_writer()
        .lpr_decl("LPR0"_str)
        .ppr_decl("PPR0"_str)
        .lpr("LPR0"_str)
        .kleene_star()
        .any()
        .close()
        .close()
        .ppr("PPR0"_str)
        .kleene_star()
        .any()
        .close()
        .close()
        .done();

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_EQ(ec.total(), 0);
    ASSERT_TRUE(actual);

    TAUL_LOG(lgr, "expected:\n{}", taul::disassemble_spec(expected));
    TAUL_LOG(lgr, "actual:\n{}", taul::disassemble_spec(actual.value()));

    EXPECT_EQ(taul::disassemble_spec(expected), taul::disassemble_spec(actual.value()));
}

TEST_F(CompileTests, KleenePlusExprs) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_12.taul"));

    auto expected =
        taul::spec_writer()
        .lpr_decl("LPR0"_str)
        .ppr_decl("PPR0"_str)
        .lpr("LPR0"_str)
        .kleene_plus()
        .any()
        .close()
        .close()
        .ppr("PPR0"_str)
        .kleene_plus()
        .any()
        .close()
        .close()
        .done();

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_EQ(ec.total(), 0);
    ASSERT_TRUE(actual);

    TAUL_LOG(lgr, "expected:\n{}", taul::disassemble_spec(expected));
    TAUL_LOG(lgr, "actual:\n{}", taul::disassemble_spec(actual.value()));

    EXPECT_EQ(taul::disassemble_spec(expected), taul::disassemble_spec(actual.value()));
}

TEST_F(CompileTests, NonTrivialExprPrecedence) {
    auto src = std::make_shared<taul::source_code>();
    ASSERT_TRUE(src->add_file(std::filesystem::current_path() / "support\\compile_tests_13.taul"));

    auto expected =
        taul::spec_writer()
        .lpr_decl("LPR0"_str)
        .ppr_decl("PPR0"_str)
        .lpr("LPR0"_str)
        .kleene_plus()
        .kleene_star()
        .optional()
        .lookahead()
        .lookahead_not()
        .not0()
        .any()
        .close()
        .close()
        .close()
        .close()
        .close()
        .close()
        .close()
        .ppr("PPR0"_str)
        .kleene_plus()
        .kleene_star()
        .optional()
        .lookahead()
        .lookahead_not()
        .not0()
        .any()
        .close()
        .close()
        .close()
        .close()
        .close()
        .close()
        .close()
        .done();

    auto actual = taul::compile(src, ec, lgr, false);

    EXPECT_EQ(ec.total(), 0);
    ASSERT_TRUE(actual);

    TAUL_LOG(lgr, "expected:\n{}", taul::disassemble_spec(expected));
    TAUL_LOG(lgr, "actual:\n{}", taul::disassemble_spec(actual.value()));

    EXPECT_EQ(taul::disassemble_spec(expected), taul::disassemble_spec(actual.value()));
}

