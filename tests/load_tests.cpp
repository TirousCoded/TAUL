

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

static_assert(taul::spec_opcodes == 7);

TEST(load_tests, success) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .grammar_bias(taul::bias::last_longest)
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .lpr("lpr0")
        .char0()
        .close()
        .lpr("lpr1")
        .close()
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);

        EXPECT_EQ(gram->bias(), taul::bias::last_longest);

        if (gram->contains_lpr("lpr0")) {
            EXPECT_EQ(gram->lpr("lpr0").name, "lpr0");
            EXPECT_EQ(gram->lpr("lpr0").index, 0);
        }
        else ADD_FAILURE();
        if (gram->contains_lpr("lpr1")) {
            EXPECT_EQ(gram->lpr("lpr1").name, "lpr1");
            EXPECT_EQ(gram->lpr("lpr1").index, 1);
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


// the below detail what errors each instruction can raise, and thus which must
// be unit tested, and being specified *in order*

static_assert(taul::spec_errors == 10);

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

// char
//      illegal-in-ppr-scope
//      illegal-in-no-scope

TEST(load_tests, char_forErr_illegal_in_ppr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .char0()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_ppr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, char_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .char0()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

