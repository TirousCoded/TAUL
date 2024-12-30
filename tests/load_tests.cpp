

#include <gtest/gtest.h>

#include <taul/logger.h>
#include <taul/str.h>
#include <taul/string_and_charset.h>
#include <taul/spec_error.h>
#include <taul/spec_opcode.h>
#include <taul/spec.h>
#include <taul/symbol_set.h>
#include <taul/grammar.h>
#include <taul/load.h>


using namespace taul::string_literals;


// we'll have a few *summary* tests for successful usage of spec instructions,
// and a number of tests for each of the different errors which could arise
// for a particular instruction

// for failure tests, the implementation is free to decide when evaluation 
// should be aborted such that later errors do not arise, w/ each failure unit 
// test thus only asserting a single instance of an error arising

// these test will NOT test usage of loaded grammar lexers/parser, only that 
// they load successful/unsuccessfully as expected (ie. these tests do not
// assert things like parse table state)


// notice that as of now we have three major 'sections' to our test suite:
//      1) tests for successful loading
//      2) tests for FIRST/FOLLOW/prefix sets being as expected
//      3) tests for failed loading

// the test suite is comprehensive, though I do feel that our tests are still
// fairly *messy*, w/ this being be no means a proper 'specification' of the
// TAUL language, in that there are probably a bunch of holes in it w/ regards
// to semantic details it may fail to cover

// furthermore, the test suite in general is also quite messy regarding things
// like naming conventions, and I also feel that the way we've organized our
// tests is suboptimal, though I'm not sure exactly how I'd like to address this


// this tests that an empty spec loads an empty grammar successfully

TEST(LoadTests, success_withEmptyGrammarSpec) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);

        EXPECT_EQ(gram->lprs(), 0);
        EXPECT_EQ(gram->pprs(), 0);
    }
    else ADD_FAILURE();
}

// test empty LPRs w/ qualifiers

TEST(LoadTests, success_empty_noneQualifier_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str, taul::qualifier::none)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_empty_skipQualifier_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str, taul::qualifier::skip)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::skip);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_empty_supportQualifier_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str, taul::qualifier::support)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::support);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// test empty PPRs w/ qualifiers

TEST(LoadTests, success_empty_noneQualifier_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("a"_str)
        .ppr("a"_str, taul::qualifier::none)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_empty_precedenceQualifier_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("a"_str)
        .ppr("a"_str, taul::qualifier::precedence)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// test having multiple empty LPRs and PPRs

TEST(LoadTests, success_empty_multipleLPRsAndPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .ppr_decl("c"_str)
        .ppr_decl("d"_str)
        .lpr("a"_str)
        .close()
        .lpr("b"_str)
        .close()
        .ppr("c"_str)
        .close()
        .ppr("d"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        EXPECT_EQ(gram->lprs(), 2);
        EXPECT_EQ(gram->pprs(), 2);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("b"_str)) {
            EXPECT_EQ(gram->lpr("b"_str)->name(), "b"_str);
            EXPECT_EQ(gram->lpr("b"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("b"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("c"_str)) {
            EXPECT_EQ(gram->ppr("c"_str)->name(), "c"_str);
            EXPECT_EQ(gram->ppr("c"_str)->index(), 0);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("d"_str)) {
            EXPECT_EQ(gram->ppr("d"_str)->name(), "d"_str);
            EXPECT_EQ(gram->ppr("d"_str)->index(), 1);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// these tests test that each expr works as expected, under *basic*
// conditions, w/ things like nesting being assumed to work if these
// basic usages, and error usages, behaviour as expected

static_assert(taul::spec_opcodes == 21); // we only care about expr instructions

// end

TEST(LoadTests, success_end_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .end()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_end_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("a"_str)
        .ppr("a"_str)
        .end()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// any

TEST(LoadTests, success_any_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .any()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_any_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("a"_str)
        .ppr("a"_str)
        .any()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// string

TEST(LoadTests, success_string) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .string("abc"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_string_allowEscapeSeqsInSingleTerminalScope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .lookahead() // lookahead encapsulates single-terminal scope
        .string("\\a"_str) // should allow this
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_string_allowNonASCIIUnicodeIncludingInEscapeSeqs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .string(taul::utf8_s(u8"abc123Δ魂💩"))
        .string(taul::utf8_s(u8"\\Δ\\魂\\💩")) // legal to escape non-ASCII Unicode
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// charset

TEST(LoadTests, success_charset) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .charset("abc"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_charset_allowNonASCIIUnicodeIncludingInEscapeSeqs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .charset(taul::utf8_s(u8"a-fα-δ魂か-く"))
        .charset(taul::utf8_s(u8"\\α-\\δ\\魂\\か-\\く"))
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// token

TEST(LoadTests, success_token) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("a"_str)
        .ppr("a"_str)
        .token()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// failure

TEST(LoadTests, success_failure) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("a"_str)
        .ppr("a"_str)
        .failure()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// name

TEST(LoadTests, success_name_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("lpr"_str)
        .lpr("a"_str)
        .name("lpr"_str)
        .close()
        .lpr("lpr"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr"_str)) {
            EXPECT_EQ(gram->lpr("lpr"_str)->name(), "lpr"_str);
            EXPECT_EQ(gram->lpr("lpr"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("lpr"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_name_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr"_str)
        .ppr_decl("a"_str)
        .ppr_decl("ppr"_str)
        .lpr("lpr"_str)
        .close()
        .ppr("a"_str)
        .name("lpr"_str) // ref to LPR
        .name("ppr"_str) // ref to PPR
        .close()
        .ppr("ppr"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("lpr"_str)) {
            EXPECT_EQ(gram->lpr("lpr"_str)->name(), "lpr"_str);
            EXPECT_EQ(gram->lpr("lpr"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("lpr"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("ppr"_str)) {
            EXPECT_EQ(gram->ppr("ppr"_str)->name(), "ppr"_str);
            EXPECT_EQ(gram->ppr("ppr"_str)->index(), 1);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// sequence

TEST(LoadTests, success_sequence_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str) // empty
        .lpr_decl("b"_str) // w/out alts
        .lpr_decl("c"_str) // w/ alts
        .lpr("a"_str)
        .sequence()
        .close()
        .close()
        .lpr("b"_str)
        .sequence()
        .string("abc"_str)
        .any()
        .close()
        .close()
        .lpr("c"_str)
        .sequence()
        .string("abc"_str)
        .any()
        .alternative()
        .string("def"_str)
        .any()
        .alternative()
        .string("ghi"_str)
        .any()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("b"_str)) {
            EXPECT_EQ(gram->lpr("b"_str)->name(), "b"_str);
            EXPECT_EQ(gram->lpr("b"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("b"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("c"_str)) {
            EXPECT_EQ(gram->lpr("c"_str)->name(), "c"_str);
            EXPECT_EQ(gram->lpr("c"_str)->index(), 2);
            EXPECT_EQ(gram->lpr("c"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_sequence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr_decl("lpr2"_str)
        .ppr_decl("a"_str) // empty
        .ppr_decl("b"_str) // w/out alts
        .ppr_decl("c"_str) // w/ alts
        .lpr("lpr0"_str)
        .string("abc"_str)
        .close()
        .lpr("lpr1"_str)
        .string("def"_str)
        .close()
        .lpr("lpr2"_str)
        .string("ghi"_str)
        .close()
        .ppr("a"_str)
        .sequence()
        .close()
        .close()
        .ppr("b"_str)
        .sequence()
        .name("lpr0"_str)
        .any()
        .close()
        .close()
        .ppr("c"_str)
        .sequence()
        .name("lpr0"_str)
        .any()
        .alternative()
        .name("lpr1"_str)
        .any()
        .alternative()
        .name("lpr2"_str)
        .any()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("b"_str)) {
            EXPECT_EQ(gram->ppr("b"_str)->name(), "b"_str);
            EXPECT_EQ(gram->ppr("b"_str)->index(), 1);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("c"_str)) {
            EXPECT_EQ(gram->ppr("c"_str)->name(), "c"_str);
            EXPECT_EQ(gram->ppr("c"_str)->index(), 2);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// lookahead

TEST(LoadTests, success_lookahead_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str) // w/out alts
        .lpr_decl("b"_str) // w/ alts
        .lpr("a"_str)
        .lookahead()
        .charset("abc")
        .close()
        .close()
        .lpr("b"_str)
        .lookahead()
        .charset("abc")
        .alternative()
        .charset("def")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("b"_str)) {
            EXPECT_EQ(gram->lpr("b"_str)->name(), "b"_str);
            EXPECT_EQ(gram->lpr("b"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("b"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_lookahead_allowCertainStringSubexprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str) // w/out alts
        .lpr_decl("b"_str) // w/ alts
        .lpr("a"_str)
        .lookahead()
        .string("a"_str)
        .close()
        .close()
        .lpr("b"_str)
        .lookahead()
        .string("a"_str)
        .alternative()
        .string("b"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("b"_str)) {
            EXPECT_EQ(gram->lpr("b"_str)->name(), "b"_str);
            EXPECT_EQ(gram->lpr("b"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("b"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_lookahead_allowCertainSequenceSubexprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .lookahead()
        .sequence()
        .charset("abc"_str)
        .alternative()
        .charset("def"_str)
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_lookahead_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr_decl("lpr2")
        .ppr_decl("a"_str) // w/out alts
        .ppr_decl("b"_str) // w/ alts
        .lpr("lpr0")
        .string("a"_str)
        .close()
        .lpr("lpr1")
        .string("b"_str)
        .close()
        .lpr("lpr2")
        .string("c"_str)
        .close()
        .ppr("a"_str)
        .lookahead()
        .name("lpr0")
        .close()
        .close()
        .ppr("b"_str)
        .lookahead()
        .name("lpr0")
        .alternative()
        .name("lpr1")
        .alternative()
        .name("lpr2")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("lpr0"_str)) {
            EXPECT_EQ(gram->lpr("lpr0"_str)->name(), "lpr0"_str);
            EXPECT_EQ(gram->lpr("lpr0"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("lpr0"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr1"_str)) {
            EXPECT_EQ(gram->lpr("lpr1"_str)->name(), "lpr1"_str);
            EXPECT_EQ(gram->lpr("lpr1"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("lpr1"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr2"_str)) {
            EXPECT_EQ(gram->lpr("lpr2"_str)->name(), "lpr2"_str);
            EXPECT_EQ(gram->lpr("lpr2"_str)->index(), 2);
            EXPECT_EQ(gram->lpr("lpr2"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("b"_str)) {
            EXPECT_EQ(gram->ppr("b"_str)->name(), "b"_str);
            EXPECT_EQ(gram->ppr("b"_str)->index(), 1);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_lookahead_allowCertainNameSubexprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    // this is literally just a copy of success_lookahead_forPPRs, lol

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr_decl("lpr2")
        .ppr_decl("a"_str) // w/out alts
        .ppr_decl("b"_str) // w/ alts
        .lpr("lpr0")
        .string("a"_str)
        .close()
        .lpr("lpr1")
        .string("b"_str)
        .close()
        .lpr("lpr2")
        .string("c"_str)
        .close()
        .ppr("a"_str)
        .lookahead()
        .name("lpr0")
        .close()
        .close()
        .ppr("b"_str)
        .lookahead()
        .name("lpr0")
        .alternative()
        .name("lpr1")
        .alternative()
        .name("lpr2")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("lpr0"_str)) {
            EXPECT_EQ(gram->lpr("lpr0"_str)->name(), "lpr0"_str);
            EXPECT_EQ(gram->lpr("lpr0"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("lpr0"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr1"_str)) {
            EXPECT_EQ(gram->lpr("lpr1"_str)->name(), "lpr1"_str);
            EXPECT_EQ(gram->lpr("lpr1"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("lpr1"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr2"_str)) {
            EXPECT_EQ(gram->lpr("lpr2"_str)->name(), "lpr2"_str);
            EXPECT_EQ(gram->lpr("lpr2"_str)->index(), 2);
            EXPECT_EQ(gram->lpr("lpr2"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("b"_str)) {
            EXPECT_EQ(gram->ppr("b"_str)->name(), "b"_str);
            EXPECT_EQ(gram->ppr("b"_str)->index(), 1);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_lookahead_allowCertainSequenceSubexprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr_decl("lpr2"_str)
        .ppr_decl("a"_str)
        .lpr("lpr0")
        .string("a"_str)
        .close()
        .lpr("lpr1")
        .string("b"_str)
        .close()
        .lpr("lpr2")
        .string("c"_str)
        .close()
        .ppr("a"_str)
        .lookahead()
        .sequence()
        .name("lpr0"_str)
        .alternative()
        .name("lpr1"_str)
        .alternative()
        .name("lpr2"_str)
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("lpr0"_str)) {
            EXPECT_EQ(gram->lpr("lpr0"_str)->name(), "lpr0"_str);
            EXPECT_EQ(gram->lpr("lpr0"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("lpr0"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr1"_str)) {
            EXPECT_EQ(gram->lpr("lpr1"_str)->name(), "lpr1"_str);
            EXPECT_EQ(gram->lpr("lpr1"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("lpr1"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr2"_str)) {
            EXPECT_EQ(gram->lpr("lpr2"_str)->name(), "lpr2"_str);
            EXPECT_EQ(gram->lpr("lpr2"_str)->index(), 2);
            EXPECT_EQ(gram->lpr("lpr2"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// lookahead-not

TEST(LoadTests, success_lookahead_not_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str) // w/out alts
        .lpr_decl("b"_str) // w/ alts
        .lpr("a"_str)
        .lookahead_not()
        .charset("abc")
        .close()
        .close()
        .lpr("b"_str)
        .lookahead_not()
        .charset("abc")
        .alternative()
        .charset("def")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("b"_str)) {
            EXPECT_EQ(gram->lpr("b"_str)->name(), "b"_str);
            EXPECT_EQ(gram->lpr("b"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("b"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_lookahead_not_allowCertainStringSubexprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str) // w/out alts
        .lpr_decl("b"_str) // w/ alts
        .lpr("a"_str)
        .lookahead_not()
        .string("a"_str)
        .close()
        .close()
        .lpr("b"_str)
        .lookahead_not()
        .string("a"_str)
        .alternative()
        .string("b"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("b"_str)) {
            EXPECT_EQ(gram->lpr("b"_str)->name(), "b"_str);
            EXPECT_EQ(gram->lpr("b"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("b"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_lookahead_not_allowCertainSequenceSubexprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .lookahead_not()
        .sequence()
        .charset("abc"_str)
        .alternative()
        .charset("def"_str)
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_lookahead_not_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr_decl("lpr2")
        .ppr_decl("a"_str) // w/out alts
        .ppr_decl("b"_str) // w/ alts
        .lpr("lpr0")
        .string("a"_str)
        .close()
        .lpr("lpr1")
        .string("b"_str)
        .close()
        .lpr("lpr2")
        .string("c"_str)
        .close()
        .ppr("a"_str)
        .lookahead_not()
        .name("lpr0")
        .close()
        .close()
        .ppr("b"_str)
        .lookahead_not()
        .name("lpr0")
        .alternative()
        .name("lpr1")
        .alternative()
        .name("lpr2")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("lpr0"_str)) {
            EXPECT_EQ(gram->lpr("lpr0"_str)->name(), "lpr0"_str);
            EXPECT_EQ(gram->lpr("lpr0"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("lpr0"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr1"_str)) {
            EXPECT_EQ(gram->lpr("lpr1"_str)->name(), "lpr1"_str);
            EXPECT_EQ(gram->lpr("lpr1"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("lpr1"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr2"_str)) {
            EXPECT_EQ(gram->lpr("lpr2"_str)->name(), "lpr2"_str);
            EXPECT_EQ(gram->lpr("lpr2"_str)->index(), 2);
            EXPECT_EQ(gram->lpr("lpr2"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("b"_str)) {
            EXPECT_EQ(gram->ppr("b"_str)->name(), "b"_str);
            EXPECT_EQ(gram->ppr("b"_str)->index(), 1);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_lookahead_not_allowCertainNameSubexprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    // this is literally just a copy of success_lookahead_forPPRs, lol

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr_decl("lpr2")
        .ppr_decl("a"_str) // w/out alts
        .ppr_decl("b"_str) // w/ alts
        .lpr("lpr0")
        .string("a"_str)
        .close()
        .lpr("lpr1")
        .string("b"_str)
        .close()
        .lpr("lpr2")
        .string("c"_str)
        .close()
        .ppr("a"_str)
        .lookahead_not()
        .name("lpr0")
        .close()
        .close()
        .ppr("b"_str)
        .lookahead_not()
        .name("lpr0")
        .alternative()
        .name("lpr1")
        .alternative()
        .name("lpr2")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("lpr0"_str)) {
            EXPECT_EQ(gram->lpr("lpr0"_str)->name(), "lpr0"_str);
            EXPECT_EQ(gram->lpr("lpr0"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("lpr0"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr1"_str)) {
            EXPECT_EQ(gram->lpr("lpr1"_str)->name(), "lpr1"_str);
            EXPECT_EQ(gram->lpr("lpr1"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("lpr1"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr2"_str)) {
            EXPECT_EQ(gram->lpr("lpr2"_str)->name(), "lpr2"_str);
            EXPECT_EQ(gram->lpr("lpr2"_str)->index(), 2);
            EXPECT_EQ(gram->lpr("lpr2"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("b"_str)) {
            EXPECT_EQ(gram->ppr("b"_str)->name(), "b"_str);
            EXPECT_EQ(gram->ppr("b"_str)->index(), 1);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_lookahead_not_allowCertainSequenceSubexprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr_decl("lpr2")
        .ppr_decl("a"_str)
        .lpr("lpr0")
        .string("a"_str)
        .close()
        .lpr("lpr1")
        .string("b"_str)
        .close()
        .lpr("lpr2")
        .string("c"_str)
        .close()
        .ppr("a"_str)
        .lookahead_not()
        .sequence()
        .name("lpr0")
        .alternative()
        .name("lpr1")
        .alternative()
        .name("lpr2")
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("lpr0"_str)) {
            EXPECT_EQ(gram->lpr("lpr0"_str)->name(), "lpr0"_str);
            EXPECT_EQ(gram->lpr("lpr0"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("lpr0"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr1"_str)) {
            EXPECT_EQ(gram->lpr("lpr1"_str)->name(), "lpr1"_str);
            EXPECT_EQ(gram->lpr("lpr1"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("lpr1"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr2"_str)) {
            EXPECT_EQ(gram->lpr("lpr2"_str)->name(), "lpr2"_str);
            EXPECT_EQ(gram->lpr("lpr2"_str)->index(), 2);
            EXPECT_EQ(gram->lpr("lpr2"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// not

TEST(LoadTests, success_not_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str) // w/out alts
        .lpr_decl("b"_str) // w/ alts
        .lpr("a"_str)
        .not0()
        .charset("abc")
        .close()
        .close()
        .lpr("b"_str)
        .not0()
        .charset("abc")
        .alternative()
        .charset("def")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("b"_str)) {
            EXPECT_EQ(gram->lpr("b"_str)->name(), "b"_str);
            EXPECT_EQ(gram->lpr("b"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("b"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_not_allowCertainStringSubexprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str) // w/out alts
        .lpr_decl("b"_str) // w/ alts
        .lpr("a"_str)
        .not0()
        .string("a"_str)
        .close()
        .close()
        .lpr("b"_str)
        .not0()
        .string("a"_str)
        .alternative()
        .string("b"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("b"_str)) {
            EXPECT_EQ(gram->lpr("b"_str)->name(), "b"_str);
            EXPECT_EQ(gram->lpr("b"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("b"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_not_allowCertainSequenceSubexprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .not0()
        .sequence()
        .charset("abc"_str)
        .alternative()
        .charset("def"_str)
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_not_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr_decl("lpr2")
        .ppr_decl("a"_str) // w/out alts
        .ppr_decl("b"_str) // w/ alts
        .lpr("lpr0")
        .string("a"_str)
        .close()
        .lpr("lpr1")
        .string("b"_str)
        .close()
        .lpr("lpr2")
        .string("c"_str)
        .close()
        .ppr("a"_str)
        .not0()
        .name("lpr0")
        .close()
        .close()
        .ppr("b"_str)
        .not0()
        .name("lpr0")
        .alternative()
        .name("lpr1")
        .alternative()
        .name("lpr2")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("lpr0"_str)) {
            EXPECT_EQ(gram->lpr("lpr0"_str)->name(), "lpr0"_str);
            EXPECT_EQ(gram->lpr("lpr0"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("lpr0"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr1"_str)) {
            EXPECT_EQ(gram->lpr("lpr1"_str)->name(), "lpr1"_str);
            EXPECT_EQ(gram->lpr("lpr1"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("lpr1"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr2"_str)) {
            EXPECT_EQ(gram->lpr("lpr2"_str)->name(), "lpr2"_str);
            EXPECT_EQ(gram->lpr("lpr2"_str)->index(), 2);
            EXPECT_EQ(gram->lpr("lpr2"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("b"_str)) {
            EXPECT_EQ(gram->ppr("b"_str)->name(), "b"_str);
            EXPECT_EQ(gram->ppr("b"_str)->index(), 1);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_not_allowCertainNameSubexprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    // this is literally just a copy of success_lookahead_forPPRs, lol

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr_decl("lpr2")
        .ppr_decl("a"_str) // w/out alts
        .ppr_decl("b"_str) // w/ alts
        .lpr("lpr0")
        .string("a"_str)
        .close()
        .lpr("lpr1")
        .string("b"_str)
        .close()
        .lpr("lpr2")
        .string("c"_str)
        .close()
        .ppr("a"_str)
        .not0()
        .name("lpr0")
        .close()
        .close()
        .ppr("b"_str)
        .not0()
        .name("lpr0")
        .alternative()
        .name("lpr1")
        .alternative()
        .name("lpr2")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("lpr0"_str)) {
            EXPECT_EQ(gram->lpr("lpr0"_str)->name(), "lpr0"_str);
            EXPECT_EQ(gram->lpr("lpr0"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("lpr0"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr1"_str)) {
            EXPECT_EQ(gram->lpr("lpr1"_str)->name(), "lpr1"_str);
            EXPECT_EQ(gram->lpr("lpr1"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("lpr1"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr2"_str)) {
            EXPECT_EQ(gram->lpr("lpr2"_str)->name(), "lpr2"_str);
            EXPECT_EQ(gram->lpr("lpr2"_str)->index(), 2);
            EXPECT_EQ(gram->lpr("lpr2"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("b"_str)) {
            EXPECT_EQ(gram->ppr("b"_str)->name(), "b"_str);
            EXPECT_EQ(gram->ppr("b"_str)->index(), 1);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_not_allowCertainSequenceSubexprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr_decl("lpr2")
        .ppr_decl("a"_str)
        .lpr("lpr0")
        .string("a"_str)
        .close()
        .lpr("lpr1")
        .string("b"_str)
        .close()
        .lpr("lpr2")
        .string("c"_str)
        .close()
        .ppr("a"_str)
        .not0()
        .sequence()
        .name("lpr0")
        .alternative()
        .name("lpr1")
        .alternative()
        .name("lpr2")
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("lpr0"_str)) {
            EXPECT_EQ(gram->lpr("lpr0"_str)->name(), "lpr0"_str);
            EXPECT_EQ(gram->lpr("lpr0"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("lpr0"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr1"_str)) {
            EXPECT_EQ(gram->lpr("lpr1"_str)->name(), "lpr1"_str);
            EXPECT_EQ(gram->lpr("lpr1"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("lpr1"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr2"_str)) {
            EXPECT_EQ(gram->lpr("lpr2"_str)->name(), "lpr2"_str);
            EXPECT_EQ(gram->lpr("lpr2"_str)->index(), 2);
            EXPECT_EQ(gram->lpr("lpr2"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// optional

TEST(LoadTests, success_optional_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .optional()
        .string("abc")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_optional_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .ppr_decl("a"_str)
        .lpr("lpr0"_str)
        .string("abc"_str)
        .close()
        .ppr("a"_str)
        .optional()
        .name("lpr0"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// kleene-star

TEST(LoadTests, success_kleene_star_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .kleene_star()
        .string("abc")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_kleene_star_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .ppr_decl("a"_str)
        .lpr("lpr0"_str)
        .string("abc"_str)
        .close()
        .ppr("a"_str)
        .kleene_star()
        .name("lpr0"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// kleene-plus

TEST(LoadTests, success_kleene_plus_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .kleene_plus()
        .string("abc")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_lpr("a"_str)) {
            EXPECT_EQ(gram->lpr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->lpr("a"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("a"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

TEST(LoadTests, success_kleene_plus_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .ppr_decl("a"_str)
        .lpr("lpr0"_str)
        .string("abc"_str)
        .close()
        .ppr("a"_str)
        .kleene_plus()
        .name("lpr0"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);
    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        if (gram->has_ppr("a"_str)) {
            EXPECT_EQ(gram->ppr("a"_str)->name(), "a"_str);
            EXPECT_EQ(gram->ppr("a"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// misc

// this tests that name works w/ lprs/pprs defined AFTER name usage

TEST(LoadTests, success_misc_withNameRefsWhichAppearBeforeLPROrPPRIsDefined) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .ppr_decl("ppr0"_str)
        .lpr_decl("lpr1"_str)
        .ppr_decl("ppr1"_str)
        .lpr("lpr0"_str)
        .name("lpr1"_str)
        .close()
        .ppr("ppr0"_str)
        .name("ppr1"_str)
        .close()
        .lpr("lpr1"_str)
        .close()
        .ppr("ppr1"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);

        if (gram->has_lpr("lpr0"_str)) {
            EXPECT_EQ(gram->lpr("lpr0"_str)->name(), "lpr0"_str);
            EXPECT_EQ(gram->lpr("lpr0"_str)->index(), 0);
            EXPECT_EQ(gram->lpr("lpr0"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_lpr("lpr1"_str)) {
            EXPECT_EQ(gram->lpr("lpr1"_str)->name(), "lpr1"_str);
            EXPECT_EQ(gram->lpr("lpr1"_str)->index(), 1);
            EXPECT_EQ(gram->lpr("lpr1"_str)->qualifier(), taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("ppr0"_str)) {
            EXPECT_EQ(gram->ppr("ppr0"_str)->name(), "ppr0"_str);
            EXPECT_EQ(gram->ppr("ppr0"_str)->index(), 0);
        }
        else ADD_FAILURE();
        if (gram->has_ppr("ppr1"_str)) {
            EXPECT_EQ(gram->ppr("ppr1"_str)->name(), "ppr1"_str);
            EXPECT_EQ(gram->ppr("ppr1"_str)->index(), 1);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// test that charset strings are allowed to have duplicate chars w/out it
// causing ambiguity errors to arise

TEST(LoadTests, success_misc_withCharsetWithDuplicateChars) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr("lpr0"_str)
        .charset("abcabcabcabcabcabcabcabcabc")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);

        EXPECT_EQ(gram->lprs(), 1);
        EXPECT_EQ(gram->pprs(), 0);
    }
    else ADD_FAILURE();
}

// test precedence PPRs w/ base and recurse alts works

TEST(LoadTests, success_misc_precedencePPR_withBaseAndRecurseAlts) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .lpr("a"_str)
        .string("a")
        .close()
        .lpr("b"_str)
        .string("b")
        .close()
        .lpr("c"_str)
        .string("c")
        .close()
        .ppr("ppr0"_str, taul::precedence)
        .name("a"_str) // base alt
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("a"_str)
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("b"_str)
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("c"_str)
        .alternative()
        .name("b"_str) // base alt
        .alternative()
        .name("c"_str) // base alt
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);

        EXPECT_EQ(gram->lprs(), 3);
        EXPECT_EQ(gram->pprs(), 1);

        if (gram->has_ppr("ppr0"_str)) {
            EXPECT_EQ(gram->ppr("ppr0"_str)->name(), "ppr0"_str);
            EXPECT_EQ(gram->ppr("ppr0"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// test precedence PPRs w/ only base alts works

TEST(LoadTests, success_misc_precedencePPR_withOnlyBaseAlts) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .lpr("a"_str)
        .string("a")
        .close()
        .lpr("b"_str)
        .string("b")
        .close()
        .lpr("c"_str)
        .string("c")
        .close()
        .ppr("ppr0"_str, taul::precedence)
        .name("a"_str) // base alt
        .alternative()
        .name("b"_str) // base alt
        .alternative()
        .name("c"_str) // base alt
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);

        EXPECT_EQ(gram->lprs(), 3);
        EXPECT_EQ(gram->pprs(), 1);

        if (gram->has_ppr("ppr0"_str)) {
            EXPECT_EQ(gram->ppr("ppr0"_str)->name(), "ppr0"_str);
            EXPECT_EQ(gram->ppr("ppr0"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// test precedence PPRs w/ base alt, and recurse alt w/ only a prefix-ref
// (which is legal, but will semantically be ignored)

TEST(LoadTests, success_misc_precedencePPR_withBaseAlt_andRecurseAltWithOnlyAPrefixRef_altWillBeIgnored) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .lpr("a"_str)
        .string("a")
        .close()
        .lpr("b"_str)
        .string("b")
        .close()
        .lpr("c"_str)
        .string("c")
        .close()
        .ppr("ppr0"_str, taul::precedence)
        .name("a"_str) // base alt
        .alternative()
        .name("ppr0"_str) // recurse alt
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);

        EXPECT_EQ(gram->lprs(), 3);
        EXPECT_EQ(gram->pprs(), 1);

        if (gram->has_ppr("ppr0"_str)) {
            EXPECT_EQ(gram->ppr("ppr0"_str)->name(), "ppr0"_str);
            EXPECT_EQ(gram->ppr("ppr0"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// test precedence PPRs w/ an empty base alt works

TEST(LoadTests, success_misc_precedencePPR_withAnEmptyBaseAlt) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .lpr("a"_str)
        .string("a")
        .close()
        .lpr("b"_str)
        .string("b")
        .close()
        .lpr("c"_str)
        .string("c")
        .close()
        .ppr("ppr0"_str, taul::precedence)
        .name("a"_str) // base alt
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("a"_str)
        .alternative()
        // our empty base alt
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("b"_str)
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("c"_str)
        .alternative()
        .name("b"_str) // base alt
        .alternative()
        .name("c"_str) // base alt
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);

        EXPECT_EQ(gram->lprs(), 3);
        EXPECT_EQ(gram->pprs(), 1);

        if (gram->has_ppr("ppr0"_str)) {
            EXPECT_EQ(gram->ppr("ppr0"_str)->name(), "ppr0"_str);
            EXPECT_EQ(gram->ppr("ppr0"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// test precedence PPRs w/ base and recurse alts works

TEST(LoadTests, success_misc_precedencePPR_withRightAssociativeRecurseAlt) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .lpr("a"_str)
        .string("a")
        .close()
        .lpr("b"_str)
        .string("b")
        .close()
        .lpr("c"_str)
        .string("c")
        .close()
        .ppr("ppr0"_str, taul::precedence)
        .right_assoc() // w/ right_assoc
        .name("a"_str) // base alt
        .alternative()
        .right_assoc() // w/ right_assoc
        .name("ppr0"_str) // recurse alt
        .name("a"_str)
        .alternative()
        .right_assoc() // w/ right_assoc
        .name("ppr0"_str) // recurse alt
        .name("b"_str)
        .alternative()
        .right_assoc() // w/ right_assoc
        .name("ppr0"_str) // recurse alt
        .name("c"_str)
        .alternative()
        .right_assoc() // w/ right_assoc
        .name("b"_str) // base alt
        .alternative()
        .right_assoc() // w/ right_assoc
        .name("c"_str) // base alt
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);

        EXPECT_EQ(gram->lprs(), 3);
        EXPECT_EQ(gram->pprs(), 1);

        if (gram->has_ppr("ppr0"_str)) {
            EXPECT_EQ(gram->ppr("ppr0"_str)->name(), "ppr0"_str);
            EXPECT_EQ(gram->ppr("ppr0"_str)->index(), 0);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}


// tests testing that the expected FIRST/FOLLOW/prefix sets are generated
// for LPRs/PPRs for each type of expr

static_assert(taul::spec_opcodes == 21); // we only care about expr instructions

// top-level

// these tests are for testing the top-level LPR/PPR expr prefixes

TEST(LoadTests, HasExpectedPrefixes_LPRs_TopLevel_Empty) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add_epsilon();

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon();

        taul::glyph_set expected_prefix_set =
            expected_first_set + expected_follow_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_LPRs_TopLevel_WithNoAlts) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .string("abc")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add(U'a');

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove(U'a');

        taul::glyph_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_LPRs_TopLevel_WithAlts_WithNoEmptyAlt) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .string("ab")
        .alternative()
        .string("cd")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add(U'a')
            .add(U'c');

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove(U'a')
            .remove(U'c');
            
        taul::glyph_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_LPRs_TopLevel_WithAlts_WithEmptyAlt) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .string("ab")
        .alternative()
        .string("cd")
        .alternative()
        // empty alt
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add(U'a')
            .add(U'c')
            .add_epsilon();

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove(U'a')
            .remove(U'c');

        taul::glyph_set expected_prefix_set =
            expected_first_set + expected_follow_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_TopLevel_WithNoPrecedence_Empty) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f"_str)
        .ppr("f"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add_epsilon();

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon();

        taul::token_set expected_prefix_set =
            expected_first_set + expected_follow_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_TopLevel_WithPrecedence_Empty) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f"_str)
        .ppr("f"_str, taul::precedence)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add_epsilon();

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon();

        taul::token_set expected_prefix_set =
            expected_first_set + expected_follow_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_TopLevel_WithNoPrecedence_WithNoAlts) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .name("a"_str)
        .name("b"_str)
        .name("c"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add(gram->lpr("a"_str).value().index());

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove(gram->lpr("a"_str).value().index());

        taul::token_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_TopLevel_WithPrecedence_WithNoAlts) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str, taul::precedence)
        .name("a"_str) // base alt
        .name("b"_str)
        .name("c"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add(gram->lpr("a"_str).value().index());

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove(gram->lpr("a"_str).value().index());

        taul::token_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_TopLevel_WithNoPrecedence_WithAlts_WithNoEmptyAlt) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .lpr_decl("d"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .lpr("d"_str)
        .string("d"_str)
        .close()
        .ppr("f"_str)
        .name("a"_str)
        .name("b"_str)
        .alternative()
        .name("c"_str)
        .name("d"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_lpr("d"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add(gram->lpr("a"_str).value().index())
            .add(gram->lpr("c"_str).value().index());

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove(gram->lpr("a"_str).value().index())
            .remove(gram->lpr("c"_str).value().index());

        taul::token_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_TopLevel_WithPrecedence_WithAlts_WithNoEmptyAlt) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .lpr_decl("d"_str)
        .lpr_decl("x"_str)
        .lpr_decl("y"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .lpr("d"_str)
        .string("d"_str)
        .close()
        .lpr("x"_str)
        .string("x"_str)
        .close()
        .lpr("y"_str)
        .string("y"_str)
        .close()
        .ppr("f"_str, taul::precedence)
        .name("a"_str) // base alt
        .name("b"_str)
        .alternative()
        .name("c"_str) // base alt
        .name("d"_str)
        // these recurse alts should have no impact on result
        .alternative()
        .name("f"_str) // recurse alt
        .name("x"_str)
        .alternative()
        .name("f"_str) // recurse alt
        .name("y"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_lpr("d"_str));
        ASSERT_TRUE(gram->has_lpr("x"_str));
        ASSERT_TRUE(gram->has_lpr("y"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add(gram->lpr("a"_str).value().index())
            .add(gram->lpr("c"_str).value().index());

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove(gram->lpr("a"_str).value().index())
            .remove(gram->lpr("c"_str).value().index());

        taul::token_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_TopLevel_WithNoPrecedence_WithAlts_WithEmptyAlt) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .lpr_decl("d"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .lpr("d"_str)
        .string("d"_str)
        .close()
        .ppr("f"_str)
        .name("a"_str)
        .name("b"_str)
        .alternative()
        .name("c"_str)
        .name("d"_str)
        .alternative()
        // empty alt
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_lpr("d"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add(gram->lpr("a"_str).value().index())
            .add(gram->lpr("c"_str).value().index())
            .add_epsilon();

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove(gram->lpr("a"_str).value().index())
            .remove(gram->lpr("c"_str).value().index());

        taul::token_set expected_prefix_set =
            expected_first_set + expected_follow_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_TopLevel_WithPrecedence_WithAlts_WithEmptyAlt) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .lpr_decl("d"_str)
        .lpr_decl("x"_str)
        .lpr_decl("y"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .lpr("d"_str)
        .string("d"_str)
        .close()
        .lpr("x"_str)
        .string("x"_str)
        .close()
        .lpr("y"_str)
        .string("y"_str)
        .close()
        .ppr("f"_str, taul::precedence)
        .name("a"_str) // base alt
        .name("b"_str)
        .alternative()
        .name("c"_str) // base alt
        .name("d"_str)
        .alternative()
        // empty base alt
        .alternative()
        .name("f"_str) // recurse alt
        .name("x"_str)
        .alternative()
        .name("f"_str) // recurse alt
        .name("y"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_lpr("d"_str));
        ASSERT_TRUE(gram->has_lpr("x"_str));
        ASSERT_TRUE(gram->has_lpr("y"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add(gram->lpr("a"_str).value().index())
            .add(gram->lpr("c"_str).value().index())
            .add(gram->lpr("x"_str).value().index()) // <- recurse alt visible when has empty base alt
            .add(gram->lpr("y"_str).value().index()) // <- recurse alt visible when has empty base alt
            .add_epsilon();

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove(gram->lpr("a"_str).value().index())
            .remove(gram->lpr("c"_str).value().index())
            .remove(gram->lpr("x"_str).value().index()) // <- recurse alt visible when has empty base alt
            .remove(gram->lpr("y"_str).value().index()); // <- recurse alt visible when has empty base alt

        taul::token_set expected_prefix_set =
            expected_first_set + expected_follow_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);

        std::cerr << std::format("             actual == expected\n");
        std::cerr << std::format("FIRST sets : {} == {}\n", f.first_set(), expected_first_set);
        std::cerr << std::format("FOLLOW sets: {} == {}\n", f.follow_set(), expected_follow_set);
        std::cerr << std::format("prefix sets: {} == {}\n", f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

// end

TEST(LoadTests, HasExpectedPrefixes_LPRs_End) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .end()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add_id(taul::end_cp_id);

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove_id(taul::end_cp_id);

        taul::glyph_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_End) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f"_str)
        .ppr("f"_str)
        .end()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add_id(taul::end_lpr_id);

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove_id(taul::end_lpr_id);

        taul::token_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

// any

TEST(LoadTests, HasExpectedPrefixes_LPRs_Any) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .any()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove_id(taul::end_cp_id);

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_id(taul::end_cp_id);

        taul::glyph_set expected_prefix_set =
            expected_first_set;

        TAUL_LOG(lgr, "f.first_set()=={}", f.first_set());
        TAUL_LOG(lgr, "f.follow_set()=={}", f.follow_set());
        TAUL_LOG(lgr, "f.prefix_set()=={}", f.prefix_set());

        TAUL_LOG(lgr, "expected_first_set=={}", expected_first_set);
        TAUL_LOG(lgr, "expected_follow_set=={}", expected_follow_set);
        TAUL_LOG(lgr, "expected_prefix_set=={}", expected_prefix_set);

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_Any) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .any()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove_id(taul::end_lpr_id);

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_id(taul::end_lpr_id);

        taul::token_set expected_prefix_set =
            expected_first_set;

        TAUL_LOG(lgr, "f.first_set()=={}", f.first_set());
        TAUL_LOG(lgr, "f.follow_set()=={}", f.follow_set());
        TAUL_LOG(lgr, "f.prefix_set()=={}", f.prefix_set());

        TAUL_LOG(lgr, "expected_first_set=={}", expected_first_set);
        TAUL_LOG(lgr, "expected_follow_set=={}", expected_follow_set);
        TAUL_LOG(lgr, "expected_prefix_set=={}", expected_prefix_set);

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

// string

TEST(LoadTests, HasExpectedPrefixes_LPRs_String) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .string("abc"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add(U'a');

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove(U'a');

        taul::glyph_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_LPRs_String_MultiByteUTF8) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .string(taul::utf8_s(u8"💩"))
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add(U'💩');

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove(U'💩');

        taul::glyph_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

// charset

TEST(LoadTests, HasExpectedPrefixes_LPRs_Charset) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .charset("abc"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add_range(U'a', U'c');

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove_range(U'a', U'c');

        taul::glyph_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_LPRs_Charset_MultiByteUTF8) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .charset(taul::utf8_s(u8"a-fα-δ魂か-く"))
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add_range(U'a', U'f')
            .add_range(U'α', U'δ')
            .add(U'魂')
            .add_range(U'か', U'く');

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove_range(U'a', U'f')
            .remove_range(U'α', U'δ')
            .remove(U'魂')
            .remove_range(U'か', U'く');

        taul::glyph_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

// token

TEST(LoadTests, HasExpectedPrefixes_PPRs_Token) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a")
        .close()
        .lpr("b"_str)
        .string("b")
        .close()
        .lpr("c"_str)
        .string("c")
        .close()
        .ppr("f"_str)
        .token()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove_id(taul::failure_lpr_id)
            .remove_id(taul::end_lpr_id);

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_id(taul::failure_lpr_id)
            .add_id(taul::end_lpr_id);

        taul::token_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

// failure

TEST(LoadTests, HasExpectedPrefixes_PPRs_Failure) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a")
        .close()
        .lpr("b"_str)
        .string("b")
        .close()
        .lpr("c"_str)
        .string("c")
        .close()
        .ppr("f"_str)
        .failure()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add_id(taul::failure_lpr_id);

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove_id(taul::failure_lpr_id);

        taul::token_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

// name

TEST(LoadTests, HasExpectedPrefixes_LPRs_Name) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr_decl("other"_str)
        .lpr("f"_str)
        .name("other"_str)
        .close()
        .lpr("other"_str)
        .charset("abc"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        ASSERT_TRUE(gram->has_lpr("other"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add_range(U'a', U'c');

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove_range(U'a', U'c');

        taul::glyph_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_Name_WithLPRName) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a")
        .close()
        .lpr("b"_str)
        .string("b")
        .close()
        .lpr("c"_str)
        .string("c")
        .close()
        .ppr("f"_str)
        .name("b"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add(gram->lpr("b"_str).value().index());

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove(gram->lpr("b"_str).value().index());

        taul::token_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

// notice that the PPR name test depends upon LPR names and sets
// working, so take that into account

TEST(LoadTests, HasExpectedPrefixes_PPRs_Name_WithPPRName) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("f"_str)
        .ppr_decl("other"_str)
        .lpr("a"_str)
        .string("a")
        .close()
        .lpr("b"_str)
        .string("b")
        .close()
        .lpr("c"_str)
        .string("c")
        .close()
        .ppr("f"_str)
        .name("other"_str)
        .close()
        .ppr("other"_str)
        .name("a"_str)
        .alternative()
        .name("b"_str)
        .alternative()
        .name("c"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add(gram->lpr("a"_str).value().index())
            .add(gram->lpr("b"_str).value().index())
            .add(gram->lpr("c"_str).value().index());

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove(gram->lpr("a"_str).value().index())
            .remove(gram->lpr("b"_str).value().index())
            .remove(gram->lpr("c"_str).value().index());

        taul::token_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

// sequence

TEST(LoadTests, HasExpectedPrefixes_LPRs_Sequence_Empty) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .sequence()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add_epsilon();

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon();

        taul::glyph_set expected_prefix_set =
            expected_first_set + expected_follow_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_LPRs_Sequence_WithNoAlts) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .sequence()
        .string("a"_str)
        .string("b"_str)
        .string("c"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add(U'a');

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove(U'a');

        taul::glyph_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_LPRs_Sequence_WithAlts_WithNoEmptyAlt) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .sequence()
        .string("a"_str)
        .string("b"_str)
        .alternative()
        .string("c"_str)
        .string("d"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add(U'a')
            .add(U'c');

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove(U'a')
            .remove(U'c');

        taul::glyph_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_LPRs_Sequence_WithAlts_WithEmptyAlt) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .sequence()
        .string("a"_str)
        .string("b"_str)
        .alternative()
        .string("c"_str)
        .string("d"_str)
        .alternative()
        // empty alt
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add(U'a')
            .add(U'c')
            .add_epsilon();

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove(U'a')
            .remove(U'c');

        taul::glyph_set expected_prefix_set =
            expected_first_set + expected_follow_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_Sequence_Empty) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a")
        .close()
        .lpr("b"_str)
        .string("b")
        .close()
        .lpr("c"_str)
        .string("c")
        .close()
        .ppr("f"_str)
        .sequence()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add_epsilon();

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon();

        taul::token_set expected_prefix_set =
            expected_first_set + expected_follow_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_Sequence_WithNoAlts) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a")
        .close()
        .lpr("b"_str)
        .string("b")
        .close()
        .lpr("c"_str)
        .string("c")
        .close()
        .ppr("f"_str)
        .sequence()
        .name("a"_str)
        .name("b"_str)
        .name("c"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add(gram->lpr("a"_str).value().index());

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove(gram->lpr("a"_str).value().index());

        taul::token_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_Sequence_WithAlts_WithNoEmptyAlt) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .lpr_decl("d"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a")
        .close()
        .lpr("b"_str)
        .string("b")
        .close()
        .lpr("c"_str)
        .string("c")
        .close()
        .lpr("d"_str)
        .string("d")
        .close()
        .ppr("f"_str)
        .sequence()
        .name("a"_str)
        .name("b"_str)
        .alternative()
        .name("c"_str)
        .name("d"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add(gram->lpr("a"_str).value().index())
            .add(gram->lpr("c"_str).value().index());

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove(gram->lpr("a"_str).value().index())
            .remove(gram->lpr("c"_str).value().index());

        taul::token_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_Sequence_WithAlts_WithEmptyAlt) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .lpr_decl("d"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a")
        .close()
        .lpr("b"_str)
        .string("b")
        .close()
        .lpr("c"_str)
        .string("c")
        .close()
        .lpr("d"_str)
        .string("d")
        .close()
        .ppr("f"_str)
        .sequence()
        .name("a"_str)
        .name("b"_str)
        .alternative()
        .name("c"_str)
        .name("d"_str)
        .alternative()
        // empty alt
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add(gram->lpr("a"_str).value().index())
            .add(gram->lpr("c"_str).value().index())
            .add_epsilon();

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove(gram->lpr("a"_str).value().index())
            .remove(gram->lpr("c"_str).value().index());

        taul::token_set expected_prefix_set =
            expected_first_set + expected_follow_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

// lookahead

TEST(LoadTests, HasExpectedPrefixes_LPRs_LookAhead) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .lookahead()
        .string("a"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add(U'a');

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove(U'a');

        taul::glyph_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_LookAhead) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .lookahead()
        .name("a"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add(gram->lpr("a"_str).value().index());

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove(gram->lpr("a"_str).value().index());

        taul::token_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

// lookahead-not

TEST(LoadTests, HasExpectedPrefixes_LPRs_LookAheadNot) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .lookahead_not()
        .string("a"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add(U'a')
            .inverse()
            .remove_id(taul::end_cp_id)
            .remove_epsilon();

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add(U'a')
            .add_id(taul::end_cp_id);

        taul::glyph_set expected_prefix_set =
            expected_first_set;

        TAUL_LOG(lgr, "f.first_set()=={}", f.first_set());
        TAUL_LOG(lgr, "f.follow_set()=={}", f.follow_set());
        TAUL_LOG(lgr, "f.prefix_set()=={}", f.prefix_set());

        TAUL_LOG(lgr, "expected_first_set=={}", expected_first_set);
        TAUL_LOG(lgr, "expected_follow_set=={}", expected_follow_set);
        TAUL_LOG(lgr, "expected_prefix_set=={}", expected_prefix_set);

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_LookAheadNot) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .lookahead_not()
        .name("b"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add_all()
            .remove(gram->lpr("b"_str).value().index())
            .remove_id(taul::end_lpr_id)
            .remove_epsilon();

        taul::token_set expected_follow_set =
            taul::token_set()
            .add(gram->lpr("b"_str).value().index())
            .add_id(taul::end_lpr_id);

        taul::token_set expected_prefix_set =
            expected_first_set;

        TAUL_LOG(lgr, "f.first_set()=={}", f.first_set());
        TAUL_LOG(lgr, "f.follow_set()=={}", f.follow_set());
        TAUL_LOG(lgr, "f.prefix_set()=={}", f.prefix_set());

        TAUL_LOG(lgr, "expected_first_set=={}", expected_first_set);
        TAUL_LOG(lgr, "expected_follow_set=={}", expected_follow_set);
        TAUL_LOG(lgr, "expected_prefix_set=={}", expected_prefix_set);

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

// not

TEST(LoadTests, HasExpectedPrefixes_LPRs_Not) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .not0()
        .string("a"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add_all()
            .remove(U'a')
            .remove_id(taul::end_cp_id)
            .remove_epsilon();

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add(U'a')
            .add_id(taul::end_cp_id);

        taul::glyph_set expected_prefix_set =
            expected_first_set;

        TAUL_LOG(lgr, "f.first_set()=={}", f.first_set());
        TAUL_LOG(lgr, "f.follow_set()=={}", f.follow_set());
        TAUL_LOG(lgr, "f.prefix_set()=={}", f.prefix_set());

        TAUL_LOG(lgr, "expected_first_set=={}", expected_first_set);
        TAUL_LOG(lgr, "expected_follow_set=={}", expected_follow_set);
        TAUL_LOG(lgr, "expected_prefix_set=={}", expected_prefix_set);

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_Not) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .not0()
        .name("b"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add_all()
            .remove(gram->lpr("b"_str).value().index())
            .remove_id(taul::end_lpr_id)
            .remove_epsilon();

        taul::token_set expected_follow_set =
            taul::token_set()
            .add(gram->lpr("b"_str).value().index())
            .add_id(taul::end_lpr_id);

        taul::token_set expected_prefix_set =
            expected_first_set;

        TAUL_LOG(lgr, "f.first_set()=={}", f.first_set());
        TAUL_LOG(lgr, "f.follow_set()=={}", f.follow_set());
        TAUL_LOG(lgr, "f.prefix_set()=={}", f.prefix_set());

        TAUL_LOG(lgr, "expected_first_set=={}", expected_first_set);
        TAUL_LOG(lgr, "expected_follow_set=={}", expected_follow_set);
        TAUL_LOG(lgr, "expected_prefix_set=={}", expected_prefix_set);

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

// optional

TEST(LoadTests, HasExpectedPrefixes_LPRs_Optional) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .optional()
        .string("abc"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add(U'a')
            .add_epsilon();

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove(U'a');

        taul::glyph_set expected_prefix_set =
            expected_first_set + expected_follow_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_Optional) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .optional()
        .name("a"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add(gram->lpr("a"_str).value().index())
            .add_epsilon();

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove(gram->lpr("a"_str).value().index());

        taul::token_set expected_prefix_set =
            expected_first_set + expected_follow_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

// kleene-star

TEST(LoadTests, HasExpectedPrefixes_LPRs_KleeneStar) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .kleene_star()
        .string("abc"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add(U'a')
            .add_epsilon();

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove(U'a');

        taul::glyph_set expected_prefix_set =
            expected_first_set + expected_follow_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_KleeneStar) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .kleene_star()
        .name("b"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add(gram->lpr("b"_str).value().index())
            .add_epsilon();

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove(gram->lpr("b"_str).value().index());

        taul::token_set expected_prefix_set =
            expected_first_set + expected_follow_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

// kleene-plus

TEST(LoadTests, HasExpectedPrefixes_LPRs_KleenePlus) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .kleene_plus()
        .string("abc"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("f"_str));
        taul::lpr_ref f = gram->lpr("f"_str).value();

        taul::glyph_set expected_first_set =
            taul::glyph_set()
            .add(U'a');

        taul::glyph_set expected_follow_set =
            taul::glyph_set()
            .add_all()
            .remove_epsilon()
            .remove(U'a');

        taul::glyph_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}

TEST(LoadTests, HasExpectedPrefixes_PPRs_KleenePlus) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("f"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .kleene_plus()
        .name("a"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);
    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);
        ASSERT_TRUE(gram->has_lpr("a"_str));
        ASSERT_TRUE(gram->has_lpr("b"_str));
        ASSERT_TRUE(gram->has_lpr("c"_str));
        ASSERT_TRUE(gram->has_ppr("f"_str));
        taul::ppr_ref f = gram->ppr("f"_str).value();

        taul::token_set expected_first_set =
            taul::token_set()
            .add(gram->lpr("a"_str).value().index());

        taul::token_set expected_follow_set =
            taul::token_set()
            .add_all()
            .remove_epsilon()
            .remove(gram->lpr("a"_str).value().index());

        taul::token_set expected_prefix_set =
            expected_first_set;

        EXPECT_EQ(f.first_set(), expected_first_set);
        EXPECT_EQ(f.follow_set(), expected_follow_set);
        EXPECT_EQ(f.prefix_set(), expected_prefix_set);
    }
    else ADD_FAILURE();
}


// the below detail what errors each instruction can raise, and thus which must
// be unit tested, and being specified *in order*

static_assert(taul::spec_opcodes == 21);
static_assert(taul::spec_errors == 25);

// notice that a lot of errors actually arise on the close instruction, however, most
// of these errors will be tested in association w/ the opening instruction instead

// the below will only broadly test for detection of ambiguity, w/ us
// then assuming that the impl can be trusted to cover other cases if
// these tests, and tests about generation of expected prefix sets, 
// work as expected

// TODO: above feels dirty, is there a better way of doing this?

// close
//      stray-close

TEST(LoadTests, close_forErr_stray_close) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::stray_close), 1);
    EXPECT_FALSE(gram);
}

// alternative
//      illegal-in-no-scope

TEST(LoadTests, alternative_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .alternative()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

// right-assoc
//      illegal-in-no-scope
//      illegal-right-assoc (in LPR)
//      illegal-right-assoc (in non-precedence PPR)
//      illegal-right-assoc (multiple right_assoc instrs)
//      illegal-right-assoc (not at start of alt) (first alt)
//      illegal-right-assoc (not at start of alt) (not first alt)
//      illegal-right-assoc (in sequence)
//      illegal-right-assoc (in lookahead)
//      illegal-right-assoc (in lookahead-not)
//      illegal-right-assoc (in not)
//      illegal-right-assoc (in optional)
//      illegal-right-assoc (in kleene-star)
//      illegal-right-assoc (in kleene-plus)

TEST(LoadTests, right_assoc_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .right_assoc()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, right_assoc_forErr_illegal_right_assoc_inLPR) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("A")
        .lpr("A")
        .right_assoc()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_right_assoc), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, right_assoc_forErr_illegal_right_assoc_inNonPrecedencePPR) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("A")
        .ppr("A")
        .right_assoc()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_right_assoc), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, right_assoc_forErr_illegal_right_assoc_multipleRightAssocInstrs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("A")
        .ppr("A", taul::precedence)
        .right_assoc()
        .right_assoc()
        .token()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_right_assoc), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, right_assoc_forErr_illegal_right_assoc_notAtStartOfAlt_firstAlt) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("A")
        .ppr("A", taul::precedence)
        .token()
        .right_assoc()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_right_assoc), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, right_assoc_forErr_illegal_right_assoc_notAtStartOfAlt_notFirstAlt) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("A")
        .ppr("A", taul::precedence)
        .end()
        .alternative()
        .token()
        .right_assoc()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_right_assoc), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, right_assoc_forErr_illegal_right_assoc_inSequence) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("A")
        .ppr("A", taul::precedence)
        .sequence()
        .end()
        .alternative() // putting right_assoc in next alt to test w/ a bit more *complexity*
        .right_assoc()
        .token()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_right_assoc), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, right_assoc_forErr_illegal_right_assoc_inLookAhead) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("A")
        .ppr("A", taul::precedence)
        .lookahead()
        .right_assoc()
        .token()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_right_assoc), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, right_assoc_forErr_illegal_right_assoc_inLookAheadNot) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("A")
        .ppr("A", taul::precedence)
        .lookahead_not()
        .right_assoc()
        .token()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_right_assoc), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, right_assoc_forErr_illegal_right_assoc_inNot) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("A")
        .ppr("A", taul::precedence)
        .not0()
        .right_assoc()
        .token()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_right_assoc), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, right_assoc_forErr_illegal_right_assoc_inOptional) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("A")
        .ppr("A", taul::precedence)
        .optional()
        .right_assoc()
        .token()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_right_assoc), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, right_assoc_forErr_illegal_right_assoc_inKleeneStar) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("A")
        .ppr("A", taul::precedence)
        .kleene_star()
        .right_assoc()
        .token()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_right_assoc), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, right_assoc_forErr_illegal_right_assoc_inKleenePlus) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("A")
        .ppr("A", taul::precedence)
        .kleene_plus()
        .right_assoc()
        .token()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_right_assoc), 1);
    EXPECT_FALSE(gram);
}

// lpr-decl
//      rule-name-conflict (w/ lpr)
//      rule-name-conflict (w/ ppr)
//      rule-never-defined
//      illegal-rule-declare (due to lpr)
//      illegal-rule-declare (due to ppr)
//      illegal-in-lpr-scope
//      illegal-in-ppr-scope

TEST(LoadTests, lpr_decl_forErr_rule_name_conflict_withLPR) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr0"_str)
        .lpr("lpr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_name_conflict), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lpr_decl_forErr_rule_name_conflict_withPPR) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .lpr_decl("ppr0"_str)
        .ppr("ppr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_name_conflict), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lpr_decl_forErr_rule_never_defined) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_never_defined), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lpr_decl_forErr_illegal_rule_declare_dueToLPR) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .close()
        .lpr_decl("b"_str) // illegal!
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_rule_declare), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lpr_decl_forErr_illegal_rule_declare_dueToPPR) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("a"_str)
        .ppr("a"_str)
        .close()
        .lpr_decl("b"_str) // illegal!
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_rule_declare), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lpr_decl_forErr_illegal_in_lpr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .close()
        .lpr("lpr1"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_lpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lpr_decl_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr("ppr0"_str)
        .lpr_decl("lpr1"_str)
        .close()
        .lpr("lpr1"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

// ppr-decl
//      rule-name-conflict (w/ lpr)
//      rule-name-conflict (w/ ppr)
//      rule-never-defined
//      illegal-rule-declare (due to lpr)
//      illegal-rule-declare (due to ppr)
//      illegal-in-lpr-scope
//      illegal-in-ppr-scope

TEST(LoadTests, ppr_decl_forErr_rule_name_conflict_withLPR) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .ppr_decl("lpr0"_str)
        .lpr("lpr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_name_conflict), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_decl_forErr_rule_name_conflict_withPPR) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr0"_str)
        .ppr("ppr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_name_conflict), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_decl_forErr_rule_never_defined) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_never_defined), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_decl_forErr_illegal_rule_declare_dueToLPR) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .close()
        .ppr_decl("b"_str) // illegal!
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_rule_declare), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_decl_forErr_illegal_rule_declare_dueToPPR) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("a"_str)
        .ppr("a"_str)
        .close()
        .ppr_decl("b"_str) // illegal!
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_rule_declare), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_decl_forErr_illegal_in_lpr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr("lpr0"_str)
        .ppr_decl("ppr1"_str)
        .close()
        .ppr("ppr1"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_lpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_decl_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .close()
        .ppr("ppr1"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

// lpr
//      scope-not-closed
//      rule-never-declared
//      illegal-in-lpr-scope
//      illegal-in-ppr-scope
//      illegal-qualifier (for precedence)
//      illegal-ambiguity (no left-recursion)
//      illegal-ambiguity (direct left-recursion)
//      illegal-ambiguity (direct left-recursion w/ single alt)
//      illegal-ambiguity (indirect left-recursion)

TEST(LoadTests, lpr_forErr_scope_not_closed) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr("lpr0"_str)
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::scope_not_closed), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lpr_forErr_rule_never_declared) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr("lpr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_never_declared), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lpr_forErr_illegal_in_lpr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .lpr("lpr1"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_lpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lpr_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .lpr_decl("lpr1"_str)
        .ppr("ppr0"_str)
        .lpr("lpr1"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lpr_forErr_illegal_qualifier_forPrecedence) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr("lpr0"_str, taul::qualifier::precedence)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_qualifier), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lpr_forErr_illegal_ambiguity_noLeftRecursion) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .charset("abc"_str) // prefixes: a, b, c
        .alternative()
        .name("lpr1"_str) // prefixes: a, b, c
        .close()
        .lpr("lpr1"_str)
        .charset("abc"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lpr_forErr_illegal_ambiguity_directLeftRecursion) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr("lpr0"_str)
        .charset("abc"_str)
        .alternative()
        .name("lpr0"_str) // direct self-ref
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lpr_forErr_illegal_ambiguity_directLeftRecursion_withSingleAlt) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr("lpr0"_str)
        .name("lpr0"_str) // direct self-ref
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lpr_forErr_illegal_ambiguity_indirectLeftRecursion) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .charset("abc"_str)
        .alternative()
        .name("lpr1"_str) // indirect self-ref
        .close()
        .lpr("lpr1"_str)
        .name("lpr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

// ppr
//      scope-not-closed
//      rule-never-declared
//      illegal-in-lpr-scope
//      illegal-in-ppr-scope
//      illegal-qualifier (for skip)
//      illegal-qualifier (for support)
//      illegal-ambiguity (no left-recursion)
//      illegal-ambiguity (direct left-recursion) (no precedence)
//      illegal-ambiguity (direct left-recursion w/ single alt) (no precedence)
//      illegal-ambiguity (indirect left-recursion) (no precedence)
//      illegal-ambiguity (precedence PPR w/ only recurse alts)
//      illegal-ambiguity (precedence PPR w/ only recurse alts) (w/ only one alt, w/ only a prefix-ref)
//      illegal-ambiguity (precedence PPR w/ ambiguous base alts)
//      illegal-ambiguity (precedence PPR w/ ambiguous recurse alts)
//      illegal-ambiguity (precedence PPR w/ indirectly left-recursive base alt)

TEST(LoadTests, ppr_forErr_scope_not_closed) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr("ppr0"_str)
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::scope_not_closed), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_forErr_rule_never_declared) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr("ppr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_never_declared), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_forErr_illegal_in_lpr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .ppr_decl("ppr1"_str)
        .lpr("lpr0"_str)
        .ppr("ppr1"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_lpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .ppr("ppr1"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_forErr_illegal_qualifier_forSkip) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr("ppr0"_str, taul::qualifier::skip)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_qualifier), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_forErr_illegal_qualifier_forSupport) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr("ppr0"_str, taul::qualifier::support)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_qualifier), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_forErr_illegal_ambiguity_noLeftRecursion) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("ppr0"_str)
        .name("a"_str) // prefixes: a
        .alternative()
        .name("b"_str) // prefixes: b
        .alternative()
        .name("c"_str) // prefixes: c
        .alternative()
        .name("ppr1"_str) // prefixes: a, b, c
        .close()
        .ppr("ppr1"_str)
        .name("a"_str) // prefixes: a
        .alternative()
        .name("b"_str) // prefixes: b
        .alternative()
        .name("c"_str) // prefixes: c
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_forErr_illegal_ambiguity_directLeftRecursion_noPrecedence) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("ppr0"_str)
        .name("a"_str)
        .alternative()
        .name("b"_str)
        .alternative()
        .name("c"_str)
        .alternative()
        .name("ppr0"_str) // direct self-ref
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_forErr_illegal_ambiguity_directLeftRecursion_withSingleAlt_noPrecedence) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("ppr0"_str)
        .name("ppr0"_str) // direct self-ref
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_forErr_illegal_ambiguity_indirectLeftRecursion_noPrecedence) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("ppr0"_str)
        .name("a"_str)
        .alternative()
        .name("b"_str)
        .alternative()
        .name("c"_str)
        .alternative()
        .name("ppr1"_str) // indirect self-ref
        .close()
        .ppr("ppr1"_str)
        .name("ppr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_forErr_illegal_ambiguity_precedencePPR_withOnlyRecurseAlts) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("ppr0"_str, taul::precedence)
        .name("ppr0"_str) // recurse alt
        .name("a"_str)
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("b"_str)
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("c"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}


TEST(LoadTests, ppr_forErr_illegal_ambiguity_precedencePPR_withOnlyRecurseAlts_withOnlyOneAlt_withOnlyAPrefixRef) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("ppr0"_str, taul::precedence)
        .name("ppr0"_str) // recurse alt    <- unique type w/ only a prefix-ref
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_forErr_illegal_ambiguity_precedencePPR_withAmbiguousBaseAlts) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("ppr0"_str, taul::precedence)
        .name("a"_str) // base alt <- ambiguous
        .alternative()
        .name("b"_str) // base alt
        .alternative()
        .name("a"_str) // base alt <- ambiguous
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("a"_str)
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("b"_str)
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("c"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_forErr_illegal_ambiguity_precedencePPR_withAmbiguousRecurseAlts) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("ppr0"_str, taul::precedence)
        .name("a"_str) // base alt
        .alternative()
        .name("b"_str) // base alt
        .alternative()
        .name("c"_str) // base alt
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("a"_str) // <- ambiguous
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("b"_str)
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("a"_str) // <- ambiguous
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, ppr_forErr_illegal_ambiguity_precedencePPR_withIndirectlyLeftRecursiveBaseAlt) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("ppr0"_str, taul::precedence)
        .name("a"_str) // base alt
        .alternative()
        .name("b"_str) // base alt
        .alternative()
        .name("ppr1"_str) // base alt <- indirect self-ref
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("a"_str)
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("b"_str)
        .alternative()
        .name("ppr0"_str) // recurse alt
        .name("c"_str)
        .close()
        .ppr("ppr1"_str)
        .name("ppr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

// end
//      illegal-in-no-scope

TEST(LoadTests, end_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .end()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

// any
//      illegal-in-no-scope

TEST(LoadTests, any_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .any()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

// string
//      illegal-in-ppr-scope
//      illegal-in-no-scope
//      illegal-string-literal (due to illegal codepoints)
//      illegal-string-literal (due to non-visible ASCII)

TEST(LoadTests, string_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("a"_str)
        .ppr("a"_str)
        .string("abc"_str) // illegal!
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, string_seq_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .string("abc"_str)
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, string_seq_forErr_illegal_string_literal_dueToIllegalCodepoints) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .string(taul::utf8_s(u8"ab\\ud800cd"))
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_string_literal), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, string_seq_forErr_illegal_string_literal_dueToNonVisibleASCII) {
    const auto lgr = taul::make_stderr_logger();
    for (size_t i = size_t('\x00'); i <= size_t('\x7f'); i++) {
        char chr = char(i);
        if (taul::is_visible_ascii(chr)) continue; // skip visible ASCII

        std::string ss = std::string("abc") + chr + std::string("def");
        ASSERT_EQ(ss.length(), 7);

        TAUL_LOG(lgr, "testing w/ '{}'", taul::fmt_taul_char(chr));

        taul::spec_error_counter ec{};

        const auto s =
            taul::spec_writer()
            .lpr_decl("f"_str)
            .lpr("f"_str)
            .string(ss)
            .close()
            .done();

        const auto gram = taul::load(s, ec, lgr);

        EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
        EXPECT_EQ(ec.count(taul::spec_error::illegal_string_literal), 1);
        EXPECT_FALSE(gram);
    }
}

// charset
//      illegal-in-ppr-scope
//      illegal-in-no-scope
//      illegal-charset-literal (due to illegal codepoints)
//      illegal-charset-literal (due to non-visible ASCII)

TEST(LoadTests, charset_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("a"_str)
        .ppr("a"_str)
        .charset("abc"_str) // illegal!
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, charset_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .charset("abc"_str)
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, charset_seq_forErr_illegal_charset_literal_dueToIllegalCodepoints) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    // \ud799-\ue000 is non-trivial to detect as invalid, since both low/high values 
    // are themselves valid, and the invalid values are those in between them

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .charset(taul::utf8_s(u8"a-z\\ud799-\\ue0001-9"))
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_charset_literal), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, charset_seq_forErr_illegal_charset_literal_dueToNonVisibleASCII) {
    const auto lgr = taul::make_stderr_logger();
    for (size_t i = size_t('\x00'); i <= size_t('\x7f'); i++) {
        char chr = char(i);
        if (taul::is_visible_ascii(chr)) continue; // skip visible ASCII

        std::string ss = std::string("a-f") + chr + std::string("123");
        ASSERT_EQ(ss.length(), 7);

        TAUL_LOG(lgr, "testing w/ '{}'", taul::fmt_taul_char(chr, false));

        taul::spec_error_counter ec{};

        const auto s =
            taul::spec_writer()
            .lpr_decl("f"_str)
            .lpr("f"_str)
            .charset(ss)
            .close()
            .done();

        const auto gram = taul::load(s, ec, lgr);

        EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
        EXPECT_EQ(ec.count(taul::spec_error::illegal_charset_literal), 1);
        EXPECT_FALSE(gram);
    }
}

// token
//      illegal-in-lpr-scope
//      illegal-in-no-scope

TEST(LoadTests, token_forErr_illegal_in_lpr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .token() // illegal!
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_lpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, token_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .token()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

// failure
//      illegal-in-lpr-scope
//      illegal-in-no-scope

TEST(LoadTests, failure_forErr_illegal_in_lpr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr("a"_str)
        .failure() // illegal!
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_lpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, failure_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .failure()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

// name
//      illegal-in-no-scope
//      rule-not-found (rule not yet declared)
//      rule-not-found (lpr not yet defined) (for lprs)
//      rule-not-found (lpr not yet defined) (for pprs)
//      rule-not-found (ppr not yet defined) (for pprs)
//      rule-may-not-be-ppr (for lprs)

TEST(LoadTests, name_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr("lpr0"_str)
        .close()
        .name("lpr0"_str)
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, name_forErr_rule_not_found_ruleNotYetDeclared) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr("lpr0"_str)
        .name("non-existent-rule"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_not_found), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, name_forErr_rule_not_found_LPRNotYetDefined_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr("lpr0"_str)
        .name("lpr1"_str)
        .close()
        .lpr_decl("lpr1"_str)
        .lpr("lpr1"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_not_found), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, name_forErr_rule_not_found_LPRNotYetDefined_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr("ppr0"_str)
        .name("lpr1"_str)
        .close()
        .lpr_decl("lpr1"_str)
        .lpr("lpr1"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_not_found), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, name_forErr_rule_not_found_PPRNotYetDefined_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr("ppr0"_str)
        .name("ppr1"_str)
        .close()
        .ppr_decl("ppr1"_str)
        .ppr("ppr1"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_not_found), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, name_forErr_rule_may_not_be_ppr_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .ppr_decl("ppr0"_str)
        .lpr("lpr0"_str)
        .name("ppr0"_str)
        .close()
        .ppr("ppr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_may_not_be_ppr), 1);
    EXPECT_FALSE(gram);
}

// below, for illegal-in-single-terminal-scope and illegal-in-single-subexpr-scope
// error tests, the sequence will be wrapped in a 'lookahead' expr to provide
// it w/ a single-terminal/subexpr scope, w/ it being assumed that this behaviour
// extends to other similar exprs when used w/ sequence

// we'll also do this for illegal-in-no-end-subexpr-scope

// sequence
//      illegal-in-no-scope
//      illegal-in-single-terminal-scope (w/ name) (for lprs)
//      illegal-in-single-terminal-scope (w/ lookahead) (for lprs)
//      illegal-in-single-terminal-scope (w/ lookahead-not) (for lprs)
//      illegal-in-single-terminal-scope (w/ not) (for lprs)
//      illegal-in-single-terminal-scope (w/ optional) (for lprs)
//      illegal-in-single-terminal-scope (w/ kleene-star) (for lprs)
//      illegal-in-single-terminal-scope (w/ kleene-plus) (for lprs)
//      illegal-in-single-terminal-scope (w/ name) (for pprs)
//      illegal-in-single-terminal-scope (w/ lookahead) (for pprs)
//      illegal-in-single-terminal-scope (w/ lookahead-not) (for pprs)
//      illegal-in-single-terminal-scope (w/ not) (for pprs)
//      illegal-in-single-terminal-scope (w/ optional) (for pprs)
//      illegal-in-single-terminal-scope (w/ kleene-star) (for pprs)
//      illegal-in-single-terminal-scope (w/ kleene-plus) (for pprs)
//      illegal-in-single-subexpr-scope (due to zero subexpr alt) (for lprs)
//      illegal-in-single-subexpr-scope (due to two subexpr alt) (for lprs)
//      illegal-in-single-subexpr-scope (due to zero subexpr alt) (for pprs)
//      illegal-in-single-subexpr-scope (due to two subexpr alt) (for pprs)
//      illegal-in-no-end-subexpr-scope (for lprs)
//      illegal-in-no-end-subexpr-scope (for pprs)
//      illegal-ambiguity (no left-recursion) (for lprs)
//      illegal-ambiguity (direct left-recursion) (for lprs)
//      illegal-ambiguity (direct left-recursion w/ single alt) (for lprs)
//      illegal-ambiguity (indirect left-recursion) (for lprs)
//      illegal-ambiguity (no left-recursion) (for pprs)
//      illegal-ambiguity (direct left-recursion) (no precedence) (for pprs)
//      illegal-ambiguity (direct left-recursion w/ single alt) (no precedence) (for pprs)
//      illegal-ambiguity (indirect left-recursion) (no precedence) (for pprs)
//      illegal-ambiguity (direct left-recursion) (precedence) (for pprs)
//      illegal-ambiguity (direct left-recursion w/ single alt) (precedence) (for pprs)
//      illegal-ambiguity (indirect left-recursion) (precedence) (for pprs)

TEST(LoadTests, sequence_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .sequence()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_terminal_scope_forName_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead()
        .sequence()
        .name("lpr0"_str)
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_terminal_scope_forLookAhead_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead()
        .sequence()
        .lookahead()
        .any()
        .close()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_terminal_scope_forLookAheadNot_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead()
        .sequence()
        .lookahead_not()
        .any()
        .close()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_terminal_scope_forNot_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead()
        .sequence()
        .not0()
        .any()
        .close()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_terminal_scope_forOptional_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead()
        .sequence()
        .optional()
        .any()
        .close()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_terminal_scope_forKleeneStar_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead()
        .sequence()
        .kleene_star()
        .any()
        .close()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_terminal_scope_forKleenePlus_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead()
        .sequence()
        .kleene_plus()
        .any()
        .close()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_terminal_scope_forName_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead()
        .sequence()
        .name("ppr0"_str)
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_terminal_scope_forLookAhead_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead()
        .sequence()
        .lookahead()
        .any()
        .close()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_terminal_scope_forLookAheadNot_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead()
        .sequence()
        .lookahead_not()
        .any()
        .close()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_terminal_scope_forNot_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead()
        .sequence()
        .not0()
        .any()
        .close()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_terminal_scope_forOptional_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead()
        .sequence()
        .optional()
        .any()
        .close()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_terminal_scope_forKleeneStar_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead()
        .sequence()
        .kleene_star()
        .any()
        .close()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_terminal_scope_forKleenePlus_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead()
        .sequence()
        .kleene_plus()
        .any()
        .close()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_subexpr_scope_dueToZeroSubexprAlt_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .lookahead()
        .sequence()
        .string("a"_str)
        .alternative()
        // zero subexprs, illegal!
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_subexpr_scope_dueToTwoSubexprAlt_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .lookahead()
        .sequence()
        .string("a"_str)
        .alternative()
        .string("b"_str)
        .string("c"_str) // two subexprs, illegal!
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_subexpr_scope_dueToZeroSubexprAlt_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .ppr_decl("f"_str)
        .lpr("lpr0"_str)
        .string("a"_str)
        .close()
        .ppr("f"_str)
        .lookahead()
        .sequence()
        .name("lpr0"_str)
        .alternative()
        // zero subexprs, illegal!
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_single_subexpr_scope_dueToTwoSubexprAlt_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr_decl("lpr2"_str)
        .ppr_decl("f"_str)
        .lpr("lpr0"_str)
        .string("a"_str)
        .close()
        .lpr("lpr1"_str)
        .string("b"_str)
        .close()
        .lpr("lpr2"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .lookahead()
        .sequence()
        .name("lpr0"_str)
        .alternative()
        .name("lpr1"_str)
        .name("lpr2"_str) // two subexprs, illegal!
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_no_end_subexpr_scope_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .lookahead()
        .sequence()
        .end() // end subexpr, illegal!
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_end_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_in_no_end_subexpr_scope_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f"_str)
        .ppr("f"_str)
        .lookahead()
        .sequence()
        .end() // end subexpr, illegal!
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_end_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_ambiguity_noLeftRecursion_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .sequence()
        .charset("abc"_str) // prefixes: a, b, c
        .alternative()
        .name("lpr1"_str) // prefixes: a, b, c
        .close()
        .close()
        .lpr("lpr1"_str)
        .charset("abc"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_ambiguity_directLeftRecursion_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr("lpr0"_str)
        .sequence()
        .charset("abc"_str)
        .alternative()
        .name("lpr0"_str) // direct self-ref
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_ambiguity_directLeftRecursion_withSingleAlt_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr("lpr0"_str)
        .sequence()
        .name("lpr0"_str) // direct self-ref
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_ambiguity_indirectLeftRecursion_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .sequence()
        .charset("abc"_str)
        .alternative()
        .name("lpr1"_str) // indirect self-ref
        .close()
        .close()
        .lpr("lpr1"_str)
        .name("lpr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_ambiguity_noLeftRecursion_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("ppr0"_str)
        .sequence()
        .name("a"_str) // prefixes: a
        .alternative()
        .name("b"_str) // prefixes: b
        .alternative()
        .name("c"_str) // prefixes: c
        .alternative()
        .name("ppr1"_str) // prefixes: a, b, c
        .close()
        .close()
        .ppr("ppr1"_str)
        .sequence()
        .name("a"_str) // prefixes: a
        .alternative()
        .name("b"_str) // prefixes: b
        .alternative()
        .name("c"_str) // prefixes: c
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_ambiguity_directLeftRecursion_noPrecedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("ppr0"_str)
        .sequence()
        .name("a"_str)
        .alternative()
        .name("b"_str)
        .alternative()
        .name("c"_str)
        .alternative()
        .name("ppr0"_str) // direct self-ref
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_ambiguity_directLeftRecursion_withSingleAlt_noPrecedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("ppr0"_str)
        .sequence()
        .name("ppr0"_str) // direct self-ref
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_ambiguity_indirectLeftRecursion_noPrecedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("ppr0"_str)
        .sequence()
        .name("a"_str)
        .alternative()
        .name("b"_str)
        .alternative()
        .name("c"_str)
        .alternative()
        .name("ppr1"_str) // indirect self-ref
        .close()
        .close()
        .ppr("ppr1"_str)
        .name("ppr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_ambiguity_directLeftRecursion_precedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("ppr0"_str, taul::precedence)
        .sequence()
        .name("a"_str)
        .alternative()
        .name("b"_str)
        .alternative()
        .name("c"_str)
        .alternative()
        .name("ppr0"_str) // direct self-ref
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_ambiguity_directLeftRecursion_withSingleAlt_precedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("ppr0"_str, taul::precedence)
        .sequence()
        .name("ppr0"_str) // direct self-ref
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, sequence_forErr_illegal_ambiguity_indirectLeftRecursion_precedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a"_str)
        .lpr_decl("b"_str)
        .lpr_decl("c"_str)
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .lpr("a"_str)
        .string("a"_str)
        .close()
        .lpr("b"_str)
        .string("b"_str)
        .close()
        .lpr("c"_str)
        .string("c"_str)
        .close()
        .ppr("ppr0"_str, taul::precedence)
        .sequence()
        .name("a"_str)
        .alternative()
        .name("b"_str)
        .alternative()
        .name("c"_str)
        .alternative()
        .name("ppr1"_str) // indirect self-ref
        .close()
        .close()
        .ppr("ppr1"_str)
        .name("ppr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

// lookahead
//      illegal-in-no-scope
//      illegal-in-single-terminal-scope (w/ name) (for lprs)
//      illegal-in-single-terminal-scope (w/ lookahead) (for lprs)
//      illegal-in-single-terminal-scope (w/ lookahead-not) (for lprs)
//      illegal-in-single-terminal-scope (w/ not) (for lprs)
//      illegal-in-single-terminal-scope (w/ optional) (for lprs)
//      illegal-in-single-terminal-scope (w/ kleene-star) (for lprs)
//      illegal-in-single-terminal-scope (w/ kleene-plus) (for lprs)
//      illegal-in-single-terminal-scope (w/ name) (for pprs)
//      illegal-in-single-terminal-scope (w/ lookahead) (for pprs)
//      illegal-in-single-terminal-scope (w/ lookahead-not) (for pprs)
//      illegal-in-single-terminal-scope (w/ not) (for pprs)
//      illegal-in-single-terminal-scope (w/ optional) (for pprs)
//      illegal-in-single-terminal-scope (w/ kleene-star) (for pprs)
//      illegal-in-single-terminal-scope (w/ kleene-plus) (for pprs)
//      illegal-in-single-subexpr-scope (due to zero subexpr alt) (for lprs)
//      illegal-in-single-subexpr-scope (due to two subexpr alt) (for lprs)
//      illegal-in-single-subexpr-scope (due to zero subexpr alt) (for pprs)
//      illegal-in-single-subexpr-scope (due to two subexpr alt) (for pprs)
//      illegal-in-no-end-subexpr-scope (for lprs)
//      illegal-in-no-end-subexpr-scope (for pprs)

TEST(LoadTests, lookahead_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lookahead()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_terminal_scope_forName_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead()
        .name("lpr0"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_terminal_scope_forLookAhead_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead()
        .lookahead()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_terminal_scope_forLookAheadNot_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead()
        .lookahead_not()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_terminal_scope_forNot_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead()
        .not0()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_terminal_scope_forOptional_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead()
        .optional()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_terminal_scope_forKleeneStar_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead()
        .kleene_star()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_terminal_scope_forKleenePlus_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead()
        .kleene_plus()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_terminal_scope_forName_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead()
        .name("ppr0"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_terminal_scope_forLookAhead_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead()
        .lookahead()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_terminal_scope_forLookAheadNot_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead()
        .lookahead_not()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_terminal_scope_forNot_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead()
        .not0()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_terminal_scope_forOptional_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead()
        .optional()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_terminal_scope_forKleeneStar_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead()
        .kleene_star()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_terminal_scope_forKleenePlus_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead()
        .kleene_plus()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_subexpr_scope_dueToZeroSubexprAlt_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .lookahead()
        .string("a"_str)
        .alternative()
        // zero subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_subexpr_scope_dueToTwoSubexprAlt_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .lookahead()
        .string("a"_str)
        .alternative()
        .string("b"_str)
        .string("c"_str) // two subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_subexpr_scope_dueToZeroSubexprAlt_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .ppr_decl("f"_str)
        .lpr("lpr0"_str)
        .string("a"_str)
        .close()
        .ppr("f"_str)
        .lookahead()
        .name("lpr0"_str)
        .alternative()
        // zero subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_single_subexpr_scope_dueToTwoSubexprAlt_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr_decl("lpr2"_str)
        .ppr_decl("f"_str)
        .lpr("lpr0"_str)
        .string("a"_str)
        .close()
        .lpr("lpr1"_str)
        .string("b"_str)
        .close()
        .lpr("lpr2"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .lookahead()
        .name("lpr0"_str)
        .alternative()
        .name("lpr1"_str)
        .name("lpr2"_str) // two subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_no_end_subexpr_scope_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .lookahead()
        .end() // end subexpr, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_end_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_forErr_illegal_in_no_end_subexpr_scope_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f"_str)
        .ppr("f"_str)
        .lookahead()
        .end() // end subexpr, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_end_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

// lookahead-not
//      illegal-in-no-scope
//      illegal-in-single-terminal-scope (w/ name) (for lprs)
//      illegal-in-single-terminal-scope (w/ lookahead) (for lprs)
//      illegal-in-single-terminal-scope (w/ lookahead-not) (for lprs)
//      illegal-in-single-terminal-scope (w/ not) (for lprs)
//      illegal-in-single-terminal-scope (w/ optional) (for lprs)
//      illegal-in-single-terminal-scope (w/ kleene-star) (for lprs)
//      illegal-in-single-terminal-scope (w/ kleene-plus) (for lprs)
//      illegal-in-single-terminal-scope (w/ name) (for pprs)
//      illegal-in-single-terminal-scope (w/ lookahead) (for pprs)
//      illegal-in-single-terminal-scope (w/ lookahead-not) (for pprs)
//      illegal-in-single-terminal-scope (w/ not) (for pprs)
//      illegal-in-single-terminal-scope (w/ optional) (for pprs)
//      illegal-in-single-terminal-scope (w/ kleene-star) (for pprs)
//      illegal-in-single-terminal-scope (w/ kleene-plus) (for pprs)
//      illegal-in-single-subexpr-scope (due to zero subexpr alt) (for lprs)
//      illegal-in-single-subexpr-scope (due to two subexpr alt) (for lprs)
//      illegal-in-single-subexpr-scope (due to zero subexpr alt) (for pprs)
//      illegal-in-single-subexpr-scope (due to two subexpr alt) (for pprs)
//      illegal-in-no-end-subexpr-scope (for lprs)
//      illegal-in-no-end-subexpr-scope (for pprs)

TEST(LoadTests, lookahead_not_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lookahead_not()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_terminal_scope_forName_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead_not()
        .name("lpr0"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_terminal_scope_forLookAhead_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead_not()
        .lookahead()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_terminal_scope_forLookAheadNot_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead_not()
        .lookahead_not()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_terminal_scope_forNot_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead_not()
        .not0()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_terminal_scope_forOptional_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead_not()
        .optional()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_terminal_scope_forKleeneStar_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead_not()
        .kleene_star()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_terminal_scope_forKleenePlus_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .lookahead_not()
        .kleene_plus()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_terminal_scope_forName_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead_not()
        .name("ppr0"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_terminal_scope_forLookAhead_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead_not()
        .lookahead()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_terminal_scope_forLookAheadNot_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead_not()
        .lookahead_not()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_terminal_scope_forNot_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead_not()
        .not0()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_terminal_scope_forOptional_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead_not()
        .optional()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_terminal_scope_forKleeneStar_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead_not()
        .kleene_star()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_terminal_scope_forKleenePlus_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .lookahead_not()
        .kleene_plus()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_subexpr_scope_dueToZeroSubexprAlt_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .lookahead_not()
        .string("a"_str)
        .alternative()
        // zero subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_subexpr_scope_dueToTwoSubexprAlt_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .lookahead_not()
        .string("a"_str)
        .alternative()
        .string("b"_str)
        .string("c"_str) // two subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_subexpr_scope_dueToZeroSubexprAlt_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .ppr_decl("f"_str)
        .lpr("lpr0"_str)
        .string("a"_str)
        .close()
        .ppr("f"_str)
        .lookahead_not()
        .name("lpr0"_str)
        .alternative()
        // zero subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_single_subexpr_scope_dueToTwoSubexprAlt_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr_decl("lpr2"_str)
        .ppr_decl("f"_str)
        .lpr("lpr0"_str)
        .string("a"_str)
        .close()
        .lpr("lpr1"_str)
        .string("b"_str)
        .close()
        .lpr("lpr2"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .lookahead_not()
        .name("lpr0"_str)
        .alternative()
        .name("lpr1"_str)
        .name("lpr2"_str) // two subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_no_end_subexpr_scope_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .lookahead_not()
        .end() // end subexpr, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_end_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, lookahead_not_forErr_illegal_in_no_end_subexpr_scope_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f"_str)
        .ppr("f"_str)
        .lookahead_not()
        .end() // end subexpr, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_end_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

// not
//      illegal-in-no-scope
//      illegal-in-single-terminal-scope (w/ name) (for lprs)
//      illegal-in-single-terminal-scope (w/ lookahead) (for lprs)
//      illegal-in-single-terminal-scope (w/ lookahead-not) (for lprs)
//      illegal-in-single-terminal-scope (w/ not) (for lprs)
//      illegal-in-single-terminal-scope (w/ optional) (for lprs)
//      illegal-in-single-terminal-scope (w/ kleene-star) (for lprs)
//      illegal-in-single-terminal-scope (w/ kleene-plus) (for lprs)
//      illegal-in-single-terminal-scope (w/ name) (for pprs)
//      illegal-in-single-terminal-scope (w/ lookahead) (for pprs)
//      illegal-in-single-terminal-scope (w/ lookahead-not) (for pprs)
//      illegal-in-single-terminal-scope (w/ not) (for pprs)
//      illegal-in-single-terminal-scope (w/ optional) (for pprs)
//      illegal-in-single-terminal-scope (w/ kleene-star) (for pprs)
//      illegal-in-single-terminal-scope (w/ kleene-plus) (for pprs)
//      illegal-in-single-subexpr-scope (due to zero subexpr alt) (for lprs)
//      illegal-in-single-subexpr-scope (due to two subexpr alt) (for lprs)
//      illegal-in-single-subexpr-scope (due to zero subexpr alt) (for pprs)
//      illegal-in-single-subexpr-scope (due to two subexpr alt) (for pprs)
//      illegal-in-no-end-subexpr-scope (for lprs)
//      illegal-in-no-end-subexpr-scope (for pprs)

TEST(LoadTests, not_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .not0()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_terminal_scope_forName_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .not0()
        .name("lpr0"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_terminal_scope_forLookAhead_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .not0()
        .lookahead()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_terminal_scope_forLookAheadNot_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .not0()
        .lookahead_not()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_terminal_scope_forNot_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .not0()
        .not0()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_terminal_scope_forOptional_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .not0()
        .optional()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_terminal_scope_forKleeneStar_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .not0()
        .kleene_star()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_terminal_scope_forKleenePlus_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .not0()
        .kleene_plus()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_terminal_scope_forName_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .not0()
        .name("ppr0"_str)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_terminal_scope_forLookAhead_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .not0()
        .lookahead()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_terminal_scope_forLookAheadNot_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .not0()
        .lookahead_not()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_terminal_scope_forNot_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .not0()
        .not0()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_terminal_scope_forOptional_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .not0()
        .optional()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_terminal_scope_forKleeneStar_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .not0()
        .kleene_star()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_terminal_scope_forKleenePlus_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .not0()
        .kleene_plus()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_subexpr_scope_dueToZeroSubexprAlt_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .not0()
        .string("a"_str)
        .alternative()
        // zero subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_subexpr_scope_dueToTwoSubexprAlt_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .not0()
        .string("a"_str)
        .alternative()
        .string("b"_str)
        .string("c"_str) // two subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_subexpr_scope_dueToZeroSubexprAlt_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .ppr_decl("f"_str)
        .lpr("lpr0"_str)
        .string("a"_str)
        .close()
        .ppr("f"_str)
        .not0()
        .name("lpr0"_str)
        .alternative()
        // zero subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_single_subexpr_scope_dueToTwoSubexprAlt_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr_decl("lpr2"_str)
        .ppr_decl("f"_str)
        .lpr("lpr0"_str)
        .string("a"_str)
        .close()
        .lpr("lpr1"_str)
        .string("b"_str)
        .close()
        .lpr("lpr2"_str)
        .string("c"_str)
        .close()
        .ppr("f"_str)
        .not0()
        .name("lpr0"_str)
        .alternative()
        .name("lpr1"_str)
        .name("lpr2"_str) // two subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_no_end_subexpr_scope_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .not0()
        .end() // end subexpr, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_end_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, not_forErr_illegal_in_no_end_subexpr_scope_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f"_str)
        .ppr("f"_str)
        .not0()
        .end() // end subexpr, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_end_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

// optional
//      illegal-in-no-scope
//      illegal-in-no-alternation-scope (for lprs)
//      illegal-in-no-alternation-scope (for pprs)
//      illegal-in-single-subexpr-scope (due to zero subexpr alt) (for lprs)
//      illegal-in-single-subexpr-scope (due to two subexpr alt) (for lprs)
//      illegal-in-single-subexpr-scope (due to zero subexpr alt) (for pprs)
//      illegal-in-single-subexpr-scope (due to two subexpr alt) (for pprs)
//      illegal-ambiguity (direct left-recursion) (for lprs)
//      illegal-ambiguity (indirect left-recursion) (for lprs)
//      illegal-ambiguity (direct left-recursion) (no precedence) (for pprs)
//      illegal-ambiguity (indirect left-recursion) (no precedence) (for pprs)
//      illegal-ambiguity (direct left-recursion) (precedence) (for pprs)
//      illegal-ambiguity (indirect left-recursion) (precedence) (for pprs)

TEST(LoadTests, optional_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .optional()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, optional_forErr_illegal_in_no_alternation_scope_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .optional()
        .string("abc")
        .alternative()
        .string("def") // alternation, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_alternation_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, optional_forErr_illegal_in_no_alternation_scope_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .ppr_decl("f"_str)
        .lpr("lpr0"_str)
        .string("abc"_str)
        .close()
        .lpr("lpr1"_str)
        .string("def"_str)
        .close()
        .ppr("f"_str)
        .optional()
        .name("lpr0"_str)
        .alternative()
        .name("lpr1"_str) // alternation, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_alternation_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, optional_forErr_illegal_in_single_subexpr_scope_dueToZeroSubexprAlt_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .optional()
        // zero subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, optional_forErr_illegal_in_single_subexpr_scope_dueToTwoSubexprAlt_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .optional()
        .string("a"_str)
        .string("b"_str) // two subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, optional_forErr_illegal_in_single_subexpr_scope_dueToZeroSubexprAlt_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f"_str)
        .ppr("f"_str)
        .optional()
        // zero subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, optional_forErr_illegal_in_single_subexpr_scope_dueToTwoSubexprAlt_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .ppr_decl("f"_str)
        .lpr("lpr0"_str)
        .string("a"_str)
        .close()
        .lpr("lpr1"_str)
        .string("b"_str)
        .close()
        .ppr("f"_str)
        .optional()
        .name("lpr0"_str)
        .name("lpr1"_str) // two subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, optional_forErr_illegal_ambiguity_directLeftRecursion_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr("lpr0"_str)
        .optional() // <- under test
        .name("lpr0"_str) // direct self-ref
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, optional_forErr_illegal_ambiguity_indirectLeftRecursion_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .optional() // <- under test
        .name("lpr1"_str) // indirect self-ref
        .close()
        .close()
        .lpr("lpr1"_str)
        .name("lpr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, optional_forErr_illegal_ambiguity_directLeftRecursion_noPrecedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr("ppr0"_str)
        .optional() // <- under test
        .name("ppr0"_str) // direct self-ref
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, optional_forErr_illegal_ambiguity_indirectLeftRecursion_noPrecedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .optional() // <- under test
        .name("ppr1"_str) // indirect self-ref
        .close()
        .close()
        .ppr("ppr1"_str)
        .name("ppr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, optional_forErr_illegal_ambiguity_directLeftRecursion_precedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr("ppr0"_str, taul::precedence)
        .optional() // <- under test
        .name("ppr0"_str) // direct self-ref
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, optional_forErr_illegal_ambiguity_indirectLeftRecursion_precedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str, taul::precedence)
        .optional() // <- under test
        .name("ppr1"_str) // indirect self-ref
        .close()
        .close()
        .ppr("ppr1"_str)
        .name("ppr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

// kleene-star
//      illegal-in-no-scope
//      illegal-in-no-alternation-scope (for lprs)
//      illegal-in-no-alternation-scope (for pprs)
//      illegal-in-single-subexpr-scope (due to zero subexpr alt) (for lprs)
//      illegal-in-single-subexpr-scope (due to two subexpr alt) (for lprs)
//      illegal-in-single-subexpr-scope (due to zero subexpr alt) (for pprs)
//      illegal-in-single-subexpr-scope (due to two subexpr alt) (for pprs)
//      illegal-ambiguity (direct left-recursion) (for lprs)
//      illegal-ambiguity (indirect left-recursion) (for lprs)
//      illegal-ambiguity (direct left-recursion) (no precedence) (for pprs)
//      illegal-ambiguity (indirect left-recursion) (no precedence) (for pprs)
//      illegal-ambiguity (direct left-recursion) (precedence) (for pprs)
//      illegal-ambiguity (indirect left-recursion) (precedence) (for pprs)

TEST(LoadTests, kleene_star_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .kleene_star()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_star_forErr_illegal_in_no_alternation_scope_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .kleene_star()
        .string("abc")
        .alternative()
        .string("def") // alternation, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_alternation_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_star_forErr_illegal_in_no_alternation_scope_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .ppr_decl("f"_str)
        .lpr("lpr0"_str)
        .string("abc"_str)
        .close()
        .lpr("lpr1"_str)
        .string("def"_str)
        .close()
        .ppr("f"_str)
        .kleene_star()
        .name("lpr0"_str)
        .alternative()
        .name("lpr1"_str) // alternation, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_alternation_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_star_forErr_illegal_in_single_subexpr_scope_dueToZeroSubexprAlt_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .kleene_star()
        // zero subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_star_forErr_illegal_in_single_subexpr_scope_dueToTwoSubexprAlt_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .kleene_star()
        .string("a"_str)
        .string("b"_str) // two subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_star_forErr_illegal_in_single_subexpr_scope_dueToZeroSubexprAlt_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f"_str)
        .ppr("f"_str)
        .kleene_star()
        // zero subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_star_forErr_illegal_in_single_subexpr_scope_dueToTwoSubexprAlt_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .ppr_decl("f"_str)
        .lpr("lpr0"_str)
        .string("a"_str)
        .close()
        .lpr("lpr1"_str)
        .string("b"_str)
        .close()
        .ppr("f"_str)
        .kleene_star()
        .name("lpr0"_str)
        .name("lpr1"_str) // two subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_star_forErr_illegal_ambiguity_directLeftRecursion_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr("lpr0"_str)
        .kleene_star() // <- under test
        .name("lpr0"_str) // direct self-ref
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_star_forErr_illegal_ambiguity_indirectLeftRecursion_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .kleene_star() // <- under test
        .name("lpr1"_str) // indirect self-ref
        .close()
        .close()
        .lpr("lpr1"_str)
        .name("lpr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_star_forErr_illegal_ambiguity_directLeftRecursion_noPrecedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr("ppr0"_str)
        .kleene_star() // <- under test
        .name("ppr0"_str) // direct self-ref
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_star_forErr_illegal_ambiguity_indirectLeftRecursion_noPrecedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .kleene_star() // <- under test
        .name("ppr1"_str) // indirect self-ref
        .close()
        .close()
        .ppr("ppr1"_str)
        .name("ppr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_star_forErr_illegal_ambiguity_directLeftRecursion_precedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr("ppr0"_str, taul::precedence)
        .kleene_star() // <- under test
        .name("ppr0"_str) // direct self-ref
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_star_forErr_illegal_ambiguity_indirectLeftRecursion_precedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str, taul::precedence)
        .kleene_star() // <- under test
        .name("ppr1"_str) // indirect self-ref
        .close()
        .close()
        .ppr("ppr1"_str)
        .name("ppr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

// kleene-plus
//      illegal-in-no-scope
//      illegal-in-no-alternation-scope (for lprs)
//      illegal-in-no-alternation-scope (for pprs)
//      illegal-in-single-subexpr-scope (due to zero subexpr alt) (for lprs)
//      illegal-in-single-subexpr-scope (due to two subexpr alt) (for lprs)
//      illegal-in-single-subexpr-scope (due to zero subexpr alt) (for pprs)
//      illegal-in-single-subexpr-scope (due to two subexpr alt) (for pprs)
//      illegal-ambiguity (direct left-recursion) (for lprs)
//      illegal-ambiguity (indirect left-recursion) (for lprs)
//      illegal-ambiguity (direct left-recursion) (no precedence) (for pprs)
//      illegal-ambiguity (indirect left-recursion) (no precedence) (for pprs)
//      illegal-ambiguity (direct left-recursion) (precedence) (for pprs)
//      illegal-ambiguity (indirect left-recursion) (precedence) (for pprs)

TEST(LoadTests, kleene_plus_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .kleene_plus()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_plus_forErr_illegal_in_no_alternation_scope_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .kleene_plus()
        .string("abc")
        .alternative()
        .string("def") // alternation, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_alternation_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_plus_forErr_illegal_in_no_alternation_scope_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .ppr_decl("f"_str)
        .lpr("lpr0"_str)
        .string("abc"_str)
        .close()
        .lpr("lpr1"_str)
        .string("def"_str)
        .close()
        .ppr("f"_str)
        .kleene_plus()
        .name("lpr0"_str)
        .alternative()
        .name("lpr1"_str) // alternation, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_alternation_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_plus_forErr_illegal_in_single_subexpr_scope_dueToZeroSubexprAlt_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .kleene_plus()
        // zero subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_plus_forErr_illegal_in_single_subexpr_scope_dueToTwoSubexprAlt_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f"_str)
        .lpr("f"_str)
        .kleene_plus()
        .string("a"_str)
        .string("b"_str) // two subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_plus_forErr_illegal_in_single_subexpr_scope_dueToZeroSubexprAlt_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f"_str)
        .ppr("f"_str)
        .kleene_plus()
        // zero subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_plus_forErr_illegal_in_single_subexpr_scope_dueToTwoSubexprAlt_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .ppr_decl("f"_str)
        .lpr("lpr0"_str)
        .string("a"_str)
        .close()
        .lpr("lpr1"_str)
        .string("b"_str)
        .close()
        .ppr("f"_str)
        .kleene_plus()
        .name("lpr0"_str)
        .name("lpr1"_str) // two subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_subexpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_plus_forErr_illegal_ambiguity_directLeftRecursion_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr("lpr0"_str)
        .kleene_plus() // <- under test
        .name("lpr0"_str) // direct self-ref
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_plus_forErr_illegal_ambiguity_indirectLeftRecursion_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .kleene_plus() // <- under test
        .name("lpr1"_str) // indirect self-ref
        .close()
        .close()
        .lpr("lpr1"_str)
        .name("lpr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_plus_forErr_illegal_ambiguity_directLeftRecursion_noPrecedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr("ppr0"_str)
        .kleene_plus() // <- under test
        .name("ppr0"_str) // direct self-ref
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_plus_forErr_illegal_ambiguity_indirectLeftRecursion_noPrecedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .kleene_plus() // <- under test
        .name("ppr1"_str) // indirect self-ref
        .close()
        .close()
        .ppr("ppr1"_str)
        .name("ppr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_plus_forErr_illegal_ambiguity_directLeftRecursion_precedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr("ppr0"_str, taul::precedence)
        .kleene_plus() // <- under test
        .name("ppr0"_str) // direct self-ref
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

TEST(LoadTests, kleene_plus_forErr_illegal_ambiguity_indirectLeftRecursion_precedence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str, taul::precedence)
        .kleene_plus() // <- under test
        .name("ppr1"_str) // indirect self-ref
        .close()
        .close()
        .ppr("ppr1"_str)
        .name("ppr0"_str)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_GE(ec.total(), 1); // <- remember, we don't impose rule that it need only raise 1
    EXPECT_GE(ec.count(taul::spec_error::illegal_ambiguity), 1); // we'll just allow multiple, and not care how many
    EXPECT_FALSE(gram);
}

