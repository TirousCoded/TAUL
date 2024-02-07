

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

static_assert(taul::spec_opcodes == 20);

TEST(load_tests, success) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    taul::spec_writer sw{};

    sw
        // gonna test successful usage of qualifiers here
        .lpr_decl("with_none_qualifier")
        .lpr_decl("with_skip_qualifier")
        .lpr_decl("with_support_qualifier")
        .lpr("with_none_qualifier", taul::qualifier::none)
        .close()
        .lpr("with_skip_qualifier", taul::qualifier::skip)
        .close()
        .lpr("with_support_qualifier", taul::qualifier::support)
        .close()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .ppr_decl("ppr0")
        .ppr_decl("ppr1");

    sw
        .lpr("lpr0");

    sw
        // test simple lexer exprs
        .end()
        .any()
        .string("abc")
        .charset("abc")
        .name("lpr0") // legal lpr ref
        .name("lpr1"); // legal lpr ref

    sw
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
        .lookahead()
        .any()
        .close()
        .lookahead_not()
        .any()
        .close()
        .not0()
        .any()
        .close()
        .optional()
        .any()
        .close()
        .kleene_star()
        .any()
        .close()
        .kleene_plus()
        .any()
        .close()
        .close(); // test w/ nested (end)

    sw
        // set
        .set() // test empty
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
        .lookahead()
        .any()
        .close()
        .lookahead_not()
        .any()
        .close()
        .not0()
        .any()
        .close()
        .optional()
        .any()
        .close()
        .kleene_star()
        .any()
        .close()
        .kleene_plus()
        .any()
        .close()
        .close(); // test w/ nested (end)
    
    sw
        // lookahead
        .lookahead()
        .set()
        .end()
        .any()
        .string("a")
        .charset("a")
        .close()
        .close();

    sw
        // lookahead-not
        .lookahead_not()
        .set()
        .end()
        .any()
        .string("a")
        .charset("a")
        .close()
        .close();

    sw
        // not
        .not0()
        .set()
        .end()
        .any()
        .string("a")
        .charset("a")
        .close()
        .close();

    sw
        // optional
        .optional() // test w/ nested #1
        .any()
        .close() // test w/ nested #1 (end)
        .optional() // test w/ nested #2
        .string("abc")
        .close() // test w/ nested #2 (end)
        .optional() // test w/ nested #3
        .sequence() // test w/ nested sequence
        .any()
        .close() // test w/ nested sequence (end)
        .close() // test w/ nested #3 (end)
        .optional() // test w/ nested #4
        .set() // test w/ nested set
        .any()
        .close() // test w/ nested set (end)
        .close() // test w/ nested #4 (end)
        .optional() // test w/ nested #5
        .lookahead()
        .any()
        .close()
        .close() // test w/ nested #5 (end)
        .optional() // test w/ nested #6
        .lookahead_not()
        .any()
        .close()
        .close() // test w/ nested #6 (end)
        .optional() // test w/ nested #7
        .not0()
        .any()
        .close()
        .close() // test w/ nested #7 (end)
        .optional() // test w/ nested #8
        .optional()
        .any()
        .close()
        .close() // test w/ nested #8 (end)
        .optional() // test w/ nested #9
        .kleene_star()
        .any()
        .close()
        .close() // test w/ nested #9 (end)
        .optional() // test w/ nested #10
        .kleene_plus()
        .any()
        .close()
        .close(); // test w/ nested #10 (end)

    sw
        // kleene-star
        .kleene_star() // test w/ nested #1
        .any()
        .close() // test w/ nested #1 (end)
        .kleene_star() // test w/ nested #2
        .string("abc")
        .close() // test w/ nested #2 (end)
        .kleene_star() // test w/ nested #3
        .sequence() // test w/ nested sequence
        .any()
        .close() // test w/ nested sequence (end)
        .close() // test w/ nested #3 (end)
        .kleene_star() // test w/ nested #4
        .set() // test w/ nested set
        .any()
        .close() // test w/ nested set (end)
        .close() // test w/ nested #4 (end)
        .kleene_star() // test w/ nested #5
        .lookahead()
        .any()
        .close()
        .close() // test w/ nested #5 (end)
        .kleene_star() // test w/ nested #6
        .lookahead_not()
        .any()
        .close()
        .close() // test w/ nested #6 (end)
        .kleene_star() // test w/ nested #7
        .not0()
        .any()
        .close()
        .close() // test w/ nested #7 (end)
        .kleene_star() // test w/ nested #8
        .optional()
        .any()
        .close()
        .close() // test w/ nested #8 (end)
        .kleene_star() // test w/ nested #9
        .kleene_star()
        .any()
        .close()
        .close() // test w/ nested #9 (end)
        .kleene_star() // test w/ nested #10
        .kleene_plus()
        .any()
        .close()
        .close(); // test w/ nested #10 (end)

    sw
        // kleene-plus
        .kleene_plus() // test w/ nested #1
        .any()
        .close() // test w/ nested #1 (end)
        .kleene_plus() // test w/ nested #2
        .string("abc")
        .close() // test w/ nested #2 (end)
        .kleene_plus() // test w/ nested #3
        .sequence() // test w/ nested sequence
        .any()
        .close() // test w/ nested sequence (end)
        .close() // test w/ nested #3 (end)
        .kleene_plus() // test w/ nested #4
        .set() // test w/ nested set
        .any()
        .close() // test w/ nested set (end)
        .close() // test w/ nested #4 (end)
        .kleene_plus() // test w/ nested #5
        .lookahead()
        .any()
        .close()
        .close() // test w/ nested #5 (end)
        .kleene_plus() // test w/ nested #6
        .lookahead_not()
        .any()
        .close()
        .close() // test w/ nested #6 (end)
        .kleene_plus() // test w/ nested #7
        .not0()
        .any()
        .close()
        .close() // test w/ nested #7 (end)
        .kleene_plus() // test w/ nested #8
        .optional()
        .any()
        .close()
        .close() // test w/ nested #8 (end)
        .kleene_plus() // test w/ nested #9
        .kleene_star()
        .any()
        .close()
        .close() // test w/ nested #9 (end)
        .kleene_plus() // test w/ nested #10
        .kleene_plus()
        .any()
        .close()
        .close(); // test w/ nested #10 (end)

    sw
        .close();

    sw
        .lpr("lpr1") // test empty
        .close();

    sw
        .ppr("ppr0");

    sw
        // test simple parser exprs
        .end()
        .any()
        .string("abc")
        .token()
        .failure()
        .name("lpr0") // legal lpr ref
        .name("lpr1") // legal lpr ref
        .name("ppr0") // legal ppr ref
        .name("ppr1"); // legal ppr ref

    sw
        // test composite parser exprs
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
        .lookahead()
        .any()
        .close()
        .lookahead_not()
        .any()
        .close()
        .not0()
        .any()
        .close()
        .optional()
        .any()
        .close()
        .kleene_star()
        .any()
        .close()
        .kleene_plus()
        .any()
        .close()
        .close(); // test w/ nested (end)

    sw
        // set
        .set() // test empty
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
        .lookahead()
        .any()
        .close()
        .lookahead_not()
        .any()
        .close()
        .not0()
        .any()
        .close()
        .optional()
        .any()
        .close()
        .kleene_star()
        .any()
        .close()
        .kleene_plus()
        .any()
        .close()
        .close(); // test w/ nested (end)
    
    sw
        // lookahead
        .lookahead()
        .set()
        .end()
        .any()
        .string("abc") // <- in pprs, may have >1 chars
        .token()
        .failure()
        .close()
        .close();

    sw
        // lookahead-not
        .lookahead_not()
        .set()
        .end()
        .any()
        .string("abc") // <- in pprs, may have >1 chars
        .token()
        .failure()
        .close()
        .close();

    sw
        // not
        .not0()
        .set()
        .end()
        .any()
        .string("abc") // <- in pprs, may have >1 chars
        .token()
        .failure()
        .close()
        .close();

    sw
        // optional
        .optional() // test w/ nested #1
        .any()
        .close() // test w/ nested #1 (end)
        .optional() // test w/ nested #2
        .string("abc")
        .close() // test w/ nested #2 (end)
        .optional() // test w/ nested #3
        .sequence() // test w/ nested sequence
        .any()
        .close() // test w/ nested sequence (end)
        .close() // test w/ nested #3 (end)
        .optional() // test w/ nested #4
        .set() // test w/ nested set
        .any()
        .close() // test w/ nested set (end)
        .close() // test w/ nested #4 (end)
        .optional() // test w/ nested #5
        .lookahead()
        .any()
        .close()
        .close() // test w/ nested #5 (end)
        .optional() // test w/ nested #6
        .lookahead_not()
        .any()
        .close()
        .close() // test w/ nested #6 (end)
        .optional() // test w/ nested #7
        .not0()
        .any()
        .close()
        .close() // test w/ nested #7 (end)
        .optional() // test w/ nested #8
        .optional()
        .any()
        .close()
        .close() // test w/ nested #8 (end)
        .optional() // test w/ nested #9
        .kleene_star()
        .any()
        .close()
        .close() // test w/ nested #9 (end)
        .optional() // test w/ nested #10
        .kleene_plus()
        .any()
        .close()
        .close(); // test w/ nested #10 (end)

    sw
        // kleene-star
        .kleene_star() // test w/ nested #1
        .any()
        .close() // test w/ nested #1 (end)
        .kleene_star() // test w/ nested #2
        .string("abc")
        .close() // test w/ nested #2 (end)
        .kleene_star() // test w/ nested #3
        .sequence() // test w/ nested sequence
        .any()
        .close() // test w/ nested sequence (end)
        .close() // test w/ nested #3 (end)
        .kleene_star() // test w/ nested #4
        .set() // test w/ nested set
        .any()
        .close() // test w/ nested set (end)
        .close() // test w/ nested #4 (end)
        .kleene_star() // test w/ nested #5
        .lookahead()
        .any()
        .close()
        .close() // test w/ nested #5 (end)
        .kleene_star() // test w/ nested #6
        .lookahead_not()
        .any()
        .close()
        .close() // test w/ nested #6 (end)
        .kleene_star() // test w/ nested #7
        .not0()
        .any()
        .close()
        .close() // test w/ nested #7 (end)
        .kleene_star() // test w/ nested #8
        .optional()
        .any()
        .close()
        .close() // test w/ nested #8 (end)
        .kleene_star() // test w/ nested #9
        .kleene_star()
        .any()
        .close()
        .close() // test w/ nested #9 (end)
        .kleene_star() // test w/ nested #10
        .kleene_plus()
        .any()
        .close()
        .close(); // test w/ nested #10 (end)

    sw
        // kleene-plus
        .kleene_plus() // test w/ nested #1
        .any()
        .close() // test w/ nested #1 (end)
        .kleene_plus() // test w/ nested #2
        .string("abc")
        .close() // test w/ nested #2 (end)
        .kleene_plus() // test w/ nested #3
        .sequence() // test w/ nested sequence
        .any()
        .close() // test w/ nested sequence (end)
        .close() // test w/ nested #3 (end)
        .kleene_plus() // test w/ nested #4
        .set() // test w/ nested set
        .any()
        .close() // test w/ nested set (end)
        .close() // test w/ nested #4 (end)
        .kleene_plus() // test w/ nested #5
        .lookahead()
        .any()
        .close()
        .close() // test w/ nested #5 (end)
        .kleene_plus() // test w/ nested #6
        .lookahead_not()
        .any()
        .close()
        .close() // test w/ nested #6 (end)
        .kleene_plus() // test w/ nested #7
        .not0()
        .any()
        .close()
        .close() // test w/ nested #7 (end)
        .kleene_plus() // test w/ nested #8
        .optional()
        .any()
        .close()
        .close() // test w/ nested #8 (end)
        .kleene_plus() // test w/ nested #9
        .kleene_star()
        .any()
        .close()
        .close() // test w/ nested #9 (end)
        .kleene_plus() // test w/ nested #10
        .kleene_plus()
        .any()
        .close()
        .close(); // test w/ nested #10 (end)

    sw
        .close();

    sw
        .ppr("ppr1") // test empty
        .close();

    const auto s = sw.done();

    const auto gram = taul::load(s, ec, lgr);

    EXPECT_EQ(ec.total(), 0);

    if (gram) {
        TAUL_LOG(lgr, "{}", *gram);

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
        if (gram->contains_lpr("with_support_qualifier")) {
            EXPECT_EQ(gram->lpr("with_support_qualifier").name, "with_support_qualifier");
            EXPECT_EQ(gram->lpr("with_support_qualifier").index, 2);
            EXPECT_EQ(gram->lpr("with_support_qualifier").qualifer, taul::qualifier::support);
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
        .name("ppr1")
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

static_assert(taul::spec_errors == 16);

// close                        <- most errors will be associated w/ opening instruction instead
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
//      illegal-qualifier (for skip)    <- PPRs may not have any qualifiers
//      illegal-qualifier (for support) <- PPRs may not have any qualifiers

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

TEST(load_tests, ppr_forErr_illegal_qualifier_forSkip) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0", taul::qualifier::skip)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_qualifier), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, ppr_forErr_illegal_qualifier_forSupport) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0", taul::qualifier::support)
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_qualifier), 1);
    EXPECT_FALSE(gram);
}

// end
//      illegal-in-no-scope

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
//      illegal-in-no-scope

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
//      illegal-in-no-scope

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
        .ppr_decl("a")
        .ppr("a")
        .charset("abc") // illegal!
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

// token
//      illegal-in-lpr-scope
//      illegal-in-no-scope

TEST(load_tests, token_forErr_illegal_in_lpr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a")
        .lpr("a")
        .token() // illegal!
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_lpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, token_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .token()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

// failure
//      illegal-in-lpr-scope
//      illegal-in-no-scope

TEST(load_tests, failure_forErr_illegal_in_lpr_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("a")
        .lpr("a")
        .failure() // illegal!
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_lpr_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, failure_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .failure()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
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

TEST(load_tests, name_forErr_rule_not_found_ruleNotYetDeclared) {
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

TEST(load_tests, name_forErr_rule_not_found_LPRNotYetDefined_forLPRs) {
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

TEST(load_tests, name_forErr_rule_not_found_LPRNotYetDefined_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
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

TEST(load_tests, name_forErr_rule_not_found_PPRNotYetDefined_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr("ppr0")
        .name("ppr1")
        .close()
        .ppr_decl("ppr1")
        .ppr("ppr1")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_not_found), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, name_forErr_rule_may_not_be_ppr_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .ppr_decl("ppr0")
        .lpr("lpr0")
        .name("ppr0")
        .close()
        .ppr("ppr0")
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::rule_may_not_be_ppr), 1);
    EXPECT_FALSE(gram);
}

// sequence
//      illegal-in-no-scope

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
//      illegal-in-no-scope

TEST(load_tests, set_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .set()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

// lookahead
//      illegal-in-no-scope
//      illegal-subexpr-count (due to <1 subexprs) (for lprs)
//      illegal-subexpr-count (due to <1 subexprs) (for pprs)
//      illegal-subexpr-count (due to >1 subexprs) (for lprs)
//      illegal-subexpr-count (due to >1 subexprs) (for pprs)
//      illegal-in-single-terminal-scope (w/ name) (for lprs)
//      illegal-in-single-terminal-scope (w/ sequence) (for lprs)
//      illegal-in-single-terminal-scope (w/ lookahead) (for lprs)
//      illegal-in-single-terminal-scope (w/ lookahead-not) (for lprs)
//      illegal-in-single-terminal-scope (w/ not) (for lprs)
//      illegal-in-single-terminal-scope (w/ optional) (for lprs)
//      illegal-in-single-terminal-scope (w/ kleene-star) (for lprs)
//      illegal-in-single-terminal-scope (w/ kleene-plus) (for lprs)
//      illegal-in-single-terminal-scope (w/ name) (for pprs)
//      illegal-in-single-terminal-scope (w/ sequence) (for pprs)
//      illegal-in-single-terminal-scope (w/ lookahead) (for pprs)
//      illegal-in-single-terminal-scope (w/ lookahead-not) (for pprs)
//      illegal-in-single-terminal-scope (w/ not) (for pprs)
//      illegal-in-single-terminal-scope (w/ optional) (for pprs)
//      illegal-in-single-terminal-scope (w/ kleene-star) (for pprs)
//      illegal-in-single-terminal-scope (w/ kleene-plus) (for pprs)

TEST(load_tests, lookahead_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lookahead()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_subexpr_count_dueToTooFewSubExprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .lookahead()
        // no subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_subexpr_count_dueToTooFewSubExprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f")
        .ppr("f")
        .lookahead()
        // no subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_subexpr_count_dueToTooManySubExprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .lookahead()
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

TEST(load_tests, lookahead_forErr_illegal_subexpr_count_dueToTooManySubExprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f")
        .ppr("f")
        .lookahead()
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

TEST(load_tests, lookahead_forErr_illegal_in_single_terminal_scope_forName_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .lookahead()
        .name("lpr0")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_in_single_terminal_scope_forSequence_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .lookahead()
        .sequence()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_in_single_terminal_scope_forLookAhead_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .lookahead()
        .lookahead()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_in_single_terminal_scope_forLookAheadNot_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .lookahead()
        .lookahead_not()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_in_single_terminal_scope_forNot_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .lookahead()
        .not0()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_in_single_terminal_scope_forOptional_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .lookahead()
        .optional()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_in_single_terminal_scope_forKleeneStar_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .lookahead()
        .kleene_star()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_in_single_terminal_scope_forKleenePlus_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .lookahead()
        .kleene_plus()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_in_single_terminal_scope_forName_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .lookahead()
        .name("ppr0")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_in_single_terminal_scope_forSequence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .lookahead()
        .sequence()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_in_single_terminal_scope_forLookAhead_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .lookahead()
        .lookahead()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_in_single_terminal_scope_forLookAheadNot_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .lookahead()
        .lookahead_not()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_in_single_terminal_scope_forNot_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .lookahead()
        .not0()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_in_single_terminal_scope_forOptional_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .lookahead()
        .optional()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_in_single_terminal_scope_forKleeneStar_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .lookahead()
        .kleene_star()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_forErr_illegal_in_single_terminal_scope_forKleenePlus_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .lookahead()
        .kleene_plus()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

// lookahead-not
//      illegal-in-no-scope
//      illegal-subexpr-count (due to <1 subexprs) (for lprs)
//      illegal-subexpr-count (due to <1 subexprs) (for pprs)
//      illegal-subexpr-count (due to >1 subexprs) (for lprs)
//      illegal-subexpr-count (due to >1 subexprs) (for pprs)
//      illegal-in-single-terminal-scope (w/ name) (for lprs)
//      illegal-in-single-terminal-scope (w/ sequence) (for lprs)
//      illegal-in-single-terminal-scope (w/ lookahead) (for lprs)
//      illegal-in-single-terminal-scope (w/ lookahead-not) (for lprs)
//      illegal-in-single-terminal-scope (w/ not) (for lprs)
//      illegal-in-single-terminal-scope (w/ optional) (for lprs)
//      illegal-in-single-terminal-scope (w/ kleene-star) (for lprs)
//      illegal-in-single-terminal-scope (w/ kleene-plus) (for lprs)
//      illegal-in-single-terminal-scope (w/ name) (for pprs)
//      illegal-in-single-terminal-scope (w/ sequence) (for pprs)
//      illegal-in-single-terminal-scope (w/ lookahead) (for pprs)
//      illegal-in-single-terminal-scope (w/ lookahead-not) (for pprs)
//      illegal-in-single-terminal-scope (w/ not) (for pprs)
//      illegal-in-single-terminal-scope (w/ optional) (for pprs)
//      illegal-in-single-terminal-scope (w/ kleene-star) (for pprs)
//      illegal-in-single-terminal-scope (w/ kleene-plus) (for pprs)

TEST(load_tests, lookahead_not_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lookahead_not()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_subexpr_count_dueToTooFewSubExprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .lookahead_not()
        // no subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_subexpr_count_dueToTooFewSubExprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f")
        .ppr("f")
        .lookahead_not()
        // no subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_subexpr_count_dueToTooManySubExprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .lookahead_not()
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

TEST(load_tests, lookahead_not_forErr_illegal_subexpr_count_dueToTooManySubExprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f")
        .ppr("f")
        .lookahead_not()
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

TEST(load_tests, lookahead_not_forErr_illegal_in_single_terminal_scope_forName_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .lookahead_not()
        .name("lpr0")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_in_single_terminal_scope_forSequence_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .lookahead_not()
        .sequence()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_in_single_terminal_scope_forLookAhead_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .lookahead_not()
        .lookahead()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_in_single_terminal_scope_forLookAheadNot_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .lookahead_not()
        .lookahead_not()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_in_single_terminal_scope_forNot_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .lookahead_not()
        .not0()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_in_single_terminal_scope_forOptional_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .lookahead_not()
        .optional()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_in_single_terminal_scope_forKleeneStar_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .lookahead_not()
        .kleene_star()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_in_single_terminal_scope_forKleenePlus_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .lookahead_not()
        .kleene_plus()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_in_single_terminal_scope_forName_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .lookahead_not()
        .name("ppr0")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_in_single_terminal_scope_forSequence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .lookahead_not()
        .sequence()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_in_single_terminal_scope_forLookAhead_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .lookahead_not()
        .lookahead()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_in_single_terminal_scope_forLookAheadNot_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .lookahead_not()
        .lookahead_not()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_in_single_terminal_scope_forNot_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .lookahead_not()
        .not0()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_in_single_terminal_scope_forOptional_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .lookahead_not()
        .optional()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_in_single_terminal_scope_forKleeneStar_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .lookahead_not()
        .kleene_star()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, lookahead_not_forErr_illegal_in_single_terminal_scope_forKleenePlus_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .lookahead_not()
        .kleene_plus()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

// not
//      illegal-in-no-scope
//      illegal-subexpr-count (due to <1 subexprs) (for lprs)
//      illegal-subexpr-count (due to <1 subexprs) (for pprs)
//      illegal-subexpr-count (due to >1 subexprs) (for lprs)
//      illegal-subexpr-count (due to >1 subexprs) (for pprs)
//      illegal-in-single-terminal-scope (w/ name) (for lprs)
//      illegal-in-single-terminal-scope (w/ sequence) (for lprs)
//      illegal-in-single-terminal-scope (w/ lookahead) (for lprs)
//      illegal-in-single-terminal-scope (w/ lookahead-not) (for lprs)
//      illegal-in-single-terminal-scope (w/ not) (for lprs)
//      illegal-in-single-terminal-scope (w/ optional) (for lprs)
//      illegal-in-single-terminal-scope (w/ kleene-star) (for lprs)
//      illegal-in-single-terminal-scope (w/ kleene-plus) (for lprs)
//      illegal-in-single-terminal-scope (w/ name) (for pprs)
//      illegal-in-single-terminal-scope (w/ sequence) (for pprs)
//      illegal-in-single-terminal-scope (w/ lookahead) (for pprs)
//      illegal-in-single-terminal-scope (w/ lookahead-not) (for pprs)
//      illegal-in-single-terminal-scope (w/ not) (for pprs)
//      illegal-in-single-terminal-scope (w/ optional) (for pprs)
//      illegal-in-single-terminal-scope (w/ kleene-star) (for pprs)
//      illegal-in-single-terminal-scope (w/ kleene-plus) (for pprs)

TEST(load_tests, not_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .not0()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_subexpr_count_dueToTooFewSubExprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .not0()
        // no subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_subexpr_count_dueToTooFewSubExprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f")
        .ppr("f")
        .not0()
        // no subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_subexpr_count_dueToTooManySubExprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .not0()
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

TEST(load_tests, not_forErr_illegal_subexpr_count_dueToTooManySubExprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f")
        .ppr("f")
        .not0()
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

TEST(load_tests, not_forErr_illegal_in_single_terminal_scope_forName_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .not0()
        .name("lpr0")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_in_single_terminal_scope_forSequence_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .not0()
        .sequence()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_in_single_terminal_scope_forLookAhead_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .not0()
        .lookahead()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_in_single_terminal_scope_forLookAheadNot_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .not0()
        .lookahead_not()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_in_single_terminal_scope_forNot_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .not0()
        .not0()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_in_single_terminal_scope_forOptional_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .not0()
        .optional()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_in_single_terminal_scope_forKleeneStar_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .not0()
        .kleene_star()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_in_single_terminal_scope_forKleenePlus_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .not0()
        .kleene_plus()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_in_single_terminal_scope_forName_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .not0()
        .name("ppr0")
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_in_single_terminal_scope_forSequence_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .not0()
        .sequence()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_in_single_terminal_scope_forLookAhead_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .not0()
        .lookahead()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_in_single_terminal_scope_forLookAheadNot_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .not0()
        .lookahead_not()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_in_single_terminal_scope_forNot_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .not0()
        .not0()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_in_single_terminal_scope_forOptional_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .not0()
        .optional()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_in_single_terminal_scope_forKleeneStar_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .not0()
        .kleene_star()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, not_forErr_illegal_in_single_terminal_scope_forKleenePlus_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .not0()
        .kleene_plus()
        .any()
        .close()
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_single_terminal_scope), 1);
    EXPECT_FALSE(gram);
}

// optional
//      illegal-in-no-scope
//      illegal-subexpr-count (due to <1 subexprs) (for lprs)
//      illegal-subexpr-count (due to <1 subexprs) (for pprs)
//      illegal-subexpr-count (due to >1 subexprs) (for lprs)
//      illegal-subexpr-count (due to >1 subexprs) (for pprs)

TEST(load_tests, optional_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .optional()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, optional_forErr_illegal_subexpr_count_dueToTooFewSubExprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .optional()
        // no subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, optional_forErr_illegal_subexpr_count_dueToTooFewSubExprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f")
        .ppr("f")
        .optional()
        // no subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, optional_forErr_illegal_subexpr_count_dueToTooManySubExprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .optional()
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

TEST(load_tests, optional_forErr_illegal_subexpr_count_dueToTooManySubExprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f")
        .ppr("f")
        .optional()
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

// kleene-star
//      illegal-in-no-scope
//      illegal-subexpr-count (due to <1 subexprs) (for lprs)
//      illegal-subexpr-count (due to <1 subexprs) (for pprs)
//      illegal-subexpr-count (due to >1 subexprs) (for lprs)
//      illegal-subexpr-count (due to >1 subexprs) (for pprs)

TEST(load_tests, kleene_star_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .kleene_star()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, kleene_star_forErr_illegal_subexpr_count_dueToTooFewSubExprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .kleene_star()
        // no subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, kleene_star_forErr_illegal_subexpr_count_dueToTooFewSubExprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f")
        .ppr("f")
        .kleene_star()
        // no subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, kleene_star_forErr_illegal_subexpr_count_dueToTooManySubExprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .kleene_star()
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

TEST(load_tests, kleene_star_forErr_illegal_subexpr_count_dueToTooManySubExprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f")
        .ppr("f")
        .kleene_star()
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

// kleene-plus
//      illegal-in-no-scope
//      illegal-subexpr-count (due to <1 subexprs) (for lprs)
//      illegal-subexpr-count (due to <1 subexprs) (for pprs)
//      illegal-subexpr-count (due to >1 subexprs) (for lprs)
//      illegal-subexpr-count (due to >1 subexprs) (for pprs)

TEST(load_tests, kleene_plus_forErr_illegal_in_no_scope) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .kleene_plus()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_in_no_scope), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, kleene_plus_forErr_illegal_subexpr_count_dueToTooFewSubExprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .kleene_plus()
        // no subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, kleene_plus_forErr_illegal_subexpr_count_dueToTooFewSubExprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f")
        .ppr("f")
        .kleene_plus()
        // no subexprs, illegal!
        .close()
        .close()
        .done();

    const auto gram = taul::load(s, ec, lgr);

    //EXPECT_EQ(ec.total(), 1); <- remember, we don't impose rule that it need only raise 1
    EXPECT_EQ(ec.count(taul::spec_error::illegal_subexpr_count), 1);
    EXPECT_FALSE(gram);
}

TEST(load_tests, kleene_plus_forErr_illegal_subexpr_count_dueToTooManySubExprs_forLPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .kleene_plus()
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

TEST(load_tests, kleene_plus_forErr_illegal_subexpr_count_dueToTooManySubExprs_forPPRs) {
    const auto lgr = taul::make_stderr_logger();
    taul::spec_error_counter ec{};

    const auto s =
        taul::spec_writer()
        .ppr_decl("f")
        .ppr("f")
        .kleene_plus()
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

