

#include <gtest/gtest.h>

#include <taul/spec.h>
#include <taul/parse_tree_pattern.h>
#include <taul/load.h>


using namespace taul::string_literals;


class ParseTreePatternTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr;
    taul::grammar gram;
    bool ready = false;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
        auto spec =
            taul::spec_writer()
            .lpr_decl("lpr"_str)
            .lpr_decl("lpr2"_str)
            .ppr_decl("ppr"_str)
            .ppr_decl("ppr2"_str)
            .lpr("lpr"_str)
            .close()
            .lpr("lpr2"_str)
            .close()
            .ppr("ppr"_str)
            .close()
            .ppr("ppr2"_str)
            .close()
            .done();
        auto loaded = taul::load(spec, lgr);
        if (loaded) gram = std::move(*loaded);
        ready =
            loaded &&
            gram.has_lpr("lpr"_str) &&
            gram.has_lpr("lpr2"_str) &&
            gram.has_ppr("ppr"_str) &&
            gram.has_ppr("ppr2"_str);
    }
};


TEST_F(ParseTreePatternTests, Ctor) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree_pattern(gram);

    EXPECT_FALSE(a.is_sealed());
}

TEST_F(ParseTreePatternTests, Sealing_LexicalNodesAreSealed) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree_pattern(gram)
        .lexical(taul::token::normal(gram, "lpr"_str, 0, 10));

    EXPECT_TRUE(a.is_sealed());
}

TEST_F(ParseTreePatternTests, Sealing_SyntacticNodesAreUnsealedUntilClosed) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree_pattern(gram)
        .syntactic(gram.ppr("ppr"_str).value(), 0);

    EXPECT_FALSE(a.is_sealed());
}

TEST_F(ParseTreePatternTests, Sealing_SyntacticNodesAreSealedOnceClosed) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree_pattern(gram)
        .syntactic(gram.ppr("ppr"_str).value(), 0)
        .close();

    EXPECT_TRUE(a.is_sealed());
}

TEST_F(ParseTreePatternTests, Sealing_LooseSyntacticNodesAreSealed) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree_pattern(gram)
        .loose_syntactic(gram.ppr("ppr"_str).value(), 0, 10);

    EXPECT_TRUE(a.is_sealed());
}

TEST_F(ParseTreePatternTests, ConstructionAndMatching_LexicalNodeAsRoot) {
    ASSERT_TRUE(ready);

    auto p =
        taul::parse_tree_pattern(gram)
        .lexical(taul::token::normal(gram, "lpr"_str, 0, 3));

    ASSERT_TRUE(p.is_sealed());

    auto success_0 =
        taul::parse_tree(gram)
        .lexical("lpr"_str, 0, 3);

    auto success_1 =
        taul::parse_tree(gram)
        .lexical("lpr"_str, 0, 3)
        .abort();

    ASSERT_TRUE(p.match(success_0, lgr));
    ASSERT_TRUE(p.match(success_1, lgr));

    auto failure_0 =
        taul::parse_tree(gram)
        .lexical("lpr"_str, 1, 3);

    auto failure_1 =
        taul::parse_tree(gram)
        .lexical("lpr"_str, 0, 4);

    auto failure_2 =
        taul::parse_tree(gram)
        .lexical("lpr2"_str, 0, 3);

    auto failure_3 =
        taul::parse_tree(gram)
        .failure(0, 3);

    auto failure_4 =
        taul::parse_tree(gram)
        .end(0);

    auto failure_5 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .close();

    ASSERT_FALSE(p.match(failure_0, lgr));
    ASSERT_FALSE(p.match(failure_1, lgr));
    ASSERT_FALSE(p.match(failure_2, lgr));
    ASSERT_FALSE(p.match(failure_3, lgr));
    ASSERT_FALSE(p.match(failure_4, lgr));
    ASSERT_FALSE(p.match(failure_5, lgr));
}

TEST_F(ParseTreePatternTests, ConstructionAndMatching_LexicalNodeAsRoot_Failure) {
    ASSERT_TRUE(ready);

    auto p =
        taul::parse_tree_pattern(gram)
        .lexical(taul::token::failure(0, 3));

    ASSERT_TRUE(p.is_sealed());

    auto success_0 =
        taul::parse_tree(gram)
        .failure(0, 3);

    auto success_1 =
        taul::parse_tree(gram)
        .failure(0, 3)
        .abort();

    ASSERT_TRUE(p.match(success_0, lgr));
    ASSERT_TRUE(p.match(success_1, lgr));

    auto failure_0 =
        taul::parse_tree(gram)
        .failure(1, 3);

    auto failure_1 =
        taul::parse_tree(gram)
        .failure(0, 4);

    auto failure_2 =
        taul::parse_tree(gram)
        .lexical("lpr"_str, 0, 3);

    auto failure_3 =
        taul::parse_tree(gram)
        .end(0);

    auto failure_4 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .close();

    ASSERT_FALSE(p.match(failure_0, lgr));
    ASSERT_FALSE(p.match(failure_1, lgr));
    ASSERT_FALSE(p.match(failure_2, lgr));
    ASSERT_FALSE(p.match(failure_3, lgr));
    ASSERT_FALSE(p.match(failure_4, lgr));
}

TEST_F(ParseTreePatternTests, ConstructionAndMatching_LexicalNodeAsRoot_End) {
    ASSERT_TRUE(ready);

    auto p =
        taul::parse_tree_pattern(gram)
        .lexical(taul::token::end(0));

    ASSERT_TRUE(p.is_sealed());

    auto success_0 =
        taul::parse_tree(gram)
        .end(0);

    auto success_1 =
        taul::parse_tree(gram)
        .end(0)
        .abort();

    ASSERT_TRUE(p.match(success_0, lgr));
    ASSERT_TRUE(p.match(success_1, lgr));

    auto failure_0 =
        taul::parse_tree(gram)
        .end(1);

    auto failure_1 =
        taul::parse_tree(gram)
        .lexical("lpr"_str, 0, 0);

    auto failure_2 =
        taul::parse_tree(gram)
        .failure(0, 0);

    auto failure_3 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .close();

    ASSERT_FALSE(p.match(failure_0, lgr));
    ASSERT_FALSE(p.match(failure_1, lgr));
    ASSERT_FALSE(p.match(failure_2, lgr));
    ASSERT_FALSE(p.match(failure_3, lgr));
}

TEST_F(ParseTreePatternTests, ConstructionAndMatching_SyntacticNodeAsRoot_Empty) {
    ASSERT_TRUE(ready);

    auto p =
        taul::parse_tree_pattern(gram)
        .syntactic(gram.ppr("ppr"_str).value(), 10)
        .close();

    ASSERT_TRUE(p.is_sealed());

    auto success_0 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 10)
        .close();

    auto success_1 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 10)
        .abort()
        .close();

    ASSERT_TRUE(p.match(success_0, lgr));
    ASSERT_TRUE(p.match(success_1, lgr));

    auto failure_0 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 9)
        .close();

    auto failure_1 =
        taul::parse_tree(gram)
        .syntactic("ppr2"_str, 10)
        .close();

    auto failure_2 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 10)
        .failure(10, 30)
        .close();

    ASSERT_FALSE(p.match(failure_0, lgr));
    ASSERT_FALSE(p.match(failure_1, lgr));
    ASSERT_FALSE(p.match(failure_2, lgr));
}

TEST_F(ParseTreePatternTests, ConstructionAndMatching_SyntacticNodeAsRoot_NestedLexicalAndSyntacticAndLooseSyntacticNodes) {
    ASSERT_TRUE(ready);

    auto p =
        taul::parse_tree_pattern(gram)
        .syntactic(gram.ppr("ppr"_str).value(), 0)
        .lexical(taul::token::normal(gram, "lpr"_str, 0, 1))
        .syntactic(gram.ppr("ppr"_str).value(), 1)
        .lexical(taul::token::normal(gram, "lpr"_str, 1, 1))
        .close()
        .lexical(taul::token::normal(gram, "lpr"_str, 2, 1))
        .loose_syntactic(gram.ppr("ppr"_str).value(), 3, 2)
        .close();

    ASSERT_TRUE(p.is_sealed());

    auto success_0 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .syntactic("ppr"_str, 1)
        .lexical("lpr"_str, 1, 1)
        .close()
        .lexical("lpr"_str, 2, 1)
        .syntactic("ppr"_str, 3)
        .lexical("lpr"_str, 3, 1)
        .lexical("lpr"_str, 4, 1)
        .close()
        .close();

    auto success_1 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .syntactic("ppr"_str, 1)
        .lexical("lpr"_str, 1, 1)
        .close()
        .lexical("lpr"_str, 2, 1)
        .syntactic("ppr"_str, 3) // syntactic node interior differs
        .syntactic("ppr"_str, 3)
        .lexical("lpr"_str, 3, 2)
        .close()
        .close()
        .close();

    auto success_2 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .syntactic("ppr"_str, 1)
        .lexical("lpr"_str, 1, 1)
        .abort()
        .close()
        .lexical("lpr"_str, 2, 1)
        .syntactic("ppr"_str, 3)
        .lexical("lpr"_str, 3, 1)
        .lexical("lpr"_str, 4, 1)
        .close()
        .close();

    ASSERT_TRUE(p.match(success_0, lgr));
    ASSERT_TRUE(p.match(success_1, lgr));
    ASSERT_TRUE(p.match(success_2, lgr));

    auto failure_0 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 10)
        .lexical("lpr"_str, 10, 1)
        .syntactic("ppr"_str, 11)
        .lexical("lpr"_str, 11, 1)
        .close()
        .lexical("lpr"_str, 12, 1)
        .syntactic("ppr"_str, 13)
        .lexical("lpr"_str, 13, 1)
        .lexical("lpr"_str, 14, 1)
        .close()
        .close();

    auto failure_1 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .syntactic("ppr"_str, 1)
        .failure(1, 1)
        .close()
        .lexical("lpr"_str, 2, 1)
        .syntactic("ppr"_str, 3)
        .lexical("lpr"_str, 3, 1)
        .lexical("lpr"_str, 4, 1)
        .close()
        .close();

    auto failure_2 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .syntactic("ppr"_str, 1)
        .close()
        .lexical("lpr"_str, 1, 1)
        .syntactic("ppr"_str, 2)
        .lexical("lpr"_str, 2, 1)
        .lexical("lpr"_str, 3, 1)
        .close()
        .close();

    auto failure_3 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .syntactic("ppr"_str, 1)
        .lexical("lpr"_str, 1, 1)
        .lexical("lpr"_str, 2, 1)
        .lexical("lpr"_str, 3, 1)
        .close()
        .lexical("lpr"_str, 4, 1)
        .lexical("lpr"_str, 5, 1)
        .lexical("lpr"_str, 6, 1)
        .syntactic("ppr"_str, 7)
        .lexical("lpr"_str, 7, 1)
        .lexical("lpr"_str, 8, 1)
        .close()
        .close();

    auto failure_4 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .syntactic("ppr"_str, 1)
        .lexical("lpr"_str, 1, 1)
        .close()
        .syntactic("ppr"_str, 2)
        .lexical("lpr"_str, 2, 1)
        .close()
        .syntactic("ppr"_str, 3)
        .lexical("lpr"_str, 3, 1)
        .close()
        .syntactic("ppr"_str, 4)
        .lexical("lpr"_str, 4, 1)
        .close()
        .lexical("lpr"_str, 5, 1)
        .syntactic("ppr"_str, 6)
        .lexical("lpr"_str, 6, 1)
        .lexical("lpr"_str, 7, 1)
        .close()
        .close();

    auto failure_5 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .syntactic("ppr"_str, 1)
        .lexical("lpr"_str, 1, 1)
        .close()
        .lexical("lpr"_str, 2, 1)
        .syntactic("ppr"_str, 3)
        .lexical("lpr"_str, 3, 1)
        .lexical("lpr"_str, 4, 1)
        .close()
        // above is identical to success_0, except w/ the below additional details
        .lexical("lpr"_str, 5, 1)
        .lexical("lpr"_str, 6, 1)
        .lexical("lpr"_str, 7, 1)
        .lexical("lpr"_str, 8, 1)
        .close();

    ASSERT_FALSE(p.match(failure_0, lgr));
    ASSERT_FALSE(p.match(failure_1, lgr));
    ASSERT_FALSE(p.match(failure_2, lgr));
    ASSERT_FALSE(p.match(failure_3, lgr));
    ASSERT_FALSE(p.match(failure_4, lgr));
    ASSERT_FALSE(p.match(failure_5, lgr));
}

TEST_F(ParseTreePatternTests, ConstructionAndMatching_SyntacticNodeAsRoot_Skip) {
    ASSERT_TRUE(ready);

    auto pa =
        taul::parse_tree_pattern(gram)
        .syntactic(gram.ppr("ppr"_str).value(), 0)
        .skip(10)
        .close();

    auto pb =
        taul::parse_tree_pattern(gram)
        .syntactic(gram.ppr("ppr"_str).value(), 0)
        .skip(0)
        .close();

    ASSERT_TRUE(pa.is_sealed());
    ASSERT_TRUE(pb.is_sealed());

    auto success_0a =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .skip(10)
        .close();

    auto success_1a =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .skip(5)
        .skip(5)
        .close();

    auto success_2a =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .skip(10)
        .abort()
        .close();

    auto success_0b =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .skip(0)
        .close();

    auto success_1b =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .close();

    ASSERT_TRUE(pa.match(success_0a, lgr));
    ASSERT_TRUE(pa.match(success_1a, lgr));
    ASSERT_TRUE(pa.match(success_2a, lgr));
    ASSERT_TRUE(pb.match(success_0b, lgr));
    ASSERT_TRUE(pb.match(success_1b, lgr));

    auto failure_0a =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .skip(11)
        .close();

    auto failure_1a =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .close();

    auto failure_0b =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .skip(1)
        .close();

    ASSERT_FALSE(pa.match(failure_0a, lgr));
    ASSERT_FALSE(pa.match(failure_1a, lgr));
    ASSERT_FALSE(pa.match(failure_0b, lgr));
}

TEST_F(ParseTreePatternTests, ConstructionAndMatching_LooseSyntacticNodeAsRoot) {
    ASSERT_TRUE(ready);

    auto p =
        taul::parse_tree_pattern(gram)
        .loose_syntactic(gram.ppr("ppr"_str).value(), 0, 10);

    ASSERT_TRUE(p.is_sealed());

    auto success_0 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 4)
        .syntactic("ppr"_str, 4)
        .lexical("lpr"_str, 4, 1)
        .lexical("lpr"_str, 5, 1)
        .lexical("lpr"_str, 6, 1)
        .close()
        .lexical("lpr"_str, 7, 1)
        .lexical("lpr"_str, 8, 1)
        .lexical("lpr"_str, 9, 1)
        .close();

    auto success_1 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .lexical("lpr"_str, 1, 1)
        .lexical("lpr"_str, 2, 1)
        .lexical("lpr"_str, 3, 1)
        .lexical("lpr"_str, 4, 1)
        .lexical("lpr"_str, 5, 1)
        .lexical("lpr"_str, 6, 1)
        .lexical("lpr"_str, 7, 1)
        .lexical("lpr"_str, 8, 1)
        .lexical("lpr"_str, 9, 1)
        .close();

    auto success_2 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .skip(10)
        .close();

    ASSERT_TRUE(p.match(success_0, lgr));
    ASSERT_TRUE(p.match(success_1, lgr));
    ASSERT_TRUE(p.match(success_2, lgr));

    auto failure_0 =
        taul::parse_tree(gram)
        .syntactic("ppr"_str, 0)
        .skip(9)
        .close();

    auto failure_1 =
        taul::parse_tree(gram)
        .syntactic("ppr2"_str, 0)
        .skip(10)
        .close();

    auto failure_2 =
        taul::parse_tree(gram)
        .lexical("lpr"_str, 0, 10);

    ASSERT_FALSE(p.match(failure_0, lgr));
    ASSERT_FALSE(p.match(failure_1, lgr));
    ASSERT_FALSE(p.match(failure_2, lgr));
}

