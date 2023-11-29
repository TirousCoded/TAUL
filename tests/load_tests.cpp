

#include <gtest/gtest.h>

#include <taul/all.h>


// we'll have a few *summary* tests for successful usage of spec instructions,
// and a number of tests for each of the different errors which could arise
// for a particular instruction

// for failure tests, the implementation is free to decide when evaluation 
// should be aborted such that later errors do not arise, w/ each failure unit 
// test thus only asserting a single instance of an error arising

// these test will NOT test usage of loaded grammar lexers/parser, only that 
// they load successful/unsuccessfully as expected


// this test is for general usage of *all* instructions, so be sure to
// update it as we add new instructions

static_assert(taul::spec_opcodes == 19);

TEST(load_tests, success) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .grammar_bias(taul::bias::last_longest)
        // gonna test successful usage of qualifiers here
        .lpr_decl("with_none_qualifier")
        .lpr_decl("with_skip_qualifier")
        .lpr_decl("with_exclude_qualifier")
        .lpr("with_none_qualifier", taul::qualifier::none)
        .close()
        .lpr("with_skip_qualifier", taul::qualifier::skip)
        .close()
        .lpr("with_exclude_qualifier", taul::qualifier::exclude)
        .close()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .lpr("lpr0")
        // test simple lexer exprs
        .begin()
        .end()
        .any()
        .string("abc")
        .charset("abc")
        .name("lpr0") // legal lpr ref
        .name("lpr1") // legal lpr ref
        // test composite lexer exprs
        // sequence
        .sequence() // test empty
        .close() // test empty (end)
        .sequence() // test w/ nested
        .any()
        .string("abc")
        .sequence() // test w/ nested sequence
        .any()
        .close() // test w/ nested sequence (end)
        .set() // test w/ nested set
        .any()
        .close() // test w/ nested set (end)
        .modifier(1, 3) // test w/ nested modifier
        .any()
        .close() // test w/ nested modifier (end)
        .assertion(taul::polarity::negative) // test w/ nested assertion
        .any()
        .close() // test w/ nested assertion (end)
        .constraint(taul::polarity::negative) // test w/ nested constraint #1
        .any()
        .junction()
        .sequence()
        .localend()
        .string("a")
        .close()
        .close() // test w/ nested constraint #1 (end)
        .constraint() // test w/ nested constraint #2
        .sequence()
        .any()
        .close()
        .junction()
        .string("a")
        .close() // test w/ nested constraint #2 (end)
        .close() // test w/ nested (end)
        // set
        .set(taul::bias::last_longest) // test empty
        .close() // test empty (end)
        .set() // test w/ nested
        .any()
        .string("abc")
        .sequence() // test w/ nested sequence
        .any()
        .close() // test w/ nested sequence (end)
        .set() // test w/ nested set
        .any()
        .close() // test w/ nested set (end)
        .modifier(1, 3) // test w/ nested modifier
        .any()
        .close() // test w/ nested modifier (end)
        .assertion(taul::polarity::negative) // test w/ nested assertion
        .any()
        .close() // test w/ nested assertion (end)
        .constraint(taul::polarity::negative) // test w/ nested constraint #1
        .any()
        .junction()
        .sequence()
        .localend()
        .string("a")
        .close()
        .close() // test w/ nested constraint #1 (end)
        .constraint() // test w/ nested constraint #2
        .sequence()
        .any()
        .close()
        .junction()
        .string("a")
        .close() // test w/ nested constraint #2 (end)
        .close() // test w/ nested (end)
        // modifier
        .modifier(1, 1) // test w/ nested #1
        .any()
        .close() // test w/ nested #1 (end)
        .modifier(1, 1) // test w/ nested #2
        .string("abc")
        .close() // test w/ nested #2 (end)
        .modifier(1, 1) // test w/ nested #3
        .sequence() // test w/ nested sequence
        .any()
        .close() // test w/ nested sequence (end)
        .close() // test w/ nested #3 (end)
        .modifier(1, 1) // test w/ nested #4
        .set() // test w/ nested set
        .any()
        .close() // test w/ nested set (end)
        .close() // test w/ nested #4 (end)
        .modifier(1, 1) // test w/ nested #5
        .modifier(1, 3) // test w/ nested modifier
        .any()
        .close() // test w/ nested modifier (end)
        .close() // test w/ nested #5 (end)
        .modifier(1, 1) // test w/ nested #6
        .assertion(taul::polarity::negative) // test w/ nested assertion
        .any()
        .close() // test w/ nested assertion (end)
        .close() // test w/ nested #6 (end)
        .modifier(1, 1) // test w/ nested #7
        .constraint(taul::polarity::negative) // test w/ nested constraint #1
        .any()
        .junction()
        .sequence()
        .localend()
        .string("a")
        .close()
        .close() // test w/ nested constraint #1 (end)
        .close() // test w/ nested #7 (end)
        .modifier(1, 1) // test w/ nested #8
        .constraint() // test w/ nested constraint #2
        .sequence()
        .any()
        .close()
        .junction()
        .string("a")
        .close() // test w/ nested constraint #2 (end)
        .close() // test w/ nested #8 (end)
        // assertion
        .assertion(taul::polarity::positive) // test w/ nested #1
        .any()
        .close() // test w/ nested #1 (end)
        .assertion(taul::polarity::positive) // test w/ nested #2
        .string("abc")
        .close() // test w/ nested #2 (end)
        .assertion(taul::polarity::positive) // test w/ nested #3
        .sequence() // test w/ nested sequence
        .any()
        .close() // test w/ nested sequence (end)
        .close() // test w/ nested #3 (end)
        .assertion(taul::polarity::positive) // test w/ nested #4
        .set() // test w/ nested set
        .any()
        .close() // test w/ nested set (end)
        .close() // test w/ nested #4 (end)
        .assertion(taul::polarity::positive) // test w/ nested #5
        .modifier(1, 3) // test w/ nested modifier
        .any()
        .close() // test w/ nested modifier (end)
        .close() // test w/ nested #5 (end)
        .assertion(taul::polarity::positive) // test w/ nested #6
        .assertion(taul::polarity::negative) // test w/ nested assertion
        .any()
        .close() // test w/ nested assertion (end)
        .close() // test w/ nested #6 (end)
        .assertion(taul::polarity::positive) // test w/ nested #7
        .constraint(taul::polarity::negative) // test w/ nested constraint #1
        .any()
        .junction()
        .sequence()
        .localend()
        .string("a")
        .close()
        .close() // test w/ nested constraint #1 (end)
        .close() // test w/ nested #7 (end)
        .assertion(taul::polarity::positive) // test w/ nested #8
        .constraint() // test w/ nested constraint #2
        .sequence()
        .any()
        .close()
        .junction()
        .string("a")
        .close() // test w/ nested constraint #2 (end)
        .close() // test w/ nested #8 (end)
        .close()
        .lpr("lpr1") // test empty
        .close()
        .ppr("ppr0")
        //TODO: add the below later
        //// test simple parser exprs
        //.begin()
        //.end()
        //.any()
        //.string("abc")
        //.charset("abc")
        //.name("lpr0") // legal lpr ref
        //.name("lpr1") // legal lpr ref
        //.name("ppr0") // legal ppr ref
        //.name("ppr1") // legal ppr ref
        //// test composite parser exprs
        //// sequence
        //.sequence() // test empty
        //.close() // test empty (end)
        //.sequence() // test w/ nested
        //.any()
        //.string("abc")
        //.sequence() // test w/ nested sequence
        //.any()
        //.close() // test w/ nested sequence (end)
        //.set() // test w/ nested set
        //.any()
        //.close() // test w/ nested set (end)
        //.modifier(1, 3) // test w/ nested modifier
        //.any()
        //.close() // test w/ nested modifier (end)
        //.assertion(taul::polarity::negative) // test w/ nested assertion
        //.any()
        //.close() // test w/ nested assertion (end)
        //.constraint(taul::polarity::negative) // test w/ nested constraint #1
        //.any()
        //.junction()
        //.sequence()
        //.localend()
        //.string("a")
        //.close()
        //.close() // test w/ nested constraint #1 (end)
        //.constraint() // test w/ nested constraint #2
        //.sequence()
        //.any()
        //.close()
        //.junction()
        //.string("a")
        //.close() // test w/ nested constraint #2 (end)
        //.close() // test w/ nested (end)
        //// set
        //.set(taul::bias::last_longest) // test empty
        //.close() // test empty (end)
        //.set() // test w/ nested
        //.any()
        //.string("abc")
        //.sequence() // test w/ nested sequence
        //.any()
        //.close() // test w/ nested sequence (end)
        //.set() // test w/ nested set
        //.any()
        //.close() // test w/ nested set (end)
        //.modifier(1, 3) // test w/ nested modifier
        //.any()
        //.close() // test w/ nested modifier (end)
        //.assertion(taul::polarity::negative) // test w/ nested assertion
        //.any()
        //.close() // test w/ nested assertion (end)
        //.constraint(taul::polarity::negative) // test w/ nested constraint #1
        //.any()
        //.junction()
        //.sequence()
        //.localend()
        //.string("a")
        //.close()
        //.close() // test w/ nested constraint #1 (end)
        //.constraint() // test w/ nested constraint #2
        //.sequence()
        //.any()
        //.close()
        //.junction()
        //.string("a")
        //.close() // test w/ nested constraint #2 (end)
        //.close() // test w/ nested (end)
        //// modifier
        //.modifier(1, 1) // test w/ nested #1
        //.any()
        //.close() // test w/ nested #1 (end)
        //.modifier(1, 1) // test w/ nested #2
        //.string("abc")
        //.close() // test w/ nested #2 (end)
        //.modifier(1, 1) // test w/ nested #3
        //.sequence() // test w/ nested sequence
        //.any()
        //.close() // test w/ nested sequence (end)
        //.close() // test w/ nested #3 (end)
        //.modifier(1, 1) // test w/ nested #4
        //.set() // test w/ nested set
        //.any()
        //.close() // test w/ nested set (end)
        //.close() // test w/ nested #4 (end)
        //.modifier(1, 1) // test w/ nested #5
        //.modifier(1, 3) // test w/ nested modifier
        //.any()
        //.close() // test w/ nested modifier (end)
        //.close() // test w/ nested #5 (end)
        //.modifier(1, 1) // test w/ nested #6
        //.assertion(taul::polarity::negative) // test w/ nested assertion
        //.any()
        //.close() // test w/ nested assertion (end)
        //.close() // test w/ nested #6 (end)
        //.modifier(1, 1) // test w/ nested #7
        //.constraint(taul::polarity::negative) // test w/ nested constraint #1
        //.any()
        //.junction()
        //.sequence()
        //.localend()
        //.string("a")
        //.close()
        //.close() // test w/ nested constraint #1 (end)
        //.close() // test w/ nested #7 (end)
        //.modifier(1, 1) // test w/ nested #8
        //.constraint() // test w/ nested constraint #2
        //.sequence()
        //.any()
        //.close()
        //.junction()
        //.string("a")
        //.close() // test w/ nested constraint #2 (end)
        //.close() // test w/ nested #8 (end)
        //// assertion
        //.assertion(taul::polarity::positive) // test w/ nested #1
        //.any()
        //.close() // test w/ nested #1 (end)
        //.assertion(taul::polarity::positive) // test w/ nested #2
        //.string("abc")
        //.close() // test w/ nested #2 (end)
        //.assertion(taul::polarity::positive) // test w/ nested #3
        //.sequence() // test w/ nested sequence
        //.any()
        //.close() // test w/ nested sequence (end)
        //.close() // test w/ nested #3 (end)
        //.assertion(taul::polarity::positive) // test w/ nested #4
        //.set() // test w/ nested set
        //.any()
        //.close() // test w/ nested set (end)
        //.close() // test w/ nested #4 (end)
        //.assertion(taul::polarity::positive) // test w/ nested #5
        //.modifier(1, 3) // test w/ nested modifier
        //.any()
        //.close() // test w/ nested modifier (end)
        //.close() // test w/ nested #5 (end)
        //.assertion(taul::polarity::positive) // test w/ nested #6
        //.assertion(taul::polarity::negative) // test w/ nested assertion
        //.any()
        //.close() // test w/ nested assertion (end)
        //.close() // test w/ nested #6 (end)
        //.assertion(taul::polarity::positive) // test w/ nested #7
        //.constraint(taul::polarity::negative) // test w/ nested constraint #1
        //.any()
        //.junction()
        //.sequence()
        //.localend()
        //.string("a")
        //.close()
        //.close() // test w/ nested constraint #1 (end)
        //.close() // test w/ nested #7 (end)
        //.assertion(taul::polarity::positive) // test w/ nested #8
        //.constraint() // test w/ nested constraint #2
        //.sequence()
        //.any()
        //.close()
        //.junction()
        //.string("a")
        //.close() // test w/ nested constraint #2 (end)
        //.close() // test w/ nested #8 (end)
        .close()
        .ppr("ppr1") // test empty
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);

        EXPECT_EQ(gram->bias(), taul::bias::last_longest);

        if (gram->contains_lpr("with_none_qualifier")) {
            EXPECT_EQ(gram->lpr("with_none_qualifier").name, "with_none_qualifier");
            EXPECT_EQ(gram->lpr("with_none_qualifier").index, 0);
            EXPECT_EQ(gram->lpr("with_none_qualifier").qualifer, taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->contains_lpr("with_skip_qualifier")) {
            EXPECT_EQ(gram->lpr("with_skip_qualifier").name, "with_skip_qualifier");
            EXPECT_EQ(gram->lpr("with_skip_qualifier").index, 1);
            EXPECT_EQ(gram->lpr("with_skip_qualifier").qualifer, taul::qualifier::skip);
        }
        else ADD_FAILURE();
        if (gram->contains_lpr("with_exclude_qualifier")) {
            EXPECT_EQ(gram->lpr("with_exclude_qualifier").name, "with_exclude_qualifier");
            EXPECT_EQ(gram->lpr("with_exclude_qualifier").index, 2);
            EXPECT_EQ(gram->lpr("with_exclude_qualifier").qualifer, taul::qualifier::exclude);
        }
        else ADD_FAILURE();
        if (gram->contains_lpr("lpr0")) {
            EXPECT_EQ(gram->lpr("lpr0").name, "lpr0");
            EXPECT_EQ(gram->lpr("lpr0").index, 3);
            EXPECT_EQ(gram->lpr("lpr0").qualifer, taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->contains_lpr("lpr1")) {
            EXPECT_EQ(gram->lpr("lpr1").name, "lpr1");
            EXPECT_EQ(gram->lpr("lpr1").index, 4);
            EXPECT_EQ(gram->lpr("lpr1").qualifer, taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->contains_ppr("ppr0")) {
            EXPECT_EQ(gram->ppr("ppr0").name, "ppr0");
            EXPECT_EQ(gram->ppr("ppr0").index, 0);
        }
        else ADD_FAILURE();
        if (gram->contains_ppr("ppr1")) {
            EXPECT_EQ(gram->ppr("ppr1").name, "ppr1");
            EXPECT_EQ(gram->ppr("ppr1").index, 1);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}

// this tests that an empty spec loads an empty grammar successfully

TEST(load_tests, success_withEmptyGrammarSpec) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);

        EXPECT_EQ(gram->bias(), taul::bias::first_longest);
        EXPECT_TRUE(gram->lprs().empty());
        EXPECT_TRUE(gram->pprs().empty());
    }
    else ADD_FAILURE();
}

// this tests that later grammar-bias instructions overwrite earlier ones

TEST(load_tests, success_withExpectedGrammarBiasOverwriting) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .grammar_bias(taul::bias::first_shortest)
        .grammar_bias(taul::bias::last_longest)
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);

        EXPECT_EQ(gram->bias(), taul::bias::last_longest);
        EXPECT_TRUE(gram->lprs().empty());
        EXPECT_TRUE(gram->pprs().empty());
    }
    else ADD_FAILURE();
}

// this tests that name works w/ lprs/pprs defined AFTER name usage

TEST(load_tests, success_withNameUsageForLPRsAndPPRsDefinedAfterNameUsage) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .ppr_decl("ppr0")
        .lpr_decl("lpr1")
        .ppr_decl("ppr1")
        .lpr("lpr0")
        .name("lpr1")
        .close()
        .ppr("ppr0")
        //.name("ppr1") TODO: add later
        .close()
        .lpr("lpr1")
        .close()
        .ppr("ppr1")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);

        EXPECT_EQ(gram->bias(), taul::bias::first_longest);

        if (gram->contains_lpr("lpr0")) {
            EXPECT_EQ(gram->lpr("lpr0").name, "lpr0");
            EXPECT_EQ(gram->lpr("lpr0").index, 0);
            EXPECT_EQ(gram->lpr("lpr0").qualifer, taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->contains_lpr("lpr1")) {
            EXPECT_EQ(gram->lpr("lpr1").name, "lpr1");
            EXPECT_EQ(gram->lpr("lpr1").index, 1);
            EXPECT_EQ(gram->lpr("lpr1").qualifer, taul::qualifier::none);
        }
        else ADD_FAILURE();
        if (gram->contains_ppr("ppr0")) {
            EXPECT_EQ(gram->ppr("ppr0").name, "ppr0");
            EXPECT_EQ(gram->ppr("ppr0").index, 0);
        }
        else ADD_FAILURE();
        if (gram->contains_ppr("ppr1")) {
            EXPECT_EQ(gram->ppr("ppr1").name, "ppr1");
            EXPECT_EQ(gram->ppr("ppr1").index, 1);
        }
        else ADD_FAILURE();
    }
    else ADD_FAILURE();
}


// the below detail what errors each instruction can raise, and thus which must
// be unit tested, and being specified *in order*

static_assert(taul::spec_errors == 17);

// grammar-bias
//      illegal-in-lpr-scope
//      illegal-in-ppr-scope

TEST(load_tests, grammar_bias_forErr_illegal_in_lpr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr("lpr0")
        .grammar_bias(taul::bias::longest)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_lpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, grammar_bias_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .grammar_bias(taul::bias::longest)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

// close                        <- most will be associated w/ opening instruction instead
//      stray-close

TEST(load_tests, close_forErr_stray_close) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::stray_close), 1);
    EXPECT_FALSE(gram);
}

// lpr-decl
//      rule-name-conflict (w/ lpr)
//      rule-name-conflict (w/ ppr)
//      rule-never-defined
//      illegal-in-lpr-scope
//      illegal-in-ppr-scope

TEST(load_tests, lpr_decl_forErr_rule_name_conflict_withLPR) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr0")
        .lpr("lpr0")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_name_conflict), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lpr_decl_forErr_rule_name_conflict_withPPR) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .lpr_decl("ppr0")
        .ppr("ppr0")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_name_conflict), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lpr_decl_forErr_rule_never_defined) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_never_defined), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lpr_decl_forErr_illegal_in_lpr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr("lpr0")
        .lpr_decl("lpr1")
        .close()
        .lpr("lpr1")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_lpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lpr_decl_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .lpr_decl("lpr1")
        .close()
        .lpr("lpr1")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

// ppr-decl
//      rule-name-conflict (w/ lpr)
//      rule-name-conflict (w/ ppr)
//      rule-never-defined
//      illegal-in-lpr-scope
//      illegal-in-ppr-scope

TEST(load_tests, ppr_decl_forErr_rule_name_conflict_withLPR) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .ppr_decl("lpr0")
        .lpr("lpr0")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_name_conflict), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, ppr_decl_forErr_rule_name_conflict_withPPR) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_name_conflict), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, ppr_decl_forErr_rule_never_defined) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_never_defined), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, ppr_decl_forErr_illegal_in_lpr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr("lpr0")
        .ppr_decl("ppr1")
        .close()
        .ppr("ppr1")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_lpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, ppr_decl_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .ppr_decl("ppr1")
        .close()
        .ppr("ppr1")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

// lpr
//      scope-not-closed
//      rule-never-declared
//      illegal-in-lpr-scope
//      illegal-in-ppr-scope

TEST(load_tests, lpr_forErr_scope_not_closed) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr("lpr0")
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::scope_not_closed), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lpr_forErr_rule_never_declared) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr("lpr0")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_never_declared), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lpr_forErr_illegal_in_lpr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .lpr("lpr1")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_lpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lpr_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .lpr_decl("lpr1")
        .ppr("ppr0")
        .lpr("lpr1")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

// ppr
//      scope-not-closed
//      rule-never-declared
//      illegal-in-lpr-scope
//      illegal-in-ppr-scope

TEST(load_tests, ppr_forErr_scope_not_closed) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::scope_not_closed), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, ppr_forErr_rule_never_declared) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr("ppr0")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_never_declared), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, ppr_forErr_illegal_in_lpr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .ppr_decl("ppr1")
        .lpr("lpr0")
        .ppr("ppr1")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_lpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, ppr_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .ppr("ppr1")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

// begin
//      illegal-in-ppr-scope
//      illegal-in-no-scope

TEST(load_tests, begin_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .begin()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, begin_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .begin()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

// end
//      illegal-in-ppr-scope
//      illegal-in-no-scope

TEST(load_tests, end_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .end()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, end_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .end()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

// any
//      illegal-in-ppr-scope
//      illegal-in-no-scope

TEST(load_tests, any_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .any()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, any_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .any()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

// string
//      illegal-in-ppr-scope
//      illegal-in-no-scope

TEST(load_tests, string_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .string("abc")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, string_seq_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .string("abc")
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

// charset
//      illegal-in-ppr-scope
//      illegal-in-no-scope

TEST(load_tests, charset_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .charset("abc")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, charset_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .charset("abc")
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

// TODO: when we add parsers, we'll need to have different versions of below tests for
//       name usages in lprs and pprs

// name
//      illegal-in-ppr-scope
//      illegal-in-no-scope
//      rule-not-found (rule never declared)
//      rule-not-found (rule declared after name referencing it) (for lprs)

TEST(load_tests, name_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .ppr_decl("ppr0")
        .lpr("lpr0")
        .close()
        .ppr("ppr0")
        .name("lpr0")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, name_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr("lpr0")
        .close()
        .name("lpr0")
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, name_forErr_rule_not_found_ruleNeverDeclared) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr("lpr0")
        .name("non-existent-rule")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_not_found), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, name_forErr_rule_not_found_ruleDeclaredAfterNameReferencingIt_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr("lpr0")
        .name("lpr1")
        .close()
        .lpr_decl("lpr1")
        .lpr("lpr1")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_not_found), 1);
    EXPECT_FALSE(gram);
}

// sequence
//      illegal-in-ppr-scope
//      illegal-in-no-scope

TEST(load_tests, sequence_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .sequence()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, sequence_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .sequence()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

// set
//      illegal-in-ppr-scope
//      illegal-in-no-scope

TEST(load_tests, set_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .set(taul::bias::last_shortest)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, set_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .set(taul::bias::last_shortest)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

// modifier
//      illegal-in-ppr-scope
//      illegal-in-no-scope
//      illegal-subexpr-count (due to <1 subexprs)
//      illegal-subexpr-count (due to >1 subexprs)

TEST(load_tests, modifier_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .modifier(0, 1)
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, modifier_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .modifier(3, 10)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, modifier_forErr_illegal_subexpr_count_dueToTooFewSubExprs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .modifier(0, 0)
        // no subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, modifier_forErr_illegal_subexpr_count_dueToTooManySubExprs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .modifier(0, 0)
        // two subexprs, illegal!
        .any()
        .any()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

// assertion
//      illegal-in-ppr-scope
//      illegal-in-no-scope
//      illegal-subexpr-count (due to <1 subexprs)
//      illegal-subexpr-count (due to >1 subexprs)

TEST(load_tests, assertion_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .assertion()
        //.any() <- impl could raise illegal_in_ppr_scope for this, so we'll just exclude it
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, assertion_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .assertion(taul::polarity::negative)
        //.any() <- impl could raise illegal_in_no_scope for this, so we'll just exclude it
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, assertion_forErr_illegal_subexpr_count_dueToTooFewSubExprs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .assertion(taul::polarity::negative)
        // no subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, assertion_forErr_illegal_subexpr_count_dueToTooManySubExprs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .assertion(taul::polarity::negative)
        // two subexprs, illegal!
        .any()
        .any()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

// constraint
//      illegal-in-ppr-scope
//      illegal-in-no-scope
//      illegal-subexpr-count (due to <2 subexprs)
//      illegal-subexpr-count (due to >2 subexprs)
//      junction-missing

TEST(load_tests, constraint_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .constraint()
        //.any() <- impl could raise illegal_in_ppr_scope for this, so we'll just exclude it
        //.junction() <- excluded this too
        //.any() <- impl could raise illegal_in_ppr_scope for this, so we'll just exclude it
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, constraint_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .constraint(taul::polarity::negative)
        //.any() <- impl could raise illegal_in_no_scope for this, so we'll just exclude it
        //.junction() <- excluded this too
        //.any() <- impl could raise illegal_in_no_scope for this, so we'll just exclude it
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, constraint_forErr_illegal_subexpr_count_dueToTooFewSubExprs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .constraint(taul::polarity::negative)
        // one subexpr, illegal!
        .any()
        .junction()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, constraint_forErr_illegal_subexpr_count_dueToTooManySubExprs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .constraint(taul::polarity::negative)
        // three subexprs, illegal!
        .any()
        .junction()
        .any()
        .any()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, constraint_forErr_junction_missing) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .constraint()
        // okay, but no junction!
        .any()
        .any()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::junction_missing), 1);
    EXPECT_FALSE(gram);
}

// junction
//      junction-not-in-constraint (for lprs)
//      junction-misplaced (due to coming after <1 prior subexprs) (for lprs)
//      junction-misplaced (due to coming after >1 prior subexprs) (for lprs)
//      junction-already-established (for lprs)

TEST(load_tests, junction_forErr_junction_not_in_constraint_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .junction()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::junction_not_in_constraint), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, junction_forErr_junction_misplaced_dueToComingAfterTooFewPriorSubExprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .constraint()
        .junction()
        .any()
        .any()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::junction_misplaced), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, junction_forErr_junction_misplaced_dueToComingAfterTooManyPriorSubExprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .constraint()
        .any()
        .any()
        .junction()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::junction_misplaced), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, junction_forErr_junction_already_established_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .constraint()
        .any()
        .junction()
        .junction()
        .any()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::junction_already_established), 1);
    EXPECT_FALSE(gram);
}

// localend
//      illegal-localend (for lprs)

TEST(load_tests, localend_forErr_illegal_localend_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .constraint()
        .localend() // the only invalid part of this is that it's not in the constraining expr
        .junction()
        .any()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_localend), 1);
    EXPECT_FALSE(gram);
}

