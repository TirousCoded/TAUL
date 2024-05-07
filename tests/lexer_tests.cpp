

#include <gtest/gtest.h>

#include <taul/spec.h>
#include <taul/grammar.h>
#include <taul/load.h>
#include <taul/source_reader.h>
#include <taul/lexer.h>

#include "parameterized_tests/token_stream_tests.h"
#include "parameterized_tests/base_lexer_tests.h"


using namespace taul::string_literals;


// test w/ non-empty input

TokenStreamParam _make_param_1() {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .string("abc"_str)
        .close()
        .done();
    auto loaded = taul::load(spec, taul::make_stderr_logger());
    // abort test if grammar load fails
    if (!loaded) return TokenStreamParam::init(nullptr, 0);
    auto stream = std::make_shared<taul::lexer>(loaded.value(), taul::make_stderr_logger());
    stream->bind_source(std::make_shared<taul::source_reader>("abcabcabc"_str));
    stream->reset();
    std::size_t n_after_done = 3;
    return TokenStreamParam::init(stream, n_after_done);
}

INSTANTIATE_TEST_SUITE_P(
    Lexer_NonEmptyInput,
    TokenStreamTests,
    testing::Values(_make_param_1()));


// test w/ empty input

TokenStreamParam _make_param_2() {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr("A"_str)
        .string("abc"_str)
        .close()
        .done();
    auto loaded = taul::load(spec, taul::make_stderr_logger());
    // abort test if grammar load fails
    if (!loaded) return TokenStreamParam::init(nullptr, 0);
    auto stream = std::make_shared<taul::lexer>(loaded.value(), taul::make_stderr_logger());
    stream->bind_source(std::make_shared<taul::source_reader>(""_str));
    stream->reset();
    std::size_t n_after_done = 0;
    return TokenStreamParam::init(stream, n_after_done);
}

INSTANTIATE_TEST_SUITE_P(
    Lexer_EmptyInput,
    TokenStreamTests,
    testing::Values(_make_param_2()));


BaseLexerParam _make_param_3() {
    auto factory = [](taul::grammar gram, std::shared_ptr<taul::logger> lgr) -> std::shared_ptr<taul::base_lexer> {
        return std::make_shared<taul::lexer>(gram, lgr);
        };
    return BaseLexerParam::init(factory);
}

INSTANTIATE_TEST_SUITE_P(
    Lexer,
    BaseLexerTests,
    testing::Values(_make_param_3()));

