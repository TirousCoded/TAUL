

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


class GrammarUsageTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr;
    std::shared_ptr<taul::context> ctx;


    void SetUp() override final {

        lgr = taul::make_stderr_logger();
        ctx = std::make_shared<taul::context>(lgr);
    }


    inline bool _example(
        const taul::node& expect,
        const std::optional<taul::node>& actual) {
        TAUL_LOG(lgr, "expected:\n{}", expect.fmt_tree());
        if ((bool)actual) {
            bool success = true;
            if (!expect.equiv(*actual)) {
                TAUL_LOG(lgr, "!expect.equiv(*actual) detected!");
                success = false;
            }
            if (expect.str() != actual->str()) {
                TAUL_LOG(lgr, "expect.str() != actual->str() detected!");
                success = false;
            }
            TAUL_LOG(lgr, "actual:\n{}", actual->fmt_tree());
            return success;
        }
        return false;
    }

    inline void _log(
        const std::string& src, 
        const taul::token_seq& tkns) {
        TAUL_LOG(lgr, "src==\"{}\"", src);
        TAUL_LOG(lgr, "tkns ({})", tkns.size());
        for (const auto& I : tkns) {
            TAUL_LOG(lgr, "    {}", I);
        }
    }
};


TEST_F(GrammarUsageTests, GrammarWide) {

    auto spec =
        taul::spec_writer()
        .lpr_decl("ab1")
        .lpr_decl("ab2")
        .lpr_decl("a")
        .lpr_decl("c")
        .lpr_decl("bc1")
        .lpr_decl("bc2")
        .lpr_decl("ws")
        .lpr_decl("abc")
        .lpr("ab1")
        .string("ab")
        .close()
        .lpr("ab2")
        .string("ab")
        .close()
        .lpr("a")
        .string("a")
        .close()
        .lpr("c")
        .string("c")
        .close()
        .lpr("bc1")
        .string("bc")
        .close()
        .lpr("bc2")
        .string("bc")
        .close()
        .lpr("ws", taul::qualifier::skip)
        .charset(" \t")
        .close()
        .lpr("abc", taul::qualifier::support)
        .string("abc")
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);


    auto adv =
        [](taul::source_pos& cntr, taul::source_pos incr) -> taul::source_pos {
        const auto result = cntr;
        cntr += incr;
        return result;
        };


    // test w/out offset w/ skip token cutting

    {
        const auto tkns = ctx->tokenize("abcabc abcabcabc abc  abcabcabc");
        if (tkns.size() == 18) {
            taul::source_pos cntr = 0;
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            adv(cntr, 2);
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    // test w/ offset w/ skip token cutting

    {
        const auto tkns = ctx->tokenize("+-=abcabc abcabcabc abc  abcabcabc", 3);
        if (tkns.size() == 18) {
            taul::source_pos cntr = 0;
            adv(cntr, 3);
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            adv(cntr, 2);
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    // test w/out offset w/out skip token cutting

    {
        const auto tkns = ctx->tokenize("abcabc abcabcabc abc  abcabcabc", 0, false);
        if (tkns.size() == 22) {
            taul::source_pos cntr = 0;
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[18], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[19], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[20], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[21], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    // test w/ offset w/out skip token cutting

    {
        const auto tkns = ctx->tokenize("+-=abcabc abcabcabc abc  abcabcabc", 3, false);
        if (tkns.size() == 22) {
            taul::source_pos cntr = 0;
            adv(cntr, 3);
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[18], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[19], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[20], taul::token(gram.lpr("ab1"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[21], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }
}


TEST_F(GrammarUsageTests, LexerExpr_TopLevel) {
    
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

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    ////const auto lex0 = gram.lexer("f0");
    ////const auto lex1 = gram.lexer("f1");


    // the below assertions expect the embedded exprs to work as expected


    // test w/out offset

    // success

    EXPECT_EQ(ctx->match("f0", ""), taul::token(lpr0, ""));
    EXPECT_EQ(ctx->match("f0", "abcde"), taul::token(lpr0, ""));
    EXPECT_EQ(ctx->match("f0", "12345"), taul::token(lpr0, ""));
    EXPECT_EQ(ctx->match("f0", "\r\n\t\v"), taul::token(lpr0, ""));

    EXPECT_EQ(ctx->match("f1", "abcde"), taul::token(lpr1, "abc"));
    EXPECT_EQ(ctx->match("f1", "12345"), taul::token(lpr1, "123"));
    EXPECT_EQ(ctx->match("f1", "\r\n\t\v"), taul::token(lpr1, "\r\n\t"));

    // failure

    EXPECT_EQ(ctx->match("f1", "a"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "1"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "\r"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "ab"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "12"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "\r\n"), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(ctx->match("f0", "abc", 3), taul::token(lpr0, "", 3));
    EXPECT_EQ(ctx->match("f0", "abcabcde", 3), taul::token(lpr0, "", 3));
    EXPECT_EQ(ctx->match("f0", "abc12345", 3), taul::token(lpr0, "", 3));
    EXPECT_EQ(ctx->match("f0", "abc\r\n\t\v", 3), taul::token(lpr0, "", 3));

    EXPECT_EQ(ctx->match("f1", "abcabcde", 3), taul::token(lpr1, "abc", 3));
    EXPECT_EQ(ctx->match("f1", "abc12345", 3), taul::token(lpr1, "123", 3));
    EXPECT_EQ(ctx->match("f1", "abc\r\n\t\v", 3), taul::token(lpr1, "\r\n\t", 3));

    // failure

    EXPECT_EQ(ctx->match("f1", "abca", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "abc1", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "abc\r", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "abcab", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "abc12", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "abc\r\n", 3), taul::token::failure("", 3));
}

TEST_F(GrammarUsageTests, ParserExpr_TopLevel) {

    // remember that all parser expr tests should include testing things like
    // that the impl can handle token sequences w/ 'skip tokens' cut out

    // testing w/ regards to failure tokens should also be ensured

    const auto spec =
        taul::spec_writer()
        .lpr_decl("ws")
        .lpr_decl("chr")
        .ppr_decl("f0")
        .ppr_decl("f1")
        .lpr("ws", taul::qualifier::skip)
        .string(" ")
        .close()
        .lpr("chr")
        .any()
        .close()
        // test w/ 0 subexprs
        .ppr("f0")
        .close()
        // test w/ 3 subexprs
        .ppr("f1")
        .any()
        .any()
        .any()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto& f1 = gram.ppr("f1");
    //const auto psr0 = gram.parser("f0");
    //const auto psr1 = gram.parser("f1");


    // the below assertions expect the embedded exprs to work as expected


    // test w/ and w/out offset

    // success

    {
        // test w/ no tokens

        std::string src = "   ";
        auto tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(f0, "", 0);

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 0)));
    }

    {
        // test w/ skip tokens cut after non-cut token(s)

        std::string src = "a   ";
        auto tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(f0, "", 0);
        auto f0_expect1 = ctx->create(f0, "", 1);

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 0)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 1)));
    }

    {
        std::string src = "abc  123 45  6";
        auto tkns = ctx->tokenize(src);
        _log(src, tkns);

        std::string_view view = src;

        auto f0_expect0 = ctx->create(f0, "", 0);
        auto f0_expect1 = ctx->create(f0, "", 2);
        auto f0_expect2 = ctx->create(f0, "", 9);
        auto f0_expect3 = ctx->create(f0, "", 14);

        auto f1_expect0 = ctx->create(f1, view.substr(0, 3), 0);
        f1_expect0.attach(ctx->create(chr, "a", 0));
        f1_expect0.attach(ctx->create(chr, "b", 1));
        f1_expect0.attach(ctx->create(chr, "c", 2));
        auto f1_expect1 = ctx->create(f1, view.substr(2, 5), 2);
        f1_expect1.attach(ctx->create(chr, "c", 2));
        f1_expect1.attach(ctx->create(chr, "1", 5));
        f1_expect1.attach(ctx->create(chr, "2", 6));
        auto f1_expect2 = ctx->create(f1, view.substr(9, 5), 9);
        f1_expect2.attach(ctx->create(chr, "4", 9));
        f1_expect2.attach(ctx->create(chr, "5", 10));
        f1_expect2.attach(ctx->create(chr, "6", 13));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 0)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 2)));
        EXPECT_TRUE(_example(f0_expect2, ctx->parse("f0", tkns, 6)));
        EXPECT_TRUE(_example(f0_expect3, ctx->parse("f0", tkns, 9)));

        EXPECT_TRUE(_example(f1_expect0, ctx->parse("f1", tkns, 0)));
        EXPECT_TRUE(_example(f1_expect1, ctx->parse("f1", tkns, 2)));
        EXPECT_TRUE(_example(f1_expect2, ctx->parse("f1", tkns, 6)));
    }

    // failure

    {
        std::string src = "abc  123 45  6";
        auto tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto tkns0 = tkns;
        auto tkns1 = tkns;
        auto tkns2 = tkns;
        tkns0.pop(tkns0.size() - 0);
        tkns1.pop(tkns1.size() - 1);
        tkns2.pop(tkns2.size() - 2);

        EXPECT_FALSE((bool)ctx->parse("f1", tkns0));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns1));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns2));

        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 7));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 8));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 9));
    }
}

TEST_F(GrammarUsageTests, LexerExpr_End) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .end()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& lpr = gram.lpr("f");
    //const auto lex = gram.lexer("f");


    // test w/out offset

    // success

    EXPECT_EQ(ctx->match("f", ""), taul::token(lpr, ""));

    // failure

    EXPECT_EQ(ctx->match("f", "a"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f", "1"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f", "&"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f", "&!%"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f", " \t"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f", "\r\n"), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(ctx->match("f", "abc", 3), taul::token(lpr, "", 3));

    // failure

    EXPECT_EQ(ctx->match("f", "abca", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f", "abc1", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f", "abc&", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f", "abcabc", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f", "abc123", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f", "abc&!%", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f", "abc \t", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f", "abc\r\n", 3), taul::token::failure("", 3));
}

TEST_F(GrammarUsageTests, ParserExpr_End) {

    // remember that all parser expr tests should include testing things like
    // that the impl can handle token sequences w/ 'skip tokens' cut out

    // testing w/ regards to failure tokens should also be ensured

    const auto spec =
        taul::spec_writer()
        .lpr_decl("ws")
        .lpr_decl("chr")
        .ppr_decl("f0")
        .lpr("ws", taul::qualifier::skip)
        .string(" ")
        .close()
        .lpr("chr")
        .any()
        .close()
        .ppr("f0")
        .end()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    //const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    // test w/ and w/out offset

    // success

    {
        std::string src = "abc  123 45  6";
        auto tkns = ctx->tokenize(src);
        _log(src, tkns);

        taul::token_seq tkns0("");

        auto f0_expect0 = ctx->create(f0, "");
        auto f0_expect1 = ctx->create(f0, "", 14);

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns0)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 9)));
    }

    // failure

    {
        std::string src = "abc  123 45  6";
        auto tkns = ctx->tokenize(src);
        _log(src, tkns);

        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 0));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 6));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 8));
    }
}

TEST_F(GrammarUsageTests, LexerExpr_Any) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .any()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& lpr = gram.lpr("f");
    //const auto lex = gram.lexer("f");


    // test w/out offset

    // success

    EXPECT_EQ(ctx->match("f", "0"), taul::token(lpr, "0"));
    EXPECT_EQ(ctx->match("f", "1"), taul::token(lpr, "1"));
    EXPECT_EQ(ctx->match("f", "!"), taul::token(lpr, "!"));
    EXPECT_EQ(ctx->match("f", "\n\r"), taul::token(lpr, "\n"));
    EXPECT_EQ(ctx->match("f", "a"), taul::token(lpr, "a"));
    EXPECT_EQ(ctx->match("f", " "), taul::token(lpr, " "));
    EXPECT_EQ(ctx->match("f", "~"), taul::token(lpr, "~"));
    EXPECT_EQ(ctx->match("f", "abc"), taul::token(lpr, "a"));

    // failure

    EXPECT_EQ(ctx->match("f", ""), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(ctx->match("f", "abc0", 3), taul::token(lpr, "0", 3));
    EXPECT_EQ(ctx->match("f", "abc1", 3), taul::token(lpr, "1", 3));
    EXPECT_EQ(ctx->match("f", "abc!", 3), taul::token(lpr, "!", 3));
    EXPECT_EQ(ctx->match("f", "abc\n\r", 3), taul::token(lpr, "\n", 3));
    EXPECT_EQ(ctx->match("f", "abca", 3), taul::token(lpr, "a", 3));
    EXPECT_EQ(ctx->match("f", "abc ", 3), taul::token(lpr, " ", 3));
    EXPECT_EQ(ctx->match("f", "abc~", 3), taul::token(lpr, "~", 3));
    EXPECT_EQ(ctx->match("f", "abcabc", 3), taul::token(lpr, "a", 3));

    // failure

    EXPECT_EQ(ctx->match("f", "abc", 3), taul::token::failure("", 3));
}

TEST_F(GrammarUsageTests, ParserExpr_Any) {

    // remember that all parser expr tests should include testing things like
    // that the impl can handle token sequences w/ 'skip tokens' cut out

    // testing w/ regards to failure tokens should also be ensured

    const auto spec =
        taul::spec_writer()
        .lpr_decl("ws")
        .lpr_decl("chr")
        .ppr_decl("f0")
        .lpr("ws", taul::qualifier::skip)
        .string(" ")
        .close()
        .lpr("chr")
        .any()
        .close()
        .ppr("f0")
        .any()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    //const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    // test w/ and w/out offset

    // success

    {
        // just gonna *hack together* a token sequence w/ a
        // failure token for us to test w/

        std::string src = "!!!";
        taul::token_seq tkns(src);
        tkns.push_failure(3);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(f0, src);
        f0_expect0.attach(ctx->create(taul::token::failure(src)));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 0)));
    }

    {
        std::string src = "abc  123 45  6";
        auto tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(f0, "a", 0);
        f0_expect0.attach(ctx->create(chr, "a", 0));
        auto f0_expect1 = ctx->create(f0, "b", 1);
        f0_expect1.attach(ctx->create(chr, "b", 1));
        auto f0_expect2 = ctx->create(f0, "c", 2);
        f0_expect2.attach(ctx->create(chr, "c", 2));
        auto f0_expect3 = ctx->create(f0, "1", 5);
        f0_expect3.attach(ctx->create(chr, "1", 5));
        auto f0_expect4 = ctx->create(f0, "2", 6);
        f0_expect4.attach(ctx->create(chr, "2", 6));
        auto f0_expect5 = ctx->create(f0, "3", 7);
        f0_expect5.attach(ctx->create(chr, "3", 7));
        auto f0_expect6 = ctx->create(f0, "4", 9);
        f0_expect6.attach(ctx->create(chr, "4", 9));
        auto f0_expect7 = ctx->create(f0, "5", 10);
        f0_expect7.attach(ctx->create(chr, "5", 10));
        auto f0_expect8 = ctx->create(f0, "6", 13);
        f0_expect8.attach(ctx->create(chr, "6", 13));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 0)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 1)));
        EXPECT_TRUE(_example(f0_expect2, ctx->parse("f0", tkns, 2)));
        EXPECT_TRUE(_example(f0_expect3, ctx->parse("f0", tkns, 3)));
        EXPECT_TRUE(_example(f0_expect4, ctx->parse("f0", tkns, 4)));
        EXPECT_TRUE(_example(f0_expect5, ctx->parse("f0", tkns, 5)));
        EXPECT_TRUE(_example(f0_expect6, ctx->parse("f0", tkns, 6)));
        EXPECT_TRUE(_example(f0_expect7, ctx->parse("f0", tkns, 7)));
        EXPECT_TRUE(_example(f0_expect8, ctx->parse("f0", tkns, 8)));
    }

    // failure

    {
        std::string src = "abc  123 45  6";
        auto tkns = ctx->tokenize(src);
        _log(src, tkns);

        taul::token_seq tkns0("");

        EXPECT_FALSE((bool)ctx->parse("f0", tkns0));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 9));
    }
}

// the below test presumes usage of string_and_charset.h/cpp code, w/
// regards to escape sequences, and will thus test for this behaviour
// in summary

TEST_F(GrammarUsageTests, LexerExpr_String) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f0")
        .lpr_decl("f1")
        .lpr("f0")
        .string("abc")
        .close()
        .lpr("f1")
        .string("\\A\\r\\n\\B")
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    //const auto lex0 = gram.lexer("f0");
    //const auto lex1 = gram.lexer("f1");


    // test w/out offset

    // success

    EXPECT_EQ(ctx->match("f0", "abc"), taul::token(lpr0, "abc"));
    EXPECT_EQ(ctx->match("f0", "abc123"), taul::token(lpr0, "abc"));
    EXPECT_EQ(ctx->match("f0", "abc&^!"), taul::token(lpr0, "abc"));
    EXPECT_EQ(ctx->match("f0", "abc\r\n\t"), taul::token(lpr0, "abc"));

    EXPECT_EQ(ctx->match("f1", "A\r\nB"), taul::token(lpr1, "A\r\nB"));
    EXPECT_EQ(ctx->match("f1", "A\r\nBabc"), taul::token(lpr1, "A\r\nB"));

    // failure

    EXPECT_EQ(ctx->match("f0", ""), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", "a"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", "ab"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", "def"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", "&^!"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", "\r\n\t"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", " abc"), taul::token::failure(""));

    EXPECT_EQ(ctx->match("f1", "A\r\nC"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "A\r\n"), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(ctx->match("f0", "abcabc", 3), taul::token(lpr0, "abc", 3));
    EXPECT_EQ(ctx->match("f0", "abcabc123", 3), taul::token(lpr0, "abc", 3));
    EXPECT_EQ(ctx->match("f0", "abcabc&^!", 3), taul::token(lpr0, "abc", 3));
    EXPECT_EQ(ctx->match("f0", "abcabc\r\n\t", 3), taul::token(lpr0, "abc", 3));

    EXPECT_EQ(ctx->match("f1", "___A\r\nB", 3), taul::token(lpr1, "A\r\nB", 3));
    EXPECT_EQ(ctx->match("f1", "___A\r\nBabc", 3), taul::token(lpr1, "A\r\nB", 3));

    // failure

    EXPECT_EQ(ctx->match("f0", "abc", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abca", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abcab", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abcdef", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abc&^!", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abc\r\n\t", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abc abc", 3), taul::token::failure("", 3));

    EXPECT_EQ(ctx->match("f1", "___A\r\nC", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___A\r\n", 3), taul::token::failure("", 3));
}

// the below test presumes usage of string_and_charset.h/cpp code, w/
// regards to escape sequences, and will thus test for this behaviour
// in summary

TEST_F(GrammarUsageTests, ParserExpr_string) {

    // remember that all parser expr tests should include testing things like
    // that the impl can handle token sequences w/ 'skip tokens' cut out

    // testing w/ regards to failure tokens should also be ensured

    const auto spec =
        taul::spec_writer()
        .lpr_decl("ws")
        .lpr_decl("chr")
        .ppr_decl("f0")
        .lpr("ws", taul::qualifier::skip)
        .string("\\x20") // <- test escape seq w/ '\x20' == ' '
        .close()
        .lpr("chr")
        .any()
        .close()
        .ppr("f0")
        .string("a")
        .any()
        .string("\\c") // <- test escape seq
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    //const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    // test w/ and w/out offset

    // success

    {
        // just gonna *hack together* a token sequence w/ a
        // failure token for us to test w/

        std::string src = "abc";
        taul::token_seq tkns(src);
        tkns.push_failure(1);
        tkns.push(gram.lpr("chr"), 1);
        tkns.push_failure(1);
        _log(src, tkns);

        EXPECT_FALSE(ctx->parse("f0", tkns));
    }

    {
        std::string src = "ab c a 1c";
        auto tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(f0, "ab c", 0);
        f0_expect0.attach(ctx->create(chr, "a", 0));
        f0_expect0.attach(ctx->create(chr, "b", 1));
        f0_expect0.attach(ctx->create(chr, "c", 3));

        auto f0_expect1 = ctx->create(f0, "a 1c", 5);
        f0_expect1.attach(ctx->create(chr, "a", 5));
        f0_expect1.attach(ctx->create(chr, "1", 7));
        f0_expect1.attach(ctx->create(chr, "c", 8));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 0)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 3)));
    }

    // failure

    {
        std::string src = "bbc abb a c";
        auto tkns = ctx->tokenize(src);
        _log(src, tkns);

        taul::token_seq tkns0("");

        EXPECT_FALSE((bool)ctx->parse("f0", tkns0));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 0));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 3));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 6));
    }
}

// the below test presumes usage of string_and_charset.h/cpp code, w/
// regards to charset strings, and will thus test for this behaviour
// in summary

TEST_F(GrammarUsageTests, LexerExpr_Charset) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f0")
        .lpr_decl("f1")
        .lpr("f0")
        .charset("ab\\c") // <- test escape seq w/ c
        .close()
        .lpr("f1")
        .charset("1a-c2z-x3") // <- expect charset string parsing by load
        .close()
        .done();

    TAUL_LOG(lgr, "taul::parse_taul_charset(\"1a-c2z-x3\") == {}", taul::parse_taul_charset("1a-c2z-x3"));

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    //const auto lex0 = gram.lexer("f0");
    //const auto lex1 = gram.lexer("f1");


    // test w/out offset

    // success

    EXPECT_EQ(ctx->match("f0", "a"), taul::token(lpr0, "a"));
    EXPECT_EQ(ctx->match("f0", "b"), taul::token(lpr0, "b"));
    EXPECT_EQ(ctx->match("f0", "c"), taul::token(lpr0, "c"));
    EXPECT_EQ(ctx->match("f0", "a123"), taul::token(lpr0, "a"));
    EXPECT_EQ(ctx->match("f0", "b&^!"), taul::token(lpr0, "b"));
    EXPECT_EQ(ctx->match("f0", "c\r\n\t"), taul::token(lpr0, "c"));

    EXPECT_EQ(ctx->match("f1", "aa"), taul::token(lpr1, "a"));
    EXPECT_EQ(ctx->match("f1", "ba"), taul::token(lpr1, "b"));
    EXPECT_EQ(ctx->match("f1", "ca"), taul::token(lpr1, "c"));
    EXPECT_EQ(ctx->match("f1", "xa"), taul::token(lpr1, "x"));
    EXPECT_EQ(ctx->match("f1", "ya"), taul::token(lpr1, "y"));
    EXPECT_EQ(ctx->match("f1", "za"), taul::token(lpr1, "z"));
    EXPECT_EQ(ctx->match("f1", "1a"), taul::token(lpr1, "1"));
    EXPECT_EQ(ctx->match("f1", "2a"), taul::token(lpr1, "2"));
    EXPECT_EQ(ctx->match("f1", "3a"), taul::token(lpr1, "3"));

    // failure

    EXPECT_EQ(ctx->match("f0", ""), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", "d"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", "da"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", "db"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", "dc"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", "&^!"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", "\r\n\t"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", " a"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", " b"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", " c"), taul::token::failure(""));

    EXPECT_EQ(ctx->match("f1", "da"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "ea"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "fa"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "ua"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "va"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "wa"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "4a"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "5a"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "6a"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "!a"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "@a"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "#a"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "\ra"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "\na"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "\ta"), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(ctx->match("f0", "abca", 3), taul::token(lpr0, "a", 3));
    EXPECT_EQ(ctx->match("f0", "abcb", 3), taul::token(lpr0, "b", 3));
    EXPECT_EQ(ctx->match("f0", "abcc", 3), taul::token(lpr0, "c", 3));
    EXPECT_EQ(ctx->match("f0", "abca123", 3), taul::token(lpr0, "a", 3));
    EXPECT_EQ(ctx->match("f0", "abcb&^!", 3), taul::token(lpr0, "b", 3));
    EXPECT_EQ(ctx->match("f0", "abcc\r\n\t", 3), taul::token(lpr0, "c", 3));

    EXPECT_EQ(ctx->match("f1", "___aa", 3), taul::token(lpr1, "a", 3));
    EXPECT_EQ(ctx->match("f1", "___ba", 3), taul::token(lpr1, "b", 3));
    EXPECT_EQ(ctx->match("f1", "___ca", 3), taul::token(lpr1, "c", 3));
    EXPECT_EQ(ctx->match("f1", "___xa", 3), taul::token(lpr1, "x", 3));
    EXPECT_EQ(ctx->match("f1", "___ya", 3), taul::token(lpr1, "y", 3));
    EXPECT_EQ(ctx->match("f1", "___za", 3), taul::token(lpr1, "z", 3));
    EXPECT_EQ(ctx->match("f1", "___1a", 3), taul::token(lpr1, "1", 3));
    EXPECT_EQ(ctx->match("f1", "___2a", 3), taul::token(lpr1, "2", 3));
    EXPECT_EQ(ctx->match("f1", "___3a", 3), taul::token(lpr1, "3", 3));

    // failure

    EXPECT_EQ(ctx->match("f0", "abc", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abcd", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abcda", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abcdb", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abcdc", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abc&^!", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abc\r\n\t", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abc a", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abc b", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abc c", 3), taul::token::failure("", 3));

    EXPECT_EQ(ctx->match("f1", "___da", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___ea", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___fa", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___ua", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___va", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___wa", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___4a", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___5a", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___6a", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___!a", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___@a", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___#a", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___\ra", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___\na", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___\ta", 3), taul::token::failure("", 3));
}

TEST_F(GrammarUsageTests, ParserExpr_Token) {

    // remember that all parser expr tests should include testing things like
    // that the impl can handle token sequences w/ 'skip tokens' cut out

    // testing w/ regards to failure tokens should also be ensured

    const auto spec =
        taul::spec_writer()
        .lpr_decl("ws")
        .lpr_decl("chr")
        .ppr_decl("f0")
        .lpr("ws", taul::qualifier::skip)
        .string(" ")
        .close()
        .lpr("chr")
        .any()
        .close()
        .ppr("f0")
        .token()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    //const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    // test w/ and w/out offset

    // success

    {
        std::string src = "abc 123 !@#";
        taul::token_seq tkns(src);
        tkns.push(gram.lpr("chr"), 3);
        tkns.skip(1);
        tkns.push(gram.lpr("chr"), 3);
        tkns.skip(1);
        tkns.push(gram.lpr("chr"), 3);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(f0, "abc", 0);
        f0_expect0.attach(ctx->create(chr, "abc", 0));
        
        auto f0_expect1 = ctx->create(f0, "123", 4);
        f0_expect1.attach(ctx->create(chr, "123", 4));
        
        auto f0_expect2 = ctx->create(f0, "!@#", 8);
        f0_expect2.attach(ctx->create(chr, "!@#", 8));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 0)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 1)));
        EXPECT_TRUE(_example(f0_expect2, ctx->parse("f0", tkns, 2)));
    }

    // failure

    {
        std::string src = "!";
        taul::token_seq tkns0(src);
        taul::token_seq tkns1(src);
        tkns1.push_failure(1);
        _log(src, tkns0);
        _log(src, tkns1);

        EXPECT_FALSE((bool)ctx->parse("f0", tkns0));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns1, 0));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns1, 1));
    }
}

TEST_F(GrammarUsageTests, ParserExpr_Failure) {

    // remember that all parser expr tests should include testing things like
    // that the impl can handle token sequences w/ 'skip tokens' cut out

    // testing w/ regards to failure tokens should also be ensured

    const auto spec =
        taul::spec_writer()
        .lpr_decl("ws")
        .lpr_decl("chr")
        .ppr_decl("f0")
        .lpr("ws", taul::qualifier::skip)
        .string(" ")
        .close()
        .lpr("chr")
        .any()
        .close()
        .ppr("f0")
        .failure()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    //const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    // test w/ and w/out offset

    // success

    {
        std::string src = "abc 123 !@#";
        taul::token_seq tkns(src);
        tkns.push_failure(3);
        tkns.skip(1);
        tkns.push_failure(3);
        tkns.skip(1);
        tkns.push_failure(3);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(f0, "abc", 0);
        f0_expect0.attach(ctx->create(taul::token::failure("abc", 0)));
        
        auto f0_expect1 = ctx->create(f0, "123", 4);
        f0_expect1.attach(ctx->create(taul::token::failure("123", 4)));
        
        auto f0_expect2 = ctx->create(f0, "!@#", 8);
        f0_expect2.attach(ctx->create(taul::token::failure("!@#", 8)));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 0)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 1)));
        EXPECT_TRUE(_example(f0_expect2, ctx->parse("f0", tkns, 2)));
    }

    // failure

    {
        std::string src = "!";
        taul::token_seq tkns0(src);
        taul::token_seq tkns1(src);
        tkns1.push(chr, 1);
        _log(src, tkns0);
        _log(src, tkns1);

        EXPECT_FALSE((bool)ctx->parse("f0", tkns0));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns1, 0));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns1, 1));
    }
}

TEST_F(GrammarUsageTests, LexerExpr_Name) {


    const auto spec =
        taul::spec_writer()
        .lpr_decl("a")
        .lpr_decl("b")
        .lpr_decl("c")
        .lpr_decl("f")
        .lpr("f")
        .name("a")
        .name("b")
        .name("c")
        .optional()
        .name("f") // test recursion
        .close()
        .close()
        // wanna be sure that impl can handle def being *after* usage
        .lpr("a")
        .string("a")
        .close()
        .lpr("b")
        .string("b")
        .close()
        .lpr("c")
        .string("c")
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& lpr = gram.lpr("f");
    //const auto lex = gram.lexer("f");


    // test w/out offset

    // success

    EXPECT_EQ(ctx->match("f", "abc"), taul::token(lpr, "abc"));
    EXPECT_EQ(ctx->match("f", "abca"), taul::token(lpr, "abc"));
    EXPECT_EQ(ctx->match("f", "abcab"), taul::token(lpr, "abc"));
    EXPECT_EQ(ctx->match("f", "abc123"), taul::token(lpr, "abc"));
    EXPECT_EQ(ctx->match("f", "abcabc"), taul::token(lpr, "abcabc"));
    EXPECT_EQ(ctx->match("f", "abcabca"), taul::token(lpr, "abcabc"));
    EXPECT_EQ(ctx->match("f", "abcabcab"), taul::token(lpr, "abcabc"));
    EXPECT_EQ(ctx->match("f", "abcabc123"), taul::token(lpr, "abcabc"));
    EXPECT_EQ(ctx->match("f", "abcabcabc"), taul::token(lpr, "abcabcabc"));
    EXPECT_EQ(ctx->match("f", "abcabcabca"), taul::token(lpr, "abcabcabc"));
    EXPECT_EQ(ctx->match("f", "abcabcabcab"), taul::token(lpr, "abcabcabc"));
    EXPECT_EQ(ctx->match("f", "abcabcabc123"), taul::token(lpr, "abcabcabc"));
    EXPECT_EQ(ctx->match("f", "abcabcabcabc"), taul::token(lpr, "abcabcabcabc"));
    EXPECT_EQ(ctx->match("f", "abcabcabcabca"), taul::token(lpr, "abcabcabcabc"));
    EXPECT_EQ(ctx->match("f", "abcabcabcabcab"), taul::token(lpr, "abcabcabcabc"));
    EXPECT_EQ(ctx->match("f", "abcabcabcabc123"), taul::token(lpr, "abcabcabcabc"));

    // failure

    EXPECT_EQ(ctx->match("f", ""), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f", "a"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f", "ab"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f", "def"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f", "ABC"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f", "&*$"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f", "\r\n\t"), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(ctx->match("f", "+-=abc", 3), taul::token(lpr, "abc", 3));
    EXPECT_EQ(ctx->match("f", "+-=abca", 3), taul::token(lpr, "abc", 3));
    EXPECT_EQ(ctx->match("f", "+-=abcab", 3), taul::token(lpr, "abc", 3));
    EXPECT_EQ(ctx->match("f", "+-=abc123", 3), taul::token(lpr, "abc", 3));
    EXPECT_EQ(ctx->match("f", "+-=abcabc", 3), taul::token(lpr, "abcabc", 3));
    EXPECT_EQ(ctx->match("f", "+-=abcabca", 3), taul::token(lpr, "abcabc", 3));
    EXPECT_EQ(ctx->match("f", "+-=abcabcab", 3), taul::token(lpr, "abcabc", 3));
    EXPECT_EQ(ctx->match("f", "+-=abcabc123", 3), taul::token(lpr, "abcabc", 3));
    EXPECT_EQ(ctx->match("f", "+-=abcabcabc", 3), taul::token(lpr, "abcabcabc", 3));
    EXPECT_EQ(ctx->match("f", "+-=abcabcabca", 3), taul::token(lpr, "abcabcabc", 3));
    EXPECT_EQ(ctx->match("f", "+-=abcabcabcab", 3), taul::token(lpr, "abcabcabc", 3));
    EXPECT_EQ(ctx->match("f", "+-=abcabcabc123", 3), taul::token(lpr, "abcabcabc", 3));
    EXPECT_EQ(ctx->match("f", "+-=abcabcabcabc", 3), taul::token(lpr, "abcabcabcabc", 3));
    EXPECT_EQ(ctx->match("f", "+-=abcabcabcabca", 3), taul::token(lpr, "abcabcabcabc", 3));
    EXPECT_EQ(ctx->match("f", "+-=abcabcabcabcab", 3), taul::token(lpr, "abcabcabcabc", 3));
    EXPECT_EQ(ctx->match("f", "+-=abcabcabcabc123", 3), taul::token(lpr, "abcabcabcabc", 3));

    // failure

    EXPECT_EQ(ctx->match("f", "+-=", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f", "+-=a", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f", "+-=ab", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f", "+-=def", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f", "+-=ABC", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f", "+-=123", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f", "+-=&*$", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f", "+-=\r\n\t", 3), taul::token::failure("", 3));
}

TEST_F(GrammarUsageTests, ParserExpr_Name) {

    // remember that all parser expr tests should include testing things like
    // that the impl can handle token sequences w/ 'skip tokens' cut out

    // testing w/ regards to failure tokens should also be ensured

    const auto spec =
        taul::spec_writer()
        .lpr_decl("ws")
        .lpr_decl("a0")
        .lpr_decl("b0")
        .lpr_decl("c0")
        .lpr_decl("misc")
        .ppr_decl("abc0")
        .ppr_decl("f0")
        .lpr("ws", taul::qualifier::skip)
        .string(" ")
        .close()
        .lpr("a0")
        .string("a")
        .close()
        .lpr("b0")
        .string("b")
        .close()
        .lpr("c0")
        .string("c")
        .close()
        .lpr("misc")
        .any()
        .close()
        .ppr("abc0")
        .name("a0")
        .name("b0")
        .name("c0")
        .close()
        .ppr("f0")
        .name("abc0")
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& a0 = gram.lpr("a0");
    const auto& b0 = gram.lpr("b0");
    const auto& c0 = gram.lpr("c0");
    const auto& misc = gram.lpr("misc");
    const auto& abc0 = gram.ppr("abc0");
    const auto& f0 = gram.ppr("f0");
    //const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    // test w/ and w/out offset

    // success

    {
        std::string src = "a b c abc";
        taul::token_seq tkns(src);
        tkns.push(a0, 1);
        tkns.skip(1);
        tkns.push(b0, 1);
        tkns.skip(1);
        tkns.push(c0, 1);
        tkns.skip(1);
        tkns.push(a0, 1);
        tkns.push(b0, 1);
        tkns.push(c0, 1);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(f0, "a b c", 0);
        auto _temp0 = ctx->create(abc0, "a b c", 0);
        _temp0.attach(ctx->create(a0, "a", 0));
        _temp0.attach(ctx->create(b0, "b", 2));
        _temp0.attach(ctx->create(c0, "c", 4));
        f0_expect0.attach(std::move(_temp0));

        auto f0_expect1 = ctx->create(f0, "abc", 6);
        auto _temp1 = ctx->create(abc0, "abc", 6);
        _temp1.attach(ctx->create(a0, "a", 6));
        _temp1.attach(ctx->create(b0, "b", 7));
        _temp1.attach(ctx->create(c0, "c", 8));
        f0_expect1.attach(std::move(_temp1));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 0)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 3)));
    }

    // failure

    {
        std::string src = "abd ab";
        taul::token_seq tkns0(src);
        taul::token_seq tkns1(src);
        tkns1.push(a0, 1);
        tkns1.push(b0, 1);
        tkns1.push(misc, 1);
        tkns1.skip(1);
        tkns1.push(a0, 1);
        tkns1.push(b0, 1);
        _log(src, tkns0);
        _log(src, tkns1);

        EXPECT_FALSE((bool)ctx->parse("f0", tkns0));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns1, 0));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns1, 1));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns1, 2));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns1, 3));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns1, 4));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns1, 5));
    }
}

TEST_F(GrammarUsageTests, LexerExpr_Sequence) {

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

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    //const auto lex0 = gram.lexer("f0");
    //const auto lex1 = gram.lexer("f1");


    // test f0 w/out offset

    // success

    EXPECT_EQ(ctx->match("f0", ""), taul::token(lpr0, ""));
    EXPECT_EQ(ctx->match("f0", "abc"), taul::token(lpr0, ""));
    EXPECT_EQ(ctx->match("f0", "123"), taul::token(lpr0, ""));
    EXPECT_EQ(ctx->match("f0", "&^$"), taul::token(lpr0, ""));
    EXPECT_EQ(ctx->match("f0", " "), taul::token(lpr0, ""));
    EXPECT_EQ(ctx->match("f0", "\r\n"), taul::token(lpr0, ""));

    // failure

    // n/a


    // test f0 w/ offset

    // success

    EXPECT_EQ(ctx->match("f0", "abc", 3), taul::token(lpr0, "", 3));
    EXPECT_EQ(ctx->match("f0", "abcabc", 3), taul::token(lpr0, "", 3));
    EXPECT_EQ(ctx->match("f0", "abc123", 3), taul::token(lpr0, "", 3));
    EXPECT_EQ(ctx->match("f0", "abc&^$", 3), taul::token(lpr0, "", 3));
    EXPECT_EQ(ctx->match("f0", "abc ", 3), taul::token(lpr0, "", 3));
    EXPECT_EQ(ctx->match("f0", "abc\r\n", 3), taul::token(lpr0, "", 3));

    // failure

    // n/a


    // test f1 w/out offset

    // success

    EXPECT_EQ(ctx->match("f1", "abcd"), taul::token(lpr1, "abcd"));
    EXPECT_EQ(ctx->match("f1", "abcd123"), taul::token(lpr1, "abcd"));
    EXPECT_EQ(ctx->match("f1", "a1c2"), taul::token(lpr1, "a1c2"));
    EXPECT_EQ(ctx->match("f1", "a1c2123"), taul::token(lpr1, "a1c2"));
    EXPECT_EQ(ctx->match("f1", "a^c("), taul::token(lpr1, "a^c("));
    EXPECT_EQ(ctx->match("f1", "a^c(123"), taul::token(lpr1, "a^c("));
    EXPECT_EQ(ctx->match("f1", "a c\n"), taul::token(lpr1, "a c\n"));
    EXPECT_EQ(ctx->match("f1", "a c\n123"), taul::token(lpr1, "a c\n"));

    // failure

    EXPECT_EQ(ctx->match("f1", ""), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "a"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "ab"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "Abcd"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "abCd"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "^&^!"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "\n \r\t"), taul::token::failure(""));


    // test f1 w/ offset

    // success

    EXPECT_EQ(ctx->match("f1", "+-=abcd", 3), taul::token(lpr1, "abcd", 3));
    EXPECT_EQ(ctx->match("f1", "+-=abcd123", 3), taul::token(lpr1, "abcd", 3));
    EXPECT_EQ(ctx->match("f1", "+-=a1c2", 3), taul::token(lpr1, "a1c2", 3));
    EXPECT_EQ(ctx->match("f1", "+-=a1c2123", 3), taul::token(lpr1, "a1c2", 3));
    EXPECT_EQ(ctx->match("f1", "+-=a^c(", 3), taul::token(lpr1, "a^c(", 3));
    EXPECT_EQ(ctx->match("f1", "+-=a^c(123", 3), taul::token(lpr1, "a^c(", 3));
    EXPECT_EQ(ctx->match("f1", "+-=a c\n", 3), taul::token(lpr1, "a c\n", 3));
    EXPECT_EQ(ctx->match("f1", "+-=a c\n123", 3), taul::token(lpr1, "a c\n", 3));

    // failure

    EXPECT_EQ(ctx->match("f1", "+-=", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "+-=a", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "+-=ab", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "+-=abc", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "+-=Abcd", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "+-=abCd", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "+-=^&^!", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "+-=\n \r\t", 3), taul::token::failure("", 3));
}

TEST_F(GrammarUsageTests, ParserExpr_Sequence) {

    // remember that all parser expr tests should include testing things like
    // that the impl can handle token sequences w/ 'skip tokens' cut out

    // testing w/ regards to failure tokens should also be ensured

    const auto spec =
        taul::spec_writer()
        .lpr_decl("ws")
        .lpr_decl("chr")
        .ppr_decl("f0")
        .ppr_decl("f1")
        .lpr("ws", taul::qualifier::skip)
        .string(" ")
        .close()
        .lpr("chr")
        .any()
        .close()
        .ppr("f0") // test empty sequence
        .sequence()
        .close()
        .close()
        .ppr("f1") // test non-empty sequence
        // w/out this 'set' expr, pre-impl tests would ALWAYS succeed, so
        // I'm using the set expr to *ensure* that it fails, lol
        // in truth tho, the below is actually a good thing to explicitly test,
        // that being that a sequence in a set is treated as a unit regarding
        // the behaviour of said set
        .set()
        .sequence() // <- dummy
        .string("*")
        .string("*")
        .string("*")
        .string("*")
        .string("*")
        .close() // <- (end) dummy
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

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto& f1 = gram.ppr("f1");
    //const auto psr0 = gram.parser("f0");
    //const auto psr1 = gram.parser("f1");


    // the below assertions expect the embedded exprs to work as expected


    // test w/ and w/out offset

    // success

    {
        std::string src0 = "";
        std::string src1 = "a bc d abc  d";
        taul::token_seq tkns0 = ctx->tokenize(src0);
        taul::token_seq tkns1 = ctx->tokenize(src1);
        _log(src0, tkns0);
        _log(src1, tkns1);

        auto f0_expect0 = ctx->create(f0, "", 0);
        auto f0_expect1 = ctx->create(f0, "", 0);
        auto f0_expect2 = ctx->create(f0, "", 2);
        auto f0_expect3 = ctx->create(f0, "", 3);
        auto f0_expect4 = ctx->create(f0, "", 5);
        auto f0_expect5 = ctx->create(f0, "", 7);
        auto f0_expect6 = ctx->create(f0, "", 8);
        auto f0_expect7 = ctx->create(f0, "", 9);
        auto f0_expect8 = ctx->create(f0, "", 12);
        auto f0_expect9 = ctx->create(f0, "", 13);

        auto f1_expect0 = ctx->create(f1, "a bc d", 0);
        f1_expect0.attach(ctx->create(taul::token(chr, "a", 0)));
        f1_expect0.attach(ctx->create(taul::token(chr, "b", 2)));
        f1_expect0.attach(ctx->create(taul::token(chr, "c", 3)));
        f1_expect0.attach(ctx->create(taul::token(chr, "d", 5)));
        auto f1_expect1 = ctx->create(f1, "abc  d", 7);
        f1_expect1.attach(ctx->create(taul::token(chr, "a", 7)));
        f1_expect1.attach(ctx->create(taul::token(chr, "b", 8)));
        f1_expect1.attach(ctx->create(taul::token(chr, "c", 9)));
        f1_expect1.attach(ctx->create(taul::token(chr, "d", 12)));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns0, 0)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns1, 0)));
        EXPECT_TRUE(_example(f0_expect2, ctx->parse("f0", tkns1, 1)));
        EXPECT_TRUE(_example(f0_expect3, ctx->parse("f0", tkns1, 2)));
        EXPECT_TRUE(_example(f0_expect4, ctx->parse("f0", tkns1, 3)));
        EXPECT_TRUE(_example(f0_expect5, ctx->parse("f0", tkns1, 4)));
        EXPECT_TRUE(_example(f0_expect6, ctx->parse("f0", tkns1, 5)));
        EXPECT_TRUE(_example(f0_expect7, ctx->parse("f0", tkns1, 6)));
        EXPECT_TRUE(_example(f0_expect8, ctx->parse("f0", tkns1, 7)));
        EXPECT_TRUE(_example(f0_expect9, ctx->parse("f0", tkns1, 8)));

        EXPECT_TRUE(_example(f1_expect0, ctx->parse("f1", tkns1, 0)));
        EXPECT_TRUE(_example(f1_expect1, ctx->parse("f1", tkns1, 4)));
    }

    // failure

    {
        std::string src = "abd abCd Abcd";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        EXPECT_FALSE((bool)ctx->parse("f1", tkns));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 0));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 1));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 2));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 3));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 4));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 5));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 6));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 7));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 8));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 9));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 10));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 11));
    }
}

TEST_F(GrammarUsageTests, LexerExpr_Set) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f0")
        .lpr_decl("f1")
        .lpr("f0") // test empty set <- should ALWAYS fail!
        .set()
        .close()
        .close()
        .lpr("f1") // test non-empty set
        .set()
        .string("123")
        .string("a")
        .string("12")
        .string("abc")
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    //const auto lex0 = gram.lexer("f0");
    //const auto lex1 = gram.lexer("f1");


    // test f0 w/out offset

    // success

    // n/a

    // failure

    EXPECT_EQ(ctx->match("f0", ""), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", "abc"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", "123"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", "&^$"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", " "), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f0", "\r\n"), taul::token::failure(""));


    // test f0 w/ offset

    // success

    // n/a

    // failure

    EXPECT_EQ(ctx->match("f0", "abc", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abcabc", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abc123", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abc&^$", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abc ", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f0", "abc\r\n", 3), taul::token::failure("", 3));


    // test f1 w/out offset

    // success

    EXPECT_EQ(ctx->match("f1", "123"), taul::token(lpr1, "123"));
    EXPECT_EQ(ctx->match("f1", "a"), taul::token(lpr1, "a"));
    EXPECT_EQ(ctx->match("f1", "12"), taul::token(lpr1, "12"));
    EXPECT_EQ(ctx->match("f1", "abc"), taul::token(lpr1, "a"));

    // failure

    EXPECT_EQ(ctx->match("f1", ""), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "1"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "&^$"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "\n\r\t"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", " "), taul::token::failure(""));


    // test f1 w/ offset

    // success

    EXPECT_EQ(ctx->match("f1", "+-=123", 3), taul::token(lpr1, "123", 3));
    EXPECT_EQ(ctx->match("f1", "+-=a", 3), taul::token(lpr1, "a", 3));
    EXPECT_EQ(ctx->match("f1", "+-=12", 3), taul::token(lpr1, "12", 3));
    EXPECT_EQ(ctx->match("f1", "+-=abc", 3), taul::token(lpr1, "a", 3));

    // failure

    EXPECT_EQ(ctx->match("f1", "+-=", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "+-=1", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "+-=&^$", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "+-=\n\r\t", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "+-= ", 3), taul::token::failure("", 3));
}

TEST_F(GrammarUsageTests, ParserExpr_Set) {

    // remember that all parser expr tests should include testing things like
    // that the impl can handle token sequences w/ 'skip tokens' cut out

    // testing w/ regards to failure tokens should also be ensured

    const auto spec =
        taul::spec_writer()
        .lpr_decl("ws")
        .lpr_decl("chr")
        .ppr_decl("f0")
        .ppr_decl("f1")
        .lpr("ws", taul::qualifier::skip)
        .string(" ")
        .close()
        .lpr("chr")
        .any()
        .close()
        .ppr("f0") // test empty set <- should ALWAYS fail!
        .set()
        .close()
        .close()
        .ppr("f1") // test non-empty set
        .set()
        .sequence()
        .string("1")
        .string("2")
        .string("3")
        .close()
        .sequence()
        .string("a")
        .close()
        .sequence()
        .string("1")
        .string("2")
        .close()
        .sequence()
        .string("a")
        .string("b")
        .string("c")
        .close()
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto& f1 = gram.ppr("f1");
    //const auto psr0 = gram.parser("f0");
    //const auto psr1 = gram.parser("f1");


    // the below assertions expect the embedded exprs to work as expected


    // test w/ and w/out offset

    // success

    {
        std::string src = "123";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f1_expect0 = ctx->create(f1, "123", 0);
        f1_expect0.attach(ctx->create(taul::token(chr, "1", 0)));
        f1_expect0.attach(ctx->create(taul::token(chr, "2", 1)));
        f1_expect0.attach(ctx->create(taul::token(chr, "3", 2)));

        auto f1_expect1 = ctx->create(f1, "123", 0);
        f1_expect1.attach(ctx->create(taul::token(chr, "1", 0)));
        f1_expect1.attach(ctx->create(taul::token(chr, "2", 1)));
        f1_expect1.attach(ctx->create(taul::token(chr, "3", 2)));

        EXPECT_TRUE(_example(f1_expect0, ctx->parse("f1", tkns)));

        EXPECT_TRUE(_example(f1_expect1, ctx->parse("f1", tkns)));
    }

    {
        std::string src = "a";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f1_expect0 = ctx->create(f1, "a", 0);
        f1_expect0.attach(ctx->create(taul::token(chr, "a", 0)));

        auto f1_expect1 = ctx->create(f1, "a", 0);
        f1_expect1.attach(ctx->create(taul::token(chr, "a", 0)));

        EXPECT_TRUE(_example(f1_expect0, ctx->parse("f1", tkns)));

        EXPECT_TRUE(_example(f1_expect1, ctx->parse("f1", tkns)));
    }

    {
        std::string src = "12";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f1_expect0 = ctx->create(f1, "12", 0);
        f1_expect0.attach(ctx->create(taul::token(chr, "1", 0)));
        f1_expect0.attach(ctx->create(taul::token(chr, "2", 1)));

        auto f1_expect1 = ctx->create(f1, "12", 0);
        f1_expect1.attach(ctx->create(taul::token(chr, "1", 0)));
        f1_expect1.attach(ctx->create(taul::token(chr, "2", 1)));

        EXPECT_TRUE(_example(f1_expect0, ctx->parse("f1", tkns)));

        EXPECT_TRUE(_example(f1_expect1, ctx->parse("f1", tkns)));
    }

    {
        std::string src = "abc";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f1_expect0 = ctx->create(f1, "a", 0);
        f1_expect0.attach(ctx->create(taul::token(chr, "a", 0)));

        auto f1_expect1 = ctx->create(f1, "a", 0);
        f1_expect1.attach(ctx->create(taul::token(chr, "a", 0)));

        EXPECT_TRUE(_example(f1_expect0, ctx->parse("f1", tkns)));

        EXPECT_TRUE(_example(f1_expect1, ctx->parse("f1", tkns)));
    }

    {
        std::string src = "___123";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f1_expect0 = ctx->create(f1, "123", 3);
        f1_expect0.attach(ctx->create(taul::token(chr, "1", 3)));
        f1_expect0.attach(ctx->create(taul::token(chr, "2", 4)));
        f1_expect0.attach(ctx->create(taul::token(chr, "3", 5)));

        EXPECT_TRUE(_example(f1_expect0, ctx->parse("f1", tkns, 3)));
    }

    {
        std::string src = "___a";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f1_expect0 = ctx->create(f1, "a", 3);
        f1_expect0.attach(ctx->create(taul::token(chr, "a", 3)));

        EXPECT_TRUE(_example(f1_expect0, ctx->parse("f1", tkns, 3)));
    }

    {
        std::string src = "___12";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f1_expect0 = ctx->create(f1, "12", 3);
        f1_expect0.attach(ctx->create(taul::token(chr, "1", 3)));
        f1_expect0.attach(ctx->create(taul::token(chr, "2", 4)));

        EXPECT_TRUE(_example(f1_expect0, ctx->parse("f1", tkns, 3)));
    }

    {
        std::string src = "___abc";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f1_expect0 = ctx->create(f1, "a", 3);
        f1_expect0.attach(ctx->create(taul::token(chr, "a", 3)));

        EXPECT_TRUE(_example(f1_expect0, ctx->parse("f1", tkns, 3)));
    }

    // failure

    {
        std::string src = "&*(!@#";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        EXPECT_FALSE((bool)ctx->parse("f0", tkns));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 0));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 0));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 3));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 3));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 6));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 6));
    }
}

TEST_F(GrammarUsageTests, LexerExpr_LookAhead) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f0")
        .lpr_decl("f1")
        .lpr_decl("f2")
        .lpr_decl("f3")
        .lpr_decl("f4")
        .lpr("f0")
        .lookahead()
        .end()
        .close()
        .close()
        .lpr("f1")
        .lookahead()
        .any()
        .close()
        .close()
        .lpr("f2")
        .lookahead()
        .string("a")
        .close()
        .close()
        .lpr("f3")
        .lookahead()
        .charset("abc")
        .close()
        .close()
        .lpr("f4")
        .lookahead()
        .set()
        .string("a")
        .string("b")
        .string("c")
        .close()
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    const auto& lpr2 = gram.lpr("f2");
    const auto& lpr3 = gram.lpr("f3");
    const auto& lpr4 = gram.lpr("f4");
    //const auto lex0 = gram.lexer("f0");
    //const auto lex1 = gram.lexer("f1");
    //const auto lex2 = gram.lexer("f2");
    //const auto lex3 = gram.lexer("f3");
    //const auto lex4 = gram.lexer("f4");


    // test f w/out offset

    // success/failure
    
    EXPECT_EQ(ctx->match("f0", ""), taul::token(lpr0, ""));
    EXPECT_EQ(ctx->match("f1", ""), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f2", ""), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f3", ""), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f4", ""), taul::token::failure(""));
    
    EXPECT_EQ(ctx->match("f0", "aa"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "aa"), taul::token(lpr1, ""));
    EXPECT_EQ(ctx->match("f2", "aa"), taul::token(lpr2, ""));
    EXPECT_EQ(ctx->match("f3", "aa"), taul::token(lpr3, ""));
    EXPECT_EQ(ctx->match("f4", "aa"), taul::token(lpr4, ""));
    
    EXPECT_EQ(ctx->match("f0", "ba"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", "ba"), taul::token(lpr1, ""));
    EXPECT_EQ(ctx->match("f2", "ba"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f3", "ba"), taul::token(lpr3, ""));
    EXPECT_EQ(ctx->match("f4", "ba"), taul::token(lpr4, ""));


    // test f w/ offset

    // success/failure
    
    EXPECT_EQ(ctx->match("f0", "___", 3), taul::token(lpr0, "", 3));
    EXPECT_EQ(ctx->match("f1", "___", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f2", "___", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f3", "___", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f4", "___", 3), taul::token::failure("", 3));

    EXPECT_EQ(ctx->match("f0", "___aa", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___aa", 3), taul::token(lpr1, "", 3));
    EXPECT_EQ(ctx->match("f2", "___aa", 3), taul::token(lpr2, "", 3));
    EXPECT_EQ(ctx->match("f3", "___aa", 3), taul::token(lpr3, "", 3));
    EXPECT_EQ(ctx->match("f4", "___aa", 3), taul::token(lpr4, "", 3));

    EXPECT_EQ(ctx->match("f0", "___ba", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___ba", 3), taul::token(lpr1, "", 3));
    EXPECT_EQ(ctx->match("f2", "___ba", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f3", "___ba", 3), taul::token(lpr3, "", 3));
    EXPECT_EQ(ctx->match("f4", "___ba", 3), taul::token(lpr4, "", 3));
}

TEST_F(GrammarUsageTests, ParserExpr_LookAhead) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("ABC")
        .lpr_decl("chr")
        .ppr_decl("f0")
        .ppr_decl("f1")
        .ppr_decl("f2")
        .ppr_decl("f3")
        .ppr_decl("f4")
        .lpr("ABC")
        .string("ABC")
        .close()
        .lpr("chr")
        .any()
        .close()
        .ppr("f0")
        .lookahead()
        .end()
        .close()
        .close()
        .ppr("f1")
        .lookahead()
        .any()
        .close()
        .close()
        .ppr("f2")
        .lookahead()
        .string("a")
        .close()
        .close()
        .ppr("f3")
        .lookahead()
        .set()
        .string("a")
        .string("b")
        .string("c")
        .close()
        .close()
        .close()
        .ppr("f4")
        .lookahead()
        .string("ABC") // <- parser allows >1 length strings
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& chr = gram.lpr("chr");
    const auto& ppr0 = gram.ppr("f0");
    const auto& ppr1 = gram.ppr("f1");
    const auto& ppr2 = gram.ppr("f2");
    const auto& ppr3 = gram.ppr("f3");
    const auto& ppr4 = gram.ppr("f4");
    //const auto psr0 = gram.parser("f0");
    //const auto psr1 = gram.parser("f1");
    //const auto psr2 = gram.parser("f2");
    //const auto psr3 = gram.parser("f3");
    //const auto psr4 = gram.parser("f4");


    // the below assertions expect the embedded exprs to work as expected


    // test w/ and w/out offset

    // success/failure

    {
        std::string src = "";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "", 0);

        auto f0_expect1 = ctx->create(ppr0, "", 0);

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns)));

        EXPECT_FALSE((bool)ctx->parse("f1", tkns));
        EXPECT_FALSE((bool)ctx->parse("f2", tkns));
        EXPECT_FALSE((bool)ctx->parse("f3", tkns));

        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 0)));

        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 0));
        EXPECT_FALSE((bool)ctx->parse("f2", tkns, 0));
        EXPECT_FALSE((bool)ctx->parse("f3", tkns, 0));
    }

    {
        std::string src = "aa";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f1_expect0 = ctx->create(ppr1, "", 0);
        auto f2_expect0 = ctx->create(ppr2, "", 0);
        auto f3_expect0 = ctx->create(ppr3, "", 0);

        auto f1_expect1 = ctx->create(ppr1, "", 0);
        auto f2_expect1 = ctx->create(ppr2, "", 0);
        auto f3_expect1 = ctx->create(ppr3, "", 0);

        EXPECT_FALSE((bool)ctx->parse("f0", tkns));

        EXPECT_TRUE(_example(f1_expect0, ctx->parse("f1", tkns)));
        EXPECT_TRUE(_example(f2_expect0, ctx->parse("f2", tkns)));
        EXPECT_TRUE(_example(f3_expect0, ctx->parse("f3", tkns)));

        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 0));

        EXPECT_TRUE(_example(f1_expect1, ctx->parse("f1", tkns, 0)));
        EXPECT_TRUE(_example(f2_expect1, ctx->parse("f2", tkns, 0)));
        EXPECT_TRUE(_example(f3_expect1, ctx->parse("f3", tkns, 0)));
    }

    {
        std::string src = "ba";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f1_expect0 = ctx->create(ppr1, "", 0);
        auto f3_expect0 = ctx->create(ppr3, "", 0);

        auto f1_expect1 = ctx->create(ppr1, "", 0);
        auto f3_expect1 = ctx->create(ppr3, "", 0);

        EXPECT_FALSE((bool)ctx->parse("f0", tkns));

        EXPECT_TRUE(_example(f1_expect0, ctx->parse("f1", tkns)));

        EXPECT_FALSE((bool)ctx->parse("f2", tkns));

        EXPECT_TRUE(_example(f3_expect0, ctx->parse("f3", tkns)));

        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 0));

        EXPECT_TRUE(_example(f1_expect1, ctx->parse("f1", tkns, 0)));

        EXPECT_FALSE((bool)ctx->parse("f2", tkns, 0));

        EXPECT_TRUE(_example(f3_expect1, ctx->parse("f3", tkns, 0)));
    }

    {
        std::string src = "___";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "", 3);

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 3)));

        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 3));
        EXPECT_FALSE((bool)ctx->parse("f2", tkns, 3));
        EXPECT_FALSE((bool)ctx->parse("f3", tkns, 3));
    }

    {
        std::string src = "___aa";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f1_expect0 = ctx->create(ppr1, "", 3);
        auto f2_expect0 = ctx->create(ppr2, "", 3);
        auto f3_expect0 = ctx->create(ppr3, "", 3);

        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 3));

        EXPECT_TRUE(_example(f1_expect0, ctx->parse("f1", tkns, 3)));
        EXPECT_TRUE(_example(f2_expect0, ctx->parse("f2", tkns, 3)));
        EXPECT_TRUE(_example(f3_expect0, ctx->parse("f3", tkns, 3)));
    }

    {
        std::string src = "___ba";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f1_expect0 = ctx->create(ppr1, "", 3);
        auto f3_expect0 = ctx->create(ppr3, "", 3);

        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 3));

        EXPECT_TRUE(_example(f1_expect0, ctx->parse("f1", tkns, 3)));

        EXPECT_FALSE((bool)ctx->parse("f2", tkns, 3));

        EXPECT_TRUE(_example(f3_expect0, ctx->parse("f3", tkns, 3)));
    }

    {
        std::string src = "ABC";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f4_expect0 = ctx->create(ppr4, "", 0);

        EXPECT_TRUE(_example(f4_expect0, ctx->parse("f4", tkns, 0)));
    }

    {
        std::string src = "___ABC";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f4_expect0 = ctx->create(ppr4, "", 3);

        EXPECT_TRUE(_example(f4_expect0, ctx->parse("f4", tkns, 3)));
    }
}

TEST_F(GrammarUsageTests, LexerExpr_LookAheadNot) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f0")
        .lpr_decl("f1")
        .lpr_decl("f2")
        .lpr_decl("f3")
        .lpr_decl("f4")
        .lpr("f0")
        .lookahead_not()
        .end()
        .close()
        .close()
        .lpr("f1")
        .lookahead_not()
        .any()
        .close()
        .close()
        .lpr("f2")
        .lookahead_not()
        .string("a")
        .close()
        .close()
        .lpr("f3")
        .lookahead_not()
        .charset("abc")
        .close()
        .close()
        .lpr("f4")
        .lookahead_not()
        .set()
        .string("a")
        .string("b")
        .string("c")
        .close()
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    const auto& lpr2 = gram.lpr("f2");
    const auto& lpr3 = gram.lpr("f3");
    const auto& lpr4 = gram.lpr("f4");
    //const auto lex0 = gram.lexer("f0");
    //const auto lex1 = gram.lexer("f1");
    //const auto lex2 = gram.lexer("f2");
    //const auto lex3 = gram.lexer("f3");
    //const auto lex4 = gram.lexer("f4");


    // test f w/out offset

    // success/failure

    EXPECT_EQ(ctx->match("f0", ""), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", ""), taul::token(lpr1, ""));
    EXPECT_EQ(ctx->match("f2", ""), taul::token(lpr2, ""));
    EXPECT_EQ(ctx->match("f3", ""), taul::token(lpr3, ""));
    EXPECT_EQ(ctx->match("f4", ""), taul::token(lpr4, ""));

    EXPECT_EQ(ctx->match("f0", "aa"), taul::token(lpr0, ""));
    EXPECT_EQ(ctx->match("f1", "aa"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f2", "aa"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f3", "aa"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f4", "aa"), taul::token::failure(""));

    EXPECT_EQ(ctx->match("f0", "ba"), taul::token(lpr0, ""));
    EXPECT_EQ(ctx->match("f1", "ba"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f2", "ba"), taul::token(lpr2, ""));
    EXPECT_EQ(ctx->match("f3", "ba"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f4", "ba"), taul::token::failure(""));


    // test f w/ offset

    // success/failure

    EXPECT_EQ(ctx->match("f0", "___", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___", 3), taul::token(lpr1, "", 3));
    EXPECT_EQ(ctx->match("f2", "___", 3), taul::token(lpr2, "", 3));
    EXPECT_EQ(ctx->match("f3", "___", 3), taul::token(lpr3, "", 3));
    EXPECT_EQ(ctx->match("f4", "___", 3), taul::token(lpr4, "", 3));

    EXPECT_EQ(ctx->match("f0", "___aa", 3), taul::token(lpr0, "", 3));
    EXPECT_EQ(ctx->match("f1", "___aa", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f2", "___aa", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f3", "___aa", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f4", "___aa", 3), taul::token::failure("", 3));

    EXPECT_EQ(ctx->match("f0", "___ba", 3), taul::token(lpr0, "", 3));
    EXPECT_EQ(ctx->match("f1", "___ba", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f2", "___ba", 3), taul::token(lpr2, "", 3));
    EXPECT_EQ(ctx->match("f3", "___ba", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f4", "___ba", 3), taul::token::failure("", 3));
}

TEST_F(GrammarUsageTests, ParserExpr_LookAheadNot) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("ABC")
        .lpr_decl("chr")
        .ppr_decl("f0")
        .ppr_decl("f1")
        .ppr_decl("f2")
        .ppr_decl("f3")
        .ppr_decl("f4")
        .lpr("ABC")
        .string("ABC")
        .close()
        .lpr("chr")
        .any()
        .close()
        .ppr("f0")
        .lookahead_not()
        .end()
        .close()
        .close()
        .ppr("f1")
        .lookahead_not()
        .any()
        .close()
        .close()
        .ppr("f2")
        .lookahead_not()
        .string("a")
        .close()
        .close()
        .ppr("f3")
        .lookahead_not()
        .set()
        .string("a")
        .string("b")
        .string("c")
        .close()
        .close()
        .close()
        .ppr("f4")
        .lookahead_not()
        .string("ABC") // <- parser allows >1 length strings
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& chr = gram.lpr("chr");
    const auto& ppr0 = gram.ppr("f0");
    const auto& ppr1 = gram.ppr("f1");
    const auto& ppr2 = gram.ppr("f2");
    const auto& ppr3 = gram.ppr("f3");
    const auto& ppr4 = gram.ppr("f4");
    //const auto psr0 = gram.parser("f0");
    //const auto psr1 = gram.parser("f1");
    //const auto psr2 = gram.parser("f2");
    //const auto psr3 = gram.parser("f3");
    //const auto psr4 = gram.parser("f4");


    // the below assertions expect the embedded exprs to work as expected


    // test w/ and w/out offset

    // success/failure

    {
        std::string src = "";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f1_expect0 = ctx->create(ppr1, "", 0);
        auto f2_expect0 = ctx->create(ppr2, "", 0);
        auto f3_expect0 = ctx->create(ppr3, "", 0);

        auto f1_expect1 = ctx->create(ppr1, "", 0);
        auto f2_expect1 = ctx->create(ppr2, "", 0);
        auto f3_expect1 = ctx->create(ppr3, "", 0);

        EXPECT_FALSE((bool)ctx->parse("f0", tkns));

        EXPECT_TRUE(_example(f1_expect0, ctx->parse("f1", tkns)));
        EXPECT_TRUE(_example(f2_expect0, ctx->parse("f2", tkns)));
        EXPECT_TRUE(_example(f3_expect0, ctx->parse("f3", tkns)));

        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 0));

        EXPECT_TRUE(_example(f1_expect1, ctx->parse("f1", tkns, 0)));
        EXPECT_TRUE(_example(f2_expect1, ctx->parse("f2", tkns, 0)));
        EXPECT_TRUE(_example(f3_expect1, ctx->parse("f3", tkns, 0)));
    }

    {
        std::string src = "aa";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "", 0);

        auto f0_expect1 = ctx->create(ppr0, "", 0);

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns)));

        EXPECT_FALSE((bool)ctx->parse("f1", tkns));
        EXPECT_FALSE((bool)ctx->parse("f2", tkns));
        EXPECT_FALSE((bool)ctx->parse("f3", tkns));

        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 0)));

        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 0));
        EXPECT_FALSE((bool)ctx->parse("f2", tkns, 0));
        EXPECT_FALSE((bool)ctx->parse("f3", tkns, 0));
    }

    {
        std::string src = "ba";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "", 0);
        auto f2_expect0 = ctx->create(ppr2, "", 0);

        auto f0_expect1 = ctx->create(ppr0, "", 0);
        auto f2_expect1 = ctx->create(ppr2, "", 0);

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns)));

        EXPECT_FALSE((bool)ctx->parse("f1", tkns));

        EXPECT_TRUE(_example(f2_expect0, ctx->parse("f2", tkns)));

        EXPECT_FALSE((bool)ctx->parse("f3", tkns));

        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 0)));

        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 0));

        EXPECT_TRUE(_example(f2_expect1, ctx->parse("f2", tkns, 0)));

        EXPECT_FALSE((bool)ctx->parse("f3", tkns, 0));
    }

    {
        std::string src = "___";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f1_expect0 = ctx->create(ppr1, "", 3);
        auto f2_expect0 = ctx->create(ppr2, "", 3);
        auto f3_expect0 = ctx->create(ppr3, "", 3);

        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 3));

        EXPECT_TRUE(_example(f1_expect0, ctx->parse("f1", tkns, 3)));
        EXPECT_TRUE(_example(f2_expect0, ctx->parse("f2", tkns, 3)));
        EXPECT_TRUE(_example(f3_expect0, ctx->parse("f3", tkns, 3)));
    }

    {
        std::string src = "___aa";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "", 3);

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 3)));

        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 3));
        EXPECT_FALSE((bool)ctx->parse("f2", tkns, 3));
        EXPECT_FALSE((bool)ctx->parse("f3", tkns, 3));
    }

    {
        std::string src = "___ba";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "", 3);
        auto f2_expect0 = ctx->create(ppr2, "", 3);

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 3)));

        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 3));

        EXPECT_TRUE(_example(f2_expect0, ctx->parse("f2", tkns, 3)));

        EXPECT_FALSE((bool)ctx->parse("f3", tkns, 3));
    }

    {
        std::string src = "ABC";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        EXPECT_FALSE((bool)ctx->parse("f4", tkns, 0));
    }

    {
        std::string src = "___ABC";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        EXPECT_FALSE((bool)ctx->parse("f4", tkns, 3));
    }
}

TEST_F(GrammarUsageTests, LexerExpr_Not) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f0")
        .lpr_decl("f1")
        .lpr_decl("f2")
        .lpr_decl("f3")
        .lpr_decl("f4")
        .lpr("f0")
        .not0()
        .end()
        .close()
        .close()
        .lpr("f1")
        .not0()
        .any()
        .close()
        .close()
        .lpr("f2")
        .not0()
        .string("a")
        .close()
        .close()
        .lpr("f3")
        .not0()
        .charset("abc")
        .close()
        .close()
        .lpr("f4")
        .not0()
        .set()
        .string("a")
        .string("c")
        .close()
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    const auto& lpr2 = gram.lpr("f2");
    const auto& lpr3 = gram.lpr("f3");
    const auto& lpr4 = gram.lpr("f4");
    //const auto lex0 = gram.lexer("f0");
    //const auto lex1 = gram.lexer("f1");
    //const auto lex2 = gram.lexer("f2");
    //const auto lex3 = gram.lexer("f3");
    //const auto lex4 = gram.lexer("f4");


    // test f w/out offset

    // success/failure

    EXPECT_EQ(ctx->match("f0", ""), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f1", ""), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f2", ""), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f3", ""), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f4", ""), taul::token::failure(""));

    EXPECT_EQ(ctx->match("f0", "aa"), taul::token(lpr0, "a"));
    EXPECT_EQ(ctx->match("f1", "aa"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f2", "aa"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f3", "aa"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f4", "aa"), taul::token::failure(""));

    EXPECT_EQ(ctx->match("f0", "ba"), taul::token(lpr0, "b"));
    EXPECT_EQ(ctx->match("f1", "ba"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f2", "ba"), taul::token(lpr2, "b"));
    EXPECT_EQ(ctx->match("f3", "ba"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f4", "ba"), taul::token(lpr4, "b"));


    // test f w/ offset

    // success/failure

    EXPECT_EQ(ctx->match("f0", "___", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f1", "___", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f2", "___", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f3", "___", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f4", "___", 3), taul::token::failure("", 3));

    EXPECT_EQ(ctx->match("f0", "___aa", 3), taul::token(lpr0, "a", 3));
    EXPECT_EQ(ctx->match("f1", "___aa", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f2", "___aa", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f3", "___aa", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f4", "___aa", 3), taul::token::failure("", 3));

    EXPECT_EQ(ctx->match("f0", "___ba", 3), taul::token(lpr0, "b", 3));
    EXPECT_EQ(ctx->match("f1", "___ba", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f2", "___ba", 3), taul::token(lpr2, "b", 3));
    EXPECT_EQ(ctx->match("f3", "___ba", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f4", "___ba", 3), taul::token(lpr4, "b", 3));
}

TEST_F(GrammarUsageTests, ParserExpr_Not) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("ABC")
        .lpr_decl("chr")
        .ppr_decl("f0")
        .ppr_decl("f1")
        .ppr_decl("f2")
        .ppr_decl("f3")
        .ppr_decl("f4")
        .lpr("ABC")
        .string("ABC")
        .close()
        .lpr("chr")
        .any()
        .close()
        .ppr("f0")
        .not0()
        .end()
        .close()
        .close()
        .ppr("f1")
        .not0()
        .any()
        .close()
        .close()
        .ppr("f2")
        .not0()
        .string("a")
        .close()
        .close()
        .ppr("f3")
        .not0()
        .set()
        .string("a")
        .string("b")
        .string("c")
        .close()
        .close()
        .close()
        .ppr("f4")
        .not0()
        .string("ABC") // <- parser allows >1 length strings
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& chr = gram.lpr("chr");
    const auto& ppr0 = gram.ppr("f0");
    const auto& ppr1 = gram.ppr("f1");
    const auto& ppr2 = gram.ppr("f2");
    const auto& ppr3 = gram.ppr("f3");
    const auto& ppr4 = gram.ppr("f4");
    //const auto psr0 = gram.parser("f0");
    //const auto psr1 = gram.parser("f1");
    //const auto psr2 = gram.parser("f2");
    //const auto psr3 = gram.parser("f3");
    //const auto psr4 = gram.parser("f4");


    // the below assertions expect the embedded exprs to work as expected


    // test w/ and w/out offset

    // success/failure

    {
        std::string src = "";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        EXPECT_FALSE((bool)ctx->parse("f0", tkns));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns));
        EXPECT_FALSE((bool)ctx->parse("f2", tkns));
        EXPECT_FALSE((bool)ctx->parse("f3", tkns));

        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 0));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 0));
        EXPECT_FALSE((bool)ctx->parse("f2", tkns, 0));
        EXPECT_FALSE((bool)ctx->parse("f3", tkns, 0));
    }

    {
        std::string src = "aa";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "a", 0);
        f0_expect0.attach(ctx->create(chr, "a", 0));

        auto f0_expect1 = ctx->create(ppr0, "a", 0);
        f0_expect1.attach(ctx->create(chr, "a", 0));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns)));

        EXPECT_FALSE((bool)ctx->parse("f1", tkns));
        EXPECT_FALSE((bool)ctx->parse("f2", tkns));
        EXPECT_FALSE((bool)ctx->parse("f3", tkns));

        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 0)));

        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 0));
        EXPECT_FALSE((bool)ctx->parse("f2", tkns, 0));
        EXPECT_FALSE((bool)ctx->parse("f3", tkns, 0));
    }

    {
        std::string src = "ba";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "b", 0);
        f0_expect0.attach(ctx->create(chr, "b", 0));
        auto f2_expect0 = ctx->create(ppr2, "b", 0);
        f2_expect0.attach(ctx->create(chr, "b", 0));

        auto f0_expect1 = ctx->create(ppr0, "b", 0);
        f0_expect1.attach(ctx->create(chr, "b", 0));
        auto f2_expect1 = ctx->create(ppr2, "b", 0);
        f2_expect1.attach(ctx->create(chr, "b", 0));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns)));
        EXPECT_TRUE(_example(f2_expect0, ctx->parse("f2", tkns)));

        EXPECT_FALSE((bool)ctx->parse("f1", tkns));
        EXPECT_FALSE((bool)ctx->parse("f3", tkns));

        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 0)));
        EXPECT_TRUE(_example(f2_expect1, ctx->parse("f2", tkns, 0)));

        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 0));
        EXPECT_FALSE((bool)ctx->parse("f3", tkns, 0));
    }

    {
        std::string src = "___";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 3));
        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 3));
        EXPECT_FALSE((bool)ctx->parse("f2", tkns, 3));
        EXPECT_FALSE((bool)ctx->parse("f3", tkns, 3));
    }

    {
        std::string src = "___aa";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "a", 3);
        f0_expect0.attach(ctx->create(chr, "a", 3));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 3)));

        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 3));
        EXPECT_FALSE((bool)ctx->parse("f2", tkns, 3));
        EXPECT_FALSE((bool)ctx->parse("f3", tkns, 3));
    }

    {
        std::string src = "___ba";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "b", 3);
        f0_expect0.attach(ctx->create(chr, "b", 3));
        auto f2_expect0 = ctx->create(ppr2, "b", 3);
        f2_expect0.attach(ctx->create(chr, "b", 3));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 3)));
        EXPECT_TRUE(_example(f2_expect0, ctx->parse("f2", tkns, 3)));

        EXPECT_FALSE((bool)ctx->parse("f1", tkns, 3));
        EXPECT_FALSE((bool)ctx->parse("f3", tkns, 3));
    }

    {
        std::string src = "ABC";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        EXPECT_FALSE((bool)ctx->parse("f4", tkns, 0));
    }

    {
        std::string src = "___ABC";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        EXPECT_FALSE((bool)ctx->parse("f4", tkns, 3));
    }
}

TEST_F(GrammarUsageTests, LexerExpr_Optional) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .optional()
        .string("ab")
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& lpr = gram.lpr("f");
    //const auto lex = gram.lexer("f");


    // test f w/out offset

    // success/failure

    // TODO: we *could* add more comprehensive tests testing strings other than 'abab...',
    //       but I'm happy w/ the below

    EXPECT_EQ(ctx->match("f", ""), taul::token(lpr, ""));
    EXPECT_EQ(ctx->match("f", "a"), taul::token(lpr, ""));
    EXPECT_EQ(ctx->match("f", "ab"), taul::token(lpr, "ab"));
    EXPECT_EQ(ctx->match("f", "aba"), taul::token(lpr, "ab"));
    EXPECT_EQ(ctx->match("f", "abab"), taul::token(lpr, "ab"));
    EXPECT_EQ(ctx->match("f", "ababa"), taul::token(lpr, "ab"));
    EXPECT_EQ(ctx->match("f", "ababab"), taul::token(lpr, "ab"));
    EXPECT_EQ(ctx->match("f", "abababa"), taul::token(lpr, "ab"));
    EXPECT_EQ(ctx->match("f", "abababab"), taul::token(lpr, "ab"));
    EXPECT_EQ(ctx->match("f", "ababababa"), taul::token(lpr, "ab"));
    EXPECT_EQ(ctx->match("f", "ababababab"), taul::token(lpr, "ab"));


    // test f w/ offset

    // success/failure

    // TODO: we *could* add more comprehensive tests testing strings other than 'abab...',
    //       but I'm happy w/ the below

    EXPECT_EQ(ctx->match("f", "___", 3), taul::token(lpr, "", 3));
    EXPECT_EQ(ctx->match("f", "___a", 3), taul::token(lpr, "", 3));
    EXPECT_EQ(ctx->match("f", "___ab", 3), taul::token(lpr, "ab", 3));
    EXPECT_EQ(ctx->match("f", "___aba", 3), taul::token(lpr, "ab", 3));
    EXPECT_EQ(ctx->match("f", "___abab", 3), taul::token(lpr, "ab", 3));
    EXPECT_EQ(ctx->match("f", "___ababa", 3), taul::token(lpr, "ab", 3));
    EXPECT_EQ(ctx->match("f", "___ababab", 3), taul::token(lpr, "ab", 3));
    EXPECT_EQ(ctx->match("f", "___abababa", 3), taul::token(lpr, "ab", 3));
    EXPECT_EQ(ctx->match("f", "___abababab", 3), taul::token(lpr, "ab", 3));
    EXPECT_EQ(ctx->match("f", "___ababababa", 3), taul::token(lpr, "ab", 3));
    EXPECT_EQ(ctx->match("f", "___ababababab", 3), taul::token(lpr, "ab", 3));
}

TEST_F(GrammarUsageTests, ParserExpr_Optional) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("chr")
        .ppr_decl("f0")
        .lpr("chr")
        .any()
        .close()
        .ppr("f0")
        .optional()
        .name("chr")
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& chr = gram.lpr("chr");
    const auto& ppr0 = gram.ppr("f0");
    //const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    // test w/ and w/out offset

    // success/failure

    {
        std::string src = "";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "", 0);
        auto f0_expect1 = ctx->create(ppr0, "", 0);

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 0)));
    }

    {
        std::string src = "a";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "a", 0);
        f0_expect0.attach(ctx->create(chr, "a", 0));
        auto f0_expect1 = ctx->create(ppr0, "a", 0);
        f0_expect1.attach(ctx->create(chr, "a", 0));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 0)));
    }

    {
        std::string src = "ab";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "a", 0);
        f0_expect0.attach(ctx->create(chr, "a", 0));
        auto f0_expect1 = ctx->create(ppr0, "a", 0);
        f0_expect1.attach(ctx->create(chr, "a", 0));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 0)));
    }

    {
        std::string src = "___";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "", 3);

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 3)));
    }

    {
        std::string src = "___a";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "a", 3);
        f0_expect0.attach(ctx->create(chr, "a", 3));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 3)));
    }

    {
        std::string src = "___ab";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "a", 3);
        f0_expect0.attach(ctx->create(chr, "a", 3));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 3)));
    }
}

TEST_F(GrammarUsageTests, LexerExpr_KleeneStar) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .kleene_star()
        .string("ab")
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& lpr = gram.lpr("f");
    //const auto lex = gram.lexer("f");


    // test f w/out offset

    // success/failure

    // TODO: we *could* add more comprehensive tests testing strings other than 'abab...',
    //       but I'm happy w/ the below

    EXPECT_EQ(ctx->match("f", ""), taul::token(lpr, ""));
    EXPECT_EQ(ctx->match("f", "a"), taul::token(lpr, ""));
    EXPECT_EQ(ctx->match("f", "ab"), taul::token(lpr, "ab"));
    EXPECT_EQ(ctx->match("f", "aba"), taul::token(lpr, "ab"));
    EXPECT_EQ(ctx->match("f", "abab"), taul::token(lpr, "abab"));
    EXPECT_EQ(ctx->match("f", "ababa"), taul::token(lpr, "abab"));
    EXPECT_EQ(ctx->match("f", "ababab"), taul::token(lpr, "ababab"));
    EXPECT_EQ(ctx->match("f", "abababa"), taul::token(lpr, "ababab"));
    EXPECT_EQ(ctx->match("f", "abababab"), taul::token(lpr, "abababab"));
    EXPECT_EQ(ctx->match("f", "ababababa"), taul::token(lpr, "abababab"));
    EXPECT_EQ(ctx->match("f", "ababababab"), taul::token(lpr, "ababababab"));


    // test f w/ offset

    // success/failure

    // TODO: we *could* add more comprehensive tests testing strings other than 'abab...',
    //       but I'm happy w/ the below

    EXPECT_EQ(ctx->match("f", "___", 3), taul::token(lpr, "", 3));
    EXPECT_EQ(ctx->match("f", "___a", 3), taul::token(lpr, "", 3));
    EXPECT_EQ(ctx->match("f", "___ab", 3), taul::token(lpr, "ab", 3));
    EXPECT_EQ(ctx->match("f", "___aba", 3), taul::token(lpr, "ab", 3));
    EXPECT_EQ(ctx->match("f", "___abab", 3), taul::token(lpr, "abab", 3));
    EXPECT_EQ(ctx->match("f", "___ababa", 3), taul::token(lpr, "abab", 3));
    EXPECT_EQ(ctx->match("f", "___ababab", 3), taul::token(lpr, "ababab", 3));
    EXPECT_EQ(ctx->match("f", "___abababa", 3), taul::token(lpr, "ababab", 3));
    EXPECT_EQ(ctx->match("f", "___abababab", 3), taul::token(lpr, "abababab", 3));
    EXPECT_EQ(ctx->match("f", "___ababababa", 3), taul::token(lpr, "abababab", 3));
    EXPECT_EQ(ctx->match("f", "___ababababab", 3), taul::token(lpr, "ababababab", 3));
}

TEST_F(GrammarUsageTests, ParserExpr_KleeneStar) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("chr")
        .ppr_decl("f0")
        .lpr("chr")
        .any()
        .close()
        .ppr("f0")
        .kleene_star()
        .name("chr")
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& chr = gram.lpr("chr");
    const auto& ppr0 = gram.ppr("f0");
    //const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    // test w/ and w/out offset

    // success/failure

    {
        std::string src = "";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "", 0);
        auto f0_expect1 = ctx->create(ppr0, "", 0);

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 0)));
    }

    {
        std::string src = "a";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "a", 0);
        f0_expect0.attach(ctx->create(chr, "a", 0));
        auto f0_expect1 = ctx->create(ppr0, "a", 0);
        f0_expect1.attach(ctx->create(chr, "a", 0));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 0)));
    }

    {
        std::string src = "ab";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "ab", 0);
        f0_expect0.attach(ctx->create(chr, "a", 0));
        f0_expect0.attach(ctx->create(chr, "b", 1));
        auto f0_expect1 = ctx->create(ppr0, "ab", 0);
        f0_expect1.attach(ctx->create(chr, "a", 0));
        f0_expect1.attach(ctx->create(chr, "b", 1));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 0)));
    }

    {
        std::string src = "___";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "", 3);

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 3)));
    }

    {
        std::string src = "___a";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "a", 3);
        f0_expect0.attach(ctx->create(chr, "a", 3));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 3)));
    }

    {
        std::string src = "___ab";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "ab", 3);
        f0_expect0.attach(ctx->create(chr, "a", 3));
        f0_expect0.attach(ctx->create(chr, "b", 4));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 3)));
    }
}

TEST_F(GrammarUsageTests, LexerExpr_KleenePlus) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .kleene_plus()
        .string("ab")
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& lpr = gram.lpr("f");
    //const auto lex = gram.lexer("f");


    // test f w/out offset

    // success/failure

    // TODO: we *could* add more comprehensive tests testing strings other than 'abab...',
    //       but I'm happy w/ the below

    EXPECT_EQ(ctx->match("f", ""), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f", "a"), taul::token::failure(""));
    EXPECT_EQ(ctx->match("f", "ab"), taul::token(lpr, "ab"));
    EXPECT_EQ(ctx->match("f", "aba"), taul::token(lpr, "ab"));
    EXPECT_EQ(ctx->match("f", "abab"), taul::token(lpr, "abab"));
    EXPECT_EQ(ctx->match("f", "ababa"), taul::token(lpr, "abab"));
    EXPECT_EQ(ctx->match("f", "ababab"), taul::token(lpr, "ababab"));
    EXPECT_EQ(ctx->match("f", "abababa"), taul::token(lpr, "ababab"));
    EXPECT_EQ(ctx->match("f", "abababab"), taul::token(lpr, "abababab"));
    EXPECT_EQ(ctx->match("f", "ababababa"), taul::token(lpr, "abababab"));
    EXPECT_EQ(ctx->match("f", "ababababab"), taul::token(lpr, "ababababab"));


    // test f w/ offset

    // success/failure

    // TODO: we *could* add more comprehensive tests testing strings other than 'abab...',
    //       but I'm happy w/ the below

    EXPECT_EQ(ctx->match("f", "___", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f", "___a", 3), taul::token::failure("", 3));
    EXPECT_EQ(ctx->match("f", "___ab", 3), taul::token(lpr, "ab", 3));
    EXPECT_EQ(ctx->match("f", "___aba", 3), taul::token(lpr, "ab", 3));
    EXPECT_EQ(ctx->match("f", "___abab", 3), taul::token(lpr, "abab", 3));
    EXPECT_EQ(ctx->match("f", "___ababa", 3), taul::token(lpr, "abab", 3));
    EXPECT_EQ(ctx->match("f", "___ababab", 3), taul::token(lpr, "ababab", 3));
    EXPECT_EQ(ctx->match("f", "___abababa", 3), taul::token(lpr, "ababab", 3));
    EXPECT_EQ(ctx->match("f", "___abababab", 3), taul::token(lpr, "abababab", 3));
    EXPECT_EQ(ctx->match("f", "___ababababa", 3), taul::token(lpr, "abababab", 3));
    EXPECT_EQ(ctx->match("f", "___ababababab", 3), taul::token(lpr, "ababababab", 3));
}

TEST_F(GrammarUsageTests, ParserExpr_KleenePlus) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("chr")
        .ppr_decl("f0")
        .lpr("chr")
        .any()
        .close()
        .ppr("f0")
        .kleene_plus()
        .name("chr")
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    ctx->bind(gram);

    const auto& chr = gram.lpr("chr");
    const auto& ppr0 = gram.ppr("f0");
    //const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    // test w/ and w/out offset

    // success/failure

    {
        std::string src = "";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        EXPECT_FALSE((bool)ctx->parse("f0", tkns));
        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 0));
    }

    {
        std::string src = "a";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "a", 0);
        f0_expect0.attach(ctx->create(chr, "a", 0));
        auto f0_expect1 = ctx->create(ppr0, "a", 0);
        f0_expect1.attach(ctx->create(chr, "a", 0));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 0)));
    }

    {
        std::string src = "ab";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "ab", 0);
        f0_expect0.attach(ctx->create(chr, "a", 0));
        f0_expect0.attach(ctx->create(chr, "b", 1));
        auto f0_expect1 = ctx->create(ppr0, "ab", 0);
        f0_expect1.attach(ctx->create(chr, "a", 0));
        f0_expect1.attach(ctx->create(chr, "b", 1));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns)));
        EXPECT_TRUE(_example(f0_expect1, ctx->parse("f0", tkns, 0)));
    }

    {
        std::string src = "___";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        EXPECT_FALSE((bool)ctx->parse("f0", tkns, 3));
    }

    {
        std::string src = "___a";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "a", 3);
        f0_expect0.attach(ctx->create(chr, "a", 3));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 3)));
    }

    {
        std::string src = "___ab";
        taul::token_seq tkns = ctx->tokenize(src);
        _log(src, tkns);

        auto f0_expect0 = ctx->create(ppr0, "ab", 3);
        f0_expect0.attach(ctx->create(chr, "a", 3));
        f0_expect0.attach(ctx->create(chr, "b", 4));

        EXPECT_TRUE(_example(f0_expect0, ctx->parse("f0", tkns, 3)));
    }
}

