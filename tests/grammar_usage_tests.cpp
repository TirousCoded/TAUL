

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


    void SetUp() override final {

        lgr = taul::make_stderr_logger();
    }
};


TEST_F(GrammarUsageTests, grammarWideLexer) {

    auto spec_fn =
        [](taul::bias b) -> taul::spec {
        return
            taul::spec_writer()
            .grammar_bias(b)
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
        };


    auto fl_spec = spec_fn(taul::bias::fl);
    auto fs_spec = spec_fn(taul::bias::fs);
    auto ll_spec = spec_fn(taul::bias::ll);
    auto ls_spec = spec_fn(taul::bias::ls);
    auto f_spec = spec_fn(taul::bias::f);
    auto l_spec = spec_fn(taul::bias::l);

    auto fl_loaded = taul::load(fl_spec, lgr);
    auto fs_loaded = taul::load(fs_spec, lgr);
    auto ll_loaded = taul::load(ll_spec, lgr);
    auto ls_loaded = taul::load(ls_spec, lgr);
    auto f_loaded = taul::load(f_spec, lgr);
    auto l_loaded = taul::load(l_spec, lgr);

    ASSERT_TRUE(fl_loaded);
    ASSERT_TRUE(fs_loaded);
    ASSERT_TRUE(ll_loaded);
    ASSERT_TRUE(ls_loaded);
    ASSERT_TRUE(f_loaded);
    ASSERT_TRUE(l_loaded);

    const taul::grammar fl_gram = std::move(*fl_loaded);
    const taul::grammar fs_gram = std::move(*fs_loaded);
    const taul::grammar ll_gram = std::move(*ll_loaded);
    const taul::grammar ls_gram = std::move(*ls_loaded);
    const taul::grammar f_gram = std::move(*f_loaded);
    const taul::grammar l_gram = std::move(*l_loaded);


    auto adv =
        [](taul::source_pos& cntr, taul::source_pos incr) -> taul::source_pos {
        const auto result = cntr;
        cntr += incr;
        return result;
        };


    // test w/out offset w/ skip token cutting

    {
        const auto& gram = fl_gram;
        const auto tkns = taul::tokenize(gram, "abcabc abcabcabc abc  abcabcabc", lgr);
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

    {
        const auto& gram = fs_gram;
        const auto tkns = taul::tokenize(gram, "abcabc abcabcabc abc  abcabcabc", lgr);
        if (tkns.size() == 18) {
            taul::source_pos cntr = 0;
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            adv(cntr, 2);
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    {
        const auto& gram = ll_gram;
        const auto tkns = taul::tokenize(gram, "abcabc abcabcabc abc  abcabcabc", lgr);
        if (tkns.size() == 18) {
            taul::source_pos cntr = 0;
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            adv(cntr, 2);
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    {
        const auto& gram = ls_gram;
        const auto tkns = taul::tokenize(gram, "abcabc abcabcabc abc  abcabcabc", lgr);
        if (tkns.size() == 18) {
            taul::source_pos cntr = 0;
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            adv(cntr, 2);
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    {
        const auto& gram = f_gram;
        const auto tkns = taul::tokenize(gram, "abcabc abcabcabc abc  abcabcabc", lgr);
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

    {
        const auto& gram = l_gram;
        const auto tkns = taul::tokenize(gram, "abcabc abcabcabc abc  abcabcabc", lgr);
        if (tkns.size() == 18) {
            taul::source_pos cntr = 0;
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            adv(cntr, 2);
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    // test w/ offset w/ skip token cutting

    {
        const auto& gram = fl_gram;
        const auto tkns = taul::tokenize(gram, "+-=abcabc abcabcabc abc  abcabcabc", 3, lgr);
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

    {
        const auto& gram = fs_gram;
        const auto tkns = taul::tokenize(gram, "+-=abcabc abcabcabc abc  abcabcabc", 3, lgr);
        if (tkns.size() == 18) {
            taul::source_pos cntr = 0;
            adv(cntr, 3);
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            adv(cntr, 2);
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    {
        const auto& gram = ll_gram;
        const auto tkns = taul::tokenize(gram, "+-=abcabc abcabcabc abc  abcabcabc", 3, lgr);
        if (tkns.size() == 18) {
            taul::source_pos cntr = 0;
            adv(cntr, 3);
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            adv(cntr, 2);
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    {
        const auto& gram = ls_gram;
        const auto tkns = taul::tokenize(gram, "+-=abcabc abcabcabc abc  abcabcabc", 3, lgr);
        if (tkns.size() == 18) {
            taul::source_pos cntr = 0;
            adv(cntr, 3);
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            adv(cntr, 2);
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    {
        const auto& gram = f_gram;
        const auto tkns = taul::tokenize(gram, "+-=abcabc abcabcabc abc  abcabcabc", 3, lgr);
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

    {
        const auto& gram = l_gram;
        const auto tkns = taul::tokenize(gram, "+-=abcabc abcabcabc abc  abcabcabc", 3, lgr);
        if (tkns.size() == 18) {
            taul::source_pos cntr = 0;
            adv(cntr, 3);
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            adv(cntr, 1);
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            adv(cntr, 2);
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    // test w/out offset w/out skip token cutting

    {
        const auto& gram = fl_gram;
        const auto tkns = taul::tokenize(gram.full_lexer(false), "abcabc abcabcabc abc  abcabcabc", lgr);
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

    {
        const auto& gram = fs_gram;
        const auto tkns = taul::tokenize(gram.full_lexer(false), "abcabc abcabcabc abc  abcabcabc", lgr);
        if (tkns.size() == 22) {
            taul::source_pos cntr = 0;
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[18], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[19], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[20], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[21], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    {
        const auto& gram = ll_gram;
        const auto tkns = taul::tokenize(gram.full_lexer(false), "abcabc abcabcabc abc  abcabcabc", lgr);
        if (tkns.size() == 22) {
            taul::source_pos cntr = 0;
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[18], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[19], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[20], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[21], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    {
        const auto& gram = ls_gram;
        const auto tkns = taul::tokenize(gram.full_lexer(false), "abcabc abcabcabc abc  abcabcabc", lgr);
        if (tkns.size() == 22) {
            taul::source_pos cntr = 0;
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[18], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[19], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[20], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[21], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    {
        const auto& gram = f_gram;
        const auto tkns = taul::tokenize(gram.full_lexer(false), "abcabc abcabcabc abc  abcabcabc", lgr);
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

    {
        const auto& gram = l_gram;
        const auto tkns = taul::tokenize(gram.full_lexer(false), "abcabc abcabcabc abc  abcabcabc", lgr);
        if (tkns.size() == 22) {
            taul::source_pos cntr = 0;
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[18], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[19], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[20], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[21], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    // test w/ offset w/out skip token cutting

    {
        const auto& gram = fl_gram;
        const auto tkns = taul::tokenize(gram.full_lexer(false), "+-=abcabc abcabcabc abc  abcabcabc", 3, lgr);
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

    {
        const auto& gram = fs_gram;
        const auto tkns = taul::tokenize(gram.full_lexer(false), "+-=abcabc abcabcabc abc  abcabcabc", 3, lgr);
        if (tkns.size() == 22) {
            taul::source_pos cntr = 0;
            adv(cntr, 3);
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[18], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[19], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[20], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[21], taul::token(gram.lpr("bc1"), "bc", adv(cntr, 2)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    {
        const auto& gram = ll_gram;
        const auto tkns = taul::tokenize(gram.full_lexer(false), "+-=abcabc abcabcabc abc  abcabcabc", 3, lgr);
        if (tkns.size() == 22) {
            taul::source_pos cntr = 0;
            adv(cntr, 3);
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[18], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[19], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
            EXPECT_EQ(tkns[20], taul::token(gram.lpr("ab2"), "ab", adv(cntr, 2)));
            EXPECT_EQ(tkns[21], taul::token(gram.lpr("c"), "c", adv(cntr, 1)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    {
        const auto& gram = ls_gram;
        const auto tkns = taul::tokenize(gram.full_lexer(false), "+-=abcabc abcabcabc abc  abcabcabc", 3, lgr);
        if (tkns.size() == 22) {
            taul::source_pos cntr = 0;
            adv(cntr, 3);
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[18], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[19], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[20], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[21], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }

    {
        const auto& gram = f_gram;
        const auto tkns = taul::tokenize(gram.full_lexer(false), "+-=abcabc abcabcabc abc  abcabcabc", 3, lgr);
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

    {
        const auto& gram = l_gram;
        const auto tkns = taul::tokenize(gram.full_lexer(false), "+-=abcabc abcabcabc abc  abcabcabc", 3, lgr);
        if (tkns.size() == 22) {
            taul::source_pos cntr = 0;
            adv(cntr, 3);
            EXPECT_EQ(tkns[0], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[1], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[2], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[3], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[4], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[5], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[6], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[7], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[8], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[9], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[10], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[11], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[12], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[13], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[14], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[15], taul::token(gram.lpr("ws"), " ", adv(cntr, 1)));
            EXPECT_EQ(tkns[16], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[17], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[18], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[19], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
            EXPECT_EQ(tkns[20], taul::token(gram.lpr("a"), "a", adv(cntr, 1)));
            EXPECT_EQ(tkns[21], taul::token(gram.lpr("bc2"), "bc", adv(cntr, 2)));
        }
        else ADD_FAILURE() << std::format("tkns.size()=={}", tkns.size());
    }
}


TEST_F(GrammarUsageTests, lexerExpr_toplevel) {

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

TEST_F(GrammarUsageTests, parserExpr_toplevel) {

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
    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto& f1 = gram.ppr("f1");
    const auto psr0 = gram.parser("f0");
    const auto psr1 = gram.parser("f1");


    // the below assertions expect the embedded exprs to work as expected


    auto _log =
        [&](const std::string& src, const taul::token_seq& tkns) {
        TAUL_LOG(lgr, "src==\"{}\"", src);
        TAUL_LOG(lgr, "tkns ({})", tkns.size());
        for (const auto& I : tkns) {
            TAUL_LOG(lgr, "    {}", I);
        }
        };

    auto nc = std::make_shared<taul::node_ctx>();


    // test w/ and w/out offset

    // success

    {
        // test w/ no tokens

        std::string src = "   ";
        auto tkns = taul::tokenize(gram, src, lgr);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "", 0);

        auto f0_result0 = psr0(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();
    }

    {
        // test w/ skip tokens cut after non-cut token(s)

        std::string src = "a   ";
        auto tkns = taul::tokenize(gram, src, lgr);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "", 0);

        auto f0_expect1 = nc->create(f0, "", 1);

        auto f0_result0 = psr0(*nc, tkns, 0, lgr);
        auto f0_result1 = psr0(*nc, tkns, 1, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();
    }

    {
        std::string src = "abc  123 45  6";
        auto tkns = taul::tokenize(gram, src, lgr);
        _log(src, tkns);

        std::string_view view = src;

        auto f0_expect0 = nc->create(f0, "", 0);

        auto f0_expect1 = nc->create(f0, "", 2);
        
        auto f0_expect2 = nc->create(f0, "", 9);
        
        auto f0_expect3 = nc->create(f0, "", 14);

        auto f1_expect0 = nc->create(f1, view.substr(0, 3), 0);
        f1_expect0.attach(nc->create(chr, "a", 0));
        f1_expect0.attach(nc->create(chr, "b", 1));
        f1_expect0.attach(nc->create(chr, "c", 2));

        auto f1_expect1 = nc->create(f1, view.substr(2, 5), 2);
        f1_expect1.attach(nc->create(chr, "c", 2));
        f1_expect1.attach(nc->create(chr, "1", 5));
        f1_expect1.attach(nc->create(chr, "2", 6));

        auto f1_expect2 = nc->create(f1, view.substr(9, 5), 9);
        f1_expect2.attach(nc->create(chr, "4", 9));
        f1_expect2.attach(nc->create(chr, "5", 10));
        f1_expect2.attach(nc->create(chr, "6", 13));

        auto f0_result0 = psr0(*nc, tkns, 0, lgr);
        auto f0_result1 = psr0(*nc, tkns, 2, lgr);
        auto f0_result2 = psr0(*nc, tkns, 6, lgr);
        auto f0_result3 = psr0(*nc, tkns, 9, lgr);

        auto f1_result0 = psr1(*nc, tkns, 0, lgr);
        auto f1_result1 = psr1(*nc, tkns, 2, lgr);
        auto f1_result2 = psr1(*nc, tkns, 6, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect2.fmt_tree());
        if ((bool)f0_result2) {
            TAUL_LOG(lgr, "result:\n{}", f0_result2->fmt_tree());
            EXPECT_TRUE(f0_expect2.equiv(*f0_result2));
            EXPECT_EQ(f0_expect2.str(), f0_result2->str());
        }
        else ADD_FAILURE();
        
        TAUL_LOG(lgr, "expected:\n{}", f0_expect3.fmt_tree());
        if ((bool)f0_result3) {
            TAUL_LOG(lgr, "result:\n{}", f0_result3->fmt_tree());
            EXPECT_TRUE(f0_expect3.equiv(*f0_result3));
            EXPECT_EQ(f0_expect3.str(), f0_result3->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect2.fmt_tree());
        if ((bool)f1_result2) {
            TAUL_LOG(lgr, "result:\n{}", f1_result2->fmt_tree());
            EXPECT_TRUE(f1_expect2.equiv(*f1_result2));
            EXPECT_EQ(f1_expect2.str(), f1_result2->str());
        }
        else ADD_FAILURE();
    }

    // failure

    {
        std::string src = "abc  123 45  6";
        auto tkns = taul::tokenize(gram, src, lgr);
        _log(src, tkns);

        auto tkns0 = tkns;
        auto tkns1 = tkns;
        auto tkns2 = tkns;
        tkns0.pop(tkns0.size() - 0);
        tkns1.pop(tkns1.size() - 1);
        tkns2.pop(tkns2.size() - 2);

        EXPECT_FALSE((bool)psr1(*nc, tkns0, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns1, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns2, lgr));

        EXPECT_FALSE((bool)psr1(*nc, tkns, 7, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 8, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 9, lgr));
    }
}

TEST_F(GrammarUsageTests, lexerExpr_begin) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f")
        .lpr("f")
        .begin()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
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

TEST_F(GrammarUsageTests, parserExpr_begin) {

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
        .begin()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    auto _log =
        [&](const std::string& src, const taul::token_seq& tkns) {
        TAUL_LOG(lgr, "src==\"{}\"", src);
        TAUL_LOG(lgr, "tkns ({})", tkns.size());
        for (const auto& I : tkns) {
            TAUL_LOG(lgr, "    {}", I);
        }
        };

    auto nc = std::make_shared<taul::node_ctx>();


    // test w/ and w/out offset

    // success

    {
        std::string src = "abc  123 45  6";
        auto tkns = taul::tokenize(gram, src, lgr);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "", 0);

        auto f0_result0 = psr0(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();
    }

    // failure

    {
        std::string src = "abc  123 45  6";
        auto tkns = taul::tokenize(gram, src, lgr);
        _log(src, tkns);

        EXPECT_FALSE((bool)psr0(*nc, tkns, 1, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 6, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 9, lgr));
    }
}

TEST_F(GrammarUsageTests, lexerExpr_end) {

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

TEST_F(GrammarUsageTests, parserExpr_end) {

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
    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    auto _log =
        [&](const std::string& src, const taul::token_seq& tkns) {
        TAUL_LOG(lgr, "src==\"{}\"", src);
        TAUL_LOG(lgr, "tkns ({})", tkns.size());
        for (const auto& I : tkns) {
            TAUL_LOG(lgr, "    {}", I);
        }
        };

    auto nc = std::make_shared<taul::node_ctx>();


    // test w/ and w/out offset

    // success

    {
        std::string src = "abc  123 45  6";
        auto tkns = taul::tokenize(gram, src, lgr);
        _log(src, tkns);

        taul::token_seq tkns0("");

        auto f0_expect0 = nc->create(f0, "");
        auto f0_expect1 = nc->create(f0, "", 14);

        auto f0_result0 = psr0(*nc, tkns0, lgr);
        auto f0_result1 = psr0(*nc, tkns, 9, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();
    }

    // failure

    {
        std::string src = "abc  123 45  6";
        auto tkns = taul::tokenize(gram, src, lgr);
        _log(src, tkns);

        EXPECT_FALSE((bool)psr0(*nc, tkns, 0, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 6, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 8, lgr));
    }
}

TEST_F(GrammarUsageTests, lexerExpr_any) {

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

TEST_F(GrammarUsageTests, parserExpr_any) {

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
    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    auto _log =
        [&](const std::string& src, const taul::token_seq& tkns) {
        TAUL_LOG(lgr, "src==\"{}\"", src);
        TAUL_LOG(lgr, "tkns ({})", tkns.size());
        for (const auto& I : tkns) {
            TAUL_LOG(lgr, "    {}", I);
        }
        };

    auto nc = std::make_shared<taul::node_ctx>();


    // test w/ and w/out offset

    // success

    {
        // just gonna *hack together* a token sequence w/ a
        // failure token for us to test w/

        std::string src = "!!!";
        taul::token_seq tkns(src);
        tkns.push_failure(3);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, src);
        f0_expect0.attach(nc->create(taul::token::failure(src)));

        auto f0_result0 = psr0(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();
    }

    {
        std::string src = "abc  123 45  6";
        auto tkns = taul::tokenize(gram, src, lgr);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "a", 0);
        f0_expect0.attach(nc->create(chr, "a", 0));
        auto f0_expect1 = nc->create(f0, "b", 1);
        f0_expect1.attach(nc->create(chr, "b", 1));
        auto f0_expect2 = nc->create(f0, "c", 2);
        f0_expect2.attach(nc->create(chr, "c", 2));
        auto f0_expect3 = nc->create(f0, "1", 5);
        f0_expect3.attach(nc->create(chr, "1", 5));
        auto f0_expect4 = nc->create(f0, "2", 6);
        f0_expect4.attach(nc->create(chr, "2", 6));
        auto f0_expect5 = nc->create(f0, "3", 7);
        f0_expect5.attach(nc->create(chr, "3", 7));
        auto f0_expect6 = nc->create(f0, "4", 9);
        f0_expect6.attach(nc->create(chr, "4", 9));
        auto f0_expect7 = nc->create(f0, "5", 10);
        f0_expect7.attach(nc->create(chr, "5", 10));
        auto f0_expect8 = nc->create(f0, "6", 13);
        f0_expect8.attach(nc->create(chr, "6", 13));

        auto f0_result0 = psr0(*nc, tkns, 0, lgr);
        auto f0_result1 = psr0(*nc, tkns, 1, lgr);
        auto f0_result2 = psr0(*nc, tkns, 2, lgr);
        auto f0_result3 = psr0(*nc, tkns, 3, lgr);
        auto f0_result4 = psr0(*nc, tkns, 4, lgr);
        auto f0_result5 = psr0(*nc, tkns, 5, lgr);
        auto f0_result6 = psr0(*nc, tkns, 6, lgr);
        auto f0_result7 = psr0(*nc, tkns, 7, lgr);
        auto f0_result8 = psr0(*nc, tkns, 8, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect2.fmt_tree());
        if ((bool)f0_result2) {
            TAUL_LOG(lgr, "result:\n{}", f0_result2->fmt_tree());
            EXPECT_TRUE(f0_expect2.equiv(*f0_result2));
            EXPECT_EQ(f0_expect2.str(), f0_result2->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect3.fmt_tree());
        if ((bool)f0_result3) {
            TAUL_LOG(lgr, "result:\n{}", f0_result3->fmt_tree());
            EXPECT_TRUE(f0_expect3.equiv(*f0_result3));
            EXPECT_EQ(f0_expect3.str(), f0_result3->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect4.fmt_tree());
        if ((bool)f0_result4) {
            TAUL_LOG(lgr, "result:\n{}", f0_result4->fmt_tree());
            EXPECT_TRUE(f0_expect4.equiv(*f0_result4));
            EXPECT_EQ(f0_expect4.str(), f0_result4->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect5.fmt_tree());
        if ((bool)f0_result5) {
            TAUL_LOG(lgr, "result:\n{}", f0_result5->fmt_tree());
            EXPECT_TRUE(f0_expect5.equiv(*f0_result5));
            EXPECT_EQ(f0_expect5.str(), f0_result5->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect6.fmt_tree());
        if ((bool)f0_result6) {
            TAUL_LOG(lgr, "result:\n{}", f0_result6->fmt_tree());
            EXPECT_TRUE(f0_expect6.equiv(*f0_result6));
            EXPECT_EQ(f0_expect6.str(), f0_result6->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect7.fmt_tree());
        if ((bool)f0_result7) {
            TAUL_LOG(lgr, "result:\n{}", f0_result7->fmt_tree());
            EXPECT_TRUE(f0_expect7.equiv(*f0_result7));
            EXPECT_EQ(f0_expect7.str(), f0_result7->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect8.fmt_tree());
        if ((bool)f0_result8) {
            TAUL_LOG(lgr, "result:\n{}", f0_result8->fmt_tree());
            EXPECT_TRUE(f0_expect8.equiv(*f0_result8));
            EXPECT_EQ(f0_expect8.str(), f0_result8->str());
        }
        else ADD_FAILURE();
    }

    // failure

    {
        std::string src = "abc  123 45  6";
        auto tkns = taul::tokenize(gram, src, lgr);
        _log(src, tkns);

        taul::token_seq tkns0("");

        EXPECT_FALSE((bool)psr0(*nc, tkns0, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 9, lgr));
    }
}

// the below test presumes usage of string_and_charset.h/cpp code, w/
// regards to escape sequences, and will thus test for this behaviour
// in summary

TEST_F(GrammarUsageTests, lexerExpr_string) {

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
    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    const auto lex0 = gram.lexer("f0");
    const auto lex1 = gram.lexer("f1");


    // test w/out offset

    // success

    EXPECT_EQ(lex0("abc", lgr), taul::token(lpr0, "abc"));
    EXPECT_EQ(lex0("abc123", lgr), taul::token(lpr0, "abc"));
    EXPECT_EQ(lex0("abc&^!", lgr), taul::token(lpr0, "abc"));
    EXPECT_EQ(lex0("abc\r\n\t", lgr), taul::token(lpr0, "abc"));

    EXPECT_EQ(lex1("A\r\nB", lgr), taul::token(lpr1, "A\r\nB"));
    EXPECT_EQ(lex1("A\r\nBabc", lgr), taul::token(lpr1, "A\r\nB"));

    // failure

    EXPECT_EQ(lex0("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("ab", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("def", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("&^!", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("\r\n\t", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0(" abc", lgr), taul::token::failure(""));

    EXPECT_EQ(lex1("A\r\nC", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("A\r\n", lgr), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(lex0("abcabc", 3, lgr), taul::token(lpr0, "abc", 3));
    EXPECT_EQ(lex0("abcabc123", 3, lgr), taul::token(lpr0, "abc", 3));
    EXPECT_EQ(lex0("abcabc&^!", 3, lgr), taul::token(lpr0, "abc", 3));
    EXPECT_EQ(lex0("abcabc\r\n\t", 3, lgr), taul::token(lpr0, "abc", 3));

    EXPECT_EQ(lex1("___A\r\nB", 3, lgr), taul::token(lpr1, "A\r\nB", 3));
    EXPECT_EQ(lex1("___A\r\nBabc", 3, lgr), taul::token(lpr1, "A\r\nB", 3));

    // failure

    EXPECT_EQ(lex0("abc", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abca", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abcab", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abcdef", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc&^!", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc\r\n\t", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc abc", 3, lgr), taul::token::failure("", 3));

    EXPECT_EQ(lex1("___A\r\nC", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("___A\r\n", 3, lgr), taul::token::failure("", 3));
}

// the below test presumes usage of string_and_charset.h/cpp code, w/
// regards to escape sequences, and will thus test for this behaviour
// in summary

TEST_F(GrammarUsageTests, parserExpr_string) {

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
    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    auto _log =
        [&](const std::string& src, const taul::token_seq& tkns) {
        TAUL_LOG(lgr, "src==\"{}\"", src);
        TAUL_LOG(lgr, "tkns ({})", tkns.size());
        for (const auto& I : tkns) {
            TAUL_LOG(lgr, "    {}", I);
        }
        };

    auto nc = std::make_shared<taul::node_ctx>();


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

        EXPECT_FALSE(psr0(*nc, tkns, lgr));
    }

    {
        std::string src = "ab c a 1c";
        auto tkns = taul::tokenize(gram, src, lgr);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab c", 0);
        f0_expect0.attach(nc->create(chr, "a", 0));
        f0_expect0.attach(nc->create(chr, "b", 1));
        f0_expect0.attach(nc->create(chr, "c", 3));

        auto f0_expect1 = nc->create(f0, "a 1c", 5);
        f0_expect1.attach(nc->create(chr, "a", 5));
        f0_expect1.attach(nc->create(chr, "1", 7));
        f0_expect1.attach(nc->create(chr, "c", 8));

        auto f0_result0 = psr0(*nc, tkns, 0, lgr);
        auto f0_result1 = psr0(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();
    }

    // failure

    {
        std::string src = "bbc abb a c";
        auto tkns = taul::tokenize(gram, src, lgr);
        _log(src, tkns);

        taul::token_seq tkns0("");

        EXPECT_FALSE((bool)psr0(*nc, tkns0, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 0, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 6, lgr));
    }
}

// the below test presumes usage of string_and_charset.h/cpp code, w/
// regards to charset strings, and will thus test for this behaviour
// in summary

TEST_F(GrammarUsageTests, lexerExpr_charset) {

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
    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    const auto lex0 = gram.lexer("f0");
    const auto lex1 = gram.lexer("f1");


    // test w/out offset

    // success

    EXPECT_EQ(lex0("a", lgr), taul::token(lpr0, "a"));
    EXPECT_EQ(lex0("b", lgr), taul::token(lpr0, "b"));
    EXPECT_EQ(lex0("c", lgr), taul::token(lpr0, "c"));
    EXPECT_EQ(lex0("a123", lgr), taul::token(lpr0, "a"));
    EXPECT_EQ(lex0("b&^!", lgr), taul::token(lpr0, "b"));
    EXPECT_EQ(lex0("c\r\n\t", lgr), taul::token(lpr0, "c"));

    EXPECT_EQ(lex1("aa", lgr), taul::token(lpr1, "a"));
    EXPECT_EQ(lex1("ba", lgr), taul::token(lpr1, "b"));
    EXPECT_EQ(lex1("ca", lgr), taul::token(lpr1, "c"));
    EXPECT_EQ(lex1("xa", lgr), taul::token(lpr1, "x"));
    EXPECT_EQ(lex1("ya", lgr), taul::token(lpr1, "y"));
    EXPECT_EQ(lex1("za", lgr), taul::token(lpr1, "z"));
    EXPECT_EQ(lex1("1a", lgr), taul::token(lpr1, "1"));
    EXPECT_EQ(lex1("2a", lgr), taul::token(lpr1, "2"));
    EXPECT_EQ(lex1("3a", lgr), taul::token(lpr1, "3"));

    // failure

    EXPECT_EQ(lex0("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("d", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("da", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("db", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("dc", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("&^!", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("\r\n\t", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0(" a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0(" b", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0(" c", lgr), taul::token::failure(""));

    EXPECT_EQ(lex1("da", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("ea", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("fa", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("ua", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("va", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("wa", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("4a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("5a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("6a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("!a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("@a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("#a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("\ra", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("\na", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("\ta", lgr), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(lex0("abca", 3, lgr), taul::token(lpr0, "a", 3));
    EXPECT_EQ(lex0("abcb", 3, lgr), taul::token(lpr0, "b", 3));
    EXPECT_EQ(lex0("abcc", 3, lgr), taul::token(lpr0, "c", 3));
    EXPECT_EQ(lex0("abca123", 3, lgr), taul::token(lpr0, "a", 3));
    EXPECT_EQ(lex0("abcb&^!", 3, lgr), taul::token(lpr0, "b", 3));
    EXPECT_EQ(lex0("abcc\r\n\t", 3, lgr), taul::token(lpr0, "c", 3));

    EXPECT_EQ(lex1("___aa", 3, lgr), taul::token(lpr1, "a", 3));
    EXPECT_EQ(lex1("___ba", 3, lgr), taul::token(lpr1, "b", 3));
    EXPECT_EQ(lex1("___ca", 3, lgr), taul::token(lpr1, "c", 3));
    EXPECT_EQ(lex1("___xa", 3, lgr), taul::token(lpr1, "x", 3));
    EXPECT_EQ(lex1("___ya", 3, lgr), taul::token(lpr1, "y", 3));
    EXPECT_EQ(lex1("___za", 3, lgr), taul::token(lpr1, "z", 3));
    EXPECT_EQ(lex1("___1a", 3, lgr), taul::token(lpr1, "1", 3));
    EXPECT_EQ(lex1("___2a", 3, lgr), taul::token(lpr1, "2", 3));
    EXPECT_EQ(lex1("___3a", 3, lgr), taul::token(lpr1, "3", 3));

    // failure

    EXPECT_EQ(lex0("abc", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abcd", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abcda", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abcdb", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abcdc", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc&^!", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc\r\n\t", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc b", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc c", 3, lgr), taul::token::failure("", 3));

    EXPECT_EQ(lex1("___da", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("___ea", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("___fa", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("___ua", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("___va", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("___wa", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("___4a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("___5a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("___6a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("___!a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("___@a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("___#a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("___\ra", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("___\na", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("___\ta", 3, lgr), taul::token::failure("", 3));
}

// TODO: if we ever re-use the code below, we'll need to revise it to
//       account for things like charset string char ranges

/*TEST_F(GrammarUsageTests, parserExpr_charset) {

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
        .charset("abc")
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    auto _log =
        [&](const std::string& src, const taul::token_seq& tkns) {
        TAUL_LOG(lgr, "src==\"{}\"", src);
        TAUL_LOG(lgr, "tkns ({})", tkns.size());
        for (const auto& I : tkns) {
            TAUL_LOG(lgr, "    {}", I);
        }
        };

    auto nc = std::make_shared<taul::node_ctx>();


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

        EXPECT_FALSE(psr0(*nc, tkns, lgr));
    }

    {
        std::string src = "a b c";
        taul::token_seq tkns(src);
        tkns.push(gram.lpr("chr"), 1);
        tkns.skip(1);
        tkns.push(gram.lpr("chr"), 1);
        tkns.skip(1);
        tkns.push(gram.lpr("chr"), 1);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "a", 0);
        f0_expect0.attach(nc->create(chr, "a", 0));
        
        auto f0_expect1 = nc->create(f0, "b", 2);
        f0_expect1.attach(nc->create(chr, "b", 2));
        
        auto f0_expect2 = nc->create(f0, "c", 4);
        f0_expect2.attach(nc->create(chr, "c", 4));

        auto f0_result0 = psr0(*nc, tkns, 0, lgr);
        auto f0_result1 = psr0(*nc, tkns, 1, lgr);
        auto f0_result2 = psr0(*nc, tkns, 2, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect2.fmt_tree());
        if ((bool)f0_result2) {
            TAUL_LOG(lgr, "result:\n{}", f0_result2->fmt_tree());
            EXPECT_TRUE(f0_expect2.equiv(*f0_result2));
            EXPECT_EQ(f0_expect2.str(), f0_result2->str());
        }
        else ADD_FAILURE();
    }

    // failure

    {
        std::string src = "d 1 3 A _ &";
        auto tkns = taul::tokenize(gram, src, lgr);
        _log(src, tkns);

        taul::token_seq tkns0("");

        EXPECT_FALSE((bool)psr0(*nc, tkns0, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 0, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 1, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 2, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 4, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 5, lgr));
    }
}*/

TEST_F(GrammarUsageTests, lexerExpr_range) {

    const auto spec =
        taul::spec_writer()
        .lpr_decl("f0")
        .lpr_decl("f1")
        .lpr("f0")
        .range('3', '6')
        .close()
        .lpr("f1")
        // test that semantic about order independence works
        .range('f', 'c')
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    const auto& lpr0 = gram.lpr("f0");
    const auto& lpr1 = gram.lpr("f1");
    const auto lex0 = gram.lexer("f0");
    const auto lex1 = gram.lexer("f1");


    // test w/out offset

    // success

    EXPECT_EQ(lex0("33", lgr), taul::token(lpr0, "3"));
    EXPECT_EQ(lex0("44", lgr), taul::token(lpr0, "4"));
    EXPECT_EQ(lex0("55", lgr), taul::token(lpr0, "5"));
    EXPECT_EQ(lex0("66", lgr), taul::token(lpr0, "6"));

    EXPECT_EQ(lex1("cc", lgr), taul::token(lpr1, "c"));
    EXPECT_EQ(lex1("dd", lgr), taul::token(lpr1, "d"));
    EXPECT_EQ(lex1("ee", lgr), taul::token(lpr1, "e"));
    EXPECT_EQ(lex1("ff", lgr), taul::token(lpr1, "f"));

    // failure

    EXPECT_EQ(lex0("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("&^!", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("\r\n\t", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0(" ", lgr), taul::token::failure(""));

    EXPECT_EQ(lex0("1", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("2", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("7", lgr), taul::token::failure(""));
    EXPECT_EQ(lex0("8", lgr), taul::token::failure(""));

    EXPECT_EQ(lex1("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("&^!", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("\r\n\t", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1(" ", lgr), taul::token::failure(""));
    
    EXPECT_EQ(lex1("a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("b", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("g", lgr), taul::token::failure(""));
    EXPECT_EQ(lex1("h", lgr), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(lex0("abc33", 3, lgr), taul::token(lpr0, "3", 3));
    EXPECT_EQ(lex0("abc44", 3, lgr), taul::token(lpr0, "4", 3));
    EXPECT_EQ(lex0("abc55", 3, lgr), taul::token(lpr0, "5", 3));
    EXPECT_EQ(lex0("abc66", 3, lgr), taul::token(lpr0, "6", 3));

    EXPECT_EQ(lex1("abccc", 3, lgr), taul::token(lpr1, "c", 3));
    EXPECT_EQ(lex1("abcdd", 3, lgr), taul::token(lpr1, "d", 3));
    EXPECT_EQ(lex1("abcee", 3, lgr), taul::token(lpr1, "e", 3));
    EXPECT_EQ(lex1("abcff", 3, lgr), taul::token(lpr1, "f", 3));

    // failure

    EXPECT_EQ(lex0("abc", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc&^!", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc\r\n\t", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc ", 3, lgr), taul::token::failure("", 3));

    EXPECT_EQ(lex0("abc1", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc2", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc7", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex0("abc8", 3, lgr), taul::token::failure("", 3));

    EXPECT_EQ(lex1("abc", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("abc&^!", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("abc\r\n\t", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("abc ", 3, lgr), taul::token::failure("", 3));

    EXPECT_EQ(lex1("abca", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("abcb", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("abcg", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex1("abch", 3, lgr), taul::token::failure("", 3));
}

TEST_F(GrammarUsageTests, parserExpr_token) {

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
    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    auto _log =
        [&](const std::string& src, const taul::token_seq& tkns) {
        TAUL_LOG(lgr, "src==\"{}\"", src);
        TAUL_LOG(lgr, "tkns ({})", tkns.size());
        for (const auto& I : tkns) {
            TAUL_LOG(lgr, "    {}", I);
        }
        };

    auto nc = std::make_shared<taul::node_ctx>();


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

        auto f0_expect0 = nc->create(f0, "abc", 0);
        f0_expect0.attach(nc->create(chr, "abc", 0));
        
        auto f0_expect1 = nc->create(f0, "123", 4);
        f0_expect1.attach(nc->create(chr, "123", 4));
        
        auto f0_expect2 = nc->create(f0, "!@#", 8);
        f0_expect2.attach(nc->create(chr, "!@#", 8));

        auto f0_result0 = psr0(*nc, tkns, 0, lgr);
        auto f0_result1 = psr0(*nc, tkns, 1, lgr);
        auto f0_result2 = psr0(*nc, tkns, 2, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect2.fmt_tree());
        if ((bool)f0_result2) {
            TAUL_LOG(lgr, "result:\n{}", f0_result2->fmt_tree());
            EXPECT_TRUE(f0_expect2.equiv(*f0_result2));
            EXPECT_EQ(f0_expect2.str(), f0_result2->str());
        }
        else ADD_FAILURE();
    }

    // failure

    {
        std::string src = "!";
        taul::token_seq tkns0(src);
        taul::token_seq tkns1(src);
        tkns1.push_failure(1);
        _log(src, tkns0);
        _log(src, tkns1);

        EXPECT_FALSE((bool)psr0(*nc, tkns0, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns1, 0, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns1, 1, lgr));
    }
}

TEST_F(GrammarUsageTests, parserExpr_failure) {

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
    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    auto _log =
        [&](const std::string& src, const taul::token_seq& tkns) {
        TAUL_LOG(lgr, "src==\"{}\"", src);
        TAUL_LOG(lgr, "tkns ({})", tkns.size());
        for (const auto& I : tkns) {
            TAUL_LOG(lgr, "    {}", I);
        }
        };

    auto nc = std::make_shared<taul::node_ctx>();


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

        auto f0_expect0 = nc->create(f0, "abc", 0);
        f0_expect0.attach(nc->create(taul::token::failure("abc", 0)));
        
        auto f0_expect1 = nc->create(f0, "123", 4);
        f0_expect1.attach(nc->create(taul::token::failure("123", 4)));
        
        auto f0_expect2 = nc->create(f0, "!@#", 8);
        f0_expect2.attach(nc->create(taul::token::failure("!@#", 8)));

        auto f0_result0 = psr0(*nc, tkns, 0, lgr);
        auto f0_result1 = psr0(*nc, tkns, 1, lgr);
        auto f0_result2 = psr0(*nc, tkns, 2, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect2.fmt_tree());
        if ((bool)f0_result2) {
            TAUL_LOG(lgr, "result:\n{}", f0_result2->fmt_tree());
            EXPECT_TRUE(f0_expect2.equiv(*f0_result2));
            EXPECT_EQ(f0_expect2.str(), f0_result2->str());
        }
        else ADD_FAILURE();
    }

    // failure

    {
        std::string src = "!";
        taul::token_seq tkns0(src);
        taul::token_seq tkns1(src);
        tkns1.push(chr, 1);
        _log(src, tkns0);
        _log(src, tkns1);

        EXPECT_FALSE((bool)psr0(*nc, tkns0, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns1, 0, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns1, 1, lgr));
    }
}

TEST_F(GrammarUsageTests, lexerExpr_name) {


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
        .modifier(0, 1)
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
    const auto& lpr = gram.lpr("f");
    const auto lex = gram.lexer("f");


    // test w/out offset

    // success

    EXPECT_EQ(lex("abc", lgr), taul::token(lpr, "abc"));
    EXPECT_EQ(lex("abca", lgr), taul::token(lpr, "abc"));
    EXPECT_EQ(lex("abcab", lgr), taul::token(lpr, "abc"));
    EXPECT_EQ(lex("abc123", lgr), taul::token(lpr, "abc"));
    EXPECT_EQ(lex("abcabc", lgr), taul::token(lpr, "abcabc"));
    EXPECT_EQ(lex("abcabca", lgr), taul::token(lpr, "abcabc"));
    EXPECT_EQ(lex("abcabcab", lgr), taul::token(lpr, "abcabc"));
    EXPECT_EQ(lex("abcabc123", lgr), taul::token(lpr, "abcabc"));
    EXPECT_EQ(lex("abcabcabc", lgr), taul::token(lpr, "abcabcabc"));
    EXPECT_EQ(lex("abcabcabca", lgr), taul::token(lpr, "abcabcabc"));
    EXPECT_EQ(lex("abcabcabcab", lgr), taul::token(lpr, "abcabcabc"));
    EXPECT_EQ(lex("abcabcabc123", lgr), taul::token(lpr, "abcabcabc"));
    EXPECT_EQ(lex("abcabcabcabc", lgr), taul::token(lpr, "abcabcabcabc"));
    EXPECT_EQ(lex("abcabcabcabca", lgr), taul::token(lpr, "abcabcabcabc"));
    EXPECT_EQ(lex("abcabcabcabcab", lgr), taul::token(lpr, "abcabcabcabc"));
    EXPECT_EQ(lex("abcabcabcabc123", lgr), taul::token(lpr, "abcabcabcabc"));

    // failure

    EXPECT_EQ(lex("", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("a", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("ab", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("def", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("ABC", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("123", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("&*$", lgr), taul::token::failure(""));
    EXPECT_EQ(lex("\r\n\t", lgr), taul::token::failure(""));


    // test w/ offset

    // success

    EXPECT_EQ(lex("+-=abc", 3, lgr), taul::token(lpr, "abc", 3));
    EXPECT_EQ(lex("+-=abca", 3, lgr), taul::token(lpr, "abc", 3));
    EXPECT_EQ(lex("+-=abcab", 3, lgr), taul::token(lpr, "abc", 3));
    EXPECT_EQ(lex("+-=abc123", 3, lgr), taul::token(lpr, "abc", 3));
    EXPECT_EQ(lex("+-=abcabc", 3, lgr), taul::token(lpr, "abcabc", 3));
    EXPECT_EQ(lex("+-=abcabca", 3, lgr), taul::token(lpr, "abcabc", 3));
    EXPECT_EQ(lex("+-=abcabcab", 3, lgr), taul::token(lpr, "abcabc", 3));
    EXPECT_EQ(lex("+-=abcabc123", 3, lgr), taul::token(lpr, "abcabc", 3));
    EXPECT_EQ(lex("+-=abcabcabc", 3, lgr), taul::token(lpr, "abcabcabc", 3));
    EXPECT_EQ(lex("+-=abcabcabca", 3, lgr), taul::token(lpr, "abcabcabc", 3));
    EXPECT_EQ(lex("+-=abcabcabcab", 3, lgr), taul::token(lpr, "abcabcabc", 3));
    EXPECT_EQ(lex("+-=abcabcabc123", 3, lgr), taul::token(lpr, "abcabcabc", 3));
    EXPECT_EQ(lex("+-=abcabcabcabc", 3, lgr), taul::token(lpr, "abcabcabcabc", 3));
    EXPECT_EQ(lex("+-=abcabcabcabca", 3, lgr), taul::token(lpr, "abcabcabcabc", 3));
    EXPECT_EQ(lex("+-=abcabcabcabcab", 3, lgr), taul::token(lpr, "abcabcabcabc", 3));
    EXPECT_EQ(lex("+-=abcabcabcabc123", 3, lgr), taul::token(lpr, "abcabcabcabc", 3));

    // failure

    EXPECT_EQ(lex("+-=", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("+-=a", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("+-=ab", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("+-=def", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("+-=ABC", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("+-=123", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("+-=&*$", 3, lgr), taul::token::failure("", 3));
    EXPECT_EQ(lex("+-=\r\n\t", 3, lgr), taul::token::failure("", 3));
}

TEST_F(GrammarUsageTests, parserExpr_name) {

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
    const auto& a0 = gram.lpr("a0");
    const auto& b0 = gram.lpr("b0");
    const auto& c0 = gram.lpr("c0");
    const auto& misc = gram.lpr("misc");
    const auto& abc0 = gram.ppr("abc0");
    const auto& f0 = gram.ppr("f0");
    const auto psr0 = gram.parser("f0");


    // the below assertions expect the embedded exprs to work as expected


    auto _log =
        [&](const std::string& src, const taul::token_seq& tkns) {
        TAUL_LOG(lgr, "src==\"{}\"", src);
        TAUL_LOG(lgr, "tkns ({})", tkns.size());
        for (const auto& I : tkns) {
            TAUL_LOG(lgr, "    {}", I);
        }
        };

    auto nc = std::make_shared<taul::node_ctx>();


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

        auto f0_expect0 = nc->create(f0, "a b c", 0);
        auto _temp0 = nc->create(abc0, "a b c", 0);
        _temp0.attach(nc->create(a0, "a", 0));
        _temp0.attach(nc->create(b0, "b", 2));
        _temp0.attach(nc->create(c0, "c", 4));
        f0_expect0.attach(std::move(_temp0));

        auto f0_expect1 = nc->create(f0, "abc", 6);
        auto _temp1 = nc->create(abc0, "abc", 6);
        _temp1.attach(nc->create(a0, "a", 6));
        _temp1.attach(nc->create(b0, "b", 7));
        _temp1.attach(nc->create(c0, "c", 8));
        f0_expect1.attach(std::move(_temp1));

        auto f0_result0 = psr0(*nc, tkns, 0, lgr);
        auto f0_result1 = psr0(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();
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

        EXPECT_FALSE((bool)psr0(*nc, tkns0, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns1, 0, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns1, 1, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns1, 2, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns1, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns1, 4, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns1, 5, lgr));
    }
}

TEST_F(GrammarUsageTests, lexerExpr_sequence) {

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

TEST_F(GrammarUsageTests, parserExpr_sequence) {

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
    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto& f1 = gram.ppr("f1");
    const auto psr0 = gram.parser("f0");
    const auto psr1 = gram.parser("f1");


    // the below assertions expect the embedded exprs to work as expected


    auto _log =
        [&](const std::string& src, const taul::token_seq& tkns) {
        TAUL_LOG(lgr, "src==\"{}\"", src);
        TAUL_LOG(lgr, "tkns ({})", tkns.size());
        for (const auto& I : tkns) {
            TAUL_LOG(lgr, "    {}", I);
        }
        };

    auto nc = std::make_shared<taul::node_ctx>();


    // test w/ and w/out offset

    // success

    {
        std::string src0 = "";
        std::string src1 = "a bc d abc  d";
        taul::token_seq tkns0 = taul::tokenize(gram, src0);
        taul::token_seq tkns1 = taul::tokenize(gram, src1);
        _log(src0, tkns0);
        _log(src1, tkns1);

        auto f0_expect0 = nc->create(f0, "", 0);
        auto f0_expect1 = nc->create(f0, "", 0);
        auto f0_expect2 = nc->create(f0, "", 2);
        auto f0_expect3 = nc->create(f0, "", 3);
        auto f0_expect4 = nc->create(f0, "", 5);
        auto f0_expect5 = nc->create(f0, "", 7);
        auto f0_expect6 = nc->create(f0, "", 8);
        auto f0_expect7 = nc->create(f0, "", 9);
        auto f0_expect8 = nc->create(f0, "", 12);
        auto f0_expect9 = nc->create(f0, "", 13);

        auto f1_expect0 = nc->create(f1, "a bc d", 0);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 2)));
        f1_expect0.attach(nc->create(taul::token(chr, "c", 3)));
        f1_expect0.attach(nc->create(taul::token(chr, "d", 5)));
        auto f1_expect1 = nc->create(f1, "abc  d", 7);
        f1_expect1.attach(nc->create(taul::token(chr, "a", 7)));
        f1_expect1.attach(nc->create(taul::token(chr, "b", 8)));
        f1_expect1.attach(nc->create(taul::token(chr, "c", 9)));
        f1_expect1.attach(nc->create(taul::token(chr, "d", 12)));

        auto f0_result0 = psr0(*nc, tkns0, 0, lgr);
        auto f0_result1 = psr0(*nc, tkns1, 0, lgr);
        auto f0_result2 = psr0(*nc, tkns1, 1, lgr);
        auto f0_result3 = psr0(*nc, tkns1, 2, lgr);
        auto f0_result4 = psr0(*nc, tkns1, 3, lgr);
        auto f0_result5 = psr0(*nc, tkns1, 4, lgr);
        auto f0_result6 = psr0(*nc, tkns1, 5, lgr);
        auto f0_result7 = psr0(*nc, tkns1, 6, lgr);
        auto f0_result8 = psr0(*nc, tkns1, 7, lgr);
        auto f0_result9 = psr0(*nc, tkns1, 8, lgr);

        auto f1_result0 = psr1(*nc, tkns1, 0, lgr);
        auto f1_result1 = psr1(*nc, tkns1, 4, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect2.fmt_tree());
        if ((bool)f0_result2) {
            TAUL_LOG(lgr, "result:\n{}", f0_result2->fmt_tree());
            EXPECT_TRUE(f0_expect2.equiv(*f0_result2));
            EXPECT_EQ(f0_expect2.str(), f0_result2->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect3.fmt_tree());
        if ((bool)f0_result3) {
            TAUL_LOG(lgr, "result:\n{}", f0_result3->fmt_tree());
            EXPECT_TRUE(f0_expect3.equiv(*f0_result3));
            EXPECT_EQ(f0_expect3.str(), f0_result3->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect4.fmt_tree());
        if ((bool)f0_result4) {
            TAUL_LOG(lgr, "result:\n{}", f0_result4->fmt_tree());
            EXPECT_TRUE(f0_expect4.equiv(*f0_result4));
            EXPECT_EQ(f0_expect4.str(), f0_result4->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect5.fmt_tree());
        if ((bool)f0_result5) {
            TAUL_LOG(lgr, "result:\n{}", f0_result5->fmt_tree());
            EXPECT_TRUE(f0_expect5.equiv(*f0_result5));
            EXPECT_EQ(f0_expect5.str(), f0_result5->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect6.fmt_tree());
        if ((bool)f0_result6) {
            TAUL_LOG(lgr, "result:\n{}", f0_result6->fmt_tree());
            EXPECT_TRUE(f0_expect6.equiv(*f0_result6));
            EXPECT_EQ(f0_expect6.str(), f0_result6->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect7.fmt_tree());
        if ((bool)f0_result7) {
            TAUL_LOG(lgr, "result:\n{}", f0_result7->fmt_tree());
            EXPECT_TRUE(f0_expect7.equiv(*f0_result7));
            EXPECT_EQ(f0_expect7.str(), f0_result7->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect8.fmt_tree());
        if ((bool)f0_result8) {
            TAUL_LOG(lgr, "result:\n{}", f0_result8->fmt_tree());
            EXPECT_TRUE(f0_expect8.equiv(*f0_result8));
            EXPECT_EQ(f0_expect8.str(), f0_result8->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect9.fmt_tree());
        if ((bool)f0_result9) {
            TAUL_LOG(lgr, "result:\n{}", f0_result9->fmt_tree());
            EXPECT_TRUE(f0_expect9.equiv(*f0_result9));
            EXPECT_EQ(f0_expect9.str(), f0_result9->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();
    }

    // failure

    {
        std::string src = "abd abCd Abcd";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        EXPECT_FALSE((bool)psr1(*nc, tkns, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 0, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 1, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 2, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 3, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 4, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 5, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 6, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 7, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 8, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 9, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 10, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 11, lgr));
    }
}

TEST_F(GrammarUsageTests, lexerExpr_set) {

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
        .lpr("f1") // test non-empty set w/ f-longest
        .set(taul::bias::fl)
        .string("123")
        .string("a")
        .string("12")
        .string("abc")
        .close()
        .close()
        .lpr("f2") // test non-empty set w/ f-shortest
        .set(taul::bias::fs)
        .string("123")
        .string("a")
        .string("12")
        .string("abc")
        .close()
        .close()
        .lpr("f3") // test non-empty set w/ l-longest
        .set(taul::bias::ll)
        .string("123")
        .string("a")
        .string("12")
        .string("abc")
        .close()
        .close()
        .lpr("f4") // test non-empty set w/ l-shortest
        .set(taul::bias::ls)
        .string("123")
        .string("a")
        .string("12")
        .string("abc")
        .close()
        .close()
        .lpr("f5") // test non-empty set w/ f
        .set(taul::bias::f)
        .string("123")
        .string("a")
        .string("12")
        .string("abc")
        .close()
        .close()
        .lpr("f6") // test non-empty set w/ l
        .set(taul::bias::l)
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

TEST_F(GrammarUsageTests, parserExpr_set) {

    // remember that all parser expr tests should include testing things like
    // that the impl can handle token sequences w/ 'skip tokens' cut out

    // testing w/ regards to failure tokens should also be ensured

    const auto spec =
        taul::spec_writer()
        .lpr_decl("ws")
        .lpr_decl("chr")
        .ppr_decl("f0")
        .ppr_decl("f1")
        .ppr_decl("f2")
        .ppr_decl("f3")
        .ppr_decl("f4")
        .ppr_decl("f5")
        .ppr_decl("f6")
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
        .ppr("f1") // test non-empty set w/ f-longest
        .set(taul::bias::fl)
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
        .ppr("f2") // test non-empty set w/ f-shortest
        .set(taul::bias::fs)
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
        .ppr("f3") // test non-empty set w/ l-longest
        .set(taul::bias::ll)
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
        .ppr("f4") // test non-empty set w/ l-shortest
        .set(taul::bias::ls)
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
        .ppr("f5") // test non-empty set w/ f
        .set(taul::bias::f)
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
        .ppr("f6") // test non-empty set w/ l
        .set(taul::bias::l)
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
    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto& f1 = gram.ppr("f1");
    const auto& f2 = gram.ppr("f2");
    const auto& f3 = gram.ppr("f3");
    const auto& f4 = gram.ppr("f4");
    const auto& f5 = gram.ppr("f5");
    const auto& f6 = gram.ppr("f6");
    const auto psr0 = gram.parser("f0");
    const auto psr1 = gram.parser("f1");
    const auto psr2 = gram.parser("f2");
    const auto psr3 = gram.parser("f3");
    const auto psr4 = gram.parser("f4");
    const auto psr5 = gram.parser("f5");
    const auto psr6 = gram.parser("f6");


    // the below assertions expect the embedded exprs to work as expected


    auto _log =
        [&](const std::string& src, const taul::token_seq& tkns) {
        TAUL_LOG(lgr, "src==\"{}\"", src);
        TAUL_LOG(lgr, "tkns ({})", tkns.size());
        for (const auto& I : tkns) {
            TAUL_LOG(lgr, "    {}", I);
        }
        };

    auto nc = std::make_shared<taul::node_ctx>();


    // test w/ and w/out offset

    // success

    {
        std::string src = "123";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f1_expect0 = nc->create(f1, "123", 0);
        f1_expect0.attach(nc->create(taul::token(chr, "1", 0)));
        f1_expect0.attach(nc->create(taul::token(chr, "2", 1)));
        f1_expect0.attach(nc->create(taul::token(chr, "3", 2)));
        auto f2_expect0 = nc->create(f2, "12", 0);
        f2_expect0.attach(nc->create(taul::token(chr, "1", 0)));
        f2_expect0.attach(nc->create(taul::token(chr, "2", 1)));
        auto f3_expect0 = nc->create(f3, "123", 0);
        f3_expect0.attach(nc->create(taul::token(chr, "1", 0)));
        f3_expect0.attach(nc->create(taul::token(chr, "2", 1)));
        f3_expect0.attach(nc->create(taul::token(chr, "3", 2)));
        auto f4_expect0 = nc->create(f4, "12", 0);
        f4_expect0.attach(nc->create(taul::token(chr, "1", 0)));
        f4_expect0.attach(nc->create(taul::token(chr, "2", 1)));
        auto f5_expect0 = nc->create(f5, "123", 0);
        f5_expect0.attach(nc->create(taul::token(chr, "1", 0)));
        f5_expect0.attach(nc->create(taul::token(chr, "2", 1)));
        f5_expect0.attach(nc->create(taul::token(chr, "3", 2)));
        auto f6_expect0 = nc->create(f6, "12", 0);
        f6_expect0.attach(nc->create(taul::token(chr, "1", 0)));
        f6_expect0.attach(nc->create(taul::token(chr, "2", 1)));

        auto f1_expect1 = nc->create(f1, "123", 0);
        f1_expect1.attach(nc->create(taul::token(chr, "1", 0)));
        f1_expect1.attach(nc->create(taul::token(chr, "2", 1)));
        f1_expect1.attach(nc->create(taul::token(chr, "3", 2)));
        auto f2_expect1 = nc->create(f2, "12", 0);
        f2_expect1.attach(nc->create(taul::token(chr, "1", 0)));
        f2_expect1.attach(nc->create(taul::token(chr, "2", 1)));
        auto f3_expect1 = nc->create(f3, "123", 0);
        f3_expect1.attach(nc->create(taul::token(chr, "1", 0)));
        f3_expect1.attach(nc->create(taul::token(chr, "2", 1)));
        f3_expect1.attach(nc->create(taul::token(chr, "3", 2)));
        auto f4_expect1 = nc->create(f4, "12", 0);
        f4_expect1.attach(nc->create(taul::token(chr, "1", 0)));
        f4_expect1.attach(nc->create(taul::token(chr, "2", 1)));
        auto f5_expect1 = nc->create(f5, "123", 0);
        f5_expect1.attach(nc->create(taul::token(chr, "1", 0)));
        f5_expect1.attach(nc->create(taul::token(chr, "2", 1)));
        f5_expect1.attach(nc->create(taul::token(chr, "3", 2)));
        auto f6_expect1 = nc->create(f6, "12", 0);
        f6_expect1.attach(nc->create(taul::token(chr, "1", 0)));
        f6_expect1.attach(nc->create(taul::token(chr, "2", 1)));

        auto f1_result0 = psr1(*nc, tkns, lgr);
        auto f2_result0 = psr2(*nc, tkns, lgr);
        auto f3_result0 = psr3(*nc, tkns, lgr);
        auto f4_result0 = psr4(*nc, tkns, lgr);
        auto f5_result0 = psr5(*nc, tkns, lgr);
        auto f6_result0 = psr6(*nc, tkns, lgr);

        auto f1_result1 = psr1(*nc, tkns, 0, lgr);
        auto f2_result1 = psr2(*nc, tkns, 0, lgr);
        auto f3_result1 = psr3(*nc, tkns, 0, lgr);
        auto f4_result1 = psr4(*nc, tkns, 0, lgr);
        auto f5_result1 = psr5(*nc, tkns, 0, lgr);
        auto f6_result1 = psr6(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect1.fmt_tree());
        if ((bool)f2_result1) {
            TAUL_LOG(lgr, "result:\n{}", f2_result1->fmt_tree());
            EXPECT_TRUE(f2_expect1.equiv(*f2_result1));
            EXPECT_EQ(f2_expect1.str(), f2_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect1.fmt_tree());
        if ((bool)f3_result1) {
            TAUL_LOG(lgr, "result:\n{}", f3_result1->fmt_tree());
            EXPECT_TRUE(f3_expect1.equiv(*f3_result1));
            EXPECT_EQ(f3_expect1.str(), f3_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect1.fmt_tree());
        if ((bool)f4_result1) {
            TAUL_LOG(lgr, "result:\n{}", f4_result1->fmt_tree());
            EXPECT_TRUE(f4_expect1.equiv(*f4_result1));
            EXPECT_EQ(f4_expect1.str(), f4_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect1.fmt_tree());
        if ((bool)f5_result1) {
            TAUL_LOG(lgr, "result:\n{}", f5_result1->fmt_tree());
            EXPECT_TRUE(f5_expect1.equiv(*f5_result1));
            EXPECT_EQ(f5_expect1.str(), f5_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect1.fmt_tree());
        if ((bool)f6_result1) {
            TAUL_LOG(lgr, "result:\n{}", f6_result1->fmt_tree());
            EXPECT_TRUE(f6_expect1.equiv(*f6_result1));
            EXPECT_EQ(f6_expect1.str(), f6_result1->str());
        }
        else ADD_FAILURE();
    }

    {
        std::string src = "a";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f1_expect0 = nc->create(f1, "a", 0);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        auto f2_expect0 = nc->create(f2, "a", 0);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        auto f3_expect0 = nc->create(f3, "a", 0);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        auto f4_expect0 = nc->create(f4, "a", 0);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        auto f5_expect0 = nc->create(f5, "a", 0);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        auto f6_expect0 = nc->create(f6, "a", 0);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 0)));

        auto f1_expect1 = nc->create(f1, "a", 0);
        f1_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        auto f2_expect1 = nc->create(f2, "a", 0);
        f2_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        auto f3_expect1 = nc->create(f3, "a", 0);
        f3_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        auto f4_expect1 = nc->create(f4, "a", 0);
        f4_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        auto f5_expect1 = nc->create(f5, "a", 0);
        f5_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        auto f6_expect1 = nc->create(f6, "a", 0);
        f6_expect1.attach(nc->create(taul::token(chr, "a", 0)));

        auto f1_result0 = psr1(*nc, tkns, lgr);
        auto f2_result0 = psr2(*nc, tkns, lgr);
        auto f3_result0 = psr3(*nc, tkns, lgr);
        auto f4_result0 = psr4(*nc, tkns, lgr);
        auto f5_result0 = psr5(*nc, tkns, lgr);
        auto f6_result0 = psr6(*nc, tkns, lgr);

        auto f1_result1 = psr1(*nc, tkns, 0, lgr);
        auto f2_result1 = psr2(*nc, tkns, 0, lgr);
        auto f3_result1 = psr3(*nc, tkns, 0, lgr);
        auto f4_result1 = psr4(*nc, tkns, 0, lgr);
        auto f5_result1 = psr5(*nc, tkns, 0, lgr);
        auto f6_result1 = psr6(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect1.fmt_tree());
        if ((bool)f2_result1) {
            TAUL_LOG(lgr, "result:\n{}", f2_result1->fmt_tree());
            EXPECT_TRUE(f2_expect1.equiv(*f2_result1));
            EXPECT_EQ(f2_expect1.str(), f2_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect1.fmt_tree());
        if ((bool)f3_result1) {
            TAUL_LOG(lgr, "result:\n{}", f3_result1->fmt_tree());
            EXPECT_TRUE(f3_expect1.equiv(*f3_result1));
            EXPECT_EQ(f3_expect1.str(), f3_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect1.fmt_tree());
        if ((bool)f4_result1) {
            TAUL_LOG(lgr, "result:\n{}", f4_result1->fmt_tree());
            EXPECT_TRUE(f4_expect1.equiv(*f4_result1));
            EXPECT_EQ(f4_expect1.str(), f4_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect1.fmt_tree());
        if ((bool)f5_result1) {
            TAUL_LOG(lgr, "result:\n{}", f5_result1->fmt_tree());
            EXPECT_TRUE(f5_expect1.equiv(*f5_result1));
            EXPECT_EQ(f5_expect1.str(), f5_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect1.fmt_tree());
        if ((bool)f6_result1) {
            TAUL_LOG(lgr, "result:\n{}", f6_result1->fmt_tree());
            EXPECT_TRUE(f6_expect1.equiv(*f6_result1));
            EXPECT_EQ(f6_expect1.str(), f6_result1->str());
        }
        else ADD_FAILURE();
    }

    {
        std::string src = "12";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f1_expect0 = nc->create(f1, "12", 0);
        f1_expect0.attach(nc->create(taul::token(chr, "1", 0)));
        f1_expect0.attach(nc->create(taul::token(chr, "2", 1)));
        auto f2_expect0 = nc->create(f2, "12", 0);
        f2_expect0.attach(nc->create(taul::token(chr, "1", 0)));
        f2_expect0.attach(nc->create(taul::token(chr, "2", 1)));
        auto f3_expect0 = nc->create(f3, "12", 0);
        f3_expect0.attach(nc->create(taul::token(chr, "1", 0)));
        f3_expect0.attach(nc->create(taul::token(chr, "2", 1)));
        auto f4_expect0 = nc->create(f4, "12", 0);
        f4_expect0.attach(nc->create(taul::token(chr, "1", 0)));
        f4_expect0.attach(nc->create(taul::token(chr, "2", 1)));
        auto f5_expect0 = nc->create(f5, "12", 0);
        f5_expect0.attach(nc->create(taul::token(chr, "1", 0)));
        f5_expect0.attach(nc->create(taul::token(chr, "2", 1)));
        auto f6_expect0 = nc->create(f6, "12", 0);
        f6_expect0.attach(nc->create(taul::token(chr, "1", 0)));
        f6_expect0.attach(nc->create(taul::token(chr, "2", 1)));

        auto f1_expect1 = nc->create(f1, "12", 0);
        f1_expect1.attach(nc->create(taul::token(chr, "1", 0)));
        f1_expect1.attach(nc->create(taul::token(chr, "2", 1)));
        auto f2_expect1 = nc->create(f2, "12", 0);
        f2_expect1.attach(nc->create(taul::token(chr, "1", 0)));
        f2_expect1.attach(nc->create(taul::token(chr, "2", 1)));
        auto f3_expect1 = nc->create(f3, "12", 0);
        f3_expect1.attach(nc->create(taul::token(chr, "1", 0)));
        f3_expect1.attach(nc->create(taul::token(chr, "2", 1)));
        auto f4_expect1 = nc->create(f4, "12", 0);
        f4_expect1.attach(nc->create(taul::token(chr, "1", 0)));
        f4_expect1.attach(nc->create(taul::token(chr, "2", 1)));
        auto f5_expect1 = nc->create(f5, "12", 0);
        f5_expect1.attach(nc->create(taul::token(chr, "1", 0)));
        f5_expect1.attach(nc->create(taul::token(chr, "2", 1)));
        auto f6_expect1 = nc->create(f6, "12", 0);
        f6_expect1.attach(nc->create(taul::token(chr, "1", 0)));
        f6_expect1.attach(nc->create(taul::token(chr, "2", 1)));

        auto f1_result0 = psr1(*nc, tkns, lgr);
        auto f2_result0 = psr2(*nc, tkns, lgr);
        auto f3_result0 = psr3(*nc, tkns, lgr);
        auto f4_result0 = psr4(*nc, tkns, lgr);
        auto f5_result0 = psr5(*nc, tkns, lgr);
        auto f6_result0 = psr6(*nc, tkns, lgr);

        auto f1_result1 = psr1(*nc, tkns, 0, lgr);
        auto f2_result1 = psr2(*nc, tkns, 0, lgr);
        auto f3_result1 = psr3(*nc, tkns, 0, lgr);
        auto f4_result1 = psr4(*nc, tkns, 0, lgr);
        auto f5_result1 = psr5(*nc, tkns, 0, lgr);
        auto f6_result1 = psr6(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect1.fmt_tree());
        if ((bool)f2_result1) {
            TAUL_LOG(lgr, "result:\n{}", f2_result1->fmt_tree());
            EXPECT_TRUE(f2_expect1.equiv(*f2_result1));
            EXPECT_EQ(f2_expect1.str(), f2_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect1.fmt_tree());
        if ((bool)f3_result1) {
            TAUL_LOG(lgr, "result:\n{}", f3_result1->fmt_tree());
            EXPECT_TRUE(f3_expect1.equiv(*f3_result1));
            EXPECT_EQ(f3_expect1.str(), f3_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect1.fmt_tree());
        if ((bool)f4_result1) {
            TAUL_LOG(lgr, "result:\n{}", f4_result1->fmt_tree());
            EXPECT_TRUE(f4_expect1.equiv(*f4_result1));
            EXPECT_EQ(f4_expect1.str(), f4_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect1.fmt_tree());
        if ((bool)f5_result1) {
            TAUL_LOG(lgr, "result:\n{}", f5_result1->fmt_tree());
            EXPECT_TRUE(f5_expect1.equiv(*f5_result1));
            EXPECT_EQ(f5_expect1.str(), f5_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect1.fmt_tree());
        if ((bool)f6_result1) {
            TAUL_LOG(lgr, "result:\n{}", f6_result1->fmt_tree());
            EXPECT_TRUE(f6_expect1.equiv(*f6_result1));
            EXPECT_EQ(f6_expect1.str(), f6_result1->str());
        }
        else ADD_FAILURE();
    }

    {
        std::string src = "abc";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f1_expect0 = nc->create(f1, "abc", 0);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f1_expect0.attach(nc->create(taul::token(chr, "c", 2)));
        auto f2_expect0 = nc->create(f2, "a", 0);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        auto f3_expect0 = nc->create(f3, "abc", 0);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f3_expect0.attach(nc->create(taul::token(chr, "c", 2)));
        auto f4_expect0 = nc->create(f4, "a", 0);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        auto f5_expect0 = nc->create(f5, "a", 0);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        auto f6_expect0 = nc->create(f6, "abc", 0);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f6_expect0.attach(nc->create(taul::token(chr, "c", 2)));

        auto f1_expect1 = nc->create(f1, "abc", 0);
        f1_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f1_expect1.attach(nc->create(taul::token(chr, "c", 2)));
        auto f2_expect1 = nc->create(f2, "a", 0);
        f2_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        auto f3_expect1 = nc->create(f3, "abc", 0);
        f3_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f3_expect1.attach(nc->create(taul::token(chr, "c", 2)));
        auto f4_expect1 = nc->create(f4, "a", 0);
        f4_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        auto f5_expect1 = nc->create(f5, "a", 0);
        f5_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        auto f6_expect1 = nc->create(f6, "abc", 0);
        f6_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f6_expect1.attach(nc->create(taul::token(chr, "c", 2)));

        auto f1_result0 = psr1(*nc, tkns, lgr);
        auto f2_result0 = psr2(*nc, tkns, lgr);
        auto f3_result0 = psr3(*nc, tkns, lgr);
        auto f4_result0 = psr4(*nc, tkns, lgr);
        auto f5_result0 = psr5(*nc, tkns, lgr);
        auto f6_result0 = psr6(*nc, tkns, lgr);

        auto f1_result1 = psr1(*nc, tkns, 0, lgr);
        auto f2_result1 = psr2(*nc, tkns, 0, lgr);
        auto f3_result1 = psr3(*nc, tkns, 0, lgr);
        auto f4_result1 = psr4(*nc, tkns, 0, lgr);
        auto f5_result1 = psr5(*nc, tkns, 0, lgr);
        auto f6_result1 = psr6(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect1.fmt_tree());
        if ((bool)f2_result1) {
            TAUL_LOG(lgr, "result:\n{}", f2_result1->fmt_tree());
            EXPECT_TRUE(f2_expect1.equiv(*f2_result1));
            EXPECT_EQ(f2_expect1.str(), f2_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect1.fmt_tree());
        if ((bool)f3_result1) {
            TAUL_LOG(lgr, "result:\n{}", f3_result1->fmt_tree());
            EXPECT_TRUE(f3_expect1.equiv(*f3_result1));
            EXPECT_EQ(f3_expect1.str(), f3_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect1.fmt_tree());
        if ((bool)f4_result1) {
            TAUL_LOG(lgr, "result:\n{}", f4_result1->fmt_tree());
            EXPECT_TRUE(f4_expect1.equiv(*f4_result1));
            EXPECT_EQ(f4_expect1.str(), f4_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect1.fmt_tree());
        if ((bool)f5_result1) {
            TAUL_LOG(lgr, "result:\n{}", f5_result1->fmt_tree());
            EXPECT_TRUE(f5_expect1.equiv(*f5_result1));
            EXPECT_EQ(f5_expect1.str(), f5_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect1.fmt_tree());
        if ((bool)f6_result1) {
            TAUL_LOG(lgr, "result:\n{}", f6_result1->fmt_tree());
            EXPECT_TRUE(f6_expect1.equiv(*f6_result1));
            EXPECT_EQ(f6_expect1.str(), f6_result1->str());
        }
        else ADD_FAILURE();
    }

    {
        std::string src = "___123";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f1_expect0 = nc->create(f1, "123", 3);
        f1_expect0.attach(nc->create(taul::token(chr, "1", 3)));
        f1_expect0.attach(nc->create(taul::token(chr, "2", 4)));
        f1_expect0.attach(nc->create(taul::token(chr, "3", 5)));
        auto f2_expect0 = nc->create(f2, "12", 3);
        f2_expect0.attach(nc->create(taul::token(chr, "1", 3)));
        f2_expect0.attach(nc->create(taul::token(chr, "2", 4)));
        auto f3_expect0 = nc->create(f3, "123", 3);
        f3_expect0.attach(nc->create(taul::token(chr, "1", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "2", 4)));
        f3_expect0.attach(nc->create(taul::token(chr, "3", 5)));
        auto f4_expect0 = nc->create(f4, "12", 3);
        f4_expect0.attach(nc->create(taul::token(chr, "1", 3)));
        f4_expect0.attach(nc->create(taul::token(chr, "2", 4)));
        auto f5_expect0 = nc->create(f5, "123", 3);
        f5_expect0.attach(nc->create(taul::token(chr, "1", 3)));
        f5_expect0.attach(nc->create(taul::token(chr, "2", 4)));
        f5_expect0.attach(nc->create(taul::token(chr, "3", 5)));
        auto f6_expect0 = nc->create(f6, "12", 3);
        f6_expect0.attach(nc->create(taul::token(chr, "1", 3)));
        f6_expect0.attach(nc->create(taul::token(chr, "2", 4)));

        auto f1_result0 = psr1(*nc, tkns, 3, lgr);
        auto f2_result0 = psr2(*nc, tkns, 3, lgr);
        auto f3_result0 = psr3(*nc, tkns, 3, lgr);
        auto f4_result0 = psr4(*nc, tkns, 3, lgr);
        auto f5_result0 = psr5(*nc, tkns, 3, lgr);
        auto f6_result0 = psr6(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();
    }

    {
        std::string src = "___a";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f1_expect0 = nc->create(f1, "a", 3);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        auto f2_expect0 = nc->create(f2, "a", 3);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        auto f3_expect0 = nc->create(f3, "a", 3);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        auto f4_expect0 = nc->create(f4, "a", 3);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        auto f5_expect0 = nc->create(f5, "a", 3);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        auto f6_expect0 = nc->create(f6, "a", 3);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 3)));

        auto f1_result0 = psr1(*nc, tkns, 3, lgr);
        auto f2_result0 = psr2(*nc, tkns, 3, lgr);
        auto f3_result0 = psr3(*nc, tkns, 3, lgr);
        auto f4_result0 = psr4(*nc, tkns, 3, lgr);
        auto f5_result0 = psr5(*nc, tkns, 3, lgr);
        auto f6_result0 = psr6(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();
    }

    {
        std::string src = "___12";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f1_expect0 = nc->create(f1, "12", 3);
        f1_expect0.attach(nc->create(taul::token(chr, "1", 3)));
        f1_expect0.attach(nc->create(taul::token(chr, "2", 4)));
        auto f2_expect0 = nc->create(f2, "12", 3);
        f2_expect0.attach(nc->create(taul::token(chr, "1", 3)));
        f2_expect0.attach(nc->create(taul::token(chr, "2", 4)));
        auto f3_expect0 = nc->create(f3, "12", 3);
        f3_expect0.attach(nc->create(taul::token(chr, "1", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "2", 4)));
        auto f4_expect0 = nc->create(f4, "12", 3);
        f4_expect0.attach(nc->create(taul::token(chr, "1", 3)));
        f4_expect0.attach(nc->create(taul::token(chr, "2", 4)));
        auto f5_expect0 = nc->create(f5, "12", 3);
        f5_expect0.attach(nc->create(taul::token(chr, "1", 3)));
        f5_expect0.attach(nc->create(taul::token(chr, "2", 4)));
        auto f6_expect0 = nc->create(f6, "12", 3);
        f6_expect0.attach(nc->create(taul::token(chr, "1", 3)));
        f6_expect0.attach(nc->create(taul::token(chr, "2", 4)));

        auto f1_result0 = psr1(*nc, tkns, 3, lgr);
        auto f2_result0 = psr2(*nc, tkns, 3, lgr);
        auto f3_result0 = psr3(*nc, tkns, 3, lgr);
        auto f4_result0 = psr4(*nc, tkns, 3, lgr);
        auto f5_result0 = psr5(*nc, tkns, 3, lgr);
        auto f6_result0 = psr6(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();
    }

    {
        std::string src = "___abc";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f1_expect0 = nc->create(f1, "abc", 3);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f1_expect0.attach(nc->create(taul::token(chr, "c", 5)));
        auto f2_expect0 = nc->create(f2, "a", 3);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        auto f3_expect0 = nc->create(f3, "abc", 3);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f3_expect0.attach(nc->create(taul::token(chr, "c", 5)));
        auto f4_expect0 = nc->create(f4, "a", 3);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        auto f5_expect0 = nc->create(f5, "a", 3);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        auto f6_expect0 = nc->create(f6, "abc", 3);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f6_expect0.attach(nc->create(taul::token(chr, "c", 5)));

        auto f1_result0 = psr1(*nc, tkns, 3, lgr);
        auto f2_result0 = psr2(*nc, tkns, 3, lgr);
        auto f3_result0 = psr3(*nc, tkns, 3, lgr);
        auto f4_result0 = psr4(*nc, tkns, 3, lgr);
        auto f5_result0 = psr5(*nc, tkns, 3, lgr);
        auto f6_result0 = psr6(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();
    }

    // failure

    {
        std::string src = "&*(!@#";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        EXPECT_FALSE((bool)psr0(*nc, tkns, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, lgr));
        EXPECT_FALSE((bool)psr2(*nc, tkns, lgr));
        EXPECT_FALSE((bool)psr3(*nc, tkns, lgr));
        EXPECT_FALSE((bool)psr4(*nc, tkns, lgr));
        EXPECT_FALSE((bool)psr5(*nc, tkns, lgr));
        EXPECT_FALSE((bool)psr6(*nc, tkns, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 0, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 0, lgr));
        EXPECT_FALSE((bool)psr2(*nc, tkns, 0, lgr));
        EXPECT_FALSE((bool)psr3(*nc, tkns, 0, lgr));
        EXPECT_FALSE((bool)psr4(*nc, tkns, 0, lgr));
        EXPECT_FALSE((bool)psr5(*nc, tkns, 0, lgr));
        EXPECT_FALSE((bool)psr6(*nc, tkns, 0, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 3, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 3, lgr));
        EXPECT_FALSE((bool)psr2(*nc, tkns, 3, lgr));
        EXPECT_FALSE((bool)psr3(*nc, tkns, 3, lgr));
        EXPECT_FALSE((bool)psr4(*nc, tkns, 3, lgr));
        EXPECT_FALSE((bool)psr5(*nc, tkns, 3, lgr));
        EXPECT_FALSE((bool)psr6(*nc, tkns, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns, 6, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns, 6, lgr));
        EXPECT_FALSE((bool)psr2(*nc, tkns, 6, lgr));
        EXPECT_FALSE((bool)psr3(*nc, tkns, 6, lgr));
        EXPECT_FALSE((bool)psr4(*nc, tkns, 6, lgr));
        EXPECT_FALSE((bool)psr5(*nc, tkns, 6, lgr));
        EXPECT_FALSE((bool)psr6(*nc, tkns, 6, lgr));
    }
}

TEST_F(GrammarUsageTests, lexerExpr_modifier) {

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

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
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

TEST_F(GrammarUsageTests, parserExpr_modifier) {

    // remember that all parser expr tests should include testing things like
    // that the impl can handle token sequences w/ 'skip tokens' cut out

    // testing w/ regards to failure tokens should also be ensured

    const auto spec =
        taul::spec_writer()
        .lpr_decl("ws")
        .lpr_decl("chr")
        .ppr_decl("f0")
        .ppr_decl("f1")
        .ppr_decl("f2")
        .ppr_decl("f3")
        .ppr_decl("f4")
        .ppr_decl("f5")
        .ppr_decl("f6")
        .ppr_decl("f7")
        .lpr("ws", taul::qualifier::skip)
        .string(" ")
        .close()
        .lpr("chr")
        .any()
        .close()
        .ppr("f0")
        .modifier(1, 1) // *control*
        .sequence()
        .string("a")
        .string("b")
        .close()
        .close()
        .close()
        .ppr("f1")
        .modifier(0, 1) // "optional" or ?
        .sequence()
        .string("a")
        .string("b")
        .close()
        .close()
        .close()
        .ppr("f2")
        .modifier(0, 0) // "kleene-star" or *
        .sequence()
        .string("a")
        .string("b")
        .close()
        .close()
        .close()
        .ppr("f3")
        .modifier(1, 0) // "kleene-plus" or +
        .sequence()
        .string("a")
        .string("b")
        .close()
        .close()
        .close()
        .ppr("f4")
        .modifier(2, 2) // "manual A" or {n}
        .sequence()
        .string("a")
        .string("b")
        .close()
        .close()
        .close()
        .ppr("f5")
        .modifier(2, 0) // "manual B" or {n,}
        .sequence()
        .string("a")
        .string("b")
        .close()
        .close()
        .close()
        .ppr("f6")
        .modifier(2, 4) // "manual C" or {n,m} (#1)
        .sequence()
        .string("a")
        .string("b")
        .close()
        .close()
        .close()
        .ppr("f7")
        .modifier(0, 4) // "manual C" or {n,m} (#2)
        .sequence()
        .string("a")
        .string("b")
        .close()
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto& f1 = gram.ppr("f1");
    const auto& f2 = gram.ppr("f2");
    const auto& f3 = gram.ppr("f3");
    const auto& f4 = gram.ppr("f4");
    const auto& f5 = gram.ppr("f5");
    const auto& f6 = gram.ppr("f6");
    const auto& f7 = gram.ppr("f7");
    const auto psr0 = gram.parser("f0");
    const auto psr1 = gram.parser("f1");
    const auto psr2 = gram.parser("f2");
    const auto psr3 = gram.parser("f3");
    const auto psr4 = gram.parser("f4");
    const auto psr5 = gram.parser("f5");
    const auto psr6 = gram.parser("f6");
    const auto psr7 = gram.parser("f7");


    // the below assertions expect the embedded exprs to work as expected


    auto _log =
        [&](const std::string& src, const taul::token_seq& tkns) {
        TAUL_LOG(lgr, "src==\"{}\"", src);
        TAUL_LOG(lgr, "tkns ({})", tkns.size());
        for (const auto& I : tkns) {
            TAUL_LOG(lgr, "    {}", I);
        }
        };

    auto nc = std::make_shared<taul::node_ctx>();


    // test w/out offset

    // success/failure

    //EXPECT_EQ(lex0("", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex1("", lgr), taul::token(lpr1, ""));
    //EXPECT_EQ(lex2("", lgr), taul::token(lpr2, ""));
    //EXPECT_EQ(lex3("", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex4("", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex5("", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex6("", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex7("", lgr), taul::token(lpr7, ""));

    {
        std::string src = "";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f1_expect0 = nc->create(f1, "", 0);
        auto f2_expect0 = nc->create(f2, "", 0);
        auto f7_expect0 = nc->create(f7, "", 0);
        
        auto f1_expect1 = nc->create(f1, "", 0);
        auto f2_expect1 = nc->create(f2, "", 0);
        auto f7_expect1 = nc->create(f7, "", 0);

        auto f0_result0 = psr0(*nc, tkns, lgr);
        auto f1_result0 = psr1(*nc, tkns, lgr);
        auto f2_result0 = psr2(*nc, tkns, lgr);
        auto f3_result0 = psr3(*nc, tkns, lgr);
        auto f4_result0 = psr4(*nc, tkns, lgr);
        auto f5_result0 = psr5(*nc, tkns, lgr);
        auto f6_result0 = psr6(*nc, tkns, lgr);
        auto f7_result0 = psr7(*nc, tkns, lgr);

        auto f0_result1 = psr0(*nc, tkns, 0, lgr);
        auto f1_result1 = psr1(*nc, tkns, 0, lgr);
        auto f2_result1 = psr2(*nc, tkns, 0, lgr);
        auto f3_result1 = psr3(*nc, tkns, 0, lgr);
        auto f4_result1 = psr4(*nc, tkns, 0, lgr);
        auto f5_result1 = psr5(*nc, tkns, 0, lgr);
        auto f6_result1 = psr6(*nc, tkns, 0, lgr);
        auto f7_result1 = psr7(*nc, tkns, 0, lgr);

        EXPECT_FALSE(f0_result0);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();
        
        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        EXPECT_FALSE(f3_result0);
        EXPECT_FALSE(f4_result0);
        EXPECT_FALSE(f5_result0);
        EXPECT_FALSE(f6_result0);

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();

        EXPECT_FALSE(f0_result1);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();
        
        TAUL_LOG(lgr, "expected:\n{}", f2_expect1.fmt_tree());
        if ((bool)f2_result1) {
            TAUL_LOG(lgr, "result:\n{}", f2_result1->fmt_tree());
            EXPECT_TRUE(f2_expect1.equiv(*f2_result1));
            EXPECT_EQ(f2_expect1.str(), f2_result1->str());
        }
        else ADD_FAILURE();

        EXPECT_FALSE(f3_result1);
        EXPECT_FALSE(f4_result1);
        EXPECT_FALSE(f5_result1);
        EXPECT_FALSE(f6_result1);

        TAUL_LOG(lgr, "expected:\n{}", f7_expect1.fmt_tree());
        if ((bool)f7_result1) {
            TAUL_LOG(lgr, "result:\n{}", f7_result1->fmt_tree());
            EXPECT_TRUE(f7_expect1.equiv(*f7_result1));
            EXPECT_EQ(f7_expect1.str(), f7_result1->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("a", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex1("a", lgr), taul::token(lpr1, ""));
    //EXPECT_EQ(lex2("a", lgr), taul::token(lpr2, ""));
    //EXPECT_EQ(lex3("a", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex4("a", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex5("a", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex6("a", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex7("a", lgr), taul::token(lpr7, ""));

    {
        std::string src = "a";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f1_expect0 = nc->create(f1, "", 0);
        auto f2_expect0 = nc->create(f2, "", 0);
        auto f7_expect0 = nc->create(f7, "", 0);

        auto f1_expect1 = nc->create(f1, "", 0);
        auto f2_expect1 = nc->create(f2, "", 0);
        auto f7_expect1 = nc->create(f7, "", 0);

        auto f0_result0 = psr0(*nc, tkns, lgr);
        auto f1_result0 = psr1(*nc, tkns, lgr);
        auto f2_result0 = psr2(*nc, tkns, lgr);
        auto f3_result0 = psr3(*nc, tkns, lgr);
        auto f4_result0 = psr4(*nc, tkns, lgr);
        auto f5_result0 = psr5(*nc, tkns, lgr);
        auto f6_result0 = psr6(*nc, tkns, lgr);
        auto f7_result0 = psr7(*nc, tkns, lgr);

        auto f0_result1 = psr0(*nc, tkns, 0, lgr);
        auto f1_result1 = psr1(*nc, tkns, 0, lgr);
        auto f2_result1 = psr2(*nc, tkns, 0, lgr);
        auto f3_result1 = psr3(*nc, tkns, 0, lgr);
        auto f4_result1 = psr4(*nc, tkns, 0, lgr);
        auto f5_result1 = psr5(*nc, tkns, 0, lgr);
        auto f6_result1 = psr6(*nc, tkns, 0, lgr);
        auto f7_result1 = psr7(*nc, tkns, 0, lgr);

        EXPECT_FALSE(f0_result0);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        EXPECT_FALSE(f3_result0);
        EXPECT_FALSE(f4_result0);
        EXPECT_FALSE(f5_result0);
        EXPECT_FALSE(f6_result0);

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();

        EXPECT_FALSE(f0_result1);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect1.fmt_tree());
        if ((bool)f2_result1) {
            TAUL_LOG(lgr, "result:\n{}", f2_result1->fmt_tree());
            EXPECT_TRUE(f2_expect1.equiv(*f2_result1));
            EXPECT_EQ(f2_expect1.str(), f2_result1->str());
        }
        else ADD_FAILURE();

        EXPECT_FALSE(f3_result1);
        EXPECT_FALSE(f4_result1);
        EXPECT_FALSE(f5_result1);
        EXPECT_FALSE(f6_result1);

        TAUL_LOG(lgr, "expected:\n{}", f7_expect1.fmt_tree());
        if ((bool)f7_result1) {
            TAUL_LOG(lgr, "result:\n{}", f7_result1->fmt_tree());
            EXPECT_TRUE(f7_expect1.equiv(*f7_result1));
            EXPECT_EQ(f7_expect1.str(), f7_result1->str());
        }
        else ADD_FAILURE();
    }
    
    //EXPECT_EQ(lex0("ab", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("ab", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("ab", lgr), taul::token(lpr2, "ab"));
    //EXPECT_EQ(lex3("ab", lgr), taul::token(lpr3, "ab"));
    //EXPECT_EQ(lex4("ab", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex5("ab", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex6("ab", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex7("ab", lgr), taul::token(lpr7, "ab"));

    {
        std::string src = "ab";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 0);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect0 = nc->create(f1, "ab", 0);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect0 = nc->create(f2, "ab", 0);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f3_expect0 = nc->create(f3, "ab", 0);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f7_expect0 = nc->create(f7, "ab", 0);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 1)));

        auto f0_expect1 = nc->create(f0, "ab", 0);
        f0_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect1 = nc->create(f1, "ab", 0);
        f1_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect1 = nc->create(f2, "ab", 0);
        f2_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f3_expect1 = nc->create(f3, "ab", 0);
        f3_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f7_expect1 = nc->create(f7, "ab", 0);
        f7_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 1)));

        auto f0_result0 = psr0(*nc, tkns, lgr);
        auto f1_result0 = psr1(*nc, tkns, lgr);
        auto f2_result0 = psr2(*nc, tkns, lgr);
        auto f3_result0 = psr3(*nc, tkns, lgr);
        auto f4_result0 = psr4(*nc, tkns, lgr);
        auto f5_result0 = psr5(*nc, tkns, lgr);
        auto f6_result0 = psr6(*nc, tkns, lgr);
        auto f7_result0 = psr7(*nc, tkns, lgr);

        auto f0_result1 = psr0(*nc, tkns, 0, lgr);
        auto f1_result1 = psr1(*nc, tkns, 0, lgr);
        auto f2_result1 = psr2(*nc, tkns, 0, lgr);
        auto f3_result1 = psr3(*nc, tkns, 0, lgr);
        auto f4_result1 = psr4(*nc, tkns, 0, lgr);
        auto f5_result1 = psr5(*nc, tkns, 0, lgr);
        auto f6_result1 = psr6(*nc, tkns, 0, lgr);
        auto f7_result1 = psr7(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        EXPECT_FALSE(f4_result0);
        EXPECT_FALSE(f5_result0);
        EXPECT_FALSE(f6_result0);

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect1.fmt_tree());
        if ((bool)f2_result1) {
            TAUL_LOG(lgr, "result:\n{}", f2_result1->fmt_tree());
            EXPECT_TRUE(f2_expect1.equiv(*f2_result1));
            EXPECT_EQ(f2_expect1.str(), f2_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect1.fmt_tree());
        if ((bool)f3_result1) {
            TAUL_LOG(lgr, "result:\n{}", f3_result1->fmt_tree());
            EXPECT_TRUE(f3_expect1.equiv(*f3_result1));
            EXPECT_EQ(f3_expect1.str(), f3_result1->str());
        }
        else ADD_FAILURE();

        EXPECT_FALSE(f4_result1);
        EXPECT_FALSE(f5_result1);
        EXPECT_FALSE(f6_result1);

        TAUL_LOG(lgr, "expected:\n{}", f7_expect1.fmt_tree());
        if ((bool)f7_result1) {
            TAUL_LOG(lgr, "result:\n{}", f7_result1->fmt_tree());
            EXPECT_TRUE(f7_expect1.equiv(*f7_result1));
            EXPECT_EQ(f7_expect1.str(), f7_result1->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("aba", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("aba", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("aba", lgr), taul::token(lpr2, "ab"));
    //EXPECT_EQ(lex3("aba", lgr), taul::token(lpr3, "ab"));
    //EXPECT_EQ(lex4("aba", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex5("aba", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex6("aba", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex7("aba", lgr), taul::token(lpr7, "ab"));

    {
        std::string src = "aba";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 0);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect0 = nc->create(f1, "ab", 0);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect0 = nc->create(f2, "ab", 0);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f3_expect0 = nc->create(f3, "ab", 0);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f7_expect0 = nc->create(f7, "ab", 0);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 1)));

        auto f0_expect1 = nc->create(f0, "ab", 0);
        f0_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect1 = nc->create(f1, "ab", 0);
        f1_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect1 = nc->create(f2, "ab", 0);
        f2_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f3_expect1 = nc->create(f3, "ab", 0);
        f3_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f7_expect1 = nc->create(f7, "ab", 0);
        f7_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 1)));

        auto f0_result0 = psr0(*nc, tkns, lgr);
        auto f1_result0 = psr1(*nc, tkns, lgr);
        auto f2_result0 = psr2(*nc, tkns, lgr);
        auto f3_result0 = psr3(*nc, tkns, lgr);
        auto f4_result0 = psr4(*nc, tkns, lgr);
        auto f5_result0 = psr5(*nc, tkns, lgr);
        auto f6_result0 = psr6(*nc, tkns, lgr);
        auto f7_result0 = psr7(*nc, tkns, lgr);

        auto f0_result1 = psr0(*nc, tkns, 0, lgr);
        auto f1_result1 = psr1(*nc, tkns, 0, lgr);
        auto f2_result1 = psr2(*nc, tkns, 0, lgr);
        auto f3_result1 = psr3(*nc, tkns, 0, lgr);
        auto f4_result1 = psr4(*nc, tkns, 0, lgr);
        auto f5_result1 = psr5(*nc, tkns, 0, lgr);
        auto f6_result1 = psr6(*nc, tkns, 0, lgr);
        auto f7_result1 = psr7(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        EXPECT_FALSE(f4_result0);
        EXPECT_FALSE(f5_result0);
        EXPECT_FALSE(f6_result0);

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect1.fmt_tree());
        if ((bool)f2_result1) {
            TAUL_LOG(lgr, "result:\n{}", f2_result1->fmt_tree());
            EXPECT_TRUE(f2_expect1.equiv(*f2_result1));
            EXPECT_EQ(f2_expect1.str(), f2_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect1.fmt_tree());
        if ((bool)f3_result1) {
            TAUL_LOG(lgr, "result:\n{}", f3_result1->fmt_tree());
            EXPECT_TRUE(f3_expect1.equiv(*f3_result1));
            EXPECT_EQ(f3_expect1.str(), f3_result1->str());
        }
        else ADD_FAILURE();

        EXPECT_FALSE(f4_result1);
        EXPECT_FALSE(f5_result1);
        EXPECT_FALSE(f6_result1);

        TAUL_LOG(lgr, "expected:\n{}", f7_expect1.fmt_tree());
        if ((bool)f7_result1) {
            TAUL_LOG(lgr, "result:\n{}", f7_result1->fmt_tree());
            EXPECT_TRUE(f7_expect1.equiv(*f7_result1));
            EXPECT_EQ(f7_expect1.str(), f7_result1->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("abab", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("abab", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("abab", lgr), taul::token(lpr2, "abab"));
    //EXPECT_EQ(lex3("abab", lgr), taul::token(lpr3, "abab"));
    //EXPECT_EQ(lex4("abab", lgr), taul::token(lpr4, "abab"));
    //EXPECT_EQ(lex5("abab", lgr), taul::token(lpr5, "abab"));
    //EXPECT_EQ(lex6("abab", lgr), taul::token(lpr6, "abab"));
    //EXPECT_EQ(lex7("abab", lgr), taul::token(lpr7, "abab"));

    {
        std::string src = "abab";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 0);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect0 = nc->create(f1, "ab", 0);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect0 = nc->create(f2, "abab", 0);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        auto f3_expect0 = nc->create(f3, "abab", 0);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        auto f4_expect0 = nc->create(f4, "abab", 0);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f4_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        auto f5_expect0 = nc->create(f5, "abab", 0);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        auto f6_expect0 = nc->create(f6, "abab", 0);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        auto f7_expect0 = nc->create(f7, "abab", 0);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 3)));

        auto f0_expect1 = nc->create(f0, "ab", 0);
        f0_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect1 = nc->create(f1, "ab", 0);
        f1_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect1 = nc->create(f2, "abab", 0);
        f2_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f2_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        auto f3_expect1 = nc->create(f3, "abab", 0);
        f3_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f3_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        auto f4_expect1 = nc->create(f4, "abab", 0);
        f4_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f4_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f4_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f4_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        auto f5_expect1 = nc->create(f5, "abab", 0);
        f5_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f5_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        auto f6_expect1 = nc->create(f6, "abab", 0);
        f6_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f6_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        auto f7_expect1 = nc->create(f7, "abab", 0);
        f7_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f7_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 3)));

        auto f0_result0 = psr0(*nc, tkns, lgr);
        auto f1_result0 = psr1(*nc, tkns, lgr);
        auto f2_result0 = psr2(*nc, tkns, lgr);
        auto f3_result0 = psr3(*nc, tkns, lgr);
        auto f4_result0 = psr4(*nc, tkns, lgr);
        auto f5_result0 = psr5(*nc, tkns, lgr);
        auto f6_result0 = psr6(*nc, tkns, lgr);
        auto f7_result0 = psr7(*nc, tkns, lgr);

        auto f0_result1 = psr0(*nc, tkns, 0, lgr);
        auto f1_result1 = psr1(*nc, tkns, 0, lgr);
        auto f2_result1 = psr2(*nc, tkns, 0, lgr);
        auto f3_result1 = psr3(*nc, tkns, 0, lgr);
        auto f4_result1 = psr4(*nc, tkns, 0, lgr);
        auto f5_result1 = psr5(*nc, tkns, 0, lgr);
        auto f6_result1 = psr6(*nc, tkns, 0, lgr);
        auto f7_result1 = psr7(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();
        
        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();
        
        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();
        
        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect1.fmt_tree());
        if ((bool)f2_result1) {
            TAUL_LOG(lgr, "result:\n{}", f2_result1->fmt_tree());
            EXPECT_TRUE(f2_expect1.equiv(*f2_result1));
            EXPECT_EQ(f2_expect1.str(), f2_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect1.fmt_tree());
        if ((bool)f3_result1) {
            TAUL_LOG(lgr, "result:\n{}", f3_result1->fmt_tree());
            EXPECT_TRUE(f3_expect1.equiv(*f3_result1));
            EXPECT_EQ(f3_expect1.str(), f3_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect1.fmt_tree());
        if ((bool)f4_result1) {
            TAUL_LOG(lgr, "result:\n{}", f4_result1->fmt_tree());
            EXPECT_TRUE(f4_expect1.equiv(*f4_result1));
            EXPECT_EQ(f4_expect1.str(), f4_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect1.fmt_tree());
        if ((bool)f5_result1) {
            TAUL_LOG(lgr, "result:\n{}", f5_result1->fmt_tree());
            EXPECT_TRUE(f5_expect1.equiv(*f5_result1));
            EXPECT_EQ(f5_expect1.str(), f5_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect1.fmt_tree());
        if ((bool)f6_result1) {
            TAUL_LOG(lgr, "result:\n{}", f6_result1->fmt_tree());
            EXPECT_TRUE(f6_expect1.equiv(*f6_result1));
            EXPECT_EQ(f6_expect1.str(), f6_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect1.fmt_tree());
        if ((bool)f7_result1) {
            TAUL_LOG(lgr, "result:\n{}", f7_result1->fmt_tree());
            EXPECT_TRUE(f7_expect1.equiv(*f7_result1));
            EXPECT_EQ(f7_expect1.str(), f7_result1->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("ababa", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("ababa", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("ababa", lgr), taul::token(lpr2, "abab"));
    //EXPECT_EQ(lex3("ababa", lgr), taul::token(lpr3, "abab"));
    //EXPECT_EQ(lex4("ababa", lgr), taul::token(lpr4, "abab"));
    //EXPECT_EQ(lex5("ababa", lgr), taul::token(lpr5, "abab"));
    //EXPECT_EQ(lex6("ababa", lgr), taul::token(lpr6, "abab"));
    //EXPECT_EQ(lex7("ababa", lgr), taul::token(lpr7, "abab"));

    {
        std::string src = "ababa";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 0);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect0 = nc->create(f1, "ab", 0);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect0 = nc->create(f2, "abab", 0);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        auto f3_expect0 = nc->create(f3, "abab", 0);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        auto f4_expect0 = nc->create(f4, "abab", 0);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f4_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        auto f5_expect0 = nc->create(f5, "abab", 0);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        auto f6_expect0 = nc->create(f6, "abab", 0);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        auto f7_expect0 = nc->create(f7, "abab", 0);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 3)));

        auto f0_expect1 = nc->create(f0, "ab", 0);
        f0_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect1 = nc->create(f1, "ab", 0);
        f1_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect1 = nc->create(f2, "abab", 0);
        f2_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f2_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        auto f3_expect1 = nc->create(f3, "abab", 0);
        f3_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f3_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        auto f4_expect1 = nc->create(f4, "abab", 0);
        f4_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f4_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f4_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f4_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        auto f5_expect1 = nc->create(f5, "abab", 0);
        f5_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f5_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        auto f6_expect1 = nc->create(f6, "abab", 0);
        f6_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f6_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        auto f7_expect1 = nc->create(f7, "abab", 0);
        f7_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f7_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 3)));

        auto f0_result0 = psr0(*nc, tkns, lgr);
        auto f1_result0 = psr1(*nc, tkns, lgr);
        auto f2_result0 = psr2(*nc, tkns, lgr);
        auto f3_result0 = psr3(*nc, tkns, lgr);
        auto f4_result0 = psr4(*nc, tkns, lgr);
        auto f5_result0 = psr5(*nc, tkns, lgr);
        auto f6_result0 = psr6(*nc, tkns, lgr);
        auto f7_result0 = psr7(*nc, tkns, lgr);

        auto f0_result1 = psr0(*nc, tkns, 0, lgr);
        auto f1_result1 = psr1(*nc, tkns, 0, lgr);
        auto f2_result1 = psr2(*nc, tkns, 0, lgr);
        auto f3_result1 = psr3(*nc, tkns, 0, lgr);
        auto f4_result1 = psr4(*nc, tkns, 0, lgr);
        auto f5_result1 = psr5(*nc, tkns, 0, lgr);
        auto f6_result1 = psr6(*nc, tkns, 0, lgr);
        auto f7_result1 = psr7(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect1.fmt_tree());
        if ((bool)f2_result1) {
            TAUL_LOG(lgr, "result:\n{}", f2_result1->fmt_tree());
            EXPECT_TRUE(f2_expect1.equiv(*f2_result1));
            EXPECT_EQ(f2_expect1.str(), f2_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect1.fmt_tree());
        if ((bool)f3_result1) {
            TAUL_LOG(lgr, "result:\n{}", f3_result1->fmt_tree());
            EXPECT_TRUE(f3_expect1.equiv(*f3_result1));
            EXPECT_EQ(f3_expect1.str(), f3_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect1.fmt_tree());
        if ((bool)f4_result1) {
            TAUL_LOG(lgr, "result:\n{}", f4_result1->fmt_tree());
            EXPECT_TRUE(f4_expect1.equiv(*f4_result1));
            EXPECT_EQ(f4_expect1.str(), f4_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect1.fmt_tree());
        if ((bool)f5_result1) {
            TAUL_LOG(lgr, "result:\n{}", f5_result1->fmt_tree());
            EXPECT_TRUE(f5_expect1.equiv(*f5_result1));
            EXPECT_EQ(f5_expect1.str(), f5_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect1.fmt_tree());
        if ((bool)f6_result1) {
            TAUL_LOG(lgr, "result:\n{}", f6_result1->fmt_tree());
            EXPECT_TRUE(f6_expect1.equiv(*f6_result1));
            EXPECT_EQ(f6_expect1.str(), f6_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect1.fmt_tree());
        if ((bool)f7_result1) {
            TAUL_LOG(lgr, "result:\n{}", f7_result1->fmt_tree());
            EXPECT_TRUE(f7_expect1.equiv(*f7_result1));
            EXPECT_EQ(f7_expect1.str(), f7_result1->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("ababab", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("ababab", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("ababab", lgr), taul::token(lpr2, "ababab"));
    //EXPECT_EQ(lex3("ababab", lgr), taul::token(lpr3, "ababab"));
    //EXPECT_EQ(lex4("ababab", lgr), taul::token(lpr4, "abab"));
    //EXPECT_EQ(lex5("ababab", lgr), taul::token(lpr5, "ababab"));
    //EXPECT_EQ(lex6("ababab", lgr), taul::token(lpr6, "ababab"));
    //EXPECT_EQ(lex7("ababab", lgr), taul::token(lpr7, "ababab"));

    {
        std::string src = "ababab";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 0);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect0 = nc->create(f1, "ab", 0);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect0 = nc->create(f2, "ababab", 0);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        auto f3_expect0 = nc->create(f3, "ababab", 0);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        auto f4_expect0 = nc->create(f4, "abab", 0);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f4_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        auto f5_expect0 = nc->create(f5, "ababab", 0);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        auto f6_expect0 = nc->create(f6, "ababab", 0);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        auto f7_expect0 = nc->create(f7, "ababab", 0);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 5)));

        auto f0_expect1 = nc->create(f0, "ab", 0);
        f0_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect1 = nc->create(f1, "ab", 0);
        f1_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect1 = nc->create(f2, "ababab", 0);
        f2_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f2_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f2_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        auto f3_expect1 = nc->create(f3, "ababab", 0);
        f3_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f3_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f3_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        auto f4_expect1 = nc->create(f4, "abab", 0);
        f4_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f4_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f4_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f4_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        auto f5_expect1 = nc->create(f5, "ababab", 0);
        f5_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f5_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f5_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        auto f6_expect1 = nc->create(f6, "ababab", 0);
        f6_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f6_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f6_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        auto f7_expect1 = nc->create(f7, "ababab", 0);
        f7_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f7_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f7_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 5)));

        auto f0_result0 = psr0(*nc, tkns, lgr);
        auto f1_result0 = psr1(*nc, tkns, lgr);
        auto f2_result0 = psr2(*nc, tkns, lgr);
        auto f3_result0 = psr3(*nc, tkns, lgr);
        auto f4_result0 = psr4(*nc, tkns, lgr);
        auto f5_result0 = psr5(*nc, tkns, lgr);
        auto f6_result0 = psr6(*nc, tkns, lgr);
        auto f7_result0 = psr7(*nc, tkns, lgr);

        auto f0_result1 = psr0(*nc, tkns, 0, lgr);
        auto f1_result1 = psr1(*nc, tkns, 0, lgr);
        auto f2_result1 = psr2(*nc, tkns, 0, lgr);
        auto f3_result1 = psr3(*nc, tkns, 0, lgr);
        auto f4_result1 = psr4(*nc, tkns, 0, lgr);
        auto f5_result1 = psr5(*nc, tkns, 0, lgr);
        auto f6_result1 = psr6(*nc, tkns, 0, lgr);
        auto f7_result1 = psr7(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect1.fmt_tree());
        if ((bool)f2_result1) {
            TAUL_LOG(lgr, "result:\n{}", f2_result1->fmt_tree());
            EXPECT_TRUE(f2_expect1.equiv(*f2_result1));
            EXPECT_EQ(f2_expect1.str(), f2_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect1.fmt_tree());
        if ((bool)f3_result1) {
            TAUL_LOG(lgr, "result:\n{}", f3_result1->fmt_tree());
            EXPECT_TRUE(f3_expect1.equiv(*f3_result1));
            EXPECT_EQ(f3_expect1.str(), f3_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect1.fmt_tree());
        if ((bool)f4_result1) {
            TAUL_LOG(lgr, "result:\n{}", f4_result1->fmt_tree());
            EXPECT_TRUE(f4_expect1.equiv(*f4_result1));
            EXPECT_EQ(f4_expect1.str(), f4_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect1.fmt_tree());
        if ((bool)f5_result1) {
            TAUL_LOG(lgr, "result:\n{}", f5_result1->fmt_tree());
            EXPECT_TRUE(f5_expect1.equiv(*f5_result1));
            EXPECT_EQ(f5_expect1.str(), f5_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect1.fmt_tree());
        if ((bool)f6_result1) {
            TAUL_LOG(lgr, "result:\n{}", f6_result1->fmt_tree());
            EXPECT_TRUE(f6_expect1.equiv(*f6_result1));
            EXPECT_EQ(f6_expect1.str(), f6_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect1.fmt_tree());
        if ((bool)f7_result1) {
            TAUL_LOG(lgr, "result:\n{}", f7_result1->fmt_tree());
            EXPECT_TRUE(f7_expect1.equiv(*f7_result1));
            EXPECT_EQ(f7_expect1.str(), f7_result1->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("abababa", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("abababa", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("abababa", lgr), taul::token(lpr2, "ababab"));
    //EXPECT_EQ(lex3("abababa", lgr), taul::token(lpr3, "ababab"));
    //EXPECT_EQ(lex4("abababa", lgr), taul::token(lpr4, "abab"));
    //EXPECT_EQ(lex5("abababa", lgr), taul::token(lpr5, "ababab"));
    //EXPECT_EQ(lex6("abababa", lgr), taul::token(lpr6, "ababab"));
    //EXPECT_EQ(lex7("abababa", lgr), taul::token(lpr7, "ababab"));

    {
        std::string src = "abababa";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 0);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect0 = nc->create(f1, "ab", 0);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect0 = nc->create(f2, "ababab", 0);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        auto f3_expect0 = nc->create(f3, "ababab", 0);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        auto f4_expect0 = nc->create(f4, "abab", 0);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f4_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        auto f5_expect0 = nc->create(f5, "ababab", 0);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        auto f6_expect0 = nc->create(f6, "ababab", 0);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        auto f7_expect0 = nc->create(f7, "ababab", 0);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 5)));

        auto f0_expect1 = nc->create(f0, "ab", 0);
        f0_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect1 = nc->create(f1, "ab", 0);
        f1_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect1 = nc->create(f2, "ababab", 0);
        f2_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f2_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f2_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        auto f3_expect1 = nc->create(f3, "ababab", 0);
        f3_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f3_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f3_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        auto f4_expect1 = nc->create(f4, "abab", 0);
        f4_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f4_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f4_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f4_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        auto f5_expect1 = nc->create(f5, "ababab", 0);
        f5_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f5_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f5_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        auto f6_expect1 = nc->create(f6, "ababab", 0);
        f6_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f6_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f6_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        auto f7_expect1 = nc->create(f7, "ababab", 0);
        f7_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f7_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f7_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 5)));

        auto f0_result0 = psr0(*nc, tkns, lgr);
        auto f1_result0 = psr1(*nc, tkns, lgr);
        auto f2_result0 = psr2(*nc, tkns, lgr);
        auto f3_result0 = psr3(*nc, tkns, lgr);
        auto f4_result0 = psr4(*nc, tkns, lgr);
        auto f5_result0 = psr5(*nc, tkns, lgr);
        auto f6_result0 = psr6(*nc, tkns, lgr);
        auto f7_result0 = psr7(*nc, tkns, lgr);

        auto f0_result1 = psr0(*nc, tkns, 0, lgr);
        auto f1_result1 = psr1(*nc, tkns, 0, lgr);
        auto f2_result1 = psr2(*nc, tkns, 0, lgr);
        auto f3_result1 = psr3(*nc, tkns, 0, lgr);
        auto f4_result1 = psr4(*nc, tkns, 0, lgr);
        auto f5_result1 = psr5(*nc, tkns, 0, lgr);
        auto f6_result1 = psr6(*nc, tkns, 0, lgr);
        auto f7_result1 = psr7(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect1.fmt_tree());
        if ((bool)f2_result1) {
            TAUL_LOG(lgr, "result:\n{}", f2_result1->fmt_tree());
            EXPECT_TRUE(f2_expect1.equiv(*f2_result1));
            EXPECT_EQ(f2_expect1.str(), f2_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect1.fmt_tree());
        if ((bool)f3_result1) {
            TAUL_LOG(lgr, "result:\n{}", f3_result1->fmt_tree());
            EXPECT_TRUE(f3_expect1.equiv(*f3_result1));
            EXPECT_EQ(f3_expect1.str(), f3_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect1.fmt_tree());
        if ((bool)f4_result1) {
            TAUL_LOG(lgr, "result:\n{}", f4_result1->fmt_tree());
            EXPECT_TRUE(f4_expect1.equiv(*f4_result1));
            EXPECT_EQ(f4_expect1.str(), f4_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect1.fmt_tree());
        if ((bool)f5_result1) {
            TAUL_LOG(lgr, "result:\n{}", f5_result1->fmt_tree());
            EXPECT_TRUE(f5_expect1.equiv(*f5_result1));
            EXPECT_EQ(f5_expect1.str(), f5_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect1.fmt_tree());
        if ((bool)f6_result1) {
            TAUL_LOG(lgr, "result:\n{}", f6_result1->fmt_tree());
            EXPECT_TRUE(f6_expect1.equiv(*f6_result1));
            EXPECT_EQ(f6_expect1.str(), f6_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect1.fmt_tree());
        if ((bool)f7_result1) {
            TAUL_LOG(lgr, "result:\n{}", f7_result1->fmt_tree());
            EXPECT_TRUE(f7_expect1.equiv(*f7_result1));
            EXPECT_EQ(f7_expect1.str(), f7_result1->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("abababab", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("abababab", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("abababab", lgr), taul::token(lpr2, "abababab"));
    //EXPECT_EQ(lex3("abababab", lgr), taul::token(lpr3, "abababab"));
    //EXPECT_EQ(lex4("abababab", lgr), taul::token(lpr4, "abab"));
    //EXPECT_EQ(lex5("abababab", lgr), taul::token(lpr5, "abababab"));
    //EXPECT_EQ(lex6("abababab", lgr), taul::token(lpr6, "abababab"));
    //EXPECT_EQ(lex7("abababab", lgr), taul::token(lpr7, "abababab"));

    {
        std::string src = "abababab";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 0);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect0 = nc->create(f1, "ab", 0);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect0 = nc->create(f2, "abababab", 0);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 6)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 7)));
        auto f3_expect0 = nc->create(f3, "abababab", 0);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 6)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 7)));
        auto f4_expect0 = nc->create(f4, "abab", 0);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f4_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        auto f5_expect0 = nc->create(f5, "abababab", 0);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 6)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 7)));
        auto f6_expect0 = nc->create(f6, "abababab", 0);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 6)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 7)));
        auto f7_expect0 = nc->create(f7, "abababab", 0);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 6)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 7)));

        auto f0_expect1 = nc->create(f0, "ab", 0);
        f0_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect1 = nc->create(f1, "ab", 0);
        f1_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect1 = nc->create(f2, "abababab", 0);
        f2_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f2_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f2_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        f2_expect1.attach(nc->create(taul::token(chr, "a", 6)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 7)));
        auto f3_expect1 = nc->create(f3, "abababab", 0);
        f3_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f3_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f3_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        f3_expect1.attach(nc->create(taul::token(chr, "a", 6)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 7)));
        auto f4_expect1 = nc->create(f4, "abab", 0);
        f4_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f4_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f4_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f4_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        auto f5_expect1 = nc->create(f5, "abababab", 0);
        f5_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f5_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f5_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        f5_expect1.attach(nc->create(taul::token(chr, "a", 6)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 7)));
        auto f6_expect1 = nc->create(f6, "abababab", 0);
        f6_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f6_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f6_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        f6_expect1.attach(nc->create(taul::token(chr, "a", 6)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 7)));
        auto f7_expect1 = nc->create(f7, "abababab", 0);
        f7_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f7_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f7_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        f7_expect1.attach(nc->create(taul::token(chr, "a", 6)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 7)));

        auto f0_result0 = psr0(*nc, tkns, lgr);
        auto f1_result0 = psr1(*nc, tkns, lgr);
        auto f2_result0 = psr2(*nc, tkns, lgr);
        auto f3_result0 = psr3(*nc, tkns, lgr);
        auto f4_result0 = psr4(*nc, tkns, lgr);
        auto f5_result0 = psr5(*nc, tkns, lgr);
        auto f6_result0 = psr6(*nc, tkns, lgr);
        auto f7_result0 = psr7(*nc, tkns, lgr);

        auto f0_result1 = psr0(*nc, tkns, 0, lgr);
        auto f1_result1 = psr1(*nc, tkns, 0, lgr);
        auto f2_result1 = psr2(*nc, tkns, 0, lgr);
        auto f3_result1 = psr3(*nc, tkns, 0, lgr);
        auto f4_result1 = psr4(*nc, tkns, 0, lgr);
        auto f5_result1 = psr5(*nc, tkns, 0, lgr);
        auto f6_result1 = psr6(*nc, tkns, 0, lgr);
        auto f7_result1 = psr7(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect1.fmt_tree());
        if ((bool)f2_result1) {
            TAUL_LOG(lgr, "result:\n{}", f2_result1->fmt_tree());
            EXPECT_TRUE(f2_expect1.equiv(*f2_result1));
            EXPECT_EQ(f2_expect1.str(), f2_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect1.fmt_tree());
        if ((bool)f3_result1) {
            TAUL_LOG(lgr, "result:\n{}", f3_result1->fmt_tree());
            EXPECT_TRUE(f3_expect1.equiv(*f3_result1));
            EXPECT_EQ(f3_expect1.str(), f3_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect1.fmt_tree());
        if ((bool)f4_result1) {
            TAUL_LOG(lgr, "result:\n{}", f4_result1->fmt_tree());
            EXPECT_TRUE(f4_expect1.equiv(*f4_result1));
            EXPECT_EQ(f4_expect1.str(), f4_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect1.fmt_tree());
        if ((bool)f5_result1) {
            TAUL_LOG(lgr, "result:\n{}", f5_result1->fmt_tree());
            EXPECT_TRUE(f5_expect1.equiv(*f5_result1));
            EXPECT_EQ(f5_expect1.str(), f5_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect1.fmt_tree());
        if ((bool)f6_result1) {
            TAUL_LOG(lgr, "result:\n{}", f6_result1->fmt_tree());
            EXPECT_TRUE(f6_expect1.equiv(*f6_result1));
            EXPECT_EQ(f6_expect1.str(), f6_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect1.fmt_tree());
        if ((bool)f7_result1) {
            TAUL_LOG(lgr, "result:\n{}", f7_result1->fmt_tree());
            EXPECT_TRUE(f7_expect1.equiv(*f7_result1));
            EXPECT_EQ(f7_expect1.str(), f7_result1->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("ababababa", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("ababababa", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("ababababa", lgr), taul::token(lpr2, "abababab"));
    //EXPECT_EQ(lex3("ababababa", lgr), taul::token(lpr3, "abababab"));
    //EXPECT_EQ(lex4("ababababa", lgr), taul::token(lpr4, "abab"));
    //EXPECT_EQ(lex5("ababababa", lgr), taul::token(lpr5, "abababab"));
    //EXPECT_EQ(lex6("ababababa", lgr), taul::token(lpr6, "abababab"));
    //EXPECT_EQ(lex7("ababababa", lgr), taul::token(lpr7, "abababab"));

    {
        std::string src = "ababababa";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 0);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect0 = nc->create(f1, "ab", 0);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect0 = nc->create(f2, "abababab", 0);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 6)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 7)));
        auto f3_expect0 = nc->create(f3, "abababab", 0);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 6)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 7)));
        auto f4_expect0 = nc->create(f4, "abab", 0);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f4_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        auto f5_expect0 = nc->create(f5, "abababab", 0);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 6)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 7)));
        auto f6_expect0 = nc->create(f6, "abababab", 0);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 6)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 7)));
        auto f7_expect0 = nc->create(f7, "abababab", 0);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 6)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 7)));

        auto f0_expect1 = nc->create(f0, "ab", 0);
        f0_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect1 = nc->create(f1, "ab", 0);
        f1_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect1 = nc->create(f2, "abababab", 0);
        f2_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f2_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f2_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        f2_expect1.attach(nc->create(taul::token(chr, "a", 6)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 7)));
        auto f3_expect1 = nc->create(f3, "abababab", 0);
        f3_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f3_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f3_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        f3_expect1.attach(nc->create(taul::token(chr, "a", 6)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 7)));
        auto f4_expect1 = nc->create(f4, "abab", 0);
        f4_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f4_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f4_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f4_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        auto f5_expect1 = nc->create(f5, "abababab", 0);
        f5_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f5_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f5_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        f5_expect1.attach(nc->create(taul::token(chr, "a", 6)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 7)));
        auto f6_expect1 = nc->create(f6, "abababab", 0);
        f6_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f6_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f6_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        f6_expect1.attach(nc->create(taul::token(chr, "a", 6)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 7)));
        auto f7_expect1 = nc->create(f7, "abababab", 0);
        f7_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f7_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f7_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        f7_expect1.attach(nc->create(taul::token(chr, "a", 6)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 7)));

        auto f0_result0 = psr0(*nc, tkns, lgr);
        auto f1_result0 = psr1(*nc, tkns, lgr);
        auto f2_result0 = psr2(*nc, tkns, lgr);
        auto f3_result0 = psr3(*nc, tkns, lgr);
        auto f4_result0 = psr4(*nc, tkns, lgr);
        auto f5_result0 = psr5(*nc, tkns, lgr);
        auto f6_result0 = psr6(*nc, tkns, lgr);
        auto f7_result0 = psr7(*nc, tkns, lgr);

        auto f0_result1 = psr0(*nc, tkns, 0, lgr);
        auto f1_result1 = psr1(*nc, tkns, 0, lgr);
        auto f2_result1 = psr2(*nc, tkns, 0, lgr);
        auto f3_result1 = psr3(*nc, tkns, 0, lgr);
        auto f4_result1 = psr4(*nc, tkns, 0, lgr);
        auto f5_result1 = psr5(*nc, tkns, 0, lgr);
        auto f6_result1 = psr6(*nc, tkns, 0, lgr);
        auto f7_result1 = psr7(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect1.fmt_tree());
        if ((bool)f2_result1) {
            TAUL_LOG(lgr, "result:\n{}", f2_result1->fmt_tree());
            EXPECT_TRUE(f2_expect1.equiv(*f2_result1));
            EXPECT_EQ(f2_expect1.str(), f2_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect1.fmt_tree());
        if ((bool)f3_result1) {
            TAUL_LOG(lgr, "result:\n{}", f3_result1->fmt_tree());
            EXPECT_TRUE(f3_expect1.equiv(*f3_result1));
            EXPECT_EQ(f3_expect1.str(), f3_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect1.fmt_tree());
        if ((bool)f4_result1) {
            TAUL_LOG(lgr, "result:\n{}", f4_result1->fmt_tree());
            EXPECT_TRUE(f4_expect1.equiv(*f4_result1));
            EXPECT_EQ(f4_expect1.str(), f4_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect1.fmt_tree());
        if ((bool)f5_result1) {
            TAUL_LOG(lgr, "result:\n{}", f5_result1->fmt_tree());
            EXPECT_TRUE(f5_expect1.equiv(*f5_result1));
            EXPECT_EQ(f5_expect1.str(), f5_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect1.fmt_tree());
        if ((bool)f6_result1) {
            TAUL_LOG(lgr, "result:\n{}", f6_result1->fmt_tree());
            EXPECT_TRUE(f6_expect1.equiv(*f6_result1));
            EXPECT_EQ(f6_expect1.str(), f6_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect1.fmt_tree());
        if ((bool)f7_result1) {
            TAUL_LOG(lgr, "result:\n{}", f7_result1->fmt_tree());
            EXPECT_TRUE(f7_expect1.equiv(*f7_result1));
            EXPECT_EQ(f7_expect1.str(), f7_result1->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("ababababab", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("ababababab", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("ababababab", lgr), taul::token(lpr2, "ababababab"));
    //EXPECT_EQ(lex3("ababababab", lgr), taul::token(lpr3, "ababababab"));
    //EXPECT_EQ(lex4("ababababab", lgr), taul::token(lpr4, "abab"));
    //EXPECT_EQ(lex5("ababababab", lgr), taul::token(lpr5, "ababababab"));
    //EXPECT_EQ(lex6("ababababab", lgr), taul::token(lpr6, "abababab"));
    //EXPECT_EQ(lex7("ababababab", lgr), taul::token(lpr7, "abababab"));

    {
        std::string src = "ababababab";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 0);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect0 = nc->create(f1, "ab", 0);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect0 = nc->create(f2, "ababababab", 0);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 6)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 7)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 8)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 9)));
        auto f3_expect0 = nc->create(f3, "ababababab", 0);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 6)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 7)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 8)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 9)));
        auto f4_expect0 = nc->create(f4, "abab", 0);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f4_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        auto f5_expect0 = nc->create(f5, "ababababab", 0);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 6)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 7)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 8)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 9)));
        auto f6_expect0 = nc->create(f6, "abababab", 0);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 6)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 7)));
        auto f7_expect0 = nc->create(f7, "abababab", 0);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 2)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 3)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 4)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 5)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 6)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 7)));

        auto f0_expect1 = nc->create(f0, "ab", 0);
        f0_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect1 = nc->create(f1, "ab", 0);
        f1_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        auto f2_expect1 = nc->create(f2, "ababababab", 0);
        f2_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f2_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f2_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        f2_expect1.attach(nc->create(taul::token(chr, "a", 6)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 7)));
        f2_expect1.attach(nc->create(taul::token(chr, "a", 8)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 9)));
        auto f3_expect1 = nc->create(f3, "ababababab", 0);
        f3_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f3_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f3_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        f3_expect1.attach(nc->create(taul::token(chr, "a", 6)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 7)));
        f3_expect1.attach(nc->create(taul::token(chr, "a", 8)));
        f3_expect1.attach(nc->create(taul::token(chr, "b", 9)));
        auto f4_expect1 = nc->create(f4, "abab", 0);
        f4_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f4_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f4_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f4_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        auto f5_expect1 = nc->create(f5, "ababababab", 0);
        f5_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f5_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f5_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        f5_expect1.attach(nc->create(taul::token(chr, "a", 6)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 7)));
        f5_expect1.attach(nc->create(taul::token(chr, "a", 8)));
        f5_expect1.attach(nc->create(taul::token(chr, "b", 9)));
        auto f6_expect1 = nc->create(f6, "abababab", 0);
        f6_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f6_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f6_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        f6_expect1.attach(nc->create(taul::token(chr, "a", 6)));
        f6_expect1.attach(nc->create(taul::token(chr, "b", 7)));
        auto f7_expect1 = nc->create(f7, "abababab", 0);
        f7_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f7_expect1.attach(nc->create(taul::token(chr, "a", 2)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 3)));
        f7_expect1.attach(nc->create(taul::token(chr, "a", 4)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 5)));
        f7_expect1.attach(nc->create(taul::token(chr, "a", 6)));
        f7_expect1.attach(nc->create(taul::token(chr, "b", 7)));

        auto f0_result0 = psr0(*nc, tkns, lgr);
        auto f1_result0 = psr1(*nc, tkns, lgr);
        auto f2_result0 = psr2(*nc, tkns, lgr);
        auto f3_result0 = psr3(*nc, tkns, lgr);
        auto f4_result0 = psr4(*nc, tkns, lgr);
        auto f5_result0 = psr5(*nc, tkns, lgr);
        auto f6_result0 = psr6(*nc, tkns, lgr);
        auto f7_result0 = psr7(*nc, tkns, lgr);

        auto f0_result1 = psr0(*nc, tkns, 0, lgr);
        auto f1_result1 = psr1(*nc, tkns, 0, lgr);
        auto f2_result1 = psr2(*nc, tkns, 0, lgr);
        auto f3_result1 = psr3(*nc, tkns, 0, lgr);
        auto f4_result1 = psr4(*nc, tkns, 0, lgr);
        auto f5_result1 = psr5(*nc, tkns, 0, lgr);
        auto f6_result1 = psr6(*nc, tkns, 0, lgr);
        auto f7_result1 = psr7(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect1.fmt_tree());
        if ((bool)f2_result1) {
            TAUL_LOG(lgr, "result:\n{}", f2_result1->fmt_tree());
            EXPECT_TRUE(f2_expect1.equiv(*f2_result1));
            EXPECT_EQ(f2_expect1.str(), f2_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect1.fmt_tree());
        if ((bool)f3_result1) {
            TAUL_LOG(lgr, "result:\n{}", f3_result1->fmt_tree());
            EXPECT_TRUE(f3_expect1.equiv(*f3_result1));
            EXPECT_EQ(f3_expect1.str(), f3_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect1.fmt_tree());
        if ((bool)f4_result1) {
            TAUL_LOG(lgr, "result:\n{}", f4_result1->fmt_tree());
            EXPECT_TRUE(f4_expect1.equiv(*f4_result1));
            EXPECT_EQ(f4_expect1.str(), f4_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect1.fmt_tree());
        if ((bool)f5_result1) {
            TAUL_LOG(lgr, "result:\n{}", f5_result1->fmt_tree());
            EXPECT_TRUE(f5_expect1.equiv(*f5_result1));
            EXPECT_EQ(f5_expect1.str(), f5_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect1.fmt_tree());
        if ((bool)f6_result1) {
            TAUL_LOG(lgr, "result:\n{}", f6_result1->fmt_tree());
            EXPECT_TRUE(f6_expect1.equiv(*f6_result1));
            EXPECT_EQ(f6_expect1.str(), f6_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect1.fmt_tree());
        if ((bool)f7_result1) {
            TAUL_LOG(lgr, "result:\n{}", f7_result1->fmt_tree());
            EXPECT_TRUE(f7_expect1.equiv(*f7_result1));
            EXPECT_EQ(f7_expect1.str(), f7_result1->str());
        }
        else ADD_FAILURE();
    }

    // test w/ offset

    // success/failure

    //EXPECT_EQ(lex0("", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex1("", lgr), taul::token(lpr1, ""));
    //EXPECT_EQ(lex2("", lgr), taul::token(lpr2, ""));
    //EXPECT_EQ(lex3("", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex4("", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex5("", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex6("", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex7("", lgr), taul::token(lpr7, ""));

    {
        std::string src = "___";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f1_expect0 = nc->create(f1, "", 3);
        auto f2_expect0 = nc->create(f2, "", 3);
        auto f7_expect0 = nc->create(f7, "", 3);

        auto f0_result0 = psr0(*nc, tkns, 3, lgr);
        auto f1_result0 = psr1(*nc, tkns, 3, lgr);
        auto f2_result0 = psr2(*nc, tkns, 3, lgr);
        auto f3_result0 = psr3(*nc, tkns, 3, lgr);
        auto f4_result0 = psr4(*nc, tkns, 3, lgr);
        auto f5_result0 = psr5(*nc, tkns, 3, lgr);
        auto f6_result0 = psr6(*nc, tkns, 3, lgr);
        auto f7_result0 = psr7(*nc, tkns, 3, lgr);

        EXPECT_FALSE(f0_result0);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        EXPECT_FALSE(f3_result0);
        EXPECT_FALSE(f4_result0);
        EXPECT_FALSE(f5_result0);
        EXPECT_FALSE(f6_result0);

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("a", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex1("a", lgr), taul::token(lpr1, ""));
    //EXPECT_EQ(lex2("a", lgr), taul::token(lpr2, ""));
    //EXPECT_EQ(lex3("a", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex4("a", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex5("a", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex6("a", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex7("a", lgr), taul::token(lpr7, ""));

    {
        std::string src = "___a";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f1_expect0 = nc->create(f1, "", 3);
        auto f2_expect0 = nc->create(f2, "", 3);
        auto f7_expect0 = nc->create(f7, "", 3);

        auto f0_result0 = psr0(*nc, tkns, 3, lgr);
        auto f1_result0 = psr1(*nc, tkns, 3, lgr);
        auto f2_result0 = psr2(*nc, tkns, 3, lgr);
        auto f3_result0 = psr3(*nc, tkns, 3, lgr);
        auto f4_result0 = psr4(*nc, tkns, 3, lgr);
        auto f5_result0 = psr5(*nc, tkns, 3, lgr);
        auto f6_result0 = psr6(*nc, tkns, 3, lgr);
        auto f7_result0 = psr7(*nc, tkns, 3, lgr);

        EXPECT_FALSE(f0_result0);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        EXPECT_FALSE(f3_result0);
        EXPECT_FALSE(f4_result0);
        EXPECT_FALSE(f5_result0);
        EXPECT_FALSE(f6_result0);

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("ab", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("ab", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("ab", lgr), taul::token(lpr2, "ab"));
    //EXPECT_EQ(lex3("ab", lgr), taul::token(lpr3, "ab"));
    //EXPECT_EQ(lex4("ab", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex5("ab", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex6("ab", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex7("ab", lgr), taul::token(lpr7, "ab"));

    {
        std::string src = "___ab";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 3);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f1_expect0 = nc->create(f1, "ab", 3);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f2_expect0 = nc->create(f2, "ab", 3);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f3_expect0 = nc->create(f3, "ab", 3);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f7_expect0 = nc->create(f7, "ab", 3);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 4)));

        auto f0_result0 = psr0(*nc, tkns, 3, lgr);
        auto f1_result0 = psr1(*nc, tkns, 3, lgr);
        auto f2_result0 = psr2(*nc, tkns, 3, lgr);
        auto f3_result0 = psr3(*nc, tkns, 3, lgr);
        auto f4_result0 = psr4(*nc, tkns, 3, lgr);
        auto f5_result0 = psr5(*nc, tkns, 3, lgr);
        auto f6_result0 = psr6(*nc, tkns, 3, lgr);
        auto f7_result0 = psr7(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        EXPECT_FALSE(f4_result0);
        EXPECT_FALSE(f5_result0);
        EXPECT_FALSE(f6_result0);

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("aba", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("aba", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("aba", lgr), taul::token(lpr2, "ab"));
    //EXPECT_EQ(lex3("aba", lgr), taul::token(lpr3, "ab"));
    //EXPECT_EQ(lex4("aba", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex5("aba", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex6("aba", lgr), taul::token::failure(""));
    //EXPECT_EQ(lex7("aba", lgr), taul::token(lpr7, "ab"));

    {
        std::string src = "___aba";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 3);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f1_expect0 = nc->create(f1, "ab", 3);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f2_expect0 = nc->create(f2, "ab", 3);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f3_expect0 = nc->create(f3, "ab", 3);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f7_expect0 = nc->create(f7, "ab", 3);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 4)));

        auto f0_result0 = psr0(*nc, tkns, 3, lgr);
        auto f1_result0 = psr1(*nc, tkns, 3, lgr);
        auto f2_result0 = psr2(*nc, tkns, 3, lgr);
        auto f3_result0 = psr3(*nc, tkns, 3, lgr);
        auto f4_result0 = psr4(*nc, tkns, 3, lgr);
        auto f5_result0 = psr5(*nc, tkns, 3, lgr);
        auto f6_result0 = psr6(*nc, tkns, 3, lgr);
        auto f7_result0 = psr7(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        EXPECT_FALSE(f4_result0);
        EXPECT_FALSE(f5_result0);
        EXPECT_FALSE(f6_result0);

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("abab", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("abab", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("abab", lgr), taul::token(lpr2, "abab"));
    //EXPECT_EQ(lex3("abab", lgr), taul::token(lpr3, "abab"));
    //EXPECT_EQ(lex4("abab", lgr), taul::token(lpr4, "abab"));
    //EXPECT_EQ(lex5("abab", lgr), taul::token(lpr5, "abab"));
    //EXPECT_EQ(lex6("abab", lgr), taul::token(lpr6, "abab"));
    //EXPECT_EQ(lex7("abab", lgr), taul::token(lpr7, "abab"));

    {
        std::string src = "___abab";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 3);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f1_expect0 = nc->create(f1, "ab", 3);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f2_expect0 = nc->create(f2, "abab", 3);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        auto f3_expect0 = nc->create(f3, "abab", 3);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        auto f4_expect0 = nc->create(f4, "abab", 3);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f4_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        auto f5_expect0 = nc->create(f5, "abab", 3);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        auto f6_expect0 = nc->create(f6, "abab", 3);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        auto f7_expect0 = nc->create(f7, "abab", 3);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 6)));

        auto f0_result0 = psr0(*nc, tkns, 3, lgr);
        auto f1_result0 = psr1(*nc, tkns, 3, lgr);
        auto f2_result0 = psr2(*nc, tkns, 3, lgr);
        auto f3_result0 = psr3(*nc, tkns, 3, lgr);
        auto f4_result0 = psr4(*nc, tkns, 3, lgr);
        auto f5_result0 = psr5(*nc, tkns, 3, lgr);
        auto f6_result0 = psr6(*nc, tkns, 3, lgr);
        auto f7_result0 = psr7(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("ababa", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("ababa", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("ababa", lgr), taul::token(lpr2, "abab"));
    //EXPECT_EQ(lex3("ababa", lgr), taul::token(lpr3, "abab"));
    //EXPECT_EQ(lex4("ababa", lgr), taul::token(lpr4, "abab"));
    //EXPECT_EQ(lex5("ababa", lgr), taul::token(lpr5, "abab"));
    //EXPECT_EQ(lex6("ababa", lgr), taul::token(lpr6, "abab"));
    //EXPECT_EQ(lex7("ababa", lgr), taul::token(lpr7, "abab"));

    {
        std::string src = "___ababa";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 3);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f1_expect0 = nc->create(f1, "ab", 3);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f2_expect0 = nc->create(f2, "abab", 3);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        auto f3_expect0 = nc->create(f3, "abab", 3);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        auto f4_expect0 = nc->create(f4, "abab", 3);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f4_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        auto f5_expect0 = nc->create(f5, "abab", 3);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        auto f6_expect0 = nc->create(f6, "abab", 3);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        auto f7_expect0 = nc->create(f7, "abab", 3);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 6)));

        auto f0_result0 = psr0(*nc, tkns, 3, lgr);
        auto f1_result0 = psr1(*nc, tkns, 3, lgr);
        auto f2_result0 = psr2(*nc, tkns, 3, lgr);
        auto f3_result0 = psr3(*nc, tkns, 3, lgr);
        auto f4_result0 = psr4(*nc, tkns, 3, lgr);
        auto f5_result0 = psr5(*nc, tkns, 3, lgr);
        auto f6_result0 = psr6(*nc, tkns, 3, lgr);
        auto f7_result0 = psr7(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("ababab", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("ababab", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("ababab", lgr), taul::token(lpr2, "ababab"));
    //EXPECT_EQ(lex3("ababab", lgr), taul::token(lpr3, "ababab"));
    //EXPECT_EQ(lex4("ababab", lgr), taul::token(lpr4, "abab"));
    //EXPECT_EQ(lex5("ababab", lgr), taul::token(lpr5, "ababab"));
    //EXPECT_EQ(lex6("ababab", lgr), taul::token(lpr6, "ababab"));
    //EXPECT_EQ(lex7("ababab", lgr), taul::token(lpr7, "ababab"));

    {
        std::string src = "___ababab";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 3);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f1_expect0 = nc->create(f1, "ab", 3);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f2_expect0 = nc->create(f2, "ababab", 3);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        auto f3_expect0 = nc->create(f3, "ababab", 3);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        auto f4_expect0 = nc->create(f4, "abab", 3);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f4_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        auto f5_expect0 = nc->create(f5, "ababab", 3);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        auto f6_expect0 = nc->create(f6, "ababab", 3);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        auto f7_expect0 = nc->create(f7, "ababab", 3);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 8)));

        auto f0_result0 = psr0(*nc, tkns, 3, lgr);
        auto f1_result0 = psr1(*nc, tkns, 3, lgr);
        auto f2_result0 = psr2(*nc, tkns, 3, lgr);
        auto f3_result0 = psr3(*nc, tkns, 3, lgr);
        auto f4_result0 = psr4(*nc, tkns, 3, lgr);
        auto f5_result0 = psr5(*nc, tkns, 3, lgr);
        auto f6_result0 = psr6(*nc, tkns, 3, lgr);
        auto f7_result0 = psr7(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("abababa", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("abababa", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("abababa", lgr), taul::token(lpr2, "ababab"));
    //EXPECT_EQ(lex3("abababa", lgr), taul::token(lpr3, "ababab"));
    //EXPECT_EQ(lex4("abababa", lgr), taul::token(lpr4, "abab"));
    //EXPECT_EQ(lex5("abababa", lgr), taul::token(lpr5, "ababab"));
    //EXPECT_EQ(lex6("abababa", lgr), taul::token(lpr6, "ababab"));
    //EXPECT_EQ(lex7("abababa", lgr), taul::token(lpr7, "ababab"));

    {
        std::string src = "___abababa";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 3);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f1_expect0 = nc->create(f1, "ab", 3);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f2_expect0 = nc->create(f2, "ababab", 3);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        auto f3_expect0 = nc->create(f3, "ababab", 3);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        auto f4_expect0 = nc->create(f4, "abab", 3);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f4_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        auto f5_expect0 = nc->create(f5, "ababab", 3);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        auto f6_expect0 = nc->create(f6, "ababab", 3);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        auto f7_expect0 = nc->create(f7, "ababab", 3);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 8)));

        auto f0_result0 = psr0(*nc, tkns, 3, lgr);
        auto f1_result0 = psr1(*nc, tkns, 3, lgr);
        auto f2_result0 = psr2(*nc, tkns, 3, lgr);
        auto f3_result0 = psr3(*nc, tkns, 3, lgr);
        auto f4_result0 = psr4(*nc, tkns, 3, lgr);
        auto f5_result0 = psr5(*nc, tkns, 3, lgr);
        auto f6_result0 = psr6(*nc, tkns, 3, lgr);
        auto f7_result0 = psr7(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("abababab", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("abababab", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("abababab", lgr), taul::token(lpr2, "abababab"));
    //EXPECT_EQ(lex3("abababab", lgr), taul::token(lpr3, "abababab"));
    //EXPECT_EQ(lex4("abababab", lgr), taul::token(lpr4, "abab"));
    //EXPECT_EQ(lex5("abababab", lgr), taul::token(lpr5, "abababab"));
    //EXPECT_EQ(lex6("abababab", lgr), taul::token(lpr6, "abababab"));
    //EXPECT_EQ(lex7("abababab", lgr), taul::token(lpr7, "abababab"));

    {
        std::string src = "___abababab";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 3);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f1_expect0 = nc->create(f1, "ab", 3);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f2_expect0 = nc->create(f2, "abababab", 3);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 9)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 10)));
        auto f3_expect0 = nc->create(f3, "abababab", 3);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 9)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 10)));
        auto f4_expect0 = nc->create(f4, "abab", 3);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f4_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        auto f5_expect0 = nc->create(f5, "abababab", 3);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 9)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 10)));
        auto f6_expect0 = nc->create(f6, "abababab", 3);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 9)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 10)));
        auto f7_expect0 = nc->create(f7, "abababab", 3);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 9)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 10)));

        auto f0_result0 = psr0(*nc, tkns, 3, lgr);
        auto f1_result0 = psr1(*nc, tkns, 3, lgr);
        auto f2_result0 = psr2(*nc, tkns, 3, lgr);
        auto f3_result0 = psr3(*nc, tkns, 3, lgr);
        auto f4_result0 = psr4(*nc, tkns, 3, lgr);
        auto f5_result0 = psr5(*nc, tkns, 3, lgr);
        auto f6_result0 = psr6(*nc, tkns, 3, lgr);
        auto f7_result0 = psr7(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("ababababa", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("ababababa", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("ababababa", lgr), taul::token(lpr2, "abababab"));
    //EXPECT_EQ(lex3("ababababa", lgr), taul::token(lpr3, "abababab"));
    //EXPECT_EQ(lex4("ababababa", lgr), taul::token(lpr4, "abab"));
    //EXPECT_EQ(lex5("ababababa", lgr), taul::token(lpr5, "abababab"));
    //EXPECT_EQ(lex6("ababababa", lgr), taul::token(lpr6, "abababab"));
    //EXPECT_EQ(lex7("ababababa", lgr), taul::token(lpr7, "abababab"));

    {
        std::string src = "___ababababa";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 3);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f1_expect0 = nc->create(f1, "ab", 3);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f2_expect0 = nc->create(f2, "abababab", 3);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 9)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 10)));
        auto f3_expect0 = nc->create(f3, "abababab", 3);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 9)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 10)));
        auto f4_expect0 = nc->create(f4, "abab", 3);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f4_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        auto f5_expect0 = nc->create(f5, "abababab", 3);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 9)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 10)));
        auto f6_expect0 = nc->create(f6, "abababab", 3);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 9)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 10)));
        auto f7_expect0 = nc->create(f7, "abababab", 3);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 9)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 10)));

        auto f0_result0 = psr0(*nc, tkns, 3, lgr);
        auto f1_result0 = psr1(*nc, tkns, 3, lgr);
        auto f2_result0 = psr2(*nc, tkns, 3, lgr);
        auto f3_result0 = psr3(*nc, tkns, 3, lgr);
        auto f4_result0 = psr4(*nc, tkns, 3, lgr);
        auto f5_result0 = psr5(*nc, tkns, 3, lgr);
        auto f6_result0 = psr6(*nc, tkns, 3, lgr);
        auto f7_result0 = psr7(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();
    }

    //EXPECT_EQ(lex0("ababababab", lgr), taul::token(lpr0, "ab"));
    //EXPECT_EQ(lex1("ababababab", lgr), taul::token(lpr1, "ab"));
    //EXPECT_EQ(lex2("ababababab", lgr), taul::token(lpr2, "ababababab"));
    //EXPECT_EQ(lex3("ababababab", lgr), taul::token(lpr3, "ababababab"));
    //EXPECT_EQ(lex4("ababababab", lgr), taul::token(lpr4, "abab"));
    //EXPECT_EQ(lex5("ababababab", lgr), taul::token(lpr5, "ababababab"));
    //EXPECT_EQ(lex6("ababababab", lgr), taul::token(lpr6, "abababab"));
    //EXPECT_EQ(lex7("ababababab", lgr), taul::token(lpr7, "abababab"));

    {
        std::string src = "___ababababab";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "ab", 3);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f1_expect0 = nc->create(f1, "ab", 3);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        auto f2_expect0 = nc->create(f2, "ababababab", 3);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 9)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 10)));
        f2_expect0.attach(nc->create(taul::token(chr, "a", 11)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 12)));
        auto f3_expect0 = nc->create(f3, "ababababab", 3);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 9)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 10)));
        f3_expect0.attach(nc->create(taul::token(chr, "a", 11)));
        f3_expect0.attach(nc->create(taul::token(chr, "b", 12)));
        auto f4_expect0 = nc->create(f4, "abab", 3);
        f4_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f4_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f4_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        auto f5_expect0 = nc->create(f5, "ababababab", 3);
        f5_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 9)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 10)));
        f5_expect0.attach(nc->create(taul::token(chr, "a", 11)));
        f5_expect0.attach(nc->create(taul::token(chr, "b", 12)));
        auto f6_expect0 = nc->create(f6, "abababab", 3);
        f6_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        f6_expect0.attach(nc->create(taul::token(chr, "a", 9)));
        f6_expect0.attach(nc->create(taul::token(chr, "b", 10)));
        auto f7_expect0 = nc->create(f7, "abababab", 3);
        f7_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 5)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 6)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 7)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 8)));
        f7_expect0.attach(nc->create(taul::token(chr, "a", 9)));
        f7_expect0.attach(nc->create(taul::token(chr, "b", 10)));

        auto f0_result0 = psr0(*nc, tkns, 3, lgr);
        auto f1_result0 = psr1(*nc, tkns, 3, lgr);
        auto f2_result0 = psr2(*nc, tkns, 3, lgr);
        auto f3_result0 = psr3(*nc, tkns, 3, lgr);
        auto f4_result0 = psr4(*nc, tkns, 3, lgr);
        auto f5_result0 = psr5(*nc, tkns, 3, lgr);
        auto f6_result0 = psr6(*nc, tkns, 3, lgr);
        auto f7_result0 = psr7(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f4_expect0.fmt_tree());
        if ((bool)f4_result0) {
            TAUL_LOG(lgr, "result:\n{}", f4_result0->fmt_tree());
            EXPECT_TRUE(f4_expect0.equiv(*f4_result0));
            EXPECT_EQ(f4_expect0.str(), f4_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f5_expect0.fmt_tree());
        if ((bool)f5_result0) {
            TAUL_LOG(lgr, "result:\n{}", f5_result0->fmt_tree());
            EXPECT_TRUE(f5_expect0.equiv(*f5_result0));
            EXPECT_EQ(f5_expect0.str(), f5_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f6_expect0.fmt_tree());
        if ((bool)f6_result0) {
            TAUL_LOG(lgr, "result:\n{}", f6_result0->fmt_tree());
            EXPECT_TRUE(f6_expect0.equiv(*f6_result0));
            EXPECT_EQ(f6_expect0.str(), f6_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f7_expect0.fmt_tree());
        if ((bool)f7_result0) {
            TAUL_LOG(lgr, "result:\n{}", f7_result0->fmt_tree());
            EXPECT_TRUE(f7_expect0.equiv(*f7_result0));
            EXPECT_EQ(f7_expect0.str(), f7_result0->str());
        }
        else ADD_FAILURE();
    }
}

TEST_F(GrammarUsageTests, lexerExpr_assertion) {

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

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
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

TEST_F(GrammarUsageTests, parserExpr_assertion) {

    // remember that all parser expr tests should include testing things like
    // that the impl can handle token sequences w/ 'skip tokens' cut out

    // testing w/ regards to failure tokens should also be ensured

    const auto spec =
        taul::spec_writer()
        .lpr_decl("ws")
        .lpr_decl("chr")
        .ppr_decl("f0")
        .ppr_decl("f1")
        .ppr_decl("f2")
        .ppr_decl("f3")
        .lpr("ws", taul::qualifier::skip)
        .string(" ")
        .close()
        .lpr("chr")
        .any()
        .close()
        .ppr("f0") // test w/ positive assertion
        .sequence()
        .string("a")
        .string("b")
        .string("c")
        .close()
        .assertion()
        .sequence()
        .string("d")
        .string("e")
        .string("f")
        .close()
        .close()
        .close()
        .ppr("f1") // test w/ negative assertion
        .sequence()
        .string("a")
        .string("b")
        .string("c")
        .close()
        .assertion(taul::polarity::negative)
        .sequence()
        .string("d")
        .string("e")
        .string("f")
        .close()
        .close()
        .close()
        // test that assertion impl works as expected within sequences
        .ppr("f2")
        .sequence()
        .string("a")
        .assertion()
        .string("b")
        .close()
        .string("b")
        .string("c")
        .close()
        .close()
        // test assertions again, w/ assertion as l one in sequence
        .ppr("f3")
        .sequence()
        .string("a")
        .assertion()
        .string("b")
        .close()
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto& f1 = gram.ppr("f1");
    const auto& f2 = gram.ppr("f2");
    const auto& f3 = gram.ppr("f3");
    const auto psr0 = gram.parser("f0");
    const auto psr1 = gram.parser("f1");
    const auto psr2 = gram.parser("f2");
    const auto psr3 = gram.parser("f3");


    // the below assertions expect the embedded exprs to work as expected


    auto _log =
        [&](const std::string& src, const taul::token_seq& tkns) {
        TAUL_LOG(lgr, "src==\"{}\"", src);
        TAUL_LOG(lgr, "tkns ({})", tkns.size());
        for (const auto& I : tkns) {
            TAUL_LOG(lgr, "    {}", I);
        }
        };

    auto nc = std::make_shared<taul::node_ctx>();


    // test w/ and w/out offset

    // success

    {
        std::string src0 = "abcdef";
        std::string src1 = "abc";
        std::string src2 = "abcd";
        std::string src3 = "abcde";
        taul::token_seq tkns0 = taul::tokenize(gram, src0);
        taul::token_seq tkns1 = taul::tokenize(gram, src1);
        taul::token_seq tkns2 = taul::tokenize(gram, src2);
        taul::token_seq tkns3 = taul::tokenize(gram, src3);
        _log(src0, tkns0);
        _log(src1, tkns1);
        _log(src2, tkns2);
        _log(src3, tkns3);

        auto f0_expect0 = nc->create(f0, "abc", 0);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f0_expect0.attach(nc->create(taul::token(chr, "c", 2)));
        auto f0_expect1 = nc->create(f0, "abc", 0);
        f0_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f0_expect1.attach(nc->create(taul::token(chr, "c", 2)));
        
        auto f1_expect0 = nc->create(f1, "abc", 0);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f1_expect0.attach(nc->create(taul::token(chr, "c", 2)));
        auto f1_expect1 = nc->create(f1, "abc", 0);
        f1_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f1_expect1.attach(nc->create(taul::token(chr, "c", 2)));
        auto f1_expect2 = nc->create(f1, "abc", 0);
        f1_expect2.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect2.attach(nc->create(taul::token(chr, "b", 1)));
        f1_expect2.attach(nc->create(taul::token(chr, "c", 2)));
        auto f1_expect3 = nc->create(f1, "abc", 0);
        f1_expect3.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect3.attach(nc->create(taul::token(chr, "b", 1)));
        f1_expect3.attach(nc->create(taul::token(chr, "c", 2)));
        auto f1_expect4 = nc->create(f1, "abc", 0);
        f1_expect4.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect4.attach(nc->create(taul::token(chr, "b", 1)));
        f1_expect4.attach(nc->create(taul::token(chr, "c", 2)));
        auto f1_expect5 = nc->create(f1, "abc", 0);
        f1_expect5.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect5.attach(nc->create(taul::token(chr, "b", 1)));
        f1_expect5.attach(nc->create(taul::token(chr, "c", 2)));

        auto f2_expect0 = nc->create(f2, "abc", 0);
        f2_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f2_expect0.attach(nc->create(taul::token(chr, "c", 2)));
        auto f2_expect1 = nc->create(f2, "abc", 0);
        f2_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f2_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f2_expect1.attach(nc->create(taul::token(chr, "c", 2)));

        auto f3_expect0 = nc->create(f3, "a", 0);
        f3_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        auto f3_expect1 = nc->create(f3, "a", 0);
        f3_expect1.attach(nc->create(taul::token(chr, "a", 0)));

        auto f0_result0 = psr0(*nc, tkns0, lgr);
        auto f0_result1 = psr0(*nc, tkns0, 0, lgr);

        auto f1_result0 = psr1(*nc, tkns1, lgr);
        auto f1_result1 = psr1(*nc, tkns2, lgr);
        auto f1_result2 = psr1(*nc, tkns3, lgr);
        auto f1_result3 = psr1(*nc, tkns1, 0, lgr);
        auto f1_result4 = psr1(*nc, tkns2, 0, lgr);
        auto f1_result5 = psr1(*nc, tkns3, 0, lgr);

        auto f2_result0 = psr2(*nc, tkns1, lgr);
        auto f2_result1 = psr2(*nc, tkns1, 0, lgr);

        auto f3_result0 = psr3(*nc, tkns1, lgr);
        auto f3_result1 = psr3(*nc, tkns1, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect2.fmt_tree());
        if ((bool)f1_result2) {
            TAUL_LOG(lgr, "result:\n{}", f1_result2->fmt_tree());
            EXPECT_TRUE(f1_expect2.equiv(*f1_result2));
            EXPECT_EQ(f1_expect2.str(), f1_result2->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect3.fmt_tree());
        if ((bool)f1_result3) {
            TAUL_LOG(lgr, "result:\n{}", f1_result3->fmt_tree());
            EXPECT_TRUE(f1_expect3.equiv(*f1_result3));
            EXPECT_EQ(f1_expect3.str(), f1_result3->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect4.fmt_tree());
        if ((bool)f1_result4) {
            TAUL_LOG(lgr, "result:\n{}", f1_result4->fmt_tree());
            EXPECT_TRUE(f1_expect4.equiv(*f1_result4));
            EXPECT_EQ(f1_expect4.str(), f1_result4->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect5.fmt_tree());
        if ((bool)f1_result5) {
            TAUL_LOG(lgr, "result:\n{}", f1_result5->fmt_tree());
            EXPECT_TRUE(f1_expect5.equiv(*f1_result5));
            EXPECT_EQ(f1_expect5.str(), f1_result5->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect0.fmt_tree());
        if ((bool)f2_result0) {
            TAUL_LOG(lgr, "result:\n{}", f2_result0->fmt_tree());
            EXPECT_TRUE(f2_expect0.equiv(*f2_result0));
            EXPECT_EQ(f2_expect0.str(), f2_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f2_expect1.fmt_tree());
        if ((bool)f2_result1) {
            TAUL_LOG(lgr, "result:\n{}", f2_result1->fmt_tree());
            EXPECT_TRUE(f2_expect1.equiv(*f2_result1));
            EXPECT_EQ(f2_expect1.str(), f2_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect0.fmt_tree());
        if ((bool)f3_result0) {
            TAUL_LOG(lgr, "result:\n{}", f3_result0->fmt_tree());
            EXPECT_TRUE(f3_expect0.equiv(*f3_result0));
            EXPECT_EQ(f3_expect0.str(), f3_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f3_expect1.fmt_tree());
        if ((bool)f3_result1) {
            TAUL_LOG(lgr, "result:\n{}", f3_result1->fmt_tree());
            EXPECT_TRUE(f3_expect1.equiv(*f3_result1));
            EXPECT_EQ(f3_expect1.str(), f3_result1->str());
        }
        else ADD_FAILURE();
    }

    {
        std::string src0 = "___abcdef";
        std::string src1 = "___abc";
        std::string src2 = "___abcd";
        std::string src3 = "___abcde";
        taul::token_seq tkns0 = taul::tokenize(gram, src0);
        taul::token_seq tkns1 = taul::tokenize(gram, src1);
        taul::token_seq tkns2 = taul::tokenize(gram, src2);
        taul::token_seq tkns3 = taul::tokenize(gram, src3);
        _log(src0, tkns0);
        _log(src1, tkns1);
        _log(src2, tkns2);
        _log(src3, tkns3);

        auto f0_expect0 = nc->create(f0, "abc", 3);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f0_expect0.attach(nc->create(taul::token(chr, "c", 5)));
        
        auto f1_expect0 = nc->create(f1, "abc", 3);
        f1_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f1_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f1_expect0.attach(nc->create(taul::token(chr, "c", 5)));
        auto f1_expect1 = nc->create(f1, "abc", 3);
        f1_expect1.attach(nc->create(taul::token(chr, "a", 3)));
        f1_expect1.attach(nc->create(taul::token(chr, "b", 4)));
        f1_expect1.attach(nc->create(taul::token(chr, "c", 5)));
        auto f1_expect2 = nc->create(f1, "abc", 3);
        f1_expect2.attach(nc->create(taul::token(chr, "a", 3)));
        f1_expect2.attach(nc->create(taul::token(chr, "b", 4)));
        f1_expect2.attach(nc->create(taul::token(chr, "c", 5)));

        auto f0_result0 = psr0(*nc, tkns0, 3, lgr);

        auto f1_result0 = psr1(*nc, tkns1, 3, lgr);
        auto f1_result1 = psr1(*nc, tkns2, 3, lgr);
        auto f1_result2 = psr1(*nc, tkns3, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect2.fmt_tree());
        if ((bool)f1_result2) {
            TAUL_LOG(lgr, "result:\n{}", f1_result2->fmt_tree());
            EXPECT_TRUE(f1_expect2.equiv(*f1_result2));
            EXPECT_EQ(f1_expect2.str(), f1_result2->str());
        }
        else ADD_FAILURE();
    }

    // failure

    {
        std::string src0 = "";
        std::string src1 = "a";
        std::string src2 = "ab";
        std::string src3 = "abc";
        std::string src4 = "abcd";
        std::string src5 = "abcde";
        std::string src6 = "abcdef";
        taul::token_seq tkns0 = taul::tokenize(gram, src0);
        taul::token_seq tkns1 = taul::tokenize(gram, src1);
        taul::token_seq tkns2 = taul::tokenize(gram, src2);
        taul::token_seq tkns3 = taul::tokenize(gram, src3);
        taul::token_seq tkns4 = taul::tokenize(gram, src4);
        taul::token_seq tkns5 = taul::tokenize(gram, src5);
        taul::token_seq tkns6 = taul::tokenize(gram, src6);
        _log(src0, tkns0);
        _log(src1, tkns1);
        _log(src2, tkns2);
        _log(src3, tkns3);
        _log(src4, tkns4);
        _log(src5, tkns5);
        _log(src6, tkns6);

        EXPECT_FALSE((bool)psr0(*nc, tkns0, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns1, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns2, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns4, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns5, lgr));

        EXPECT_FALSE((bool)psr1(*nc, tkns0, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns1, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns2, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns6, lgr));
    }

    {
        std::string src0 = "___";
        std::string src1 = "___a";
        std::string src2 = "___ab";
        std::string src3 = "___abc";
        std::string src4 = "___abcd";
        std::string src5 = "___abcde";
        std::string src6 = "___abcdef";
        taul::token_seq tkns0 = taul::tokenize(gram, src0);
        taul::token_seq tkns1 = taul::tokenize(gram, src1);
        taul::token_seq tkns2 = taul::tokenize(gram, src2);
        taul::token_seq tkns3 = taul::tokenize(gram, src3);
        taul::token_seq tkns4 = taul::tokenize(gram, src4);
        taul::token_seq tkns5 = taul::tokenize(gram, src5);
        taul::token_seq tkns6 = taul::tokenize(gram, src6);
        _log(src0, tkns0);
        _log(src1, tkns1);
        _log(src2, tkns2);
        _log(src3, tkns3);
        _log(src4, tkns4);
        _log(src5, tkns5);
        _log(src6, tkns6);

        EXPECT_FALSE((bool)psr0(*nc, tkns0, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns1, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns2, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns3, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns4, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns5, 3, lgr));

        EXPECT_FALSE((bool)psr1(*nc, tkns0, 3, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns1, 3, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns2, 3, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns6, 3, lgr));
    }
}

TEST_F(GrammarUsageTests, lexerExpr_constraint) {

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

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
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

TEST_F(GrammarUsageTests, parserExpr_constraint) {

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
        .ppr("f0") // test w/ positive constraint
        .constraint()
        .sequence()
        .any()
        .any()
        .any()
        .close()
        .junction()
        .sequence()
        .string("a")
        .string("b")
        .string("c")
        .close()
        .close()
        .close()
        .ppr("f1") // test w/ negative constraint
        .constraint(taul::polarity::negative)
        .sequence()
        .any()
        .any()
        .any()
        .close()
        .junction()
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
    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto& f1 = gram.ppr("f1");
    const auto psr0 = gram.parser("f0");
    const auto psr1 = gram.parser("f1");


    // the below assertions expect the embedded exprs to work as expected


    auto _log =
        [&](const std::string& src, const taul::token_seq& tkns) {
        TAUL_LOG(lgr, "src==\"{}\"", src);
        TAUL_LOG(lgr, "tkns ({})", tkns.size());
        for (const auto& I : tkns) {
            TAUL_LOG(lgr, "    {}", I);
        }
        };

    auto nc = std::make_shared<taul::node_ctx>();


    // test w/ and w/out offset

    // success

    {
        std::string src = "abcdef";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "abc", 0);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f0_expect0.attach(nc->create(taul::token(chr, "c", 2)));
        auto f0_expect1 = nc->create(f0, "abc", 0);
        f0_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f0_expect1.attach(nc->create(taul::token(chr, "c", 2)));

        auto f0_result0 = psr0(*nc, tkns, lgr);
        auto f0_result1 = psr0(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();
    }

    {
        std::string src0 = "123xyz";
        std::string src1 = "!@#xyz";
        taul::token_seq tkns0 = taul::tokenize(gram, src0);
        taul::token_seq tkns1 = taul::tokenize(gram, src1);
        _log(src0, tkns0);
        _log(src1, tkns1);

        auto f1_expect0 = nc->create(f1, "123", 0);
        f1_expect0.attach(nc->create(taul::token(chr, "1", 0)));
        f1_expect0.attach(nc->create(taul::token(chr, "2", 1)));
        f1_expect0.attach(nc->create(taul::token(chr, "3", 2)));
        auto f1_expect1 = nc->create(f1, "!@#", 0);
        f1_expect1.attach(nc->create(taul::token(chr, "!", 0)));
        f1_expect1.attach(nc->create(taul::token(chr, "@", 1)));
        f1_expect1.attach(nc->create(taul::token(chr, "#", 2)));
        auto f1_expect2 = nc->create(f1, "123", 0);
        f1_expect2.attach(nc->create(taul::token(chr, "1", 0)));
        f1_expect2.attach(nc->create(taul::token(chr, "2", 1)));
        f1_expect2.attach(nc->create(taul::token(chr, "3", 2)));
        auto f1_expect3 = nc->create(f1, "!@#", 0);
        f1_expect3.attach(nc->create(taul::token(chr, "!", 0)));
        f1_expect3.attach(nc->create(taul::token(chr, "@", 1)));
        f1_expect3.attach(nc->create(taul::token(chr, "#", 2)));

        auto f1_result0 = psr1(*nc, tkns0, lgr);
        auto f1_result1 = psr1(*nc, tkns1, lgr);
        auto f1_result2 = psr1(*nc, tkns0, 0, lgr);
        auto f1_result3 = psr1(*nc, tkns1, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect2.fmt_tree());
        if ((bool)f1_result2) {
            TAUL_LOG(lgr, "result:\n{}", f1_result2->fmt_tree());
            EXPECT_TRUE(f1_expect2.equiv(*f1_result2));
            EXPECT_EQ(f1_expect2.str(), f1_result2->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect3.fmt_tree());
        if ((bool)f1_result3) {
            TAUL_LOG(lgr, "result:\n{}", f1_result3->fmt_tree());
            EXPECT_TRUE(f1_expect3.equiv(*f1_result3));
            EXPECT_EQ(f1_expect3.str(), f1_result3->str());
        }
        else ADD_FAILURE();
    }

    {
        std::string src = "___abcdef";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "abc", 3);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f0_expect0.attach(nc->create(taul::token(chr, "c", 5)));

        auto f0_result0 = psr0(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();
    }

    {
        std::string src0 = "___123xyz";
        std::string src1 = "___!@#xyz";
        taul::token_seq tkns0 = taul::tokenize(gram, src0);
        taul::token_seq tkns1 = taul::tokenize(gram, src1);
        _log(src0, tkns0);
        _log(src1, tkns1);

        auto f1_expect0 = nc->create(f1, "123", 3);
        f1_expect0.attach(nc->create(taul::token(chr, "1", 3)));
        f1_expect0.attach(nc->create(taul::token(chr, "2", 4)));
        f1_expect0.attach(nc->create(taul::token(chr, "3", 5)));
        auto f1_expect1 = nc->create(f1, "!@#", 3);
        f1_expect1.attach(nc->create(taul::token(chr, "!", 3)));
        f1_expect1.attach(nc->create(taul::token(chr, "@", 4)));
        f1_expect1.attach(nc->create(taul::token(chr, "#", 5)));

        auto f1_result0 = psr1(*nc, tkns0, 3, lgr);
        auto f1_result1 = psr1(*nc, tkns1, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();
    }

    // failure

    {
        std::string src0 = "";
        std::string src1 = "a";
        std::string src2 = "ab";
        std::string src3 = "abc";
        std::string src4 = "123";
        std::string src5 = "!@#";
        taul::token_seq tkns0 = taul::tokenize(gram, src0);
        taul::token_seq tkns1 = taul::tokenize(gram, src1);
        taul::token_seq tkns2 = taul::tokenize(gram, src2);
        taul::token_seq tkns3 = taul::tokenize(gram, src3);
        taul::token_seq tkns4 = taul::tokenize(gram, src4);
        taul::token_seq tkns5 = taul::tokenize(gram, src5);
        _log(src0, tkns0);
        _log(src1, tkns1);
        _log(src2, tkns2);
        _log(src3, tkns3);
        _log(src4, tkns4);
        _log(src5, tkns5);

        EXPECT_FALSE((bool)psr0(*nc, tkns0, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns1, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns2, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns4, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns5, lgr));
        
        EXPECT_FALSE((bool)psr1(*nc, tkns0, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns1, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns2, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns3, lgr));
    }

    {
        std::string src0 = "___";
        std::string src1 = "___a";
        std::string src2 = "___ab";
        std::string src3 = "___abc";
        std::string src4 = "___123";
        std::string src5 = "___!@#";
        taul::token_seq tkns0 = taul::tokenize(gram, src0);
        taul::token_seq tkns1 = taul::tokenize(gram, src1);
        taul::token_seq tkns2 = taul::tokenize(gram, src2);
        taul::token_seq tkns3 = taul::tokenize(gram, src3);
        taul::token_seq tkns4 = taul::tokenize(gram, src4);
        taul::token_seq tkns5 = taul::tokenize(gram, src5);
        _log(src0, tkns0);
        _log(src1, tkns1);
        _log(src2, tkns2);
        _log(src3, tkns3);
        _log(src4, tkns4);
        _log(src5, tkns5);

        EXPECT_FALSE((bool)psr0(*nc, tkns0, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns1, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns2, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns4, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns5, 3, lgr));
        
        EXPECT_FALSE((bool)psr1(*nc, tkns0, 3, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns1, 3, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns2, 3, lgr));
        EXPECT_FALSE((bool)psr1(*nc, tkns3, 3, lgr));
    }
}

TEST_F(GrammarUsageTests, lexerExpr_localend) {

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

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
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

TEST_F(GrammarUsageTests, parserExpr_localend) {

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
        .ppr("f0") // test w/ positive constraint
        .constraint()
        .modifier(0, 5)
        .any()
        .close()
        .junction()
        .sequence()
        .sequence()
        .string("a")
        .string("b")
        .string("c")
        .close()
        .localend()
        .close()
        .close()
        .close()
        .ppr("f1") // test w/ negative constraint
        .constraint(taul::polarity::negative)
        .modifier(0, 5)
        .any()
        .close()
        .junction()
        .sequence()
        .sequence()
        .string("a")
        .string("b")
        .string("c")
        .close()
        .localend()
        .close()
        .close()
        .close()
        .done();

    auto loaded = taul::load(spec, lgr);

    ASSERT_TRUE(loaded);

    const taul::grammar gram = std::move(*loaded);
    const auto& chr = gram.lpr("chr");
    const auto& f0 = gram.ppr("f0");
    const auto& f1 = gram.ppr("f1");
    const auto psr0 = gram.parser("f0");
    const auto psr1 = gram.parser("f1");


    // the below assertions expect the embedded exprs to work as expected


    auto _log =
        [&](const std::string& src, const taul::token_seq& tkns) {
        TAUL_LOG(lgr, "src==\"{}\"", src);
        TAUL_LOG(lgr, "tkns ({})", tkns.size());
        for (const auto& I : tkns) {
            TAUL_LOG(lgr, "    {}", I);
        }
        };

    auto nc = std::make_shared<taul::node_ctx>();


    // test w/ and w/out offset

    // success

    {
        std::string src = "abc";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "abc", 0);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 1)));
        f0_expect0.attach(nc->create(taul::token(chr, "c", 2)));
        auto f0_expect1 = nc->create(f0, "abc", 0);
        f0_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        f0_expect1.attach(nc->create(taul::token(chr, "b", 1)));
        f0_expect1.attach(nc->create(taul::token(chr, "c", 2)));

        auto f0_result0 = psr0(*nc, tkns, lgr);
        auto f0_result1 = psr0(*nc, tkns, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f0_expect1.fmt_tree());
        if ((bool)f0_result1) {
            TAUL_LOG(lgr, "result:\n{}", f0_result1->fmt_tree());
            EXPECT_TRUE(f0_expect1.equiv(*f0_result1));
            EXPECT_EQ(f0_expect1.str(), f0_result1->str());
        }
        else ADD_FAILURE();
    }

    {
        std::string src0 = "";
        std::string src1 = "a";
        std::string src2 = "ab";
        std::string src3 = "abcd";
        std::string src4 = "abcde";
        std::string src5 = "123";
        std::string src6 = "!@#";
        taul::token_seq tkns0 = taul::tokenize(gram, src0);
        taul::token_seq tkns1 = taul::tokenize(gram, src1);
        taul::token_seq tkns2 = taul::tokenize(gram, src2);
        taul::token_seq tkns3 = taul::tokenize(gram, src3);
        taul::token_seq tkns4 = taul::tokenize(gram, src4);
        taul::token_seq tkns5 = taul::tokenize(gram, src5);
        taul::token_seq tkns6 = taul::tokenize(gram, src6);
        _log(src0, tkns0);
        _log(src1, tkns1);
        _log(src2, tkns2);
        _log(src3, tkns3);
        _log(src4, tkns4);
        _log(src5, tkns5);
        _log(src6, tkns6);

        auto f1_expect0 = nc->create(f1, "", 0);
        auto f1_expect1 = nc->create(f1, "a", 0);
        f1_expect1.attach(nc->create(taul::token(chr, "a", 0)));
        auto f1_expect2 = nc->create(f1, "ab", 0);
        f1_expect2.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect2.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect3 = nc->create(f1, "abcd", 0);
        f1_expect3.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect3.attach(nc->create(taul::token(chr, "b", 1)));
        f1_expect3.attach(nc->create(taul::token(chr, "c", 2)));
        f1_expect3.attach(nc->create(taul::token(chr, "d", 3)));
        auto f1_expect4 = nc->create(f1, "abcde", 0);
        f1_expect4.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect4.attach(nc->create(taul::token(chr, "b", 1)));
        f1_expect4.attach(nc->create(taul::token(chr, "c", 2)));
        f1_expect4.attach(nc->create(taul::token(chr, "d", 3)));
        f1_expect4.attach(nc->create(taul::token(chr, "e", 4)));
        auto f1_expect5 = nc->create(f1, "123", 0);
        f1_expect5.attach(nc->create(taul::token(chr, "1", 0)));
        f1_expect5.attach(nc->create(taul::token(chr, "2", 1)));
        f1_expect5.attach(nc->create(taul::token(chr, "3", 2)));
        auto f1_expect6 = nc->create(f1, "!@#", 0);
        f1_expect6.attach(nc->create(taul::token(chr, "!", 0)));
        f1_expect6.attach(nc->create(taul::token(chr, "@", 1)));
        f1_expect6.attach(nc->create(taul::token(chr, "#", 2)));

        auto f1_expect7 = nc->create(f1, "", 0);
        auto f1_expect8 = nc->create(f1, "a", 0);
        f1_expect8.attach(nc->create(taul::token(chr, "a", 0)));
        auto f1_expect9 = nc->create(f1, "ab", 0);
        f1_expect9.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect9.attach(nc->create(taul::token(chr, "b", 1)));
        auto f1_expect10 = nc->create(f1, "abcd", 0);
        f1_expect10.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect10.attach(nc->create(taul::token(chr, "b", 1)));
        f1_expect10.attach(nc->create(taul::token(chr, "c", 2)));
        f1_expect10.attach(nc->create(taul::token(chr, "d", 3)));
        auto f1_expect11 = nc->create(f1, "abcde", 0);
        f1_expect11.attach(nc->create(taul::token(chr, "a", 0)));
        f1_expect11.attach(nc->create(taul::token(chr, "b", 1)));
        f1_expect11.attach(nc->create(taul::token(chr, "c", 2)));
        f1_expect11.attach(nc->create(taul::token(chr, "d", 3)));
        f1_expect11.attach(nc->create(taul::token(chr, "e", 4)));
        auto f1_expect12 = nc->create(f1, "123", 0);
        f1_expect12.attach(nc->create(taul::token(chr, "1", 0)));
        f1_expect12.attach(nc->create(taul::token(chr, "2", 1)));
        f1_expect12.attach(nc->create(taul::token(chr, "3", 2)));
        auto f1_expect13 = nc->create(f1, "!@#", 0);
        f1_expect13.attach(nc->create(taul::token(chr, "!", 0)));
        f1_expect13.attach(nc->create(taul::token(chr, "@", 1)));
        f1_expect13.attach(nc->create(taul::token(chr, "#", 2)));

        auto f1_result0 = psr1(*nc, tkns0, lgr);
        auto f1_result1 = psr1(*nc, tkns1, lgr);
        auto f1_result2 = psr1(*nc, tkns2, lgr);
        auto f1_result3 = psr1(*nc, tkns3, lgr);
        auto f1_result4 = psr1(*nc, tkns4, lgr);
        auto f1_result5 = psr1(*nc, tkns5, lgr);
        auto f1_result6 = psr1(*nc, tkns6, lgr);

        auto f1_result7 = psr1(*nc, tkns0, 0, lgr);
        auto f1_result8 = psr1(*nc, tkns1, 0, lgr);
        auto f1_result9 = psr1(*nc, tkns2, 0, lgr);
        auto f1_result10 = psr1(*nc, tkns3, 0, lgr);
        auto f1_result11 = psr1(*nc, tkns4, 0, lgr);
        auto f1_result12 = psr1(*nc, tkns5, 0, lgr);
        auto f1_result13 = psr1(*nc, tkns6, 0, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect2.fmt_tree());
        if ((bool)f1_result2) {
            TAUL_LOG(lgr, "result:\n{}", f1_result2->fmt_tree());
            EXPECT_TRUE(f1_expect2.equiv(*f1_result2));
            EXPECT_EQ(f1_expect2.str(), f1_result2->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect3.fmt_tree());
        if ((bool)f1_result3) {
            TAUL_LOG(lgr, "result:\n{}", f1_result3->fmt_tree());
            EXPECT_TRUE(f1_expect3.equiv(*f1_result3));
            EXPECT_EQ(f1_expect3.str(), f1_result3->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect4.fmt_tree());
        if ((bool)f1_result4) {
            TAUL_LOG(lgr, "result:\n{}", f1_result4->fmt_tree());
            EXPECT_TRUE(f1_expect4.equiv(*f1_result4));
            EXPECT_EQ(f1_expect4.str(), f1_result4->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect5.fmt_tree());
        if ((bool)f1_result5) {
            TAUL_LOG(lgr, "result:\n{}", f1_result5->fmt_tree());
            EXPECT_TRUE(f1_expect5.equiv(*f1_result5));
            EXPECT_EQ(f1_expect5.str(), f1_result5->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect6.fmt_tree());
        if ((bool)f1_result6) {
            TAUL_LOG(lgr, "result:\n{}", f1_result6->fmt_tree());
            EXPECT_TRUE(f1_expect6.equiv(*f1_result6));
            EXPECT_EQ(f1_expect6.str(), f1_result6->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect7.fmt_tree());
        if ((bool)f1_result7) {
            TAUL_LOG(lgr, "result:\n{}", f1_result7->fmt_tree());
            EXPECT_TRUE(f1_expect7.equiv(*f1_result7));
            EXPECT_EQ(f1_expect7.str(), f1_result7->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect8.fmt_tree());
        if ((bool)f1_result8) {
            TAUL_LOG(lgr, "result:\n{}", f1_result8->fmt_tree());
            EXPECT_TRUE(f1_expect8.equiv(*f1_result8));
            EXPECT_EQ(f1_expect8.str(), f1_result8->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect9.fmt_tree());
        if ((bool)f1_result9) {
            TAUL_LOG(lgr, "result:\n{}", f1_result9->fmt_tree());
            EXPECT_TRUE(f1_expect9.equiv(*f1_result9));
            EXPECT_EQ(f1_expect9.str(), f1_result9->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect10.fmt_tree());
        if ((bool)f1_result10) {
            TAUL_LOG(lgr, "result:\n{}", f1_result10->fmt_tree());
            EXPECT_TRUE(f1_expect10.equiv(*f1_result10));
            EXPECT_EQ(f1_expect10.str(), f1_result10->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect11.fmt_tree());
        if ((bool)f1_result11) {
            TAUL_LOG(lgr, "result:\n{}", f1_result11->fmt_tree());
            EXPECT_TRUE(f1_expect11.equiv(*f1_result11));
            EXPECT_EQ(f1_expect11.str(), f1_result11->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect12.fmt_tree());
        if ((bool)f1_result12) {
            TAUL_LOG(lgr, "result:\n{}", f1_result12->fmt_tree());
            EXPECT_TRUE(f1_expect12.equiv(*f1_result12));
            EXPECT_EQ(f1_expect12.str(), f1_result12->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect13.fmt_tree());
        if ((bool)f1_result13) {
            TAUL_LOG(lgr, "result:\n{}", f1_result13->fmt_tree());
            EXPECT_TRUE(f1_expect13.equiv(*f1_result13));
            EXPECT_EQ(f1_expect13.str(), f1_result13->str());
        }
        else ADD_FAILURE();
    }

    {
        std::string src = "___abc";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        auto f0_expect0 = nc->create(f0, "abc", 3);
        f0_expect0.attach(nc->create(taul::token(chr, "a", 3)));
        f0_expect0.attach(nc->create(taul::token(chr, "b", 4)));
        f0_expect0.attach(nc->create(taul::token(chr, "c", 5)));

        auto f0_result0 = psr0(*nc, tkns, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f0_expect0.fmt_tree());
        if ((bool)f0_result0) {
            TAUL_LOG(lgr, "result:\n{}", f0_result0->fmt_tree());
            EXPECT_TRUE(f0_expect0.equiv(*f0_result0));
            EXPECT_EQ(f0_expect0.str(), f0_result0->str());
        }
        else ADD_FAILURE();
    }

    {
        std::string src0 = "___";
        std::string src1 = "___a";
        std::string src2 = "___ab";
        std::string src3 = "___abcd";
        std::string src4 = "___abcde";
        std::string src5 = "___123";
        std::string src6 = "___!@#";
        taul::token_seq tkns0 = taul::tokenize(gram, src0);
        taul::token_seq tkns1 = taul::tokenize(gram, src1);
        taul::token_seq tkns2 = taul::tokenize(gram, src2);
        taul::token_seq tkns3 = taul::tokenize(gram, src3);
        taul::token_seq tkns4 = taul::tokenize(gram, src4);
        taul::token_seq tkns5 = taul::tokenize(gram, src5);
        taul::token_seq tkns6 = taul::tokenize(gram, src6);
        _log(src0, tkns0);
        _log(src1, tkns1);
        _log(src2, tkns2);
        _log(src3, tkns3);
        _log(src4, tkns4);
        _log(src5, tkns5);
        _log(src6, tkns6);

        auto f1_expect0 = nc->create(f1, "", 3);
        auto f1_expect1 = nc->create(f1, "a", 3);
        f1_expect1.attach(nc->create(taul::token(chr, "a", 3)));
        auto f1_expect2 = nc->create(f1, "ab", 3);
        f1_expect2.attach(nc->create(taul::token(chr, "a", 3)));
        f1_expect2.attach(nc->create(taul::token(chr, "b", 4)));
        auto f1_expect3 = nc->create(f1, "abcd", 3);
        f1_expect3.attach(nc->create(taul::token(chr, "a", 3)));
        f1_expect3.attach(nc->create(taul::token(chr, "b", 4)));
        f1_expect3.attach(nc->create(taul::token(chr, "c", 5)));
        f1_expect3.attach(nc->create(taul::token(chr, "d", 6)));
        auto f1_expect4 = nc->create(f1, "abcde", 3);
        f1_expect4.attach(nc->create(taul::token(chr, "a", 3)));
        f1_expect4.attach(nc->create(taul::token(chr, "b", 4)));
        f1_expect4.attach(nc->create(taul::token(chr, "c", 5)));
        f1_expect4.attach(nc->create(taul::token(chr, "d", 6)));
        f1_expect4.attach(nc->create(taul::token(chr, "e", 7)));
        auto f1_expect5 = nc->create(f1, "123", 3);
        f1_expect5.attach(nc->create(taul::token(chr, "1", 3)));
        f1_expect5.attach(nc->create(taul::token(chr, "2", 4)));
        f1_expect5.attach(nc->create(taul::token(chr, "3", 5)));
        auto f1_expect6 = nc->create(f1, "!@#", 3);
        f1_expect6.attach(nc->create(taul::token(chr, "!", 3)));
        f1_expect6.attach(nc->create(taul::token(chr, "@", 4)));
        f1_expect6.attach(nc->create(taul::token(chr, "#", 5)));

        auto f1_result0 = psr1(*nc, tkns0, 3, lgr);
        auto f1_result1 = psr1(*nc, tkns1, 3, lgr);
        auto f1_result2 = psr1(*nc, tkns2, 3, lgr);
        auto f1_result3 = psr1(*nc, tkns3, 3, lgr);
        auto f1_result4 = psr1(*nc, tkns4, 3, lgr);
        auto f1_result5 = psr1(*nc, tkns5, 3, lgr);
        auto f1_result6 = psr1(*nc, tkns6, 3, lgr);

        TAUL_LOG(lgr, "expected:\n{}", f1_expect0.fmt_tree());
        if ((bool)f1_result0) {
            TAUL_LOG(lgr, "result:\n{}", f1_result0->fmt_tree());
            EXPECT_TRUE(f1_expect0.equiv(*f1_result0));
            EXPECT_EQ(f1_expect0.str(), f1_result0->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect1.fmt_tree());
        if ((bool)f1_result1) {
            TAUL_LOG(lgr, "result:\n{}", f1_result1->fmt_tree());
            EXPECT_TRUE(f1_expect1.equiv(*f1_result1));
            EXPECT_EQ(f1_expect1.str(), f1_result1->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect2.fmt_tree());
        if ((bool)f1_result2) {
            TAUL_LOG(lgr, "result:\n{}", f1_result2->fmt_tree());
            EXPECT_TRUE(f1_expect2.equiv(*f1_result2));
            EXPECT_EQ(f1_expect2.str(), f1_result2->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect3.fmt_tree());
        if ((bool)f1_result3) {
            TAUL_LOG(lgr, "result:\n{}", f1_result3->fmt_tree());
            EXPECT_TRUE(f1_expect3.equiv(*f1_result3));
            EXPECT_EQ(f1_expect3.str(), f1_result3->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect4.fmt_tree());
        if ((bool)f1_result4) {
            TAUL_LOG(lgr, "result:\n{}", f1_result4->fmt_tree());
            EXPECT_TRUE(f1_expect4.equiv(*f1_result4));
            EXPECT_EQ(f1_expect4.str(), f1_result4->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect5.fmt_tree());
        if ((bool)f1_result5) {
            TAUL_LOG(lgr, "result:\n{}", f1_result5->fmt_tree());
            EXPECT_TRUE(f1_expect5.equiv(*f1_result5));
            EXPECT_EQ(f1_expect5.str(), f1_result5->str());
        }
        else ADD_FAILURE();

        TAUL_LOG(lgr, "expected:\n{}", f1_expect6.fmt_tree());
        if ((bool)f1_result6) {
            TAUL_LOG(lgr, "result:\n{}", f1_result6->fmt_tree());
            EXPECT_TRUE(f1_expect6.equiv(*f1_result6));
            EXPECT_EQ(f1_expect6.str(), f1_result6->str());
        }
        else ADD_FAILURE();
    }

    // failure

    {
        std::string src0 = "";
        std::string src1 = "a";
        std::string src2 = "ab";
        std::string src3 = "abcd";
        std::string src4 = "abcde";
        std::string src5 = "123";
        std::string src6 = "!@#";
        taul::token_seq tkns0 = taul::tokenize(gram, src0);
        taul::token_seq tkns1 = taul::tokenize(gram, src1);
        taul::token_seq tkns2 = taul::tokenize(gram, src2);
        taul::token_seq tkns3 = taul::tokenize(gram, src3);
        taul::token_seq tkns4 = taul::tokenize(gram, src4);
        taul::token_seq tkns5 = taul::tokenize(gram, src5);
        taul::token_seq tkns6 = taul::tokenize(gram, src6);
        _log(src0, tkns0);
        _log(src1, tkns1);
        _log(src2, tkns2);
        _log(src3, tkns3);
        _log(src4, tkns4);
        _log(src5, tkns5);
        _log(src6, tkns6);

        EXPECT_FALSE((bool)psr0(*nc, tkns0, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns1, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns2, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns4, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns5, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns6, lgr));
    }

    {
        std::string src = "abc";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        EXPECT_FALSE((bool)psr1(*nc, tkns, lgr));
    }

    {
        std::string src0 = "___";
        std::string src1 = "___a";
        std::string src2 = "___ab";
        std::string src3 = "___abcd";
        std::string src4 = "___abcde";
        std::string src5 = "___123";
        std::string src6 = "___!@#";
        taul::token_seq tkns0 = taul::tokenize(gram, src0);
        taul::token_seq tkns1 = taul::tokenize(gram, src1);
        taul::token_seq tkns2 = taul::tokenize(gram, src2);
        taul::token_seq tkns3 = taul::tokenize(gram, src3);
        taul::token_seq tkns4 = taul::tokenize(gram, src4);
        taul::token_seq tkns5 = taul::tokenize(gram, src5);
        taul::token_seq tkns6 = taul::tokenize(gram, src6);
        _log(src0, tkns0);
        _log(src1, tkns1);
        _log(src2, tkns2);
        _log(src3, tkns3);
        _log(src4, tkns4);
        _log(src5, tkns5);
        _log(src6, tkns6);

        EXPECT_FALSE((bool)psr0(*nc, tkns0, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns1, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns2, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns3, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns4, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns5, 3, lgr));
        EXPECT_FALSE((bool)psr0(*nc, tkns6, 3, lgr));
    }

    {
        std::string src = "___abc";
        taul::token_seq tkns = taul::tokenize(gram, src);
        _log(src, tkns);

        EXPECT_FALSE((bool)psr1(*nc, tkns, 3, lgr));
    }
}

