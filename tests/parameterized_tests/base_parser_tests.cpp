

#include "base_parser_tests.h"

#include <taul/spec.h>
#include <taul/load.h>
#include <taul/source_reader.h>
#include <taul/lexer.h>


#include "../helpers/test_listener.h"
#include "../helpers/test_token_stream_for_reset.h"
#include "../helpers/test_error_handler_for_input_recording.h"


using namespace taul::string_literals;


GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(BaseParserTests);


// these two error handler impls are used to test error handler invocation

// success_error_handler advancing the lexer stream by one token, which is to
// be enough to recover from the error

class success_error_handler final : public taul::error_handler {
public:

    taul::base_parser* client = nullptr;


    success_error_handler(std::shared_ptr<taul::logger> lgr = nullptr) 
        : error_handler(lgr) {}


    void startup(taul::base_parser* client) override final {
        TAUL_ASSERT(client);
        this->client = client;
    }
    void shutdown() override final {
        // do nothing
    }
    void terminal_error(taul::token_range ids, taul::token input) override final {
        TAUL_DEREF_SAFE(client) {
            client->eh_next();
        }
    }
    void nonterminal_error(taul::symbol_id id, taul::token input) override final {
        TAUL_DEREF_SAFE(client) {
            client->eh_next();
        }
    }
};

// failure_error_handler does nothing to recover from the error

class failure_error_handler final : public taul::error_handler {
public:

    failure_error_handler(std::shared_ptr<taul::logger> lgr = nullptr)
        : error_handler(lgr) {}


    void startup(taul::base_parser*) override final {
        // do nothing
    }
    void shutdown() override final {
        // do nothing
    }
    void terminal_error(taul::token_range, taul::token) override final {
        // do nothing
    }
    void nonterminal_error(taul::symbol_id, taul::token) override final {
        // do nothing
    }
};


// these are tests for the basic usage of the parser as a component

// listeners will be tested w/ testing parse and parse_notree


static std::optional<taul::grammar> make_grammar_1a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("abcabc"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("abcabc"_str)
        .name("A"_str)
        .name("B"_str)
        .name("C"_str)
        .name("A"_str)
        .name("B"_str)
        .name("C"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Parse) {
    auto gram = make_grammar_1a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("abcabc"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("abcabc"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    test_listener lstnr{};
    psr->bind_listener(&lstnr);

    test_error_handler_for_input_recording eh{};
    psr->bind_error_handler(&eh);

    psr->reset();

    auto result = psr->parse(gram->ppr("abcabc"_str).value());

    test_listener expected{};
    expected.on_startup();
    expected.on_syntactic(gram->ppr("abcabc"_str).value(), 0);
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 3, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "B"_str, 4, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "C"_str, 5, 1));
    expected.on_close();
    expected.on_shutdown();

    test_listener playback{};
    playback.playback(result);

    test_error_handler_for_input_recording eh_expected{};
    eh_expected.startup(psr.get());
    eh_expected.shutdown();

    EXPECT_EQ(expected.output, lstnr.output);
    EXPECT_EQ(expected.output, playback.output);
    EXPECT_EQ(eh.output, eh_expected.output);

    EXPECT_EQ(lstnr.terminal_errors, 0);
    EXPECT_EQ(lstnr.nonterminal_errors, 0);
}

TEST_P(BaseParserTests, Parse_ErrorRecovery_TerminalError) {
    auto gram = make_grammar_1a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("abcabc"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("abcadbc"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    test_listener lstnr{};
    psr->bind_listener(&lstnr);

    success_error_handler eh_inner{};
    test_error_handler_for_input_recording eh(&eh_inner);
    psr->bind_error_handler(&eh);

    psr->reset();

    auto result = psr->parse(gram->ppr("abcabc"_str).value());

    test_listener expected{};
    expected.on_startup();
    expected.on_syntactic(gram->ppr("abcabc"_str).value(), 0);
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 3, 1));
    // error at 4
    expected.on_lexical(taul::token::normal(gram.value(), "B"_str, 5, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "C"_str, 6, 1));
    expected.on_close();
    expected.on_shutdown();

    test_listener playback{};
    playback.playback(result);

    test_error_handler_for_input_recording eh_expected{};
    eh_expected.startup(psr.get());
    eh_expected.terminal_error(
        taul::token_range::create(gram->lpr("B"_str).value().id(), gram->lpr("B"_str).value().id()), 
        taul::token::failure(4, 1));
    eh_expected.shutdown();

    EXPECT_EQ(expected.output, lstnr.output);
    EXPECT_EQ(expected.output, playback.output);
    EXPECT_EQ(eh.output, eh_expected.output);

    EXPECT_GE(lstnr.terminal_errors, 1);
    EXPECT_EQ(lstnr.nonterminal_errors, 0);
}

TEST_P(BaseParserTests, Parse_ErrorRecovery_NonTerminalError) {
    auto gram = make_grammar_1a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("abcabc"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("dabcabc"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    test_listener lstnr{};
    psr->bind_listener(&lstnr);

    success_error_handler eh_inner{};
    test_error_handler_for_input_recording eh(&eh_inner);
    psr->bind_error_handler(&eh);

    psr->reset();

    auto result = psr->parse(gram->ppr("abcabc"_str).value());

    test_listener expected{};
    expected.on_startup();
    // error at 0
    expected.on_syntactic(gram->ppr("abcabc"_str).value(), 1);
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 1, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "B"_str, 2, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "C"_str, 3, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 4, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "B"_str, 5, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "C"_str, 6, 1));
    expected.on_close();
    expected.on_shutdown();

    test_listener playback{};
    playback.playback(result);

    test_error_handler_for_input_recording eh_expected{};
    eh_expected.startup(psr.get());
    eh_expected.nonterminal_error(gram->ppr("abcabc"_str).value().id(), taul::token::failure(0, 1));
    eh_expected.shutdown();

    EXPECT_EQ(expected.output, lstnr.output);
    EXPECT_EQ(expected.output, playback.output);
    EXPECT_EQ(eh.output, eh_expected.output);

    EXPECT_EQ(lstnr.terminal_errors, 0);
    EXPECT_GE(lstnr.nonterminal_errors, 1);
}

TEST_P(BaseParserTests, Parse_FailErrorRecovery_TerminalError) {
    auto gram = make_grammar_1a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("abcabc"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("abcad"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    test_listener lstnr{};
    psr->bind_listener(&lstnr);

    failure_error_handler eh_inner{};
    test_error_handler_for_input_recording eh(&eh_inner);
    psr->bind_error_handler(&eh);

    psr->reset();

    auto result = psr->parse(gram->ppr("abcabc"_str).value());

    test_listener expected{};
    expected.on_startup();
    expected.on_syntactic(gram->ppr("abcabc"_str).value(), 0);
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 3, 1));
    expected.on_close();
    expected.on_abort();
    expected.on_shutdown();

    test_listener playback{};
    playback.playback(result);

    test_error_handler_for_input_recording eh_expected{};
    eh_expected.startup(psr.get());
    eh_expected.terminal_error(
        taul::token_range::create(gram->lpr("B"_str).value().id(), gram->lpr("B"_str).value().id()), 
        taul::token::failure(4, 1));
    eh_expected.shutdown();

    EXPECT_EQ(expected.output, lstnr.output);
    EXPECT_EQ(expected.output, playback.output);
    EXPECT_EQ(eh.output, eh_expected.output);

    EXPECT_GE(lstnr.terminal_errors, 1);
    EXPECT_EQ(lstnr.nonterminal_errors, 0);
}

TEST_P(BaseParserTests, Parse_FailErrorRecovery_NonTerminalError) {
    auto gram = make_grammar_1a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("abcabc"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("d"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    test_listener lstnr{};
    psr->bind_listener(&lstnr);

    failure_error_handler eh_inner{};
    test_error_handler_for_input_recording eh(&eh_inner);
    psr->bind_error_handler(&eh);

    psr->reset();

    auto result = psr->parse(gram->ppr("abcabc"_str).value());

    test_listener expected{};
    expected.on_startup();
    expected.on_abort();
    expected.on_shutdown();

    test_listener playback{};
    playback.playback(result);

    test_error_handler_for_input_recording eh_expected{};
    eh_expected.startup(psr.get());
    eh_expected.nonterminal_error(gram->ppr("abcabc"_str).value().id(), taul::token::failure(0, 1));
    eh_expected.shutdown();

    EXPECT_EQ(expected.output, lstnr.output);
    EXPECT_EQ(expected.output, playback.output);
    EXPECT_EQ(eh.output, eh_expected.output);

    EXPECT_EQ(lstnr.terminal_errors, 0);
    EXPECT_GE(lstnr.nonterminal_errors, 1);
}

TEST_P(BaseParserTests, ParseNoTree) {
    auto gram = make_grammar_1a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("abcabc"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("abcabc"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    test_listener lstnr{};
    psr->bind_listener(&lstnr);

    psr->reset();

    psr->parse_notree(gram->ppr("abcabc"_str).value());

    test_listener expected{};
    expected.on_startup();
    expected.on_syntactic(gram->ppr("abcabc"_str).value(), 0);
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 3, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "B"_str, 4, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "C"_str, 5, 1));
    expected.on_close();
    expected.on_shutdown();

    EXPECT_EQ(expected.output, lstnr.output);

    EXPECT_EQ(lstnr.terminal_errors, 0);
    EXPECT_EQ(lstnr.nonterminal_errors, 0);
}

TEST_P(BaseParserTests, ParseNoTree_ErrorRecovery_TerminalError) {
    auto gram = make_grammar_1a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("abcabc"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("abcadbc"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    test_listener lstnr{};
    psr->bind_listener(&lstnr);

    success_error_handler eh_inner{};
    test_error_handler_for_input_recording eh(&eh_inner);
    psr->bind_error_handler(&eh);

    psr->reset();

    psr->parse_notree(gram->ppr("abcabc"_str).value());

    test_listener expected{};
    expected.on_startup();
    expected.on_syntactic(gram->ppr("abcabc"_str).value(), 0);
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 3, 1));
    // error at 4
    expected.on_lexical(taul::token::normal(gram.value(), "B"_str, 5, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "C"_str, 6, 1));
    expected.on_close();
    expected.on_shutdown();

    test_error_handler_for_input_recording eh_expected{};
    eh_expected.startup(psr.get());
    eh_expected.terminal_error(
        taul::token_range::create(gram->lpr("B"_str).value().id(), gram->lpr("B"_str).value().id()),
        taul::token::failure(4, 1));
    eh_expected.shutdown();

    EXPECT_EQ(expected.output, lstnr.output);
    EXPECT_EQ(eh.output, eh_expected.output);

    EXPECT_GE(lstnr.terminal_errors, 1);
    EXPECT_EQ(lstnr.nonterminal_errors, 0);
}

TEST_P(BaseParserTests, ParseNoTree_ErrorRecovery_NonTerminalError) {
    auto gram = make_grammar_1a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("abcabc"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("dabcabc"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    test_listener lstnr{};
    psr->bind_listener(&lstnr);

    success_error_handler eh_inner{};
    test_error_handler_for_input_recording eh(&eh_inner);
    psr->bind_error_handler(&eh);

    psr->reset();

    psr->parse_notree(gram->ppr("abcabc"_str).value());

    test_listener expected{};
    expected.on_startup();
    // error at 0
    expected.on_syntactic(gram->ppr("abcabc"_str).value(), 1);
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 1, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "B"_str, 2, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "C"_str, 3, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 4, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "B"_str, 5, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "C"_str, 6, 1));
    expected.on_close();
    expected.on_shutdown();

    test_error_handler_for_input_recording eh_expected{};
    eh_expected.startup(psr.get());
    eh_expected.nonterminal_error(gram->ppr("abcabc"_str).value().id(), taul::token::failure(0, 1));
    eh_expected.shutdown();

    EXPECT_EQ(expected.output, lstnr.output);
    EXPECT_EQ(eh.output, eh_expected.output);

    EXPECT_EQ(lstnr.terminal_errors, 0);
    EXPECT_GE(lstnr.nonterminal_errors, 1);
}

TEST_P(BaseParserTests, ParseNoTree_FailErrorRecovery_TerminalError) {
    auto gram = make_grammar_1a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("abcabc"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("abcad"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    test_listener lstnr{};
    psr->bind_listener(&lstnr);

    failure_error_handler eh_inner{};
    test_error_handler_for_input_recording eh(&eh_inner);
    psr->bind_error_handler(&eh);

    psr->reset();

    psr->parse_notree(gram->ppr("abcabc"_str).value());

    test_listener expected{};
    expected.on_startup();
    expected.on_syntactic(gram->ppr("abcabc"_str).value(), 0);
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 3, 1));
    expected.on_close();
    expected.on_abort();
    expected.on_shutdown();

    test_error_handler_for_input_recording eh_expected{};
    eh_expected.startup(psr.get());
    eh_expected.terminal_error(
        taul::token_range::create(gram->lpr("B"_str).value().id(), gram->lpr("B"_str).value().id()),
        taul::token::failure(4, 1));
    eh_expected.shutdown();

    EXPECT_EQ(expected.output, lstnr.output);
    EXPECT_EQ(eh.output, eh_expected.output);

    EXPECT_GE(lstnr.terminal_errors, 1);
    EXPECT_EQ(lstnr.nonterminal_errors, 0);
}

TEST_P(BaseParserTests, ParseNoTree_FailErrorRecovery_NonTerminalError) {
    auto gram = make_grammar_1a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("abcabc"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("d"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    test_listener lstnr{};
    psr->bind_listener(&lstnr);

    failure_error_handler eh_inner{};
    test_error_handler_for_input_recording eh(&eh_inner);
    psr->bind_error_handler(&eh);

    psr->reset();

    psr->parse_notree(gram->ppr("abcabc"_str).value());

    test_listener expected{};
    expected.on_startup();
    expected.on_abort();
    expected.on_shutdown();

    test_error_handler_for_input_recording eh_expected{};
    eh_expected.startup(psr.get());
    eh_expected.nonterminal_error(gram->ppr("abcabc"_str).value().id(), taul::token::failure(0, 1));
    eh_expected.shutdown();

    EXPECT_EQ(expected.output, lstnr.output);
    EXPECT_EQ(eh.output, eh_expected.output);

    EXPECT_EQ(lstnr.terminal_errors, 0);
    EXPECT_GE(lstnr.nonterminal_errors, 1);
}

TEST_P(BaseParserTests, Reset_PropagateUpstream) {
    auto gram = make_grammar_1a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("abcabc"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);
    
    test_token_stream_for_reset upstream{};
    psr->bind_source(&upstream);

    ASSERT_FALSE(upstream.flag);

    psr->reset(); // should propagate

    ASSERT_TRUE(upstream.flag);
}

TEST_P(BaseParserTests, Reset_PipelineReUse) {
    auto gram = make_grammar_1a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("abcabc"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input0("abcad"_str);
    taul::lexer lxr0(gram.value());
    lxr0.bind_source(&input0);

    taul::source_reader input1("abcabc"_str);
    taul::lexer lxr1(gram.value());
    lxr1.bind_source(&input1);

    test_listener lstnr0{};
    test_listener lstnr1{};

    // the *previous* state of the pipeline

    psr->bind_source(&lxr0);
    psr->bind_listener(&lstnr0);

    psr->reset();

    auto result0 = psr->parse(gram->ppr("abcabc"_str).value());

    // the *current* state of the pipeline (ie. the one actually under test)

    psr->bind_source(&lxr1);
    psr->bind_listener(&lstnr1);

    psr->reset(); // <- reset is the method we're actually testing here

    auto result1 = psr->parse(gram->ppr("abcabc"_str).value());

    EXPECT_TRUE(input1.done());
    EXPECT_TRUE(lxr1.done());

    test_listener playback{};
    playback.playback(result1);

    test_listener expected{};
    expected.on_startup();
    expected.on_syntactic(gram->ppr("abcabc"_str).value(), 0);
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "A"_str, 3, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "B"_str, 4, 1));
    expected.on_lexical(taul::token::normal(gram.value(), "C"_str, 5, 1));
    expected.on_close();
    expected.on_shutdown();

    EXPECT_EQ(expected.output, lstnr1.output);
    EXPECT_EQ(expected.output, playback.output);
}

static std::optional<taul::grammar> make_grammar_1b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("abc1"_str)
        .ppr_decl("abc2"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("abc1"_str)
        .name("A"_str)
        .name("B"_str)
        .name("C"_str)
        .close()
        .ppr("abc2"_str)
        .name("A"_str)
        .name("B"_str)
        .name("C"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, AllowAnyPPRToBeTheStartRule) {
    auto gram = make_grammar_1b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("abc1"_str));
    ASSERT_TRUE(gram->has_ppr("abc2"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("abc"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    psr->reset();

    auto result0 = psr->parse(gram->ppr("abc1"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    psr->reset();

    auto result1 = psr->parse(gram->ppr("abc2"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("abc1"_str).value(), 0);
    expected0.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected0.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected0.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expected0.on_close();
    expected0.on_shutdown();
    
    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("abc2"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected1.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected1.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expected1.on_close();
    expected1.on_shutdown();

    test_listener actual0{};
    actual0.playback(result0);

    test_listener actual1{};
    actual1.playback(result1);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
}

static std::optional<taul::grammar> make_grammar_1c(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .name("A"_str)
        .name("B"_str)
        .name("C"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Parse_AllowMultipleRoundsOfParsingUsingSameStreamOfInputs) {
    auto gram = make_grammar_1c(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("abcabcabc"_str);
    taul::lexer lxr(gram.value());
    test_listener lstnr{};
    lxr.bind_source(&input);
    psr->bind_source(&lxr);
    psr->bind_listener(&lstnr);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());
    auto result1 = psr->parse(gram->ppr("f"_str).value());
    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expectedA{};
    expectedA.on_startup();
    expectedA.on_syntactic(gram->ppr("f"_str).value(), 0);
    expectedA.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expectedA.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expectedA.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expectedA.on_close();
    expectedA.on_shutdown();
    expectedA.on_startup();
    expectedA.on_syntactic(gram->ppr("f"_str).value(), 3);
    expectedA.on_lexical(taul::token::normal(gram.value(), "A"_str, 3, 1));
    expectedA.on_lexical(taul::token::normal(gram.value(), "B"_str, 4, 1));
    expectedA.on_lexical(taul::token::normal(gram.value(), "C"_str, 5, 1));
    expectedA.on_close();
    expectedA.on_shutdown();
    expectedA.on_startup();
    expectedA.on_syntactic(gram->ppr("f"_str).value(), 6);
    expectedA.on_lexical(taul::token::normal(gram.value(), "A"_str, 6, 1));
    expectedA.on_lexical(taul::token::normal(gram.value(), "B"_str, 7, 1));
    expectedA.on_lexical(taul::token::normal(gram.value(), "C"_str, 8, 1));
    expectedA.on_close();
    expectedA.on_shutdown();
    
    test_listener expectedB0{};
    expectedB0.on_startup();
    expectedB0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expectedB0.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expectedB0.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expectedB0.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expectedB0.on_close();
    expectedB0.on_shutdown();
    
    test_listener expectedB1{};
    expectedB1.on_startup();
    expectedB1.on_syntactic(gram->ppr("f"_str).value(), 3);
    expectedB1.on_lexical(taul::token::normal(gram.value(), "A"_str, 3, 1));
    expectedB1.on_lexical(taul::token::normal(gram.value(), "B"_str, 4, 1));
    expectedB1.on_lexical(taul::token::normal(gram.value(), "C"_str, 5, 1));
    expectedB1.on_close();
    expectedB1.on_shutdown();
    
    test_listener expectedB2{};
    expectedB2.on_startup();
    expectedB2.on_syntactic(gram->ppr("f"_str).value(), 6);
    expectedB2.on_lexical(taul::token::normal(gram.value(), "A"_str, 6, 1));
    expectedB2.on_lexical(taul::token::normal(gram.value(), "B"_str, 7, 1));
    expectedB2.on_lexical(taul::token::normal(gram.value(), "C"_str, 8, 1));
    expectedB2.on_close();
    expectedB2.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);

    EXPECT_EQ(expectedA.output, lstnr.output);

    EXPECT_EQ(expectedB0.output, actual0.output);
    EXPECT_EQ(expectedB1.output, actual1.output);
    EXPECT_EQ(expectedB2.output, actual2.output);
}

TEST_P(BaseParserTests, ParseNoTree_AllowMultipleRoundsOfParsingUsingSameStreamOfInputs) {
    auto gram = make_grammar_1c(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("abcabcabc"_str);
    taul::lexer lxr(gram.value());
    test_listener lstnr{};
    lxr.bind_source(&input);
    psr->bind_source(&lxr);
    psr->bind_listener(&lstnr);
    psr->reset();

    psr->parse_notree(gram->ppr("f"_str).value());
    psr->parse_notree(gram->ppr("f"_str).value());
    psr->parse_notree(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expectedA{};
    expectedA.on_startup();
    expectedA.on_syntactic(gram->ppr("f"_str).value(), 0);
    expectedA.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expectedA.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expectedA.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expectedA.on_close();
    expectedA.on_shutdown();
    expectedA.on_startup();
    expectedA.on_syntactic(gram->ppr("f"_str).value(), 3);
    expectedA.on_lexical(taul::token::normal(gram.value(), "A"_str, 3, 1));
    expectedA.on_lexical(taul::token::normal(gram.value(), "B"_str, 4, 1));
    expectedA.on_lexical(taul::token::normal(gram.value(), "C"_str, 5, 1));
    expectedA.on_close();
    expectedA.on_shutdown();
    expectedA.on_startup();
    expectedA.on_syntactic(gram->ppr("f"_str).value(), 6);
    expectedA.on_lexical(taul::token::normal(gram.value(), "A"_str, 6, 1));
    expectedA.on_lexical(taul::token::normal(gram.value(), "B"_str, 7, 1));
    expectedA.on_lexical(taul::token::normal(gram.value(), "C"_str, 8, 1));
    expectedA.on_close();
    expectedA.on_shutdown();

    EXPECT_EQ(expectedA.output, lstnr.output);
}


// these tests are for expected TAUL semantics


static_assert(taul::spec_opcodes == 21);


// for composite exprs, these tests will largely assume that so long
// as some basic nesting works, that it'll work in an arbitrary case


// ppr


static std::optional<taul::grammar> make_grammar_2a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .ppr("f"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, PPR_Empty) {
    auto gram = make_grammar_2a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("a"_str);
    psr->reset();
    
    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_close();
    expected0.on_shutdown();
    
    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_close();
    expected1.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    actual0.playback(result0);
    actual1.playback(result1);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
}

static std::optional<taul::grammar> make_grammar_2b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("a"_str)
        .close()
        .ppr("f"_str)
        .name("A"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, PPR_NonEmpty_WithNoAlts) {
    auto gram = make_grammar_2b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();
    auto result0 = psr->parse(gram->ppr("f"_str).value());

    input.change_input("b"_str);
    psr->reset();
    auto result1 = psr->parse(gram->ppr("f"_str).value());

    input.change_input(""_str);
    psr->reset();
    auto result2 = psr->parse(gram->ppr("f"_str).value());

    input.change_input("aa"_str);
    psr->reset();
    auto result3 = psr->parse(gram->ppr("f"_str).value());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected0.on_close();
    expected0.on_shutdown();
    
    test_listener expected1{};
    expected1.on_startup();
    expected1.on_abort();
    expected1.on_shutdown();
    
    test_listener expected2{};
    expected2.on_startup();
    expected2.on_abort();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
}

static std::optional<taul::grammar> make_grammar_2c(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .ppr("f"_str)
        .name("A"_str)
        .alternative()
        .name("B"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, PPR_NonEmpty_WithAlts_WithNoEmptyAlts) {
    auto gram = make_grammar_2c(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();
    auto result0 = psr->parse(gram->ppr("f"_str).value());

    input.change_input("b"_str);
    psr->reset();
    auto result1 = psr->parse(gram->ppr("f"_str).value());

    input.change_input(""_str);
    psr->reset();
    auto result2 = psr->parse(gram->ppr("f"_str).value());

    input.change_input("ab"_str);
    psr->reset();
    auto result3 = psr->parse(gram->ppr("f"_str).value());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected0.on_close();
    expected0.on_shutdown();
    
    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "B"_str, 0, 1));
    expected1.on_close();
    expected1.on_shutdown();
    
    test_listener expected2{};
    expected2.on_startup();
    expected2.on_abort();
    expected2.on_shutdown();
    
    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
}

static std::optional<taul::grammar> make_grammar_2d(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .ppr("f"_str)
        .name("A"_str)
        .alternative()
        .name("B"_str)
        .alternative()
        // empty alt
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, PPR_NonEmpty_WithAlts_WithEmptyAlts) {
    auto gram = make_grammar_2d(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("a"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());
    
    input.change_input(""_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());
    
    input.change_input("ab"_str);
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "B"_str, 0, 1));
    expected1.on_close();
    expected1.on_shutdown();
    
    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
}

static std::optional<taul::grammar> make_grammar_2e(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .name("A"_str)
        .sequence()
        .name("B"_str)
        .close()
        .name("C"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, PPR_Nesting) {
    auto gram = make_grammar_2e(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("abc"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("abca"_str);
    psr->reset();
    
    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());
    
    input.change_input("ab"_str);
    psr->reset();
    
    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());
    
    input.change_input(""_str);
    psr->reset();
    
    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected0.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected0.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expected0.on_close();
    expected0.on_shutdown();
    
    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected1.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected1.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expected1.on_close();
    expected1.on_shutdown();
    
    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected2.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected2.on_close();
    expected2.on_abort();
    expected2.on_shutdown();
    
    test_listener expected3{};
    expected3.on_startup();
    expected3.on_abort();
    expected3.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
}


// end


static std::optional<taul::grammar> make_grammar_3a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .name("A"_str)
        .name("B"_str)
        .name("C"_str)
        .end()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, End) {
    auto gram = make_grammar_3a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("abc"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("abca"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected0.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected0.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected1.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected1.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expected1.on_close();
    expected1.on_abort();
    expected1.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    actual0.playback(result0);
    actual1.playback(result1);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
}

static std::optional<taul::grammar> make_grammar_3b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .ppr("f"_str)
        .end()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, End_EndOnly) {
    auto gram = make_grammar_3b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input(""_str);
    psr->reset();
    auto result0 = psr->parse(gram->ppr("f"_str).value());

    input.change_input("a"_str);
    psr->reset();
    auto result1 = psr->parse(gram->ppr("f"_str).value());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_abort();
    expected1.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    actual0.playback(result0);
    actual1.playback(result1);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
}


// any


static std::optional<taul::grammar> make_grammar_4a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .any()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Any) {
    auto gram = make_grammar_4a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());
    
    input.change_input("c"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());
    
    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());
    
    input.change_input("aa"_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());
    
    input.change_input(""_str);
    psr->reset();

    auto result5 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected0.on_close();
    expected0.on_shutdown();
    
    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "B"_str, 0, 1));
    expected1.on_close();
    expected1.on_shutdown();
    
    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "C"_str, 0, 1));
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::failure(0, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected4.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected4.on_close();
    expected4.on_shutdown();
    
    test_listener expected5{};
    expected5.on_startup();
    expected5.on_abort();
    expected5.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    test_listener actual5{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);
    actual5.playback(result5);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
    EXPECT_EQ(expected5.output, actual5.output);
}


// token


static std::optional<taul::grammar> make_grammar_5a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .token()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Token) {
    auto gram = make_grammar_5a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("c"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aa"_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input(""_str);
    psr->reset();

    auto result5 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "B"_str, 0, 1));
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "C"_str, 0, 1));
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_abort();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected4.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected4.on_close();
    expected4.on_shutdown();

    test_listener expected5{};
    expected5.on_startup();
    expected5.on_abort();
    expected5.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    test_listener actual5{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);
    actual5.playback(result5);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
    EXPECT_EQ(expected5.output, actual5.output);
}


// failure


static std::optional<taul::grammar> make_grammar_6a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .failure()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Failure) {
    auto gram = make_grammar_6a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("c"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("da"_str); // <- test w/ failure followed by another token
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input(""_str);
    psr->reset();

    auto result5 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_abort();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_abort();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_abort();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::failure(0, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected4.on_lexical(taul::token::failure(0, 1));
    expected4.on_close();
    expected4.on_shutdown();

    test_listener expected5{};
    expected5.on_startup();
    expected5.on_abort();
    expected5.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    test_listener actual5{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);
    actual5.playback(result5);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
    EXPECT_EQ(expected5.output, actual5.output);
}


// name


static std::optional<taul::grammar> make_grammar_7a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .ppr("f"_str)
        .name("A"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Name_RefToLPR) {
    auto gram = make_grammar_7a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str); // <- test w/ failure token
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aa"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input(""_str);
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_abort();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_abort();
    expected3.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
}

static std::optional<taul::grammar> make_grammar_7b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .ppr_decl("other"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .ppr("other"_str)
        .name("A"_str)
        .close()
        .ppr("f"_str)
        .name("other"_str)
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Name_RefToPPR) {
    auto gram = make_grammar_7b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_ppr("other"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str); // <- test w/ failure token
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aa"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input(""_str);
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_syntactic(gram->ppr("other"_str).value(), 0);
    expected0.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected0.on_close();
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_abort();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_syntactic(gram->ppr("other"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected2.on_close();
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_abort();
    expected3.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
}

static std::optional<taul::grammar> make_grammar_7c(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .ppr("f"_str)
        .name("A"_str)
        .name("f"_str) // <- recursive self-reference
        .alternative()
        // empty alt
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Name_RefToPPR_Recursion) {
    auto gram = make_grammar_7c(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input(""_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("a"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aa"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aaa"_str);
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aaab"_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());
    
    input.change_input("b"_str);
    psr->reset();

    auto result5 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected1.on_syntactic(gram->ppr("f"_str).value(), 1);
    expected1.on_close();
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected2.on_syntactic(gram->ppr("f"_str).value(), 1);
    expected2.on_lexical(taul::token::normal(gram.value(), "A"_str, 1, 1));
    expected2.on_syntactic(gram->ppr("f"_str).value(), 2);
    expected2.on_close();
    expected2.on_close();
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected3.on_syntactic(gram->ppr("f"_str).value(), 1);
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 1, 1));
    expected3.on_syntactic(gram->ppr("f"_str).value(), 2);
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 2, 1));
    expected3.on_syntactic(gram->ppr("f"_str).value(), 3);
    expected3.on_close();
    expected3.on_close();
    expected3.on_close();
    expected3.on_close();
    expected3.on_shutdown();
    
    test_listener expected4{};
    expected4.on_startup();
    expected4.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected4.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected4.on_syntactic(gram->ppr("f"_str).value(), 1);
    expected4.on_lexical(taul::token::normal(gram.value(), "A"_str, 1, 1));
    expected4.on_syntactic(gram->ppr("f"_str).value(), 2);
    expected4.on_lexical(taul::token::normal(gram.value(), "A"_str, 2, 1));
    expected4.on_syntactic(gram->ppr("f"_str).value(), 3);
    expected4.on_close();
    expected4.on_close();
    expected4.on_close();
    expected4.on_close();
    expected4.on_shutdown();
    
    test_listener expected5{};
    expected5.on_startup();
    expected5.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected5.on_close();
    expected5.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    test_listener actual5{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);
    actual5.playback(result5);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
    EXPECT_EQ(expected5.output, actual5.output);
}


// sequence


static std::optional<taul::grammar> make_grammar_8a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .ppr("f"_str)
        .sequence()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Sequence_Empty) {
    auto gram = make_grammar_8a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("a"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_close();
    expected1.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    actual0.playback(result0);
    actual1.playback(result1);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
}

static std::optional<taul::grammar> make_grammar_8b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("a"_str)
        .close()
        .ppr("f"_str)
        .sequence()
        .name("A"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Sequence_NonEmpty_WithNoAlts) {
    auto gram = make_grammar_8b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("a"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input(""_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aa"_str);
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_abort();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_abort();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
}

static std::optional<taul::grammar> make_grammar_8c(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .ppr("f"_str)
        .sequence()
        .name("A"_str)
        .alternative()
        .name("B"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Sequence_NonEmpty_WithAlts_WithNoEmptyAlt) {
    auto gram = make_grammar_8c(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("a"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input(""_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("ab"_str);
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "B"_str, 0, 1));
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_abort();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
}

static std::optional<taul::grammar> make_grammar_8d(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .ppr("f"_str)
        .sequence()
        .name("A"_str)
        .alternative()
        .name("B"_str)
        .alternative()
        // empty alt
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Sequence_NonEmpty_WithAlts_WithEmptyAlt) {
    auto gram = make_grammar_8d(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input("a"_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input(""_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("ab"_str);
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "B"_str, 0, 1));
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
}

static std::optional<taul::grammar> make_grammar_8e(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .sequence()
        .name("A"_str)
        .sequence()
        .name("B"_str)
        .close()
        .name("C"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Sequence_Nesting) {
    auto gram = make_grammar_8e(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("abc"_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("abca"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("ab"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input(""_str);
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected0.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected0.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected1.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected1.on_lexical(taul::token::normal(gram.value(), "C"_str, 2, 1));
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected2.on_lexical(taul::token::normal(gram.value(), "B"_str, 1, 1));
    expected2.on_close();
    expected2.on_abort();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_abort();
    expected3.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
}


// lookahead


static std::optional<taul::grammar> make_grammar_9a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .lookahead()
        .name("A"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, LookAhead_NonEmpty_WithNoAlts) {
    auto gram = make_grammar_9a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("c"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());
    
    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());
    
    input.change_input(""_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_abort();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_abort();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_abort();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_abort();
    expected4.on_shutdown();
    
    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
}

static std::optional<taul::grammar> make_grammar_9b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .lookahead()
        .name("A"_str)
        .alternative()
        .name("B"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, LookAhead_NonEmpty_WithAlts) {
    auto gram = make_grammar_9b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("c"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input(""_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_abort();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_abort();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_abort();
    expected4.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
}

static std::optional<taul::grammar> make_grammar_9c(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .lookahead()
        .name("A"_str)
        .alternative()
        .failure()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, LookAhead_WithFailureAlt) {
    auto gram = make_grammar_9c(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("c"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input(""_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_abort();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_abort();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_close();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_abort();
    expected4.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
}

static std::optional<taul::grammar> make_grammar_9d(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .lookahead()
        .sequence()
        .name("A"_str)
        .alternative()
        .name("B"_str)
        .close()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, LookAhead_Nesting) {
    auto gram = make_grammar_9d(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("c"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input(""_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_abort();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_abort();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_abort();
    expected4.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
}


// lookahead-not


static std::optional<taul::grammar> make_grammar_10a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .lookahead_not()
        .name("A"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, LookAheadNot_NonEmpty_WithNoAlts) {
    auto gram = make_grammar_10a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("c"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input(""_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_abort();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_close();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_abort();
    expected4.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
}

static std::optional<taul::grammar> make_grammar_10b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .lookahead_not()
        .name("A"_str)
        .alternative()
        .name("B"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, LookAheadNot_NonEmpty_WithAlts) {
    auto gram = make_grammar_10b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();
    auto result0 = psr->parse(gram->ppr("f"_str).value());

    input.change_input("b"_str);
    psr->reset();
    auto result1 = psr->parse(gram->ppr("f"_str).value());

    input.change_input("c"_str);
    psr->reset();
    auto result2 = psr->parse(gram->ppr("f"_str).value());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();
    auto result3 = psr->parse(gram->ppr("f"_str).value());

    input.change_input(""_str);
    psr->reset();
    auto result4 = psr->parse(gram->ppr("f"_str).value());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_abort();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_abort();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_close();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_abort();
    expected4.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
}

static std::optional<taul::grammar> make_grammar_10c(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .lookahead_not()
        .name("A"_str)
        .alternative()
        .failure()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, LookAheadNot_WithFailureAlt) {
    auto gram = make_grammar_10c(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();
    auto result0 = psr->parse(gram->ppr("f"_str).value());

    input.change_input("b"_str);
    psr->reset();
    auto result1 = psr->parse(gram->ppr("f"_str).value());

    input.change_input("c"_str);
    psr->reset();
    auto result2 = psr->parse(gram->ppr("f"_str).value());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();
    auto result3 = psr->parse(gram->ppr("f"_str).value());

    input.change_input(""_str);
    psr->reset();
    auto result4 = psr->parse(gram->ppr("f"_str).value());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_abort();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_abort();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_abort();
    expected4.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
}

static std::optional<taul::grammar> make_grammar_10d(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .lookahead_not()
        .sequence()
        .name("A"_str)
        .alternative()
        .name("B"_str)
        .close()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, LookAheadNot_Nesting) {
    auto gram = make_grammar_10d(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();
    auto result0 = psr->parse(gram->ppr("f"_str).value());

    input.change_input("b"_str);
    psr->reset();
    auto result1 = psr->parse(gram->ppr("f"_str).value());

    input.change_input("c"_str);
    psr->reset();
    auto result2 = psr->parse(gram->ppr("f"_str).value());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();
    auto result3 = psr->parse(gram->ppr("f"_str).value());

    input.change_input(""_str);
    psr->reset();
    auto result4 = psr->parse(gram->ppr("f"_str).value());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_abort();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_abort();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_close();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_abort();
    expected4.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
}


// not


static std::optional<taul::grammar> make_grammar_11a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .not0()
        .name("A"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Not_NonEmpty_WithNoAlts) {
    auto gram = make_grammar_11a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("c"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input(""_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_abort();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "B"_str, 0, 1));
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "C"_str, 0, 1));
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::failure(0, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_abort();
    expected4.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
}

static std::optional<taul::grammar> make_grammar_11b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .not0()
        .name("A"_str)
        .alternative()
        .name("B"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Not_NonEmpty_WithAlts) {
    auto gram = make_grammar_11b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("c"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input(""_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_abort();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_abort();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "C"_str, 0, 1));
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::failure(0, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_abort();
    expected4.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
}

static std::optional<taul::grammar> make_grammar_11c(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .not0()
        .name("A"_str)
        .alternative()
        .failure()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Not_WithFailureAlt) {
    auto gram = make_grammar_11c(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("c"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input(""_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_abort();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "B"_str, 0, 1));
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "C"_str, 0, 1));
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_abort();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_abort();
    expected4.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
}

static std::optional<taul::grammar> make_grammar_11d(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .not0()
        .sequence()
        .name("A"_str)
        .alternative()
        .name("B"_str)
        .close()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Not_Nesting) {
    auto gram = make_grammar_11d(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input("a"_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("c"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input(""_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_abort();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_abort();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "C"_str, 0, 1));
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::failure(0, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_abort();
    expected4.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
}


// optional


static std::optional<taul::grammar> make_grammar_12a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .optional()
        .name("A"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Optional) {
    auto gram = make_grammar_12a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input(""_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("a"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aa"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());
    
    input.change_input("aaa"_str);
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());
    
    input.change_input("b"_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("ab"_str);
    psr->reset();

    auto result5 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result6 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());
    
    input.change_input("ad"_str); // <- test w/ failure token
    psr->reset();

    auto result7 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected4.on_close();
    expected4.on_shutdown();

    test_listener expected5{};
    expected5.on_startup();
    expected5.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected5.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected5.on_close();
    expected5.on_shutdown();
    
    test_listener expected6{};
    expected6.on_startup();
    expected6.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected6.on_close();
    expected6.on_shutdown();

    test_listener expected7{};
    expected7.on_startup();
    expected7.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected7.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected7.on_close();
    expected7.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    test_listener actual5{};
    test_listener actual6{};
    test_listener actual7{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);
    actual5.playback(result5);
    actual6.playback(result6);
    actual7.playback(result7);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
    EXPECT_EQ(expected5.output, actual5.output);
    EXPECT_EQ(expected6.output, actual6.output);
    EXPECT_EQ(expected7.output, actual7.output);
}

static std::optional<taul::grammar> make_grammar_12b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .optional()
        .sequence()
        .name("A"_str)
        .close()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, Optional_Nesting) {
    auto gram = make_grammar_12b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input(""_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("a"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aa"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aaa"_str);
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("ab"_str);
    psr->reset();

    auto result5 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result6 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("ad"_str); // <- test w/ failure token
    psr->reset();

    auto result7 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected4.on_close();
    expected4.on_shutdown();

    test_listener expected5{};
    expected5.on_startup();
    expected5.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected5.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected5.on_close();
    expected5.on_shutdown();

    test_listener expected6{};
    expected6.on_startup();
    expected6.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected6.on_close();
    expected6.on_shutdown();

    test_listener expected7{};
    expected7.on_startup();
    expected7.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected7.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected7.on_close();
    expected7.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    test_listener actual5{};
    test_listener actual6{};
    test_listener actual7{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);
    actual5.playback(result5);
    actual6.playback(result6);
    actual7.playback(result7);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
    EXPECT_EQ(expected5.output, actual5.output);
    EXPECT_EQ(expected6.output, actual6.output);
    EXPECT_EQ(expected7.output, actual7.output);
}


// kleene-star


static std::optional<taul::grammar> make_grammar_13a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .kleene_star()
        .name("A"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, KleeneStar) {
    auto gram = make_grammar_13a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input(""_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("a"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aa"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aaa"_str);
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("ab"_str);
    psr->reset();

    auto result5 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result6 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("ad"_str); // <- test w/ failure token
    psr->reset();

    auto result7 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected2.on_lexical(taul::token::normal(gram.value(), "A"_str, 1, 1));
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 1, 1));
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 2, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected4.on_close();
    expected4.on_shutdown();

    test_listener expected5{};
    expected5.on_startup();
    expected5.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected5.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected5.on_close();
    expected5.on_shutdown();

    test_listener expected6{};
    expected6.on_startup();
    expected6.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected6.on_close();
    expected6.on_shutdown();

    test_listener expected7{};
    expected7.on_startup();
    expected7.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected7.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected7.on_close();
    expected7.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    test_listener actual5{};
    test_listener actual6{};
    test_listener actual7{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);
    actual5.playback(result5);
    actual6.playback(result6);
    actual7.playback(result7);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
    EXPECT_EQ(expected5.output, actual5.output);
    EXPECT_EQ(expected6.output, actual6.output);
    EXPECT_EQ(expected7.output, actual7.output);
}

static std::optional<taul::grammar> make_grammar_13b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .kleene_star()
        .sequence()
        .name("A"_str)
        .close()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, KleeneStar_Nesting) {
    auto gram = make_grammar_13b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input(""_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("a"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aa"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aaa"_str);
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("ab"_str);
    psr->reset();

    auto result5 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result6 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("ad"_str); // <- test w/ failure token
    psr->reset();

    auto result7 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected0.on_close();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected2.on_lexical(taul::token::normal(gram.value(), "A"_str, 1, 1));
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 1, 1));
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 2, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected4.on_close();
    expected4.on_shutdown();

    test_listener expected5{};
    expected5.on_startup();
    expected5.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected5.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected5.on_close();
    expected5.on_shutdown();

    test_listener expected6{};
    expected6.on_startup();
    expected6.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected6.on_close();
    expected6.on_shutdown();

    test_listener expected7{};
    expected7.on_startup();
    expected7.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected7.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected7.on_close();
    expected7.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    test_listener actual5{};
    test_listener actual6{};
    test_listener actual7{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);
    actual5.playback(result5);
    actual6.playback(result6);
    actual7.playback(result7);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
    EXPECT_EQ(expected5.output, actual5.output);
    EXPECT_EQ(expected6.output, actual6.output);
    EXPECT_EQ(expected7.output, actual7.output);
}


// kleene-plus


static std::optional<taul::grammar> make_grammar_14a(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .kleene_plus()
        .name("A"_str)
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, KleenePlus) {
    auto gram = make_grammar_14a(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input(""_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("a"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aa"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aaa"_str);
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("ab"_str);
    psr->reset();

    auto result5 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result6 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("ad"_str); // <- test w/ failure token
    psr->reset();

    auto result7 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_abort();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected2.on_lexical(taul::token::normal(gram.value(), "A"_str, 1, 1));
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 1, 1));
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 2, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_abort();
    expected4.on_shutdown();

    test_listener expected5{};
    expected5.on_startup();
    expected5.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected5.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected5.on_close();
    expected5.on_shutdown();

    test_listener expected6{};
    expected6.on_startup();
    expected6.on_abort();
    expected6.on_shutdown();

    test_listener expected7{};
    expected7.on_startup();
    expected7.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected7.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected7.on_close();
    expected7.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    test_listener actual5{};
    test_listener actual6{};
    test_listener actual7{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);
    actual5.playback(result5);
    actual6.playback(result6);
    actual7.playback(result7);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
    EXPECT_EQ(expected5.output, actual5.output);
    EXPECT_EQ(expected6.output, actual6.output);
    EXPECT_EQ(expected7.output, actual7.output);
}

static std::optional<taul::grammar> make_grammar_14b(std::shared_ptr<taul::logger> lgr) {
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .ppr_decl("f"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .kleene_plus()
        .sequence()
        .name("A"_str)
        .close()
        .close()
        .close()
        .done();
    return taul::load(spec, lgr);
}

TEST_P(BaseParserTests, KleenePlus_Nesting) {
    auto gram = make_grammar_14b(lgr);
    //if (gram) TAUL_LOG(lgr, "{}", gram->fmt_internals());
    ASSERT_TRUE(gram);
    ASSERT_TRUE(gram->has_lpr("A"_str));
    ASSERT_TRUE(gram->has_lpr("B"_str));
    ASSERT_TRUE(gram->has_lpr("C"_str));
    ASSERT_TRUE(gram->has_ppr("f"_str));

    auto psr = GetParam().factory(gram.value(), lgr);
    ASSERT_TRUE(psr);

    taul::source_reader input(""_str);
    taul::lexer lxr(gram.value());
    lxr.bind_source(&input);
    psr->bind_source(&lxr);

    input.change_input(""_str);
    psr->reset();

    auto result0 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("a"_str);
    psr->reset();

    auto result1 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aa"_str);
    psr->reset();

    auto result2 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("aaa"_str);
    psr->reset();

    auto result3 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("b"_str);
    psr->reset();

    auto result4 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("ab"_str);
    psr->reset();

    auto result5 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("d"_str); // <- test w/ failure token
    psr->reset();

    auto result6 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    input.change_input("ad"_str); // <- test w/ failure token
    psr->reset();

    auto result7 = psr->parse(gram->ppr("f"_str).value());

    //EXPECT_TRUE(input.done());
    //EXPECT_TRUE(lxr.done());

    test_listener expected0{};
    expected0.on_startup();
    expected0.on_abort();
    expected0.on_shutdown();

    test_listener expected1{};
    expected1.on_startup();
    expected1.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected1.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected1.on_close();
    expected1.on_shutdown();

    test_listener expected2{};
    expected2.on_startup();
    expected2.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected2.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected2.on_lexical(taul::token::normal(gram.value(), "A"_str, 1, 1));
    expected2.on_close();
    expected2.on_shutdown();

    test_listener expected3{};
    expected3.on_startup();
    expected3.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 1, 1));
    expected3.on_lexical(taul::token::normal(gram.value(), "A"_str, 2, 1));
    expected3.on_close();
    expected3.on_shutdown();

    test_listener expected4{};
    expected4.on_startup();
    expected4.on_abort();
    expected4.on_shutdown();

    test_listener expected5{};
    expected5.on_startup();
    expected5.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected5.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected5.on_close();
    expected5.on_shutdown();

    test_listener expected6{};
    expected6.on_startup();
    expected6.on_abort();
    expected6.on_shutdown();

    test_listener expected7{};
    expected7.on_startup();
    expected7.on_syntactic(gram->ppr("f"_str).value(), 0);
    expected7.on_lexical(taul::token::normal(gram.value(), "A"_str, 0, 1));
    expected7.on_close();
    expected7.on_shutdown();

    test_listener actual0{};
    test_listener actual1{};
    test_listener actual2{};
    test_listener actual3{};
    test_listener actual4{};
    test_listener actual5{};
    test_listener actual6{};
    test_listener actual7{};
    actual0.playback(result0);
    actual1.playback(result1);
    actual2.playback(result2);
    actual3.playback(result3);
    actual4.playback(result4);
    actual5.playback(result5);
    actual6.playback(result6);
    actual7.playback(result7);

    EXPECT_EQ(expected0.output, actual0.output);
    EXPECT_EQ(expected1.output, actual1.output);
    EXPECT_EQ(expected2.output, actual2.output);
    EXPECT_EQ(expected3.output, actual3.output);
    EXPECT_EQ(expected4.output, actual4.output);
    EXPECT_EQ(expected5.output, actual5.output);
    EXPECT_EQ(expected6.output, actual6.output);
    EXPECT_EQ(expected7.output, actual7.output);
}

