

#include "error_handler_tests.h"

#include <taul/spec.h>
#include <taul/source_reader.h>
#include <taul/lexer.h>
#include <taul/parser.h>
#include <taul/load.h>


using namespace taul::string_literals;


GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(ErrorHandlerTests);


TEST_P(ErrorHandlerTests, RecoverableError_ArisingFromTerminal) {
    ASSERT_TRUE(gram);
    taul::source_reader rdr(""_str);
    taul::lexer lxr(gram.value());
    taul::parser psr(gram.value());
    auto eh = GetParam().factory(gram.value(), lgr);
    rdr.change_input("abca_bc"_str);
    lxr.bind_source(&rdr);
    psr.bind_source(&lxr);
    psr.bind_error_handler(eh.get());
    psr.reset();

    taul::parse_tree expected(gram.value());

    if (GetParam().can_recover) {
        expected
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
    }
    else {
        expected
            .syntactic("ABCABC"_str, 0)
            .syntactic("ABC"_str, 0)
            .lexical("A"_str, 0, 1)
            .lexical("B"_str, 1, 1)
            .lexical("C"_str, 2, 1)
            .close()
            .syntactic("ABC"_str, 3)
            .lexical("A"_str, 3, 1)
            .close()
            .close()
            .abort();
    }

    auto actual = psr.parse("ABCABC"_str);

    ASSERT_EQ(expected.fmt(), actual.fmt());
    ASSERT_EQ(actual.is_aborted(), !GetParam().can_recover);
}

TEST_P(ErrorHandlerTests, RecoverableError_ArisingFromNonTerminal_NonRoot) {
    ASSERT_TRUE(gram);
    taul::source_reader rdr(""_str);
    taul::lexer lxr(gram.value());
    taul::parser psr(gram.value());
    auto eh = GetParam().factory(gram.value(), lgr);
    rdr.change_input("abc_abc"_str);
    lxr.bind_source(&rdr);
    psr.bind_source(&lxr);
    psr.bind_error_handler(eh.get());
    psr.reset();

    taul::parse_tree expected(gram.value());

    if (GetParam().can_recover) {
        expected
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
    }
    else {
        expected
            .syntactic("ABCABC"_str, 0)
            .syntactic("ABC"_str, 0)
            .lexical("A"_str, 0, 1)
            .lexical("B"_str, 1, 1)
            .lexical("C"_str, 2, 1)
            .close()
            .close()
            .abort();
    }

    auto actual = psr.parse("ABCABC"_str);

    ASSERT_EQ(expected.fmt(), actual.fmt());
    ASSERT_EQ(actual.is_aborted(), !GetParam().can_recover);
}

TEST_P(ErrorHandlerTests, RecoverableError_ArisingFromNonTerminal_Root) {
    ASSERT_TRUE(gram);
    taul::source_reader rdr(""_str);
    taul::lexer lxr(gram.value());
    taul::parser psr(gram.value());
    auto eh = GetParam().factory(gram.value(), lgr);
    rdr.change_input("_abcabc"_str);
    lxr.bind_source(&rdr);
    psr.bind_source(&lxr);
    psr.bind_error_handler(eh.get());
    psr.reset();

    taul::parse_tree expected(gram.value());

    if (GetParam().can_recover) {
        expected
            // error at 0
            .syntactic("ABCABC"_str, 1)
            .syntactic("ABC"_str, 1)
            .lexical("A"_str, 1, 1)
            .lexical("B"_str, 2, 1)
            .lexical("C"_str, 3, 1)
            .close()
            .syntactic("ABC"_str, 4)
            .lexical("A"_str, 4, 1)
            .lexical("B"_str, 5, 1)
            .lexical("C"_str, 6, 1)
            .close()
            .close();
    }
    else {
        expected
            .abort();
    }

    auto actual = psr.parse("ABCABC"_str);

    ASSERT_EQ(expected.fmt(), actual.fmt());
    ASSERT_EQ(actual.is_aborted(), !GetParam().can_recover);
}

TEST_P(ErrorHandlerTests, UnrecoverableError_ArisingFromTerminal) {
    ASSERT_TRUE(gram);
    taul::source_reader rdr(""_str);
    taul::lexer lxr(gram.value());
    taul::parser psr(gram.value());
    auto eh = GetParam().factory(gram.value(), lgr);
    rdr.change_input("abca___"_str);
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
        .close()
        .close()
        .abort();

    auto actual = psr.parse("ABCABC"_str);

    ASSERT_EQ(expected.fmt(), actual.fmt());
    ASSERT_TRUE(actual.is_aborted());
}

TEST_P(ErrorHandlerTests, UnrecoverableError_ArisingFromNonTerminal_NonRoot) {
    ASSERT_TRUE(gram);
    taul::source_reader rdr(""_str);
    taul::lexer lxr(gram.value());
    taul::parser psr(gram.value());
    auto eh = GetParam().factory(gram.value(), lgr);
    rdr.change_input("abc____"_str);
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
        .close()
        .abort();

    auto actual = psr.parse("ABCABC"_str);

    ASSERT_EQ(expected.fmt(), actual.fmt());
    ASSERT_TRUE(actual.is_aborted());
}

TEST_P(ErrorHandlerTests, UnrecoverableError_ArisingFromNonTerminal_Root) {
    ASSERT_TRUE(gram);
    taul::source_reader rdr(""_str);
    taul::lexer lxr(gram.value());
    taul::parser psr(gram.value());
    auto eh = GetParam().factory(gram.value(), lgr);
    rdr.change_input("_______"_str);
    lxr.bind_source(&rdr);
    psr.bind_source(&lxr);
    psr.bind_error_handler(eh.get());
    psr.reset();

    auto expected =
        taul::parse_tree(gram.value())
        .abort();

    auto actual = psr.parse("ABCABC"_str);

    ASSERT_EQ(expected.fmt(), actual.fmt());
    ASSERT_TRUE(actual.is_aborted());
}

