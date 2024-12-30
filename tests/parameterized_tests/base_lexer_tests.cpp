

#include "base_lexer_tests.h"

#include <taul/spec.h>
#include <taul/load.h>
#include <taul/source_reader.h>

#include "../helpers/test_token_observer.h"
#include "../helpers/test_glyph_stream_for_reset.h"


using namespace taul::string_literals;


GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(BaseLexerTests);


// these are tests for the basic usage of the lexer as a component


static std::optional<taul::grammar> make_grammar_1a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Observers) {
    auto gram = make_grammar_1a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("aaaa"_str);
    lxr->bind_source(&input);

    test_token_observer obsvr{};
    lxr->bind_observer(&obsvr);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(4));

    test_token_observer expected{};
    expected.observe(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected.observe(taul::token::normal(gram.value(), "A"_str, 1, 1));
    expected.observe(taul::token::normal(gram.value(), "A"_str, 2, 1));
    expected.observe(taul::token::normal(gram.value(), "A"_str, 3, 1));
    expected.observe(taul::token::end(4));

    EXPECT_EQ(expected.output, obsvr.output);
}

TEST_P(BaseLexerTests, PeekNextAndDone) {
    auto gram = make_grammar_1a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("aaaa"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_FALSE(lxr->done());

    EXPECT_EQ(lxr->peek(), taul::token::normal(gram.value(), "A"_str, 0, 1));
    EXPECT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 1));

    ASSERT_FALSE(lxr->done());

    EXPECT_EQ(lxr->peek(), taul::token::normal(gram.value(), "A"_str, 1, 1));
    EXPECT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 1));

    ASSERT_FALSE(lxr->done());

    EXPECT_EQ(lxr->peek(), taul::token::normal(gram.value(), "A"_str, 2, 1));
    EXPECT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 2, 1));

    ASSERT_FALSE(lxr->done());

    EXPECT_EQ(lxr->peek(), taul::token::normal(gram.value(), "A"_str, 3, 1));
    EXPECT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 1));

    ASSERT_TRUE(lxr->done());

    EXPECT_EQ(lxr->peek(), taul::token::end(4));
    EXPECT_EQ(lxr->next(), taul::token::end(4));

    ASSERT_TRUE(lxr->done());
}

TEST_P(BaseLexerTests, Reset_PropagateUpstream) {
    auto gram = make_grammar_1a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);
    
    test_glyph_stream_for_reset upstream{};
    lxr->bind_source(&upstream);

    ASSERT_FALSE(upstream.flag);

    lxr->reset(); // should propagate

    ASSERT_TRUE(upstream.flag);
}

TEST_P(BaseLexerTests, Reset_PipelineReuse) {
    auto gram = make_grammar_1a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input1("aaa"_str);
    lxr->bind_source(&input1);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 2, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(3));

    taul::source_reader input2("bbbb"_str);
    lxr->bind_source(&input2); // test that impl can handle switching out upstream
    lxr->reset(); // reset progress made

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 1, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 3, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(4));
}


// these tests are for certain tokenization semantics


static std::optional<taul::grammar> make_grammar_2a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A1"_str)
        .lpr_decl("A2"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .lpr("A1"_str)
        .string("a"_str)
        .close()
        .lpr("A2"_str) // A1 should ALWAYS be preferred over A2
        .string("abc"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, MultipleLexerRules_PickFirstMatch) {
    auto gram = make_grammar_2a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A1"_str));
    ASSERT_TRUE(gram->has_lpr("A2"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcbbabc"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A1"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 1, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "C"_str, 2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 3, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A1"_str, 5, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 6, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "C"_str, 7, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(8));
}

static std::optional<taul::grammar> make_grammar_2b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str, taul::skip)
        .string("b"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, SkipLexerRules_Cut_IfCutSkipTokensEqualsTrue_ButStillSendTokenToObserver) {
    auto gram = make_grammar_2b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    lxr->cut_skip_tokens = true;

    taul::source_reader input("aababbaba"_str);
    lxr->bind_source(&input);

    test_token_observer obsvr{};
    lxr->bind_observer(&obsvr);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 1));
    //ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 1));
    //ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 4, 1));
    //ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 5, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 6, 1));
    //ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 7, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 8, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(9));

    test_token_observer expected{};
    expected.observe(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected.observe(taul::token::normal(gram.value(), "A"_str, 1, 1));
    expected.observe(taul::token::normal(gram.value(), "B"_str, 2, 1));
    expected.observe(taul::token::normal(gram.value(), "A"_str, 3, 1));
    expected.observe(taul::token::normal(gram.value(), "B"_str, 4, 1));
    expected.observe(taul::token::normal(gram.value(), "B"_str, 5, 1));
    expected.observe(taul::token::normal(gram.value(), "A"_str, 6, 1));
    expected.observe(taul::token::normal(gram.value(), "B"_str, 7, 1));
    expected.observe(taul::token::normal(gram.value(), "A"_str, 8, 1));
    expected.observe(taul::token::end(9));

    EXPECT_EQ(expected.output, obsvr.output);
}

TEST_P(BaseLexerTests, SkipLexerRules_DoNotCut_IfCutSkipTokensEqualsFalse) {
    auto gram = make_grammar_2b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    lxr->cut_skip_tokens = false;

    taul::source_reader input("aababbaba"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 5, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 6, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 7, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 8, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(9));
}

static std::optional<taul::grammar> make_grammar_2c(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("ABC"_str)
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .lpr("ABC"_str, taul::support) // put this first to test it is genuinely *skipped*
        .string("abc"_str)
        .close()
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, SupportLexerRules_DoNotEval) {
    auto gram = make_grammar_2c(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("ABC"_str));
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcabcabc"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 1, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "C"_str, 2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "C"_str, 5, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 6, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "B"_str, 7, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "C"_str, 8, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(9));
}

static std::optional<taul::grammar> make_grammar_2d(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .lpr("A"_str, taul::support)
        .string("a"_str)
        .close()
        .lpr("B"_str, taul::support)
        .string("b"_str)
        .close()
        .lpr("C"_str, taul::support)
        .string("c"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, SupportLexerRules_EdgeCaseWhereAllLPRsAreSupport) {
    auto gram = make_grammar_2d(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcabcabc"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::failure(0, 9)); // no LPRs to match against
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(9));
}

static std::optional<taul::grammar> make_grammar_2e(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("SPACE"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("SPACE"_str, taul::skip) // make sure impl works w/ failure mixed w/ skip tokens
        .string(" "_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, FailureTokenPolicy) {
    auto gram = make_grammar_2e(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("SPACE"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    lxr->cut_skip_tokens = true; // note we want to cut skip tokens for this test

    taul::source_reader input("aa ab& baabcdaab "_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 1));
    //ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "SPACE"_str, 2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(4, 2));
    //ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "SPACE"_str, 6, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(7, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 8, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 9, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(10, 3));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 13, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 14, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(15, 1));
    //ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "SPACE"_str, 16, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(17));
}

static std::optional<taul::grammar> make_grammar_2f(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("EMPTY"_str)
        .lpr_decl("A"_str)
        .lpr("EMPTY"_str)
        .close()
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, ZeroLengthTokenPolicy) {
    auto gram = make_grammar_2f(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("EMPTY"_str));
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("aaaaa"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "EMPTY"_str, 0, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "EMPTY"_str, 1, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(1, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "EMPTY"_str, 2, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "EMPTY"_str, 3, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(3, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "EMPTY"_str, 4, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "EMPTY"_str, 5, 0)); // no following failure token
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(5)); // same location as final length 0 token
}

static std::optional<taul::grammar> make_grammar_2g(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("EMPTY"_str)
        .lpr_decl("A"_str)
        .lpr("EMPTY"_str, taul::skip)
        .close()
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, ZeroLengthTokenPolicy_IfZeroLengthTokenIsSkip) {
    auto gram = make_grammar_2g(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("EMPTY"_str));
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("aaaaa"_str);
    lxr->bind_source(&input);

    test_token_observer obsvr{};
    lxr->bind_observer(&obsvr);

    lxr->reset();

    //ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "EMPTY"_str, 0, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(0, 1));
    //ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "EMPTY"_str, 1, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(1, 1));
    //ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "EMPTY"_str, 2, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(2, 1));
    //ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "EMPTY"_str, 3, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(3, 1));
    //ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "EMPTY"_str, 4, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(4, 1));
    //ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "EMPTY"_str, 5, 0));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(5));

    test_token_observer expected{};
    expected.observe(taul::token::normal(gram.value(), "EMPTY"_str, 0, 0));
    expected.observe(taul::token::failure(0, 1));
    expected.observe(taul::token::normal(gram.value(), "EMPTY"_str, 1, 0));
    expected.observe(taul::token::failure(1, 1));
    expected.observe(taul::token::normal(gram.value(), "EMPTY"_str, 2, 0));
    expected.observe(taul::token::failure(2, 1));
    expected.observe(taul::token::normal(gram.value(), "EMPTY"_str, 3, 0));
    expected.observe(taul::token::failure(3, 1));
    expected.observe(taul::token::normal(gram.value(), "EMPTY"_str, 4, 0));
    expected.observe(taul::token::failure(4, 1));
    expected.observe(taul::token::normal(gram.value(), "EMPTY"_str, 5, 0));
    expected.observe(taul::token::end(5));

    EXPECT_EQ(expected.output, obsvr.output);
}


// these tests are for expected TAUL semantics


static_assert(taul::spec_opcodes == 21);


// for composite exprs, these tests will largely assume that so long
// as some basic nesting works, that it'll work in an arbitrary case


// lpr


static std::optional<taul::grammar> make_grammar_3a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, LPR_Empty) {
    auto gram = make_grammar_3a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abc"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(1, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 2, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 0));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(3));
}

static std::optional<taul::grammar> make_grammar_3b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, LPR_NonEmpty_WithNoAlts) {
    auto gram = make_grammar_3b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abaab"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(1, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(4, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(5));
}

static std::optional<taul::grammar> make_grammar_3c(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .string("a"_str)
        .alternative()
        .string("b"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, LPR_NonEmpty_WithAlts_WithNoEmptyAlt) {
    auto gram = make_grammar_3c(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcbaabc"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 5, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 6, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(7, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(8));
}

static std::optional<taul::grammar> make_grammar_3d(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .string("a"_str)
        .alternative()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, LPR_NonEmpty_WithAlts_WithEmptyAlt) {
    auto gram = make_grammar_3d(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abaab"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(1, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 5, 0));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(5));
}

static std::optional<taul::grammar> make_grammar_3e(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .string("a"_str)
        .sequence()
        .string("b"_str)
        .close()
        .string("c"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, LPR_Nesting) {
    auto gram = make_grammar_3e(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcabcabcdabc"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 3));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 3));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 6, 3));
    ASSERT_EQ(lxr->next(), taul::token::failure(9, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 10, 3));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(13));
}


// end


static std::optional<taul::grammar> make_grammar_4a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .end()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, End) {
    auto gram = make_grammar_4a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("a"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::failure(0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 0));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(1));
}

TEST_P(BaseLexerTests, End_EmptyInput) {
    auto gram = make_grammar_4a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input(""_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 0));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(0));
}


// any


static std::optional<taul::grammar> make_grammar_5a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .any()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Any) {
    auto gram = make_grammar_5a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    // TODO: this is an extremely *incomplete* sample of the different inputs we could give it

    taul::source_reader input("abc\n\t 123!@(*$#_+-=;\\~"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 5, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 6, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 7, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 8, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 9, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 10, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 11, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 12, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 13, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 14, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 15, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 16, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 17, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 18, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 19, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 20, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 21, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(22));
}

TEST_P(BaseLexerTests, Any_EmptyInput) {
    auto gram = make_grammar_5a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input(""_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(0));
}


// string


static std::optional<taul::grammar> make_grammar_6a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .string("abc"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, String) {
    auto gram = make_grammar_6a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcabcdabcdef"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 3));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 3));
    ASSERT_EQ(lxr->next(), taul::token::failure(6, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 7, 3));
    ASSERT_EQ(lxr->next(), taul::token::failure(10, 3));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(13));
}

TEST_P(BaseLexerTests, String_EmptyInput) {
    auto gram = make_grammar_6a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input(""_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(0));
}


// charset


static std::optional<taul::grammar> make_grammar_7a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .charset("a-cef1-5"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Charset) {
    auto gram = make_grammar_7a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcdefghi01234567"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 2, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(3, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 5, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(6, 4));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 10, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 11, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 12, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 13, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 14, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(15, 2));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(17));
}

TEST_P(BaseLexerTests, Charset_EmptyInput) {
    auto gram = make_grammar_7a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input(""_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(0));
}


// name


static std::optional<taul::grammar> make_grammar_8a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str) // put second to ensure it never gets picked
        .lpr("A"_str)
        .name("B"_str)
        .close()
        .lpr("B"_str)
        .string("abc"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Name) {
    auto gram = make_grammar_8a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcdabcdef"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 3));
    ASSERT_EQ(lxr->next(), taul::token::failure(3, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 3));
    ASSERT_EQ(lxr->next(), taul::token::failure(7, 3));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(10));
}

TEST_P(BaseLexerTests, Name_EmptyInput) {
    auto gram = make_grammar_8a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input(""_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(0));
}

static std::optional<taul::grammar> make_grammar_8b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str) // put second to ensure it never gets picked
        .lpr("A"_str)
        .name("B"_str)
        .close()
        .lpr("B"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Name_RefRuleCanFormZeroLengthToken) {
    auto gram = make_grammar_8b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("a"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 0));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(1));
}

TEST_P(BaseLexerTests, Name_RefRuleCanFormZeroLengthToken_EmptyInput) {
    auto gram = make_grammar_8b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input(""_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 0));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(0));
}

static std::optional<taul::grammar> make_grammar_8c(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str) // put second to ensure it never gets picked
        .lpr("A"_str)
        .name("B"_str)
        .close()
        .lpr("B"_str, taul::skip)
        .string("abc"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Name_RefSkip) {
    auto gram = make_grammar_8c(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcdabcdef"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 3));
    ASSERT_EQ(lxr->next(), taul::token::failure(3, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 3));
    ASSERT_EQ(lxr->next(), taul::token::failure(7, 3));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(10));
}

static std::optional<taul::grammar> make_grammar_8d(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str) // put second to ensure it never gets picked
        .lpr("A"_str)
        .name("B"_str)
        .close()
        .lpr("B"_str, taul::support)
        .string("abc"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Name_RefSupport) {
    auto gram = make_grammar_8d(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcdabcdef"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 3));
    ASSERT_EQ(lxr->next(), taul::token::failure(3, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 3));
    ASSERT_EQ(lxr->next(), taul::token::failure(7, 3));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(10));
}

static std::optional<taul::grammar> make_grammar_8e(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .string("a"_str)
        .name("A"_str)
        .alternative()
        // empty alt
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Name_Recursion) {
    auto gram = make_grammar_8e(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("aaabbabaa"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 3));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(3, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 5, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 6, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(6, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 7, 2));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 9, 0));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(9));
}


// sequence


static std::optional<taul::grammar> make_grammar_9a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .sequence()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Sequence_Empty) {
    auto gram = make_grammar_9a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abc"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(1, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 2, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 0));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(3));
}

static std::optional<taul::grammar> make_grammar_9b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .sequence()
        .string("a"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Sequence_NonEmpty_WithNoAlts) {
    auto gram = make_grammar_9b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abaab"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(1, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(4, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(5));
}

static std::optional<taul::grammar> make_grammar_9c(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .sequence()
        .string("a"_str)
        .alternative()
        .string("b"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Sequence_NonEmpty_WithAlts_WithNoEmptyAlt) {
    auto gram = make_grammar_9c(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcbaabc"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 5, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 6, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(7, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(8));
}

static std::optional<taul::grammar> make_grammar_9d(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .sequence()
        .string("a"_str)
        .alternative()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Sequence_NonEmpty_WithAlts_WithEmptyAlt) {
    auto gram = make_grammar_9d(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abaab"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(1, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 2, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 5, 0));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(5));
}

static std::optional<taul::grammar> make_grammar_9e(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .sequence()
        .string("a"_str)
        .sequence()
        .string("b"_str)
        .close()
        .string("c"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Sequence_Nesting) {
    auto gram = make_grammar_9e(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcabcabcdabc"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 3));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 3));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 6, 3));
    ASSERT_EQ(lxr->next(), taul::token::failure(9, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 10, 3));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(13));
}


// lookahead


static std::optional<taul::grammar> make_grammar_10a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .lookahead()
        .string("a"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, LookAhead_NonEmpty_WithNoAlts) {
    auto gram = make_grammar_10a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("aabbaa"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(1, 3));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 5, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(5, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(6));
}

static std::optional<taul::grammar> make_grammar_10b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .lookahead()
        .string("a"_str)
        .alternative()
        .string("b"_str)
        .alternative()
        .string("d"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, LookAhead_NonEmpty_WithAlts) {
    auto gram = make_grammar_10b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcdef"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(1, 2));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(3, 3));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(6));
}

static std::optional<taul::grammar> make_grammar_10c(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .lookahead()
        .sequence()
        .string("a"_str)
        .alternative()
        .string("b"_str)
        .alternative()
        .string("d"_str)
        .close()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, LookAhead_Nesting) {
    auto gram = make_grammar_10c(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcdef"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(1, 2));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(3, 3));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(6));
}


// lookahead-not


static std::optional<taul::grammar> make_grammar_11a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .lookahead_not()
        .string("a"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, LookAheadNot_NonEmpty_WithNoAlts) {
    auto gram = make_grammar_11a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("bbaabb"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(1, 3));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 5, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(5, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(6));
}

static std::optional<taul::grammar> make_grammar_11b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .lookahead_not()
        .string("a"_str)
        .alternative()
        .string("b"_str)
        .alternative()
        .string("d"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, LookAheadNot_NonEmpty_WithAlts) {
    auto gram = make_grammar_11b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcdef"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::failure(0, 2));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 2, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(2, 2));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 5, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(5, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(6));
}

static std::optional<taul::grammar> make_grammar_11c(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .lookahead_not()
        .sequence()
        .string("a"_str)
        .alternative()
        .string("b"_str)
        .alternative()
        .string("d"_str)
        .close()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, LookAheadNot_Nesting) {
    auto gram = make_grammar_11c(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcdef"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::failure(0, 2));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 2, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(2, 2));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 5, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(5, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(6));
}


// not


static std::optional<taul::grammar> make_grammar_12a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .not0()
        .string("a"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Not_NonEmpty_WithNoAlts) {
    auto gram = make_grammar_12a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("bbaabb"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 1, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(2, 2));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 5, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(6));
}

static std::optional<taul::grammar> make_grammar_12b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .not0()
        .string("a"_str)
        .alternative()
        .string("b"_str)
        .alternative()
        .string("d"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Not_NonEmpty_WithAlts) {
    auto gram = make_grammar_12b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcdef"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::failure(0, 2));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 2, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(3, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 5, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(6));
}

static std::optional<taul::grammar> make_grammar_12c(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .not0()
        .sequence()
        .string("a"_str)
        .alternative()
        .string("b"_str)
        .alternative()
        .string("d"_str)
        .close()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Not_Nesting) {
    auto gram = make_grammar_12c(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcdef"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::failure(0, 2));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 2, 1));
    ASSERT_EQ(lxr->next(), taul::token::failure(3, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 4, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 5, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(6));
}


// optional


static std::optional<taul::grammar> make_grammar_13a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .optional()
        .string("abc"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Optional) {
    auto gram = make_grammar_13a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcabcdabc"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 3));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 3));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 6, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(6, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 7, 3));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 10, 0));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(10));
}

TEST_P(BaseLexerTests, Optional_EmptyInput) {
    auto gram = make_grammar_13a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input(""_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 0));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(0));
}

static std::optional<taul::grammar> make_grammar_13b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .optional()
        .sequence()
        .string("abc"_str)
        .close()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, Optional_Nesting) {
    auto gram = make_grammar_13b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcabcdabc"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 3));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 3, 3));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 6, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(6, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 7, 3));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 10, 0));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(10));
}


// kleene-star


static std::optional<taul::grammar> make_grammar_14a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .kleene_star()
        .string("abc"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, KleeneStar) {
    auto gram = make_grammar_14a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcabcabcdabcabcd"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 9));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 9, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(9, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 10, 6));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 16, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(16, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 17, 0));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(17));
}

TEST_P(BaseLexerTests, KleeneStar_EmptyInput) {
    auto gram = make_grammar_14a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input(""_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 0));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(0));
}

static std::optional<taul::grammar> make_grammar_14b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .kleene_star()
        .sequence()
        .string("abc"_str)
        .close()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, KleeneStar_Nesting) {
    auto gram = make_grammar_14b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcabcabcdabcabcd"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 9));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 9, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(9, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 10, 6));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 16, 0));
    ASSERT_EQ(lxr->next(), taul::token::failure(16, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 17, 0));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(17));
}


// kleene-plus


static std::optional<taul::grammar> make_grammar_15a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .kleene_plus()
        .string("abc"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, KleenePlus) {
    auto gram = make_grammar_15a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcabcabcdabcabcd"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 9));
    ASSERT_EQ(lxr->next(), taul::token::failure(9, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 10, 6));
    ASSERT_EQ(lxr->next(), taul::token::failure(16, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(17));
}

TEST_P(BaseLexerTests, KleenePlus_EmptyInput) {
    auto gram = make_grammar_15a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input(""_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(0));
}

static std::optional<taul::grammar> make_grammar_15b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .kleene_plus()
        .sequence()
        .string("abc"_str)
        .close()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseLexerTests, KleenePlus_Nesting) {
    auto gram = make_grammar_15b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));

    auto lxr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(lxr);

    taul::source_reader input("abcabcabcdabcabcd"_str);
    lxr->bind_source(&input);

    lxr->reset();

    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 0, 9));
    ASSERT_EQ(lxr->next(), taul::token::failure(9, 1));
    ASSERT_EQ(lxr->next(), taul::token::normal(gram.value(), "A"_str, 10, 6));
    ASSERT_EQ(lxr->next(), taul::token::failure(16, 1));
    ASSERT_TRUE(lxr->done());
    ASSERT_EQ(lxr->next(), taul::token::end(17));
}

