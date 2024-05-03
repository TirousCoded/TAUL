

#include "error_handler_tests.h"

#include <taul/spec.h>
#include <taul/string_reader.h>
#include <taul/lexer.h>
#include <taul/parser.h>
#include <taul/load.h>


using namespace taul::string_literals;


GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(ErrorHandlerTests);


TEST_P(ErrorHandlerTests, RecoverableError_ArisingFromTerminal) {
    auto lgr = taul::make_stderr_logger();
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("ABC"_str)
        .name("A"_str)
        .name("B"_str)
        .name("C"_str)
        .close()
        .ppr("ABCABC"_str)
        .name("ABC"_str)
        .name("ABC"_str)
        .close()
        .done();
    auto gram = taul::load(spec, lgr);
    ASSERT_TRUE(gram);
    taul::string_reader rdr("abca_bc"_str);
    taul::lexer lxr(gram.value());
    taul::parser psr(gram.value());
    auto eh = GetParam().factory(gram.value(), lgr);
    lxr.bind_source(&rdr);
    psr.bind_source(&lxr);
    psr.bind_error_handler(eh.get());
    psr.reset();

    auto expected =
        taul::parse_tree(gram.value())
        .syntactic("ABCABC"_str, 0)
        .syntactic("ABC"_str, 0)
        .lexical("A"_str, 0, 1)
        .lexical("B"_str, 1, 1)
        .lexical("C"_str, 2, 1)
        .close()
        .syntactic("ABC"_str, 3)
        .lexical("A"_str, 3, 1)
        // error at 4
        .lexical("B"_str, 5, 1)
        .lexical("C"_str, 6, 1)
        .close()
        .close();

    auto actual = psr.parse("ABCABC"_str);

    ASSERT_EQ(expected.fmt(), actual.fmt());
    ASSERT_FALSE(actual.contains_abort());
}

TEST_P(ErrorHandlerTests, RecoverableError_ArisingFromNonTerminal) {
    auto lgr = taul::make_stderr_logger();
    auto spec =
        taul::spec_writer()
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("C"_str)
        .lpr("A"_str)
        .string("a"_str)
        .close()
        .lpr("B"_str)
        .string("b"_str)
        .close()
        .lpr("C"_str)
        .string("c"_str)
        .close()
        .ppr("ABC"_str)
        .name("A"_str)
        .name("B"_str)
        .name("C"_str)
        .close()
        .ppr("ABCABC"_str)
        .name("ABC"_str)
        .name("ABC"_str)
        .close()
        .done();
    auto gram = taul::load(spec, lgr);
    ASSERT_TRUE(gram);
    taul::string_reader rdr("abc_abc"_str);
    taul::lexer lxr(gram.value());
    taul::parser psr(gram.value());
    auto eh = GetParam().factory(gram.value(), lgr);
    lxr.bind_source(&rdr);
    psr.bind_source(&lxr);
    psr.bind_error_handler(eh.get());
    psr.reset();

    auto expected =
        taul::parse_tree(gram.value())
        .syntactic("ABCABC"_str, 0)
        .syntactic("ABC"_str, 0)
        .lexical("A"_str, 0, 1)
        .lexical("B"_str, 1, 1)
        .lexical("C"_str, 2, 1)
        .close()
        // error at 3
        .syntactic("ABC"_str, 4)
        .lexical("A"_str, 4, 1)
        .lexical("B"_str, 5, 1)
        .lexical("C"_str, 6, 1)
        .close()
        .close();

    auto actual = psr.parse("ABCABC"_str);

    ASSERT_EQ(expected.fmt(), actual.fmt());
    ASSERT_FALSE(actual.contains_abort());
}

TEST_P(ErrorHandlerTests, UnrecoverableError_ArisingFromTerminal) {
    //
}

TEST_P(ErrorHandlerTests, UnrecoverableError_ArisingFromNonTerminal) {
    //
}

