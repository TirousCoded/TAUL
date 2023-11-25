

#include <gtest/gtest.h>

#include <taul/all.h>


// these tests cover the actual usage of loaded TAUL grammars

// of course, these tests can't cover *literally every use case*, especially 
// regarding successful usage, but for failure cases we do strive to be quite
// rigorous, w/ success cases being, again, more summary


// TODO: right now our tests just test a bunch of specific cases, w/ arbitrary
//       strings which together do not in any way cover the whole range of possible
//       strings possible as inputs
//
//       maybe we could use some kind of pseudo-random string generation system
//       to improve the below tests, of some sort, but for now these will suffice


class GrammarUsage : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr;


    void SetUp() override final {

        lgr = taul::make_stderr_logger();
    }
};


TEST_F(GrammarUsage, lexerExpr_toplevel) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f0")
        .lpr_decl("f1")
        // test w/ 0 subexprs
        .lpr("f0")
        .close()
        // test w/ 3 subexprs
        .lpr("f1")
        .any()
        .any()
        .any()
        .close()
        .done();

    const taul::grammar gram = std::move(*taul::load(spec, lgr));
    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    const auto lex0 = gram.lexer("f0");
    const auto lex1 = gram.lexer("f1");


    // the below assertions expect the embedded exprs to work as expected


    // test w/out offset

    // success

    EXPECT_EQ(lex0("", lgr), taul::token(lpr0, ""));
    EXPECT_EQ(lex0("abcde", lgr), taul::token(lpr0, ""));
    EXPECT_EQ(lex0("12345", lgr), taul::token(lpr0, ""));
    EXPECT_EQ(lex0("\r\n\t\v", lgr), taul::token(lpr0, ""));

    EXPECT_EQ(lex1("abcde", lgr), taul::token(lpr1, "abc"));
    EXPECT_EQ(lex1("12345", lgr), taul::token(lpr1, "123"));
    EXPECT_EQ(lex1("\r\n\t\v", lgr), taul::token(lpr1, "\r\n\t"));

    // failure

    EXPECT_EQ(lex1("a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("1", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("\r", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("ab", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("12", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("\r\n", lgr), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(lex0("abc", 3, lgr), taul::token(lpr0, "", 3));
    EXPECT_EQ(lex0("abcabcde", 3, lgr), taul::token(lpr0, "", 3));
    EXPECT_EQ(lex0("abc12345", 3, lgr), taul::token(lpr0, "", 3));
    EXPECT_EQ(lex0("abc\r\n\t\v", 3, lgr), taul::token(lpr0, "", 3));

    EXPECT_EQ(lex1("abcabcde", 3, lgr), taul::token(lpr1, "abc", 3));
    EXPECT_EQ(lex1("abc12345", 3, lgr), taul::token(lpr1, "123", 3));
    EXPECT_EQ(lex1("abc\r\n\t\v", 3, lgr), taul::token(lpr1, "\r\n\t", 3));

    // failure

    EXPECT_EQ(lex1("abca", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("abc1", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("abc\r", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("abcab", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("abc12", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("abc\r\n", 3, lgr), taul::token::failure("", 3));
}

TEST_F(GrammarUsage, lexerExpr_begin) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .begin()
        .close()
        .done();

    const taul::grammar gram = std::move(*taul::load(spec, lgr));
    const auto& lpr = gram.lpr("f");
    const auto lex = gram.lexer("f");


    // test w/out offset

    // success

    EXPECT_EQ(lex("", lgr), taul::token(lpr, ""));
    EXPECT_EQ(lex("abc", lgr), taul::token(lpr, ""));
    EXPECT_EQ(lex("123", lgr), taul::token(lpr, ""));
    EXPECT_EQ(lex("&!%", lgr), taul::token(lpr, ""));
    EXPECT_EQ(lex(" \t", lgr), taul::token(lpr, ""));
    EXPECT_EQ(lex("\r\n", lgr), taul::token(lpr, ""));

    // failure

    // n/a


    // test w/ offset

    // success

    // n/a

    // failure

    EXPECT_EQ(lex("abc", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abcabc", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc123", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc&!%", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc \t", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc\r\n", 3, lgr), taul::token::failure("", 3));
}

TEST_F(GrammarUsage, lexerExpr_end) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .end()
        .close()
        .done();

    const taul::grammar gram = std::move(*taul::load(spec, lgr));
    const auto& lpr = gram.lpr("f");
    const auto lex = gram.lexer("f");


    // test w/out offset

    // success

    EXPECT_EQ(lex("", lgr), taul::token(lpr, ""));

    // failure

    EXPECT_EQ(lex("a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("1", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("&", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("abc", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("123", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("&!%", lgr), taul::token::failure(""));
    EXPECT_EQ(lex(" \t", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("\r\n", lgr), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(lex("abc", 3, lgr), taul::token(lpr, "", 3));

    // failure

    EXPECT_EQ(lex("abca", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc1", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc&", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abcabc", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc123", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc&!%", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc \t", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc\r\n", 3, lgr), taul::token::failure("", 3));
}

TEST_F(GrammarUsage, lexerExpr_any) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .any()
        .close()
        .done();

    const taul::grammar gram = std::move(*taul::load(spec, lgr));
    const auto& lpr = gram.lpr("f");
    const auto lex = gram.lexer("f");


    // test w/out offset

    // success

    EXPECT_EQ(lex("0", lgr), taul::token(lpr, "0"));
    EXPECT_EQ(lex("1", lgr), taul::token(lpr, "1"));
    EXPECT_EQ(lex("!", lgr), taul::token(lpr, "!"));
    EXPECT_EQ(lex("\n\r", lgr), taul::token(lpr, "\n"));
    EXPECT_EQ(lex("a", lgr), taul::token(lpr, "a"));
    EXPECT_EQ(lex(" ", lgr), taul::token(lpr, " "));
    EXPECT_EQ(lex("~", lgr), taul::token(lpr, "~"));
    EXPECT_EQ(lex("abc", lgr), taul::token(lpr, "a"));

    // failure

    EXPECT_EQ(lex("", lgr), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(lex("abc0", 3, lgr), taul::token(lpr, "0", 3));
    EXPECT_EQ(lex("abc1", 3, lgr), taul::token(lpr, "1", 3));
    EXPECT_EQ(lex("abc!", 3, lgr), taul::token(lpr, "!", 3));
    EXPECT_EQ(lex("abc\n\r", 3, lgr), taul::token(lpr, "\n", 3));
    EXPECT_EQ(lex("abca", 3, lgr), taul::token(lpr, "a", 3));
    EXPECT_EQ(lex("abc ", 3, lgr), taul::token(lpr, " ", 3));
    EXPECT_EQ(lex("abc~", 3, lgr), taul::token(lpr, "~", 3));
    EXPECT_EQ(lex("abcabc", 3, lgr), taul::token(lpr, "a", 3));

    // failure

    EXPECT_EQ(lex("abc", 3, lgr), taul::token::failure("", 3));
}

TEST_F(GrammarUsage, lexerExpr_string) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .string("abc")
        .close()
        .done();

    const taul::grammar gram = std::move(*taul::load(spec, lgr));
    const auto& lpr = gram.lpr("f");
    const auto lex = gram.lexer("f");


    // test w/out offset

    // success

    EXPECT_EQ(lex("abc", lgr), taul::token(lpr, "abc"));
    EXPECT_EQ(lex("abc123", lgr), taul::token(lpr, "abc"));
    EXPECT_EQ(lex("abc&^!", lgr), taul::token(lpr, "abc"));
    EXPECT_EQ(lex("abc\r\n\t", lgr), taul::token(lpr, "abc"));

    // failure

    EXPECT_EQ(lex("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("ab", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("def", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("&^!", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("\r\n\t", lgr), taul::token::failure(""));
    EXPECT_EQ(lex(" abc", lgr), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(lex("abcabc", 3, lgr), taul::token(lpr, "abc", 3));
    EXPECT_EQ(lex("abcabc123", 3, lgr), taul::token(lpr, "abc", 3));
    EXPECT_EQ(lex("abcabc&^!", 3, lgr), taul::token(lpr, "abc", 3));
    EXPECT_EQ(lex("abcabc\r\n\t", 3, lgr), taul::token(lpr, "abc", 3));

    // failure

    EXPECT_EQ(lex("abc", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abca", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abcab", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abcdef", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc&^!", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc\r\n\t", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc abc", 3, lgr), taul::token::failure("", 3));
}

TEST_F(GrammarUsage, lexerExpr_charset) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .charset("abc")
        .close()
        .done();

    const taul::grammar gram = std::move(*taul::load(spec, lgr));
    const auto& lpr = gram.lpr("f");
    const auto lex = gram.lexer("f");


    // test w/out offset

    // success

    EXPECT_EQ(lex("a", lgr), taul::token(lpr, "a"));
    EXPECT_EQ(lex("b", lgr), taul::token(lpr, "b"));
    EXPECT_EQ(lex("c", lgr), taul::token(lpr, "c"));
    EXPECT_EQ(lex("a123", lgr), taul::token(lpr, "a"));
    EXPECT_EQ(lex("b&^!", lgr), taul::token(lpr, "b"));
    EXPECT_EQ(lex("c\r\n\t", lgr), taul::token(lpr, "c"));

    // failure

    EXPECT_EQ(lex("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("d", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("da", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("db", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("dc", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("&^!", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("\r\n\t", lgr), taul::token::failure(""));
    EXPECT_EQ(lex(" a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex(" b", lgr), taul::token::failure(""));
    EXPECT_EQ(lex(" c", lgr), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(lex("abca", 3, lgr), taul::token(lpr, "a", 3));
    EXPECT_EQ(lex("abcb", 3, lgr), taul::token(lpr, "b", 3));
    EXPECT_EQ(lex("abcc", 3, lgr), taul::token(lpr, "c", 3));
    EXPECT_EQ(lex("abca123", 3, lgr), taul::token(lpr, "a", 3));
    EXPECT_EQ(lex("abcb&^!", 3, lgr), taul::token(lpr, "b", 3));
    EXPECT_EQ(lex("abcc\r\n\t", 3, lgr), taul::token(lpr, "c", 3));

    // failure

    EXPECT_EQ(lex("abc", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abcd", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abcda", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abcdb", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abcdc", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc&^!", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc\r\n\t", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc b", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("abc c", 3, lgr), taul::token::failure("", 3));
}

TEST_F(GrammarUsage, lexerExpr_sequence) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f0")
        .lpr_decl("f1")
        .lpr("f0") // test empty sequence
        .sequence()
        .close()
        .close()
        .lpr("f1") // test non-empty sequence
        // w/out this 'set' expr, pre-impl tests would ALWAYS succeed, so
        // I'm using the set expr to *ensure* that it fails, lol
        // in truth tho, the below is actually a good thing to explicitly test,
        // that being that a sequence in a set is treated as a unit regarding
        // the behaviour of said set
        .set()
        .string("*****") // <- dummy
        .sequence()
        .string("a")
        .sequence()
        .any()
        .string("c")
        .close()
        .any()
        .close()
        .close()
        .close()
        .done();

    const taul::grammar gram = std::move(*taul::load(spec, lgr));
    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    const auto lex0 = gram.lexer("f0");
    const auto lex1 = gram.lexer("f1");


    // test f0 w/out offset

    // success

    EXPECT_EQ(lex0("", lgr), taul::token(lpr0, ""));
    EXPECT_EQ(lex0("abc", lgr), taul::token(lpr0, ""));
    EXPECT_EQ(lex0("123", lgr), taul::token(lpr0, ""));
    EXPECT_EQ(lex0("&^$", lgr), taul::token(lpr0, ""));
    EXPECT_EQ(lex0(" ", lgr), taul::token(lpr0, ""));
    EXPECT_EQ(lex0("\r\n", lgr), taul::token(lpr0, ""));

    // failure

    // n/a


    // test f0 w/ offset

    // success

    EXPECT_EQ(lex0("abc", 3, lgr), taul::token(lpr0, "", 3));
    EXPECT_EQ(lex0("abcabc", 3, lgr), taul::token(lpr0, "", 3));
    EXPECT_EQ(lex0("abc123", 3, lgr), taul::token(lpr0, "", 3));
    EXPECT_EQ(lex0("abc&^$", 3, lgr), taul::token(lpr0, "", 3));
    EXPECT_EQ(lex0("abc ", 3, lgr), taul::token(lpr0, "", 3));
    EXPECT_EQ(lex0("abc\r\n", 3, lgr), taul::token(lpr0, "", 3));

    // failure

    // n/a


    // test f1 w/out offset

    // success

    EXPECT_EQ(lex1("abcd", lgr), taul::token(lpr1, "abcd"));
    EXPECT_EQ(lex1("abcd123", lgr), taul::token(lpr1, "abcd"));
    EXPECT_EQ(lex1("a1c2", lgr), taul::token(lpr1, "a1c2"));
    EXPECT_EQ(lex1("a1c2123", lgr), taul::token(lpr1, "a1c2"));
    EXPECT_EQ(lex1("a^c(", lgr), taul::token(lpr1, "a^c("));
    EXPECT_EQ(lex1("a^c(123", lgr), taul::token(lpr1, "a^c("));
    EXPECT_EQ(lex1("a c\n", lgr), taul::token(lpr1, "a c\n"));
    EXPECT_EQ(lex1("a c\n123", lgr), taul::token(lpr1, "a c\n"));

    // failure

    EXPECT_EQ(lex1("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("ab", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("abc", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("Abcd", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("abCd", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("^&^!", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("\n \r\t", lgr), taul::token::failure(""));


    // test f1 w/ offset

    // success

    EXPECT_EQ(lex1("+-=abcd", 3, lgr), taul::token(lpr1, "abcd", 3));
    EXPECT_EQ(lex1("+-=abcd123", 3, lgr), taul::token(lpr1, "abcd", 3));
    EXPECT_EQ(lex1("+-=a1c2", 3, lgr), taul::token(lpr1, "a1c2", 3));
    EXPECT_EQ(lex1("+-=a1c2123", 3, lgr), taul::token(lpr1, "a1c2", 3));
    EXPECT_EQ(lex1("+-=a^c(", 3, lgr), taul::token(lpr1, "a^c(", 3));
    EXPECT_EQ(lex1("+-=a^c(123", 3, lgr), taul::token(lpr1, "a^c(", 3));
    EXPECT_EQ(lex1("+-=a c\n", 3, lgr), taul::token(lpr1, "a c\n", 3));
    EXPECT_EQ(lex1("+-=a c\n123", 3, lgr), taul::token(lpr1, "a c\n", 3));

    // failure

    EXPECT_EQ(lex1("+-=", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=ab", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=abc", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=Abcd", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=abCd", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=^&^!", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=\n \r\t", 3, lgr), taul::token::failure("", 3));
}

TEST_F(GrammarUsage, lexerExpr_set) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f0")
        .lpr_decl("f1")
        .lpr_decl("f2")
        .lpr_decl("f3")
        .lpr_decl("f4")
        .lpr_decl("f5")
        .lpr_decl("f6")
        .lpr("f0") // test empty set <- should ALWAYS fail!
        .set()
        .close()
        .close()
        .lpr("f1") // test non-empty set w/ first-longest
        .set(taul::bias::first_longest)
        .string("123")
        .string("a")
        .string("12")
        .string("abc")
        .close()
        .close()
        .lpr("f2") // test non-empty set w/ first-shortest
        .set(taul::bias::first_shortest)
        .string("123")
        .string("a")
        .string("12")
        .string("abc")
        .close()
        .close()
        .lpr("f3") // test non-empty set w/ last-longest
        .set(taul::bias::last_longest)
        .string("123")
        .string("a")
        .string("12")
        .string("abc")
        .close()
        .close()
        .lpr("f4") // test non-empty set w/ last-shortest
        .set(taul::bias::last_shortest)
        .string("123")
        .string("a")
        .string("12")
        .string("abc")
        .close()
        .close()
        .lpr("f5") // test non-empty set w/ first
        .set(taul::bias::first)
        .string("123")
        .string("a")
        .string("12")
        .string("abc")
        .close()
        .close()
        .lpr("f6") // test non-empty set w/ last
        .set(taul::bias::last)
        .string("123")
        .string("a")
        .string("12")
        .string("abc")
        .close()
        .close()
        .done();

    const taul::grammar gram = std::move(*taul::load(spec, lgr));
    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    const auto& lpr2 = gram.lpr("f2");
    const auto& lpr3 = gram.lpr("f3");
    const auto& lpr4 = gram.lpr("f4");
    const auto& lpr5 = gram.lpr("f5");
    const auto& lpr6 = gram.lpr("f6");
    const auto lex0 = gram.lexer("f0");
    const auto lex1 = gram.lexer("f1");
    const auto lex2 = gram.lexer("f2");
    const auto lex3 = gram.lexer("f3");
    const auto lex4 = gram.lexer("f4");
    const auto lex5 = gram.lexer("f5");
    const auto lex6 = gram.lexer("f6");


    // test f0 w/out offset

    // success

    // n/a

    // failure

    EXPECT_EQ(lex0("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("abc", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("123", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("&^$", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0(" ", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("\r\n", lgr), taul::token::failure(""));


    // test f0 w/ offset

    // success

    // n/a

    // failure

    EXPECT_EQ(lex0("abc", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abcabc", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc123", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc&^$", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc ", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc\r\n", 3, lgr), taul::token::failure("", 3));


    // test f1, f2, f3, f4, f5, and f6 w/out offset

    // success

    EXPECT_EQ(lex1("123", lgr), taul::token(lpr1, "123"));
    EXPECT_EQ(lex2("123", lgr), taul::token(lpr2, "12"));
    EXPECT_EQ(lex3("123", lgr), taul::token(lpr3, "123"));
    EXPECT_EQ(lex4("123", lgr), taul::token(lpr4, "12"));
    EXPECT_EQ(lex5("123", lgr), taul::token(lpr5, "123"));
    EXPECT_EQ(lex6("123", lgr), taul::token(lpr6, "12"));
    EXPECT_EQ(lex1("a", lgr), taul::token(lpr1, "a"));
    EXPECT_EQ(lex2("a", lgr), taul::token(lpr2, "a"));
    EXPECT_EQ(lex3("a", lgr), taul::token(lpr3, "a"));
    EXPECT_EQ(lex4("a", lgr), taul::token(lpr4, "a"));
    EXPECT_EQ(lex5("a", lgr), taul::token(lpr5, "a"));
    EXPECT_EQ(lex6("a", lgr), taul::token(lpr6, "a"));
    EXPECT_EQ(lex1("12", lgr), taul::token(lpr1, "12"));
    EXPECT_EQ(lex2("12", lgr), taul::token(lpr2, "12"));
    EXPECT_EQ(lex3("12", lgr), taul::token(lpr3, "12"));
    EXPECT_EQ(lex4("12", lgr), taul::token(lpr4, "12"));
    EXPECT_EQ(lex5("12", lgr), taul::token(lpr5, "12"));
    EXPECT_EQ(lex6("12", lgr), taul::token(lpr6, "12"));
    EXPECT_EQ(lex1("abc", lgr), taul::token(lpr1, "abc"));
    EXPECT_EQ(lex2("abc", lgr), taul::token(lpr2, "a"));
    EXPECT_EQ(lex3("abc", lgr), taul::token(lpr3, "abc"));
    EXPECT_EQ(lex4("abc", lgr), taul::token(lpr4, "a"));
    EXPECT_EQ(lex5("abc", lgr), taul::token(lpr5, "a"));
    EXPECT_EQ(lex6("abc", lgr), taul::token(lpr6, "abc"));

    // failure

    EXPECT_EQ(lex1("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex2("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex3("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex4("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex5("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex6("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("1", lgr), taul::token::failure(""));
    EXPECT_EQ(lex2("1", lgr), taul::token::failure(""));
    EXPECT_EQ(lex3("1", lgr), taul::token::failure(""));
    EXPECT_EQ(lex4("1", lgr), taul::token::failure(""));
    EXPECT_EQ(lex5("1", lgr), taul::token::failure(""));
    EXPECT_EQ(lex6("1", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("&^$", lgr), taul::token::failure(""));
    EXPECT_EQ(lex2("&^$", lgr), taul::token::failure(""));
    EXPECT_EQ(lex3("&^$", lgr), taul::token::failure(""));
    EXPECT_EQ(lex4("&^$", lgr), taul::token::failure(""));
    EXPECT_EQ(lex5("&^$", lgr), taul::token::failure(""));
    EXPECT_EQ(lex6("&^$", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("\n\r\t", lgr), taul::token::failure(""));
    EXPECT_EQ(lex2("\n\r\t", lgr), taul::token::failure(""));
    EXPECT_EQ(lex3("\n\r\t", lgr), taul::token::failure(""));
    EXPECT_EQ(lex4("\n\r\t", lgr), taul::token::failure(""));
    EXPECT_EQ(lex5("\n\r\t", lgr), taul::token::failure(""));
    EXPECT_EQ(lex6("\n\r\t", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1(" ", lgr), taul::token::failure(""));
    EXPECT_EQ(lex2(" ", lgr), taul::token::failure(""));
    EXPECT_EQ(lex3(" ", lgr), taul::token::failure(""));
    EXPECT_EQ(lex4(" ", lgr), taul::token::failure(""));
    EXPECT_EQ(lex5(" ", lgr), taul::token::failure(""));
    EXPECT_EQ(lex6(" ", lgr), taul::token::failure(""));


    // test f1, f2, f3, f4, f5, and f6 w/ offset

    // success

    EXPECT_EQ(lex1("+-=123", 3, lgr), taul::token(lpr1, "123", 3));
    EXPECT_EQ(lex2("+-=123", 3, lgr), taul::token(lpr2, "12", 3));
    EXPECT_EQ(lex3("+-=123", 3, lgr), taul::token(lpr3, "123", 3));
    EXPECT_EQ(lex4("+-=123", 3, lgr), taul::token(lpr4, "12", 3));
    EXPECT_EQ(lex5("+-=123", 3, lgr), taul::token(lpr5, "123", 3));
    EXPECT_EQ(lex6("+-=123", 3, lgr), taul::token(lpr6, "12", 3));
    EXPECT_EQ(lex1("+-=a", 3, lgr), taul::token(lpr1, "a", 3));
    EXPECT_EQ(lex2("+-=a", 3, lgr), taul::token(lpr2, "a", 3));
    EXPECT_EQ(lex3("+-=a", 3, lgr), taul::token(lpr3, "a", 3));
    EXPECT_EQ(lex4("+-=a", 3, lgr), taul::token(lpr4, "a", 3));
    EXPECT_EQ(lex5("+-=a", 3, lgr), taul::token(lpr5, "a", 3));
    EXPECT_EQ(lex6("+-=a", 3, lgr), taul::token(lpr6, "a", 3));
    EXPECT_EQ(lex1("+-=12", 3, lgr), taul::token(lpr1, "12", 3));
    EXPECT_EQ(lex2("+-=12", 3, lgr), taul::token(lpr2, "12", 3));
    EXPECT_EQ(lex3("+-=12", 3, lgr), taul::token(lpr3, "12", 3));
    EXPECT_EQ(lex4("+-=12", 3, lgr), taul::token(lpr4, "12", 3));
    EXPECT_EQ(lex5("+-=12", 3, lgr), taul::token(lpr5, "12", 3));
    EXPECT_EQ(lex6("+-=12", 3, lgr), taul::token(lpr6, "12", 3));
    EXPECT_EQ(lex1("+-=abc", 3, lgr), taul::token(lpr1, "abc", 3));
    EXPECT_EQ(lex2("+-=abc", 3, lgr), taul::token(lpr2, "a", 3));
    EXPECT_EQ(lex3("+-=abc", 3, lgr), taul::token(lpr3, "abc", 3));
    EXPECT_EQ(lex4("+-=abc", 3, lgr), taul::token(lpr4, "a", 3));
    EXPECT_EQ(lex5("+-=abc", 3, lgr), taul::token(lpr5, "a", 3));
    EXPECT_EQ(lex6("+-=abc", 3, lgr), taul::token(lpr6, "abc", 3));

    // failure

    EXPECT_EQ(lex1("+-=", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex2("+-=", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex3("+-=", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex4("+-=", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex5("+-=", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex6("+-=", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=1", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex2("+-=1", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex3("+-=1", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex4("+-=1", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex5("+-=1", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex6("+-=1", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=&^$", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex2("+-=&^$", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex3("+-=&^$", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex4("+-=&^$", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex5("+-=&^$", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex6("+-=&^$", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=\n\r\t", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex2("+-=\n\r\t", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex3("+-=\n\r\t", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex4("+-=\n\r\t", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex5("+-=\n\r\t", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex6("+-=\n\r\t", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-= ", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex2("+-= ", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex3("+-= ", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex4("+-= ", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex5("+-= ", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex6("+-= ", 3, lgr), taul::token::failure("", 3));
}

TEST_F(GrammarUsage, lexerExpr_modifier) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f0")
        .lpr_decl("f1")
        .lpr_decl("f2")
        .lpr_decl("f3")
        .lpr_decl("f4")
        .lpr_decl("f5")
        .lpr_decl("f6")
        .lpr_decl("f7")
        .lpr("f0")
        .modifier(1, 1) // *control*
        .string("ab")
        .close()
        .close()
        .lpr("f1")
        .modifier(0, 1) // "optional" or ?
        .string("ab")
        .close()
        .close()
        .lpr("f2")
        .modifier(0, 0) // "kleene-star" or *
        .string("ab")
        .close()
        .close()
        .lpr("f3")
        .modifier(1, 0) // "kleene-plus" or +
        .string("ab")
        .close()
        .close()
        .lpr("f4")
        .modifier(2, 2) // "manual A" or {n}
        .string("ab")
        .close()
        .close()
        .lpr("f5")
        .modifier(2, 0) // "manual B" or {n,}
        .string("ab")
        .close()
        .close()
        .lpr("f6")
        .modifier(2, 4) // "manual C" or {n,m} (#1)
        .string("ab")
        .close()
        .close()
        .lpr("f7")
        .modifier(0, 4) // "manual C" or {n,m} (#2)
        .string("ab")
        .close()
        .close()
        .done();

    const taul::grammar gram = std::move(*taul::load(spec, lgr));
    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    const auto& lpr2 = gram.lpr("f2");
    const auto& lpr3 = gram.lpr("f3");
    const auto& lpr4 = gram.lpr("f4");
    const auto& lpr5 = gram.lpr("f5");
    const auto& lpr6 = gram.lpr("f6");
    const auto& lpr7 = gram.lpr("f7");
    const auto lex0 = gram.lexer("f0");
    const auto lex1 = gram.lexer("f1");
    const auto lex2 = gram.lexer("f2");
    const auto lex3 = gram.lexer("f3");
    const auto lex4 = gram.lexer("f4");
    const auto lex5 = gram.lexer("f5");
    const auto lex6 = gram.lexer("f6");
    const auto lex7 = gram.lexer("f7");


    // test f0, f1, f2, f3, f4, f5, f6, and f7 w/out offset

    // success/failure

    // TODO: we *could* add more comprehensive tests testing strings other than 'abab...',
    //       but I'm happy w/ the below

    EXPECT_EQ(lex0("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("", lgr), taul::token(lpr1, ""));
    EXPECT_EQ(lex2("", lgr), taul::token(lpr2, ""));
    EXPECT_EQ(lex3("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex4("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex5("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex6("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex7("", lgr), taul::token(lpr7, ""));
    EXPECT_EQ(lex0("a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("a", lgr), taul::token(lpr1, ""));
    EXPECT_EQ(lex2("a", lgr), taul::token(lpr2, ""));
    EXPECT_EQ(lex3("a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex4("a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex5("a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex6("a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex7("a", lgr), taul::token(lpr7, ""));
    EXPECT_EQ(lex0("ab", lgr), taul::token(lpr0, "ab"));
    EXPECT_EQ(lex1("ab", lgr), taul::token(lpr1, "ab"));
    EXPECT_EQ(lex2("ab", lgr), taul::token(lpr2, "ab"));
    EXPECT_EQ(lex3("ab", lgr), taul::token(lpr3, "ab"));
    EXPECT_EQ(lex4("ab", lgr), taul::token::failure(""));
    EXPECT_EQ(lex5("ab", lgr), taul::token::failure(""));
    EXPECT_EQ(lex6("ab", lgr), taul::token::failure(""));
    EXPECT_EQ(lex7("ab", lgr), taul::token(lpr7, "ab"));
    EXPECT_EQ(lex0("aba", lgr), taul::token(lpr0, "ab"));
    EXPECT_EQ(lex1("aba", lgr), taul::token(lpr1, "ab"));
    EXPECT_EQ(lex2("aba", lgr), taul::token(lpr2, "ab"));
    EXPECT_EQ(lex3("aba", lgr), taul::token(lpr3, "ab"));
    EXPECT_EQ(lex4("aba", lgr), taul::token::failure(""));
    EXPECT_EQ(lex5("aba", lgr), taul::token::failure(""));
    EXPECT_EQ(lex6("aba", lgr), taul::token::failure(""));
    EXPECT_EQ(lex7("aba", lgr), taul::token(lpr7, "ab"));
    EXPECT_EQ(lex0("abab", lgr), taul::token(lpr0, "ab"));
    EXPECT_EQ(lex1("abab", lgr), taul::token(lpr1, "ab"));
    EXPECT_EQ(lex2("abab", lgr), taul::token(lpr2, "abab"));
    EXPECT_EQ(lex3("abab", lgr), taul::token(lpr3, "abab"));
    EXPECT_EQ(lex4("abab", lgr), taul::token(lpr4, "abab"));
    EXPECT_EQ(lex5("abab", lgr), taul::token(lpr5, "abab"));
    EXPECT_EQ(lex6("abab", lgr), taul::token(lpr6, "abab"));
    EXPECT_EQ(lex7("abab", lgr), taul::token(lpr7, "abab"));
    EXPECT_EQ(lex0("ababa", lgr), taul::token(lpr0, "ab"));
    EXPECT_EQ(lex1("ababa", lgr), taul::token(lpr1, "ab"));
    EXPECT_EQ(lex2("ababa", lgr), taul::token(lpr2, "abab"));
    EXPECT_EQ(lex3("ababa", lgr), taul::token(lpr3, "abab"));
    EXPECT_EQ(lex4("ababa", lgr), taul::token(lpr4, "abab"));
    EXPECT_EQ(lex5("ababa", lgr), taul::token(lpr5, "abab"));
    EXPECT_EQ(lex6("ababa", lgr), taul::token(lpr6, "abab"));
    EXPECT_EQ(lex7("ababa", lgr), taul::token(lpr7, "abab"));
    EXPECT_EQ(lex0("ababab", lgr), taul::token(lpr0, "ab"));
    EXPECT_EQ(lex1("ababab", lgr), taul::token(lpr1, "ab"));
    EXPECT_EQ(lex2("ababab", lgr), taul::token(lpr2, "ababab"));
    EXPECT_EQ(lex3("ababab", lgr), taul::token(lpr3, "ababab"));
    EXPECT_EQ(lex4("ababab", lgr), taul::token(lpr4, "abab"));
    EXPECT_EQ(lex5("ababab", lgr), taul::token(lpr5, "ababab"));
    EXPECT_EQ(lex6("ababab", lgr), taul::token(lpr6, "ababab"));
    EXPECT_EQ(lex7("ababab", lgr), taul::token(lpr7, "ababab"));
    EXPECT_EQ(lex0("abababa", lgr), taul::token(lpr0, "ab"));
    EXPECT_EQ(lex1("abababa", lgr), taul::token(lpr1, "ab"));
    EXPECT_EQ(lex2("abababa", lgr), taul::token(lpr2, "ababab"));
    EXPECT_EQ(lex3("abababa", lgr), taul::token(lpr3, "ababab"));
    EXPECT_EQ(lex4("abababa", lgr), taul::token(lpr4, "abab"));
    EXPECT_EQ(lex5("abababa", lgr), taul::token(lpr5, "ababab"));
    EXPECT_EQ(lex6("abababa", lgr), taul::token(lpr6, "ababab"));
    EXPECT_EQ(lex7("abababa", lgr), taul::token(lpr7, "ababab"));
    EXPECT_EQ(lex0("abababab", lgr), taul::token(lpr0, "ab"));
    EXPECT_EQ(lex1("abababab", lgr), taul::token(lpr1, "ab"));
    EXPECT_EQ(lex2("abababab", lgr), taul::token(lpr2, "abababab"));
    EXPECT_EQ(lex3("abababab", lgr), taul::token(lpr3, "abababab"));
    EXPECT_EQ(lex4("abababab", lgr), taul::token(lpr4, "abab"));
    EXPECT_EQ(lex5("abababab", lgr), taul::token(lpr5, "abababab"));
    EXPECT_EQ(lex6("abababab", lgr), taul::token(lpr6, "abababab"));
    EXPECT_EQ(lex7("abababab", lgr), taul::token(lpr7, "abababab"));
    EXPECT_EQ(lex0("ababababa", lgr), taul::token(lpr0, "ab"));
    EXPECT_EQ(lex1("ababababa", lgr), taul::token(lpr1, "ab"));
    EXPECT_EQ(lex2("ababababa", lgr), taul::token(lpr2, "abababab"));
    EXPECT_EQ(lex3("ababababa", lgr), taul::token(lpr3, "abababab"));
    EXPECT_EQ(lex4("ababababa", lgr), taul::token(lpr4, "abab"));
    EXPECT_EQ(lex5("ababababa", lgr), taul::token(lpr5, "abababab"));
    EXPECT_EQ(lex6("ababababa", lgr), taul::token(lpr6, "abababab"));
    EXPECT_EQ(lex7("ababababa", lgr), taul::token(lpr7, "abababab"));
    EXPECT_EQ(lex0("ababababab", lgr), taul::token(lpr0, "ab"));
    EXPECT_EQ(lex1("ababababab", lgr), taul::token(lpr1, "ab"));
    EXPECT_EQ(lex2("ababababab", lgr), taul::token(lpr2, "ababababab"));
    EXPECT_EQ(lex3("ababababab", lgr), taul::token(lpr3, "ababababab"));
    EXPECT_EQ(lex4("ababababab", lgr), taul::token(lpr4, "abab"));
    EXPECT_EQ(lex5("ababababab", lgr), taul::token(lpr5, "ababababab"));
    EXPECT_EQ(lex6("ababababab", lgr), taul::token(lpr6, "abababab"));
    EXPECT_EQ(lex7("ababababab", lgr), taul::token(lpr7, "abababab"));


    // test f0, f1, f2, f3, f4, f5, f6, and f7 w/ offset

    // success/failure

    // TODO: we *could* add more comprehensive tests testing strings other than 'abab...',
    //       but I'm happy w/ the below

    EXPECT_EQ(lex0("+-=", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=", 3, lgr), taul::token(lpr1, "", 3));
    EXPECT_EQ(lex2("+-=", 3, lgr), taul::token(lpr2, "", 3));
    EXPECT_EQ(lex3("+-=", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex4("+-=", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex5("+-=", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex6("+-=", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex7("+-=", 3, lgr), taul::token(lpr7, "", 3));
    EXPECT_EQ(lex0("+-=a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=a", 3, lgr), taul::token(lpr1, "", 3));
    EXPECT_EQ(lex2("+-=a", 3, lgr), taul::token(lpr2, "", 3));
    EXPECT_EQ(lex3("+-=a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex4("+-=a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex5("+-=a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex6("+-=a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex7("+-=a", 3, lgr), taul::token(lpr7, "", 3));
    EXPECT_EQ(lex0("+-=ab", 3, lgr), taul::token(lpr0, "ab", 3));
    EXPECT_EQ(lex1("+-=ab", 3, lgr), taul::token(lpr1, "ab", 3));
    EXPECT_EQ(lex2("+-=ab", 3, lgr), taul::token(lpr2, "ab", 3));
    EXPECT_EQ(lex3("+-=ab", 3, lgr), taul::token(lpr3, "ab", 3));
    EXPECT_EQ(lex4("+-=ab", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex5("+-=ab", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex6("+-=ab", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex7("+-=ab", 3, lgr), taul::token(lpr7, "ab", 3));
    EXPECT_EQ(lex0("+-=aba", 3, lgr), taul::token(lpr0, "ab", 3));
    EXPECT_EQ(lex1("+-=aba", 3, lgr), taul::token(lpr1, "ab", 3));
    EXPECT_EQ(lex2("+-=aba", 3, lgr), taul::token(lpr2, "ab", 3));
    EXPECT_EQ(lex3("+-=aba", 3, lgr), taul::token(lpr3, "ab", 3));
    EXPECT_EQ(lex4("+-=aba", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex5("+-=aba", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex6("+-=aba", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex7("+-=aba", 3, lgr), taul::token(lpr7, "ab", 3));
    EXPECT_EQ(lex0("+-=abab", 3, lgr), taul::token(lpr0, "ab", 3));
    EXPECT_EQ(lex1("+-=abab", 3, lgr), taul::token(lpr1, "ab", 3));
    EXPECT_EQ(lex2("+-=abab", 3, lgr), taul::token(lpr2, "abab", 3));
    EXPECT_EQ(lex3("+-=abab", 3, lgr), taul::token(lpr3, "abab", 3));
    EXPECT_EQ(lex4("+-=abab", 3, lgr), taul::token(lpr4, "abab", 3));
    EXPECT_EQ(lex5("+-=abab", 3, lgr), taul::token(lpr5, "abab", 3));
    EXPECT_EQ(lex6("+-=abab", 3, lgr), taul::token(lpr6, "abab", 3));
    EXPECT_EQ(lex7("+-=abab", 3, lgr), taul::token(lpr7, "abab", 3));
    EXPECT_EQ(lex0("+-=ababa", 3, lgr), taul::token(lpr0, "ab", 3));
    EXPECT_EQ(lex1("+-=ababa", 3, lgr), taul::token(lpr1, "ab", 3));
    EXPECT_EQ(lex2("+-=ababa", 3, lgr), taul::token(lpr2, "abab", 3));
    EXPECT_EQ(lex3("+-=ababa", 3, lgr), taul::token(lpr3, "abab", 3));
    EXPECT_EQ(lex4("+-=ababa", 3, lgr), taul::token(lpr4, "abab", 3));
    EXPECT_EQ(lex5("+-=ababa", 3, lgr), taul::token(lpr5, "abab", 3));
    EXPECT_EQ(lex6("+-=ababa", 3, lgr), taul::token(lpr6, "abab", 3));
    EXPECT_EQ(lex7("+-=ababa", 3, lgr), taul::token(lpr7, "abab", 3));
    EXPECT_EQ(lex0("+-=ababab", 3, lgr), taul::token(lpr0, "ab", 3));
    EXPECT_EQ(lex1("+-=ababab", 3, lgr), taul::token(lpr1, "ab", 3));
    EXPECT_EQ(lex2("+-=ababab", 3, lgr), taul::token(lpr2, "ababab", 3));
    EXPECT_EQ(lex3("+-=ababab", 3, lgr), taul::token(lpr3, "ababab", 3));
    EXPECT_EQ(lex4("+-=ababab", 3, lgr), taul::token(lpr4, "abab", 3));
    EXPECT_EQ(lex5("+-=ababab", 3, lgr), taul::token(lpr5, "ababab", 3));
    EXPECT_EQ(lex6("+-=ababab", 3, lgr), taul::token(lpr6, "ababab", 3));
    EXPECT_EQ(lex7("+-=ababab", 3, lgr), taul::token(lpr7, "ababab", 3));
    EXPECT_EQ(lex0("+-=abababa", 3, lgr), taul::token(lpr0, "ab", 3));
    EXPECT_EQ(lex1("+-=abababa", 3, lgr), taul::token(lpr1, "ab", 3));
    EXPECT_EQ(lex2("+-=abababa", 3, lgr), taul::token(lpr2, "ababab", 3));
    EXPECT_EQ(lex3("+-=abababa", 3, lgr), taul::token(lpr3, "ababab", 3));
    EXPECT_EQ(lex4("+-=abababa", 3, lgr), taul::token(lpr4, "abab", 3));
    EXPECT_EQ(lex5("+-=abababa", 3, lgr), taul::token(lpr5, "ababab", 3));
    EXPECT_EQ(lex6("+-=abababa", 3, lgr), taul::token(lpr6, "ababab", 3));
    EXPECT_EQ(lex7("+-=abababa", 3, lgr), taul::token(lpr7, "ababab", 3));
    EXPECT_EQ(lex0("+-=abababab", 3, lgr), taul::token(lpr0, "ab", 3));
    EXPECT_EQ(lex1("+-=abababab", 3, lgr), taul::token(lpr1, "ab", 3));
    EXPECT_EQ(lex2("+-=abababab", 3, lgr), taul::token(lpr2, "abababab", 3));
    EXPECT_EQ(lex3("+-=abababab", 3, lgr), taul::token(lpr3, "abababab", 3));
    EXPECT_EQ(lex4("+-=abababab", 3, lgr), taul::token(lpr4, "abab", 3));
    EXPECT_EQ(lex5("+-=abababab", 3, lgr), taul::token(lpr5, "abababab", 3));
    EXPECT_EQ(lex6("+-=abababab", 3, lgr), taul::token(lpr6, "abababab", 3));
    EXPECT_EQ(lex7("+-=abababab", 3, lgr), taul::token(lpr7, "abababab", 3));
    EXPECT_EQ(lex0("+-=ababababa", 3, lgr), taul::token(lpr0, "ab", 3));
    EXPECT_EQ(lex1("+-=ababababa", 3, lgr), taul::token(lpr1, "ab", 3));
    EXPECT_EQ(lex2("+-=ababababa", 3, lgr), taul::token(lpr2, "abababab", 3));
    EXPECT_EQ(lex3("+-=ababababa", 3, lgr), taul::token(lpr3, "abababab", 3));
    EXPECT_EQ(lex4("+-=ababababa", 3, lgr), taul::token(lpr4, "abab", 3));
    EXPECT_EQ(lex5("+-=ababababa", 3, lgr), taul::token(lpr5, "abababab", 3));
    EXPECT_EQ(lex6("+-=ababababa", 3, lgr), taul::token(lpr6, "abababab", 3));
    EXPECT_EQ(lex7("+-=ababababa", 3, lgr), taul::token(lpr7, "abababab", 3));
    EXPECT_EQ(lex0("+-=ababababab", 3, lgr), taul::token(lpr0, "ab", 3));
    EXPECT_EQ(lex1("+-=ababababab", 3, lgr), taul::token(lpr1, "ab", 3));
    EXPECT_EQ(lex2("+-=ababababab", 3, lgr), taul::token(lpr2, "ababababab", 3));
    EXPECT_EQ(lex3("+-=ababababab", 3, lgr), taul::token(lpr3, "ababababab", 3));
    EXPECT_EQ(lex4("+-=ababababab", 3, lgr), taul::token(lpr4, "abab", 3));
    EXPECT_EQ(lex5("+-=ababababab", 3, lgr), taul::token(lpr5, "ababababab", 3));
    EXPECT_EQ(lex6("+-=ababababab", 3, lgr), taul::token(lpr6, "abababab", 3));
    EXPECT_EQ(lex7("+-=ababababab", 3, lgr), taul::token(lpr7, "abababab", 3));
}

TEST_F(GrammarUsage, lexerExpr_assertion) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f0")
        .lpr_decl("f1")
        .lpr("f0") // test w/ positive assertion
        .string("abc")
        .assertion()
        .string("def")
        .close()
        .close()
        .lpr("f1") // test w/ negative assertion
        .string("abc")
        .assertion(taul::polarity::negative)
        .string("def")
        .close()
        .close()
        .done();

    const taul::grammar gram = std::move(*taul::load(spec, lgr));
    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    const auto lex0 = gram.lexer("f0");
    const auto lex1 = gram.lexer("f1");


    // test w/out offset

    // success/failure

    EXPECT_EQ(lex0("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("ab", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("ab", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("abc", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("abc", lgr), taul::token(lpr1, "abc"));
    EXPECT_EQ(lex0("abcd", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("abcd", lgr), taul::token(lpr1, "abc"));
    EXPECT_EQ(lex0("abcde", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("abcde", lgr), taul::token(lpr1, "abc"));
    EXPECT_EQ(lex0("abcdef", lgr), taul::token(lpr0, "abc"));
    EXPECT_EQ(lex1("abcdef", lgr), taul::token::failure(""));

    EXPECT_EQ(lex0("abcdef123", lgr), taul::token(lpr0, "abc"));
    EXPECT_EQ(lex1("abcdef123", lgr), taul::token::failure(""));

    EXPECT_EQ(lex0("abc\r\n\t", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("abc\r\n\t", lgr), taul::token(lpr1, "abc"));


    // test w/ offset

    // success/failure

    EXPECT_EQ(lex0("+-=", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=ab", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=ab", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=abc", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=abc", 3, lgr), taul::token(lpr1, "abc", 3));
    EXPECT_EQ(lex0("+-=abcd", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=abcd", 3, lgr), taul::token(lpr1, "abc", 3));
    EXPECT_EQ(lex0("+-=abcde", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=abcde", 3, lgr), taul::token(lpr1, "abc", 3));
    EXPECT_EQ(lex0("+-=abcdef", 3, lgr), taul::token(lpr0, "abc", 3));
    EXPECT_EQ(lex1("+-=abcdef", 3, lgr), taul::token::failure("", 3));

    EXPECT_EQ(lex0("+-=abcdef123", 3, lgr), taul::token(lpr0, "abc", 3));
    EXPECT_EQ(lex1("+-=abcdef123", 3, lgr), taul::token::failure("", 3));

    EXPECT_EQ(lex0("+-=abc\r\n\t", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("+-=abc\r\n\t", 3, lgr), taul::token(lpr1, "abc", 3));
}

TEST_F(GrammarUsage, lexerExpr_constraint) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f0")
        .lpr_decl("f1")
        .lpr("f0") // test w/ positive constraint
        .constraint()
        .sequence()
        .any()
        .any()
        .any()
        .close()
        .junction()
        .string("abc")
        .close()
        .close()
        .lpr("f1") // test w/ negative constraint
        .constraint(taul::polarity::negative)
        .sequence()
        .any()
        .any()
        .any()
        .close()
        .junction()
        .string("abc")
        .close()
        .close()
        .done();

    const taul::grammar gram = std::move(*taul::load(spec, lgr));
    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    const auto lex0 = gram.lexer("f0");
    const auto lex1 = gram.lexer("f1");


    // test w/out offset

    // success

    EXPECT_EQ(lex0("abc", lgr), taul::token(lpr0, "abc"));
    EXPECT_EQ(lex0("abc123", lgr), taul::token(lpr0, "abc"));

    EXPECT_EQ(lex1("def", lgr), taul::token(lpr1, "def"));
    EXPECT_EQ(lex1("123", lgr), taul::token(lpr1, "123"));
    EXPECT_EQ(lex1("ABC", lgr), taul::token(lpr1, "ABC"));
    EXPECT_EQ(lex1("$*&", lgr), taul::token(lpr1, "$*&"));
    EXPECT_EQ(lex1(" \t\v", lgr), taul::token(lpr1, " \t\v"));
    EXPECT_EQ(lex1("\r\n\t", lgr), taul::token(lpr1, "\r\n\t"));
    EXPECT_EQ(lex1("def123", lgr), taul::token(lpr1, "def"));
    EXPECT_EQ(lex1("123123", lgr), taul::token(lpr1, "123"));
    EXPECT_EQ(lex1("ABC123", lgr), taul::token(lpr1, "ABC"));
    EXPECT_EQ(lex1("$*&123", lgr), taul::token(lpr1, "$*&"));
    EXPECT_EQ(lex1(" \t\v123", lgr), taul::token(lpr1, " \t\v"));
    EXPECT_EQ(lex1("\r\n\t123", lgr), taul::token(lpr1, "\r\n\t"));

    // failure

    EXPECT_EQ(lex0("a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("ab", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("abd", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("def", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("123", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("$*&", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0(" \t\v", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("\r\n\t", lgr), taul::token::failure(""));

    EXPECT_EQ(lex1("abc", lgr), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(lex0("+-=abc", 3, lgr), taul::token(lpr0, "abc", 3));
    EXPECT_EQ(lex0("+-=abc123", 3, lgr), taul::token(lpr0, "abc", 3));

    EXPECT_EQ(lex1("+-=def", 3, lgr), taul::token(lpr1, "def", 3));
    EXPECT_EQ(lex1("+-=123", 3, lgr), taul::token(lpr1, "123", 3));
    EXPECT_EQ(lex1("+-=ABC", 3, lgr), taul::token(lpr1, "ABC", 3));
    EXPECT_EQ(lex1("+-=$*&", 3, lgr), taul::token(lpr1, "$*&", 3));
    EXPECT_EQ(lex1("+-= \t\v", 3, lgr), taul::token(lpr1, " \t\v", 3));
    EXPECT_EQ(lex1("+-=\r\n\t", 3, lgr), taul::token(lpr1, "\r\n\t", 3));
    EXPECT_EQ(lex1("+-=def123", 3, lgr), taul::token(lpr1, "def", 3));
    EXPECT_EQ(lex1("+-=123123", 3, lgr), taul::token(lpr1, "123", 3));
    EXPECT_EQ(lex1("+-=ABC123", 3, lgr), taul::token(lpr1, "ABC", 3));
    EXPECT_EQ(lex1("+-=$*&123", 3, lgr), taul::token(lpr1, "$*&", 3));
    EXPECT_EQ(lex1("+-= \t\v123", 3, lgr), taul::token(lpr1, " \t\v", 3));
    EXPECT_EQ(lex1("+-=\r\n\t123", 3, lgr), taul::token(lpr1, "\r\n\t", 3));

    // failure

    EXPECT_EQ(lex0("+-=a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=ab", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=abd", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=def", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=123", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=$*&", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-= \t\v", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=\r\n\t", 3, lgr), taul::token::failure("", 3));

    EXPECT_EQ(lex1("+-=abc", 3, lgr), taul::token::failure("", 3));
}

TEST_F(GrammarUsage, lexerExpr_localend) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f0")
        .lpr_decl("f1")
        .lpr("f0") // test w/ positive constraint
        .constraint()
        .modifier(0, 5)
        .any()
        .close()
        .junction()
        .sequence()
        .string("abc")
        .localend()
        .close()
        .close()
        .close()
        .lpr("f1") // test w/ negative constraint
        .constraint(taul::polarity::negative)
        .modifier(0, 5)
        .any()
        .close()
        .junction()
        .sequence()
        .string("abc")
        .localend()
        .close()
        .close()
        .close()
        .done();

    const taul::grammar gram = std::move(*taul::load(spec, lgr));
    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    const auto lex0 = gram.lexer("f0");
    const auto lex1 = gram.lexer("f1");


    // test w/out offset

    // success

    EXPECT_EQ(lex0("abc", lgr), taul::token(lpr0, "abc"));

    EXPECT_EQ(lex1("", lgr), taul::token(lpr1, ""));
    EXPECT_EQ(lex1("a", lgr), taul::token(lpr1, "a"));
    EXPECT_EQ(lex1("ab", lgr), taul::token(lpr1, "ab"));
    EXPECT_EQ(lex1("abcd", lgr), taul::token(lpr1, "abcd"));
    EXPECT_EQ(lex1("abcde", lgr), taul::token(lpr1, "abcde"));
    EXPECT_EQ(lex1("abcdef", lgr), taul::token(lpr1, "abcde"));

    EXPECT_EQ(lex1("def", lgr), taul::token(lpr1, "def"));
    EXPECT_EQ(lex1("123", lgr), taul::token(lpr1, "123"));
    EXPECT_EQ(lex1("ABC", lgr), taul::token(lpr1, "ABC"));
    EXPECT_EQ(lex1("$*&^%", lgr), taul::token(lpr1, "$*&^%"));
    EXPECT_EQ(lex1("\r\n\t \t\n\r", lgr), taul::token(lpr1, "\r\n\t \t"));

    // failure

    EXPECT_EQ(lex0("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("ab", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("abcd", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("abcde", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("abcdef", lgr), taul::token::failure(""));

    EXPECT_EQ(lex0("def", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("123", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("ABC", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("$*&^%", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("\r\n\t \t\n\r", lgr), taul::token::failure(""));

    EXPECT_EQ(lex1("abc", lgr), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(lex0("+-=abc", 3, lgr), taul::token(lpr0, "abc", 3));

    EXPECT_EQ(lex1("+-=", 3, lgr), taul::token(lpr1, "", 3));
    EXPECT_EQ(lex1("+-=a", 3, lgr), taul::token(lpr1, "a", 3));
    EXPECT_EQ(lex1("+-=ab", 3, lgr), taul::token(lpr1, "ab", 3));
    EXPECT_EQ(lex1("+-=abcd", 3, lgr), taul::token(lpr1, "abcd", 3));
    EXPECT_EQ(lex1("+-=abcde", 3, lgr), taul::token(lpr1, "abcde", 3));
    EXPECT_EQ(lex1("+-=abcdef", 3, lgr), taul::token(lpr1, "abcde", 3));

    EXPECT_EQ(lex1("+-=def", 3, lgr), taul::token(lpr1, "def", 3));
    EXPECT_EQ(lex1("+-=123", 3, lgr), taul::token(lpr1, "123", 3));
    EXPECT_EQ(lex1("+-=ABC", 3, lgr), taul::token(lpr1, "ABC", 3));
    EXPECT_EQ(lex1("+-=$*&^%", 3, lgr), taul::token(lpr1, "$*&^%", 3));
    EXPECT_EQ(lex1("+-=\r\n\t \t\n\r", 3, lgr), taul::token(lpr1, "\r\n\t \t", 3));

    // failure

    EXPECT_EQ(lex0("+-=", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=ab", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=abcd", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=abcde", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=abcdef", 3, lgr), taul::token::failure("", 3));

    EXPECT_EQ(lex0("+-=def", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=123", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=ABC", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=$*&^%", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("+-=\r\n\t \t\n\r", 3, lgr), taul::token::failure("", 3));

    EXPECT_EQ(lex1("+-=abc", 3, lgr), taul::token::failure("", 3));
}

