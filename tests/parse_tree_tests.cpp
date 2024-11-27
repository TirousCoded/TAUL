

#include <gtest/gtest.h>

#include <taul/logger.h>
#include <taul/str.h>
#include <taul/spec_error.h>
#include <taul/spec_opcode.h>
#include <taul/spec.h>
#include <taul/grammar.h>
#include <taul/parse_tree.h>
#include <taul/load.h>


using namespace taul::string_literals;


class ParseTreeTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr;
    taul::grammar gram;
    bool ready = false;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
        auto spec =
            taul::spec_writer()
            .lpr_decl("lpr"_str)
            .ppr_decl("ppr"_str)
            .lpr("lpr"_str)
            .close()
            .ppr("ppr"_str)
            .close()
            .done();
        auto loaded = taul::load(spec, lgr);
        if (loaded) gram = std::move(*loaded);
        ready = 
            loaded && 
            gram.has_lpr("lpr"_str) && 
            gram.has_ppr("ppr"_str);
    }
};


// TODO: copy/move init/assign are assumed to work (<- maybe a mistake?)

TEST_F(ParseTreeTests, Ctor) {
    ASSERT_TRUE(ready);
    taul::parse_tree pt(gram);

    EXPECT_FALSE(pt.is_sealed());
}

TEST_F(ParseTreeTests, Iterators) {
    ASSERT_TRUE(ready);
    taul::parse_tree pt(gram);
    pt
        .syntactic(gram.ppr("ppr"_str).value(), 0)
        .lexical(taul::token::normal(gram, "lpr"_str, 0, 1))
        .syntactic(gram.ppr("ppr"_str).value(), 1)
        .syntactic(gram.ppr("ppr"_str).value(), 1)
        .lexical(taul::token::normal(gram, "lpr"_str, 1, 1))
        .close()
        .syntactic(gram.ppr("ppr"_str).value(), 2)
        .lexical(taul::token::normal(gram, "lpr"_str, 2, 1))
        .close()
        .close()
        .close();

    ASSERT_TRUE(pt.is_sealed());
    ASSERT_EQ(pt.nodes(), 7);

    auto it0 = pt.cbegin();
    auto it1 = pt.begin();

    for (std::size_t i = 0; i < pt.nodes(); i++) {
        if (it0 != pt.cend()) {
            EXPECT_EQ(&(*it0), &(pt.at(i)));
        }
        else ADD_FAILURE() << std::format("i=={}", i);
        it0++;
    }

    EXPECT_EQ(it0, pt.cend());

    for (std::size_t i = 0; i < pt.nodes(); i++) {
        if (it1 != pt.cend()) {
            EXPECT_EQ(&(*it1), &(pt.at(i)));
        }
        else ADD_FAILURE() << std::format("i=={}", i);
        it1++;
    }

    EXPECT_EQ(it1, pt.cend());
}

TEST_F(ParseTreeTests, Equality_Basics) {
    ASSERT_TRUE(ready);
    taul::parse_tree tree0(gram);
    tree0
        .syntactic(gram.ppr("ppr"_str).value(), 0)
        .lexical(taul::token::normal(gram, "lpr"_str, 0, 1))
        .syntactic(gram.ppr("ppr"_str).value(), 1)
        .lexical(taul::token::normal(gram, "lpr"_str, 1, 1))
        .lexical(taul::token::normal(gram, "lpr"_str, 2, 1))
        .close()
        .close();
    // tree0 and tree1 are structurally equivalent
    taul::parse_tree tree1(gram);
    tree1
        .syntactic(gram.ppr("ppr"_str).value(), 0)
        .lexical(taul::token::normal(gram, "lpr"_str, 0, 1))
        .syntactic(gram.ppr("ppr"_str).value(), 1)
        .lexical(taul::token::normal(gram, "lpr"_str, 1, 1))
        .lexical(taul::token::normal(gram, "lpr"_str, 2, 1))
        .close()
        .close();
    taul::parse_tree tree2(gram);
    tree2
        .syntactic(gram.ppr("ppr"_str).value(), 0)
        .lexical(taul::token::normal(gram, "lpr"_str, 0, 1))
        .close();
    // tree2 and tree3 are structurally equivalent
    taul::parse_tree tree3(gram);
    tree3
        .syntactic(gram.ppr("ppr"_str).value(), 0)
        .lexical(taul::token::normal(gram, "lpr"_str, 0, 1))
        .close();

    ASSERT_TRUE(tree0.is_sealed());
    ASSERT_TRUE(tree1.is_sealed());
    ASSERT_TRUE(tree2.is_sealed());
    ASSERT_TRUE(tree3.is_sealed());

    EXPECT_TRUE(tree0.equal(tree0));
    EXPECT_TRUE(tree0.equal(tree1));
    EXPECT_FALSE(tree0.equal(tree2));
    EXPECT_FALSE(tree0.equal(tree3));

    EXPECT_TRUE(tree1.equal(tree0));
    EXPECT_TRUE(tree1.equal(tree1));
    EXPECT_FALSE(tree1.equal(tree2));
    EXPECT_FALSE(tree1.equal(tree3));

    EXPECT_FALSE(tree2.equal(tree0));
    EXPECT_FALSE(tree2.equal(tree1));
    EXPECT_TRUE(tree2.equal(tree2));
    EXPECT_TRUE(tree2.equal(tree3));

    EXPECT_FALSE(tree3.equal(tree0));
    EXPECT_FALSE(tree3.equal(tree1));
    EXPECT_TRUE(tree3.equal(tree2));
    EXPECT_TRUE(tree3.equal(tree3));

    EXPECT_TRUE(tree0 == tree0);
    EXPECT_TRUE(tree0 == tree1);
    EXPECT_FALSE(tree0 == tree2);
    EXPECT_FALSE(tree0 == tree3);

    EXPECT_TRUE(tree1 == tree0);
    EXPECT_TRUE(tree1 == tree1);
    EXPECT_FALSE(tree1 == tree2);
    EXPECT_FALSE(tree1 == tree3);

    EXPECT_FALSE(tree2 == tree0);
    EXPECT_FALSE(tree2 == tree1);
    EXPECT_TRUE(tree2 == tree2);
    EXPECT_TRUE(tree2 == tree3);

    EXPECT_FALSE(tree3 == tree0);
    EXPECT_FALSE(tree3 == tree1);
    EXPECT_TRUE(tree3 == tree2);
    EXPECT_TRUE(tree3 == tree3);

    EXPECT_FALSE(tree0 != tree0);
    EXPECT_FALSE(tree0 != tree1);
    EXPECT_TRUE(tree0 != tree2);
    EXPECT_TRUE(tree0 != tree3);

    EXPECT_FALSE(tree1 != tree0);
    EXPECT_FALSE(tree1 != tree1);
    EXPECT_TRUE(tree1 != tree2);
    EXPECT_TRUE(tree1 != tree3);

    EXPECT_TRUE(tree2 != tree0);
    EXPECT_TRUE(tree2 != tree1);
    EXPECT_FALSE(tree2 != tree2);
    EXPECT_FALSE(tree2 != tree3);

    EXPECT_TRUE(tree3 != tree0);
    EXPECT_TRUE(tree3 != tree1);
    EXPECT_FALSE(tree3 != tree2);
    EXPECT_FALSE(tree3 != tree3);
}

TEST_F(ParseTreeTests, Equality_IndifferenceToIsAborted) {
    ASSERT_TRUE(ready);
    taul::parse_tree tree0(gram);
    tree0
        .syntactic(gram.ppr("ppr"_str).value(), 0)
        .lexical(taul::token::normal(gram, "lpr"_str, 0, 1))
        .syntactic(gram.ppr("ppr"_str).value(), 1)
        .lexical(taul::token::normal(gram, "lpr"_str, 1, 1))
        .lexical(taul::token::normal(gram, "lpr"_str, 2, 1))
        .close()
        .close();
    // tree0 and tree1 are structurally equivalent
    taul::parse_tree tree1(gram);
    tree1
        .syntactic(gram.ppr("ppr"_str).value(), 0)
        .lexical(taul::token::normal(gram, "lpr"_str, 0, 1))
        .syntactic(gram.ppr("ppr"_str).value(), 1)
        .lexical(taul::token::normal(gram, "lpr"_str, 1, 1))
        .lexical(taul::token::normal(gram, "lpr"_str, 2, 1))
        .abort() // <- shouldn't matter
        .close()
        .close();

    ASSERT_TRUE(tree0.is_sealed());
    ASSERT_TRUE(tree1.is_sealed());

    ASSERT_FALSE(tree0.is_aborted());
    ASSERT_TRUE(tree1.is_aborted());

    EXPECT_TRUE(tree0.equal(tree1));
    EXPECT_TRUE(tree1.equal(tree0));

    EXPECT_TRUE(tree0 == tree1);
    EXPECT_TRUE(tree1 == tree0);

    EXPECT_FALSE(tree0 != tree1);
    EXPECT_FALSE(tree1 != tree0);
}

TEST_F(ParseTreeTests, Sealing_LexicalNodesAreSealed) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree(gram)
        .lexical(taul::token::normal(gram, "lpr"_str, 0, 10));

    EXPECT_TRUE(a.is_sealed());
}

TEST_F(ParseTreeTests, Sealing_SyntacticNodesAreUnsealedUntilClosed) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree(gram)
        .syntactic(gram.ppr("ppr"_str).value(), 0);

    EXPECT_FALSE(a.is_sealed());
}

TEST_F(ParseTreeTests, Sealing_SyntacticNodesAreSealedOnceClosed) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree(gram)
        .syntactic(gram.ppr("ppr"_str).value(), 0)
        .close();

    EXPECT_TRUE(a.is_sealed());
}

// NOTE: the below 'Construction_' tests cover testing the methods of taul::parse_tree::node

TEST_F(ParseTreeTests, Construction_LexicalNodeAsRoot) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree(gram)
        .lexical(taul::token::normal(gram, "lpr"_str, 1, 3));

    ASSERT_TRUE(a.is_sealed());

    EXPECT_FALSE(a.is_aborted());

    constexpr size_t expected_nodes = 1;

    EXPECT_EQ(a.nodes(), expected_nodes);
    EXPECT_TRUE(a.has_nodes());

    EXPECT_EQ(std::distance(a.cbegin(), a.cend()), expected_nodes);
    EXPECT_EQ(std::distance(a.begin(), a.end()), expected_nodes);

    if (a.has_nodes()) {
        EXPECT_EQ(&(a.root()), &(a.at(0)));
    }

    EXPECT_THROW(a.at(expected_nodes), std::out_of_range);

    if (a.nodes() >= 1) {
        const auto& nd = a.at(0);
        EXPECT_EQ(nd.index(), 0);
        EXPECT_EQ(nd.level(), 0);

        EXPECT_EQ(nd.parent(), a.end());
        EXPECT_EQ(nd.left_sibling(), a.end());
        EXPECT_EQ(nd.right_sibling(), a.end());
        EXPECT_EQ(nd.left_child(), a.end());
        EXPECT_EQ(nd.right_child(), a.end());
        EXPECT_FALSE(nd.has_parent());
        EXPECT_FALSE(nd.has_left_sibling());
        EXPECT_FALSE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 0);
        EXPECT_FALSE(nd.has_children());

        EXPECT_TRUE(nd.is_lexical());
        EXPECT_FALSE(nd.is_syntactic());
        EXPECT_TRUE(nd.is_normal());
        EXPECT_FALSE(nd.is_failure());
        EXPECT_FALSE(nd.is_end());

        EXPECT_EQ(nd.id(), gram.lpr("lpr"_str).value().id());
        EXPECT_EQ(nd.pos(), 1);
        EXPECT_EQ(nd.len(), 3);
        EXPECT_EQ(nd.low_pos(), 1);
        EXPECT_EQ(nd.high_pos(), 4);
        EXPECT_EQ(nd.lpr(), gram.lpr("lpr"_str));
        EXPECT_EQ(nd.ppr(), std::nullopt);
        EXPECT_EQ(nd.str("abcde"_str), "bcd"_str);
        EXPECT_EQ(nd.tkn(), std::make_optional(taul::token::normal(gram, "lpr"_str, 1, 3)));
    }
}

TEST_F(ParseTreeTests, Construction_LexicalNodeAsRoot_Failure) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree(gram)
        .lexical(taul::token::failure(1, 3));

    ASSERT_TRUE(a.is_sealed());

    EXPECT_FALSE(a.is_aborted());

    constexpr size_t expected_nodes = 1;

    EXPECT_EQ(a.nodes(), expected_nodes);
    EXPECT_TRUE(a.has_nodes());

    EXPECT_EQ(std::distance(a.cbegin(), a.cend()), expected_nodes);
    EXPECT_EQ(std::distance(a.begin(), a.end()), expected_nodes);

    if (a.has_nodes()) {
        EXPECT_EQ(&(a.root()), &(a.at(0)));
    }

    EXPECT_THROW(a.at(expected_nodes), std::out_of_range);

    if (a.nodes() >= 1) {
        const auto& nd = a.at(0);
        EXPECT_EQ(nd.index(), 0);
        EXPECT_EQ(nd.level(), 0);

        EXPECT_EQ(nd.parent(), a.end());
        EXPECT_EQ(nd.left_sibling(), a.end());
        EXPECT_EQ(nd.right_sibling(), a.end());
        EXPECT_EQ(nd.left_child(), a.end());
        EXPECT_EQ(nd.right_child(), a.end());
        EXPECT_FALSE(nd.has_parent());
        EXPECT_FALSE(nd.has_left_sibling());
        EXPECT_FALSE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 0);
        EXPECT_FALSE(nd.has_children());

        EXPECT_TRUE(nd.is_lexical());
        EXPECT_FALSE(nd.is_syntactic());
        EXPECT_FALSE(nd.is_normal());
        EXPECT_TRUE(nd.is_failure());
        EXPECT_FALSE(nd.is_end());

        EXPECT_EQ(nd.id(), taul::failure_lpr_id);
        EXPECT_EQ(nd.pos(), 1);
        EXPECT_EQ(nd.len(), 3);
        EXPECT_EQ(nd.low_pos(), 1);
        EXPECT_EQ(nd.high_pos(), 4);
        EXPECT_EQ(nd.lpr(), std::nullopt);
        EXPECT_EQ(nd.ppr(), std::nullopt);
        EXPECT_EQ(nd.str("abcde"_str), "bcd"_str);
        EXPECT_EQ(nd.tkn(), std::make_optional(taul::token::failure(1, 3)));
    }
}

TEST_F(ParseTreeTests, Construction_LexicalNodeAsRoot_End) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree(gram)
        .lexical(taul::token::end(1));

    ASSERT_TRUE(a.is_sealed());

    EXPECT_FALSE(a.is_aborted());

    constexpr size_t expected_nodes = 1;

    EXPECT_EQ(a.nodes(), expected_nodes);
    EXPECT_TRUE(a.has_nodes());

    EXPECT_EQ(std::distance(a.cbegin(), a.cend()), expected_nodes);
    EXPECT_EQ(std::distance(a.begin(), a.end()), expected_nodes);

    if (a.has_nodes()) {
        EXPECT_EQ(&(a.root()), &(a.at(0)));
    }

    EXPECT_THROW(a.at(expected_nodes), std::out_of_range);

    if (a.nodes() >= 1) {
        const auto& nd = a.at(0);
        EXPECT_EQ(nd.index(), 0);
        EXPECT_EQ(nd.level(), 0);

        EXPECT_EQ(nd.parent(), a.end());
        EXPECT_EQ(nd.left_sibling(), a.end());
        EXPECT_EQ(nd.right_sibling(), a.end());
        EXPECT_EQ(nd.left_child(), a.end());
        EXPECT_EQ(nd.right_child(), a.end());
        EXPECT_FALSE(nd.has_parent());
        EXPECT_FALSE(nd.has_left_sibling());
        EXPECT_FALSE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 0);
        EXPECT_FALSE(nd.has_children());

        EXPECT_TRUE(nd.is_lexical());
        EXPECT_FALSE(nd.is_syntactic());
        EXPECT_FALSE(nd.is_normal());
        EXPECT_FALSE(nd.is_failure());
        EXPECT_TRUE(nd.is_end());

        EXPECT_EQ(nd.id(), taul::end_lpr_id);
        EXPECT_EQ(nd.pos(), 1);
        EXPECT_EQ(nd.len(), 0);
        EXPECT_EQ(nd.low_pos(), 1);
        EXPECT_EQ(nd.high_pos(), 1);
        EXPECT_EQ(nd.lpr(), std::nullopt);
        EXPECT_EQ(nd.ppr(), std::nullopt);
        EXPECT_EQ(nd.str("abcde"_str), ""_str);
        EXPECT_EQ(nd.tkn(), std::make_optional(taul::token::end(1)));
    }
}

TEST_F(ParseTreeTests, Construction_LexicalNodeAsRoot_Abort) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree(gram)
        .lexical(taul::token::normal(gram, "lpr"_str, 1, 3))
        .abort();

    ASSERT_TRUE(a.is_sealed());

    EXPECT_TRUE(a.is_aborted());

    constexpr size_t expected_nodes = 1;

    EXPECT_EQ(a.nodes(), expected_nodes);
    EXPECT_TRUE(a.has_nodes());

    EXPECT_EQ(std::distance(a.cbegin(), a.cend()), expected_nodes);
    EXPECT_EQ(std::distance(a.begin(), a.end()), expected_nodes);

    if (a.has_nodes()) {
        EXPECT_EQ(&(a.root()), &(a.at(0)));
    }

    EXPECT_THROW(a.at(expected_nodes), std::out_of_range);

    if (a.nodes() >= 1) {
        const auto& nd = a.at(0);
        EXPECT_EQ(nd.index(), 0);
        EXPECT_EQ(nd.level(), 0);

        EXPECT_EQ(nd.parent(), a.end());
        EXPECT_EQ(nd.left_sibling(), a.end());
        EXPECT_EQ(nd.right_sibling(), a.end());
        EXPECT_EQ(nd.left_child(), a.end());
        EXPECT_EQ(nd.right_child(), a.end());
        EXPECT_FALSE(nd.has_parent());
        EXPECT_FALSE(nd.has_left_sibling());
        EXPECT_FALSE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 0);
        EXPECT_FALSE(nd.has_children());

        EXPECT_TRUE(nd.is_lexical());
        EXPECT_FALSE(nd.is_syntactic());
        EXPECT_TRUE(nd.is_normal());
        EXPECT_FALSE(nd.is_failure());
        EXPECT_FALSE(nd.is_end());

        EXPECT_EQ(nd.id(), gram.lpr("lpr"_str).value().id());
        EXPECT_EQ(nd.pos(), 1);
        EXPECT_EQ(nd.len(), 3);
        EXPECT_EQ(nd.low_pos(), 1);
        EXPECT_EQ(nd.high_pos(), 4);
        EXPECT_EQ(nd.lpr(), gram.lpr("lpr"_str));
        EXPECT_EQ(nd.ppr(), std::nullopt);
        EXPECT_EQ(nd.str("abcde"_str), "bcd"_str);
        EXPECT_EQ(nd.tkn(), std::make_optional(taul::token::normal(gram, "lpr"_str, 1, 3)));
    }
}

TEST_F(ParseTreeTests, Construction_SyntacticNodeAsRoot_Empty) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree(gram)
        .syntactic(gram.ppr("ppr"_str).value(), 1)
        .close();

    ASSERT_TRUE(a.is_sealed());

    EXPECT_FALSE(a.is_aborted());

    constexpr size_t expected_nodes = 1;

    EXPECT_EQ(a.nodes(), expected_nodes);
    EXPECT_TRUE(a.has_nodes());

    EXPECT_EQ(std::distance(a.cbegin(), a.cend()), expected_nodes);
    EXPECT_EQ(std::distance(a.begin(), a.end()), expected_nodes);

    if (a.has_nodes()) {
        EXPECT_EQ(&(a.root()), &(a.at(0)));
    }

    EXPECT_THROW(a.at(expected_nodes), std::out_of_range);

    if (a.nodes() >= 1) {
        const auto& nd = a.at(0);
        EXPECT_EQ(nd.index(), 0);
        EXPECT_EQ(nd.level(), 0);

        EXPECT_EQ(nd.parent(), a.end());
        EXPECT_EQ(nd.left_sibling(), a.end());
        EXPECT_EQ(nd.right_sibling(), a.end());
        EXPECT_EQ(nd.left_child(), a.end());
        EXPECT_EQ(nd.right_child(), a.end());
        EXPECT_FALSE(nd.has_parent());
        EXPECT_FALSE(nd.has_left_sibling());
        EXPECT_FALSE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 0);
        EXPECT_FALSE(nd.has_children());

        EXPECT_FALSE(nd.is_lexical());
        EXPECT_TRUE(nd.is_syntactic());
        EXPECT_TRUE(nd.is_normal());
        EXPECT_FALSE(nd.is_failure());
        EXPECT_FALSE(nd.is_end());

        EXPECT_EQ(nd.id(), gram.ppr("ppr"_str).value().id());
        EXPECT_EQ(nd.pos(), 1);
        EXPECT_EQ(nd.len(), 0);
        EXPECT_EQ(nd.low_pos(), 1);
        EXPECT_EQ(nd.high_pos(), 1);
        EXPECT_EQ(nd.lpr(), std::nullopt);
        EXPECT_EQ(nd.ppr(), gram.ppr("ppr"_str));
        EXPECT_EQ(nd.str("abcde"_str), ""_str);
        EXPECT_EQ(nd.tkn(), std::nullopt);
    }
}

TEST_F(ParseTreeTests, Construction_SyntacticNodeAsRoot_NestedLexicalAndSyntacticNodes) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree(gram)
        .syntactic(gram.ppr("ppr"_str).value(), 1)
        .lexical(taul::token::normal(gram, "lpr"_str, 1, 1))
        .syntactic(gram.ppr("ppr"_str).value(), 2)
        .lexical(taul::token::normal(gram, "lpr"_str, 2, 1))
        .close()
        .lexical(taul::token::normal(gram, "lpr"_str, 3, 1))
        .close();

    ASSERT_TRUE(a.is_sealed());

    EXPECT_FALSE(a.is_aborted());

    constexpr size_t expected_nodes = 5;

    EXPECT_EQ(a.nodes(), expected_nodes);
    EXPECT_TRUE(a.has_nodes());

    EXPECT_EQ(std::distance(a.cbegin(), a.cend()), expected_nodes);
    EXPECT_EQ(std::distance(a.begin(), a.end()), expected_nodes);

    if (a.has_nodes()) {
        EXPECT_EQ(&(a.root()), &(a.at(0)));
    }

    EXPECT_THROW(a.at(expected_nodes), std::out_of_range);

    if (a.nodes() >= 1) {
        const auto& nd = a.at(0);
        EXPECT_EQ(nd.index(), 0);
        EXPECT_EQ(nd.level(), 0);

        EXPECT_EQ(nd.parent(), a.end());
        EXPECT_EQ(nd.left_sibling(), a.end());
        EXPECT_EQ(nd.right_sibling(), a.end());
        EXPECT_EQ(nd.left_child(), std::next(a.begin(), 1));
        EXPECT_EQ(nd.right_child(), std::next(a.begin(), 4));
        EXPECT_FALSE(nd.has_parent());
        EXPECT_FALSE(nd.has_left_sibling());
        EXPECT_FALSE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 3);
        EXPECT_TRUE(nd.has_children());

        EXPECT_FALSE(nd.is_lexical());
        EXPECT_TRUE(nd.is_syntactic());
        EXPECT_TRUE(nd.is_normal());
        EXPECT_FALSE(nd.is_failure());
        EXPECT_FALSE(nd.is_end());

        EXPECT_EQ(nd.id(), gram.ppr("ppr"_str).value().id());
        EXPECT_EQ(nd.pos(), 1);
        EXPECT_EQ(nd.len(), 3);
        EXPECT_EQ(nd.low_pos(), 1);
        EXPECT_EQ(nd.high_pos(), 4);
        EXPECT_EQ(nd.lpr(), std::nullopt);
        EXPECT_EQ(nd.ppr(), gram.ppr("ppr"_str));
        EXPECT_EQ(nd.str("abcde"_str), "bcd"_str);
        EXPECT_EQ(nd.tkn(), std::nullopt);
    }

    if (a.nodes() >= 2) {
        const auto& nd = a.at(1);
        EXPECT_EQ(nd.index(), 1);
        EXPECT_EQ(nd.level(), 1);

        EXPECT_EQ(nd.parent(), std::next(a.begin(), 0));
        EXPECT_EQ(nd.left_sibling(), a.end());
        EXPECT_EQ(nd.right_sibling(), std::next(a.begin(), 2));
        EXPECT_EQ(nd.left_child(), a.end());
        EXPECT_EQ(nd.right_child(), a.end());
        EXPECT_TRUE(nd.has_parent());
        EXPECT_FALSE(nd.has_left_sibling());
        EXPECT_TRUE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 0);
        EXPECT_FALSE(nd.has_children());

        EXPECT_TRUE(nd.is_lexical());
        EXPECT_FALSE(nd.is_syntactic());
        EXPECT_TRUE(nd.is_normal());
        EXPECT_FALSE(nd.is_failure());
        EXPECT_FALSE(nd.is_end());

        EXPECT_EQ(nd.id(), gram.lpr("lpr"_str).value().id());
        EXPECT_EQ(nd.pos(), 1);
        EXPECT_EQ(nd.len(), 1);
        EXPECT_EQ(nd.low_pos(), 1);
        EXPECT_EQ(nd.high_pos(), 2);
        EXPECT_EQ(nd.lpr(), gram.lpr("lpr"_str));
        EXPECT_EQ(nd.ppr(), std::nullopt);
        EXPECT_EQ(nd.str("abcde"_str), "b"_str);
        EXPECT_EQ(nd.tkn(), std::make_optional(taul::token::normal(gram, "lpr"_str, 1, 1)));
    }

    if (a.nodes() >= 3) {
        const auto& nd = a.at(2);
        EXPECT_EQ(nd.index(), 2);
        EXPECT_EQ(nd.level(), 1);

        EXPECT_EQ(nd.parent(), std::next(a.begin(), 0));
        EXPECT_EQ(nd.left_sibling(), std::next(a.begin(), 1));
        EXPECT_EQ(nd.right_sibling(), std::next(a.begin(), 4));
        EXPECT_EQ(nd.left_child(), std::next(a.begin(), 3));
        EXPECT_EQ(nd.right_child(), std::next(a.begin(), 3));
        EXPECT_TRUE(nd.has_parent());
        EXPECT_TRUE(nd.has_left_sibling());
        EXPECT_TRUE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 1);
        EXPECT_TRUE(nd.has_children());

        EXPECT_FALSE(nd.is_lexical());
        EXPECT_TRUE(nd.is_syntactic());
        EXPECT_TRUE(nd.is_normal());
        EXPECT_FALSE(nd.is_failure());
        EXPECT_FALSE(nd.is_end());

        EXPECT_EQ(nd.id(), gram.ppr("ppr"_str).value().id());
        EXPECT_EQ(nd.pos(), 2);
        EXPECT_EQ(nd.len(), 1);
        EXPECT_EQ(nd.low_pos(), 2);
        EXPECT_EQ(nd.high_pos(), 3);
        EXPECT_EQ(nd.lpr(), std::nullopt);
        EXPECT_EQ(nd.ppr(), gram.ppr("ppr"_str));
        EXPECT_EQ(nd.str("abcde"_str), "c"_str);
        EXPECT_EQ(nd.tkn(), std::nullopt);
    }

    if (a.nodes() >= 4) {
        const auto& nd = a.at(3);
        EXPECT_EQ(nd.index(), 3);
        EXPECT_EQ(nd.level(), 2);

        EXPECT_EQ(nd.parent(), std::next(a.begin(), 2));
        EXPECT_EQ(nd.left_sibling(), a.end());
        EXPECT_EQ(nd.right_sibling(), a.end());
        EXPECT_EQ(nd.left_child(), a.end());
        EXPECT_EQ(nd.right_child(), a.end());
        EXPECT_TRUE(nd.has_parent());
        EXPECT_FALSE(nd.has_left_sibling());
        EXPECT_FALSE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 0);
        EXPECT_FALSE(nd.has_children());

        EXPECT_TRUE(nd.is_lexical());
        EXPECT_FALSE(nd.is_syntactic());
        EXPECT_TRUE(nd.is_normal());
        EXPECT_FALSE(nd.is_failure());
        EXPECT_FALSE(nd.is_end());

        EXPECT_EQ(nd.id(), gram.lpr("lpr"_str).value().id());
        EXPECT_EQ(nd.pos(), 2);
        EXPECT_EQ(nd.len(), 1);
        EXPECT_EQ(nd.low_pos(), 2);
        EXPECT_EQ(nd.high_pos(), 3);
        EXPECT_EQ(nd.lpr(), gram.lpr("lpr"_str));
        EXPECT_EQ(nd.ppr(), std::nullopt);
        EXPECT_EQ(nd.str("abcde"_str), "c"_str);
        EXPECT_EQ(nd.tkn(), std::make_optional(taul::token::normal(gram, "lpr"_str, 2, 1)));
    }

    if (a.nodes() >= 5) {
        const auto& nd = a.at(4);
        EXPECT_EQ(nd.index(), 4);
        EXPECT_EQ(nd.level(), 1);

        EXPECT_EQ(nd.parent(), std::next(a.begin(), 0));
        EXPECT_EQ(nd.left_sibling(), std::next(a.begin(), 2));
        EXPECT_EQ(nd.right_sibling(), a.end());
        EXPECT_EQ(nd.left_child(), a.end());
        EXPECT_EQ(nd.right_child(), a.end());
        EXPECT_TRUE(nd.has_parent());
        EXPECT_TRUE(nd.has_left_sibling());
        EXPECT_FALSE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 0);
        EXPECT_FALSE(nd.has_children());

        EXPECT_TRUE(nd.is_lexical());
        EXPECT_FALSE(nd.is_syntactic());
        EXPECT_TRUE(nd.is_normal());
        EXPECT_FALSE(nd.is_failure());
        EXPECT_FALSE(nd.is_end());

        EXPECT_EQ(nd.id(), gram.lpr("lpr"_str).value().id());
        EXPECT_EQ(nd.pos(), 3);
        EXPECT_EQ(nd.len(), 1);
        EXPECT_EQ(nd.low_pos(), 3);
        EXPECT_EQ(nd.high_pos(), 4);
        EXPECT_EQ(nd.lpr(), gram.lpr("lpr"_str));
        EXPECT_EQ(nd.ppr(), std::nullopt);
        EXPECT_EQ(nd.str("abcde"_str), "d"_str);
        EXPECT_EQ(nd.tkn(), std::make_optional(taul::token::normal(gram, "lpr"_str, 3, 1)));
    }
}

TEST_F(ParseTreeTests, Construction_SyntacticNodeAsRoot_Skip) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree(gram)
        .syntactic(gram.ppr("ppr"_str).value(), 1)
        .skip(2)
        .lexical(taul::token::normal(gram, "lpr"_str, 3, 1))
        .skip(2)
        .close();

    ASSERT_TRUE(a.is_sealed());

    EXPECT_FALSE(a.is_aborted());

    constexpr size_t expected_nodes = 2;

    EXPECT_EQ(a.nodes(), expected_nodes);
    EXPECT_TRUE(a.has_nodes());

    EXPECT_EQ(std::distance(a.cbegin(), a.cend()), expected_nodes);
    EXPECT_EQ(std::distance(a.begin(), a.end()), expected_nodes);

    if (a.has_nodes()) {
        EXPECT_EQ(&(a.root()), &(a.at(0)));
    }

    EXPECT_THROW(a.at(expected_nodes), std::out_of_range);

    if (a.nodes() >= 1) {
        const auto& nd = a.at(0);
        EXPECT_EQ(nd.index(), 0);
        EXPECT_EQ(nd.level(), 0);

        EXPECT_EQ(nd.parent(), a.end());
        EXPECT_EQ(nd.left_sibling(), a.end());
        EXPECT_EQ(nd.right_sibling(), a.end());
        EXPECT_EQ(nd.left_child(), std::next(a.begin(), 1));
        EXPECT_EQ(nd.right_child(), std::next(a.begin(), 1));
        EXPECT_FALSE(nd.has_parent());
        EXPECT_FALSE(nd.has_left_sibling());
        EXPECT_FALSE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 1);
        EXPECT_TRUE(nd.has_children());

        EXPECT_FALSE(nd.is_lexical());
        EXPECT_TRUE(nd.is_syntactic());
        EXPECT_TRUE(nd.is_normal());
        EXPECT_FALSE(nd.is_failure());
        EXPECT_FALSE(nd.is_end());

        EXPECT_EQ(nd.id(), gram.ppr("ppr"_str).value().id());
        EXPECT_EQ(nd.pos(), 1);
        EXPECT_EQ(nd.len(), 5);
        EXPECT_EQ(nd.low_pos(), 1);
        EXPECT_EQ(nd.high_pos(), 6);
        EXPECT_EQ(nd.lpr(), std::nullopt);
        EXPECT_EQ(nd.ppr(), gram.ppr("ppr"_str));
        EXPECT_EQ(nd.str("abcdefg"_str), "bcdef"_str);
        EXPECT_EQ(nd.tkn(), std::nullopt);
    }

    if (a.nodes() >= 2) {
        const auto& nd = a.at(1);
        EXPECT_EQ(nd.index(), 1);
        EXPECT_EQ(nd.level(), 1);

        EXPECT_EQ(nd.parent(), std::next(a.begin(), 0));
        EXPECT_EQ(nd.left_sibling(), a.end());
        EXPECT_EQ(nd.right_sibling(), a.end());
        EXPECT_EQ(nd.left_child(), a.end());
        EXPECT_EQ(nd.right_child(), a.end());
        EXPECT_TRUE(nd.has_parent());
        EXPECT_FALSE(nd.has_left_sibling());
        EXPECT_FALSE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 0);
        EXPECT_FALSE(nd.has_children());

        EXPECT_TRUE(nd.is_lexical());
        EXPECT_FALSE(nd.is_syntactic());
        EXPECT_TRUE(nd.is_normal());
        EXPECT_FALSE(nd.is_failure());
        EXPECT_FALSE(nd.is_end());

        EXPECT_EQ(nd.id(), gram.lpr("lpr"_str).value().id());
        EXPECT_EQ(nd.pos(), 3);
        EXPECT_EQ(nd.len(), 1);
        EXPECT_EQ(nd.low_pos(), 3);
        EXPECT_EQ(nd.high_pos(), 4);
        EXPECT_EQ(nd.lpr(), gram.lpr("lpr"_str));
        EXPECT_EQ(nd.ppr(), std::nullopt);
        EXPECT_EQ(nd.str("abcdefg"_str), "d"_str);
        EXPECT_EQ(nd.tkn(), std::make_optional(taul::token::normal(gram, "lpr"_str, 3, 1)));
    }
}

TEST_F(ParseTreeTests, Construction_SyntacticNodeAsRoot_ImplicitSkipDueToLowPosOfNewChildNodeExceedingCurrentHighPosOfParentNode) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree(gram)
        .syntactic(gram.ppr("ppr"_str).value(), 1)
        .lexical(taul::token::normal(gram, "lpr"_str, 9, 1))
        .close();

    ASSERT_TRUE(a.is_sealed());

    EXPECT_FALSE(a.is_aborted());

    constexpr size_t expected_nodes = 2;

    EXPECT_EQ(a.nodes(), expected_nodes);
    EXPECT_TRUE(a.has_nodes());

    EXPECT_EQ(std::distance(a.cbegin(), a.cend()), expected_nodes);
    EXPECT_EQ(std::distance(a.begin(), a.end()), expected_nodes);

    if (a.has_nodes()) {
        EXPECT_EQ(&(a.root()), &(a.at(0)));
    }

    EXPECT_THROW(a.at(expected_nodes), std::out_of_range);

    if (a.nodes() >= 1) {
        const auto& nd = a.at(0);
        EXPECT_EQ(nd.index(), 0);
        EXPECT_EQ(nd.level(), 0);

        EXPECT_EQ(nd.parent(), a.end());
        EXPECT_EQ(nd.left_sibling(), a.end());
        EXPECT_EQ(nd.right_sibling(), a.end());
        EXPECT_EQ(nd.left_child(), std::next(a.begin(), 1));
        EXPECT_EQ(nd.right_child(), std::next(a.begin(), 1));
        EXPECT_FALSE(nd.has_parent());
        EXPECT_FALSE(nd.has_left_sibling());
        EXPECT_FALSE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 1);
        EXPECT_TRUE(nd.has_children());

        EXPECT_FALSE(nd.is_lexical());
        EXPECT_TRUE(nd.is_syntactic());
        EXPECT_TRUE(nd.is_normal());
        EXPECT_FALSE(nd.is_failure());
        EXPECT_FALSE(nd.is_end());

        EXPECT_EQ(nd.id(), gram.ppr("ppr"_str).value().id());
        EXPECT_EQ(nd.pos(), 1);
        EXPECT_EQ(nd.len(), 9);
        EXPECT_EQ(nd.low_pos(), 1);
        EXPECT_EQ(nd.high_pos(), 10);
        EXPECT_EQ(nd.lpr(), std::nullopt);
        EXPECT_EQ(nd.ppr(), gram.ppr("ppr"_str));
        EXPECT_EQ(nd.str("abcdefghijk"_str), "bcdefghij"_str);
        EXPECT_EQ(nd.tkn(), std::nullopt);
    }

    if (a.nodes() >= 2) {
        const auto& nd = a.at(1);
        EXPECT_EQ(nd.index(), 1);
        EXPECT_EQ(nd.level(), 1);

        EXPECT_EQ(nd.parent(), std::next(a.begin(), 0));
        EXPECT_EQ(nd.left_sibling(), a.end());
        EXPECT_EQ(nd.right_sibling(), a.end());
        EXPECT_EQ(nd.left_child(), a.end());
        EXPECT_EQ(nd.right_child(), a.end());
        EXPECT_TRUE(nd.has_parent());
        EXPECT_FALSE(nd.has_left_sibling());
        EXPECT_FALSE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 0);
        EXPECT_FALSE(nd.has_children());

        EXPECT_TRUE(nd.is_lexical());
        EXPECT_FALSE(nd.is_syntactic());
        EXPECT_TRUE(nd.is_normal());
        EXPECT_FALSE(nd.is_failure());
        EXPECT_FALSE(nd.is_end());

        EXPECT_EQ(nd.id(), gram.lpr("lpr"_str).value().id());
        EXPECT_EQ(nd.pos(), 9);
        EXPECT_EQ(nd.len(), 1);
        EXPECT_EQ(nd.low_pos(), 9);
        EXPECT_EQ(nd.high_pos(), 10);
        EXPECT_EQ(nd.lpr(), gram.lpr("lpr"_str));
        EXPECT_EQ(nd.ppr(), std::nullopt);
        EXPECT_EQ(nd.str("abcdefghijk"_str), "j"_str);
        EXPECT_EQ(nd.tkn(), std::make_optional(taul::token::normal(gram, "lpr"_str, 9, 1)));
    }
}

TEST_F(ParseTreeTests, Construction_SyntacticNodeAsRoot_LowPosOfNewChildNodesMayBeLowerThanLowPosOfParentNode) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree(gram)
        .syntactic(gram.ppr("ppr"_str).value(), 5)
        .lexical(taul::token::normal(gram, "lpr"_str, 4, 1))
        .close();

    ASSERT_TRUE(a.is_sealed());

    EXPECT_FALSE(a.is_aborted());

    constexpr size_t expected_nodes = 2;

    EXPECT_EQ(a.nodes(), expected_nodes);
    EXPECT_TRUE(a.has_nodes());

    EXPECT_EQ(std::distance(a.cbegin(), a.cend()), expected_nodes);
    EXPECT_EQ(std::distance(a.begin(), a.end()), expected_nodes);

    if (a.has_nodes()) {
        EXPECT_EQ(&(a.root()), &(a.at(0)));
    }

    EXPECT_THROW(a.at(expected_nodes), std::out_of_range);

    if (a.nodes() >= 1) {
        const auto& nd = a.at(0);
        EXPECT_EQ(nd.index(), 0);
        EXPECT_EQ(nd.level(), 0);

        EXPECT_EQ(nd.parent(), a.end());
        EXPECT_EQ(nd.left_sibling(), a.end());
        EXPECT_EQ(nd.right_sibling(), a.end());
        EXPECT_EQ(nd.left_child(), std::next(a.begin(), 1));
        EXPECT_EQ(nd.right_child(), std::next(a.begin(), 1));
        EXPECT_FALSE(nd.has_parent());
        EXPECT_FALSE(nd.has_left_sibling());
        EXPECT_FALSE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 1);
        EXPECT_TRUE(nd.has_children());

        EXPECT_FALSE(nd.is_lexical());
        EXPECT_TRUE(nd.is_syntactic());
        EXPECT_TRUE(nd.is_normal());
        EXPECT_FALSE(nd.is_failure());
        EXPECT_FALSE(nd.is_end());

        EXPECT_EQ(nd.id(), gram.ppr("ppr"_str).value().id());
        EXPECT_EQ(nd.pos(), 5);
        EXPECT_EQ(nd.len(), 0);
        EXPECT_EQ(nd.low_pos(), 5);
        EXPECT_EQ(nd.high_pos(), 5);
        EXPECT_EQ(nd.lpr(), std::nullopt);
        EXPECT_EQ(nd.ppr(), gram.ppr("ppr"_str));
        EXPECT_EQ(nd.str("abcdef"_str), ""_str);
        EXPECT_EQ(nd.tkn(), std::nullopt);
    }

    if (a.nodes() >= 2) {
        const auto& nd = a.at(1);
        EXPECT_EQ(nd.index(), 1);
        EXPECT_EQ(nd.level(), 1);

        EXPECT_EQ(nd.parent(), std::next(a.begin(), 0));
        EXPECT_EQ(nd.left_sibling(), a.end());
        EXPECT_EQ(nd.right_sibling(), a.end());
        EXPECT_EQ(nd.left_child(), a.end());
        EXPECT_EQ(nd.right_child(), a.end());
        EXPECT_TRUE(nd.has_parent());
        EXPECT_FALSE(nd.has_left_sibling());
        EXPECT_FALSE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 0);
        EXPECT_FALSE(nd.has_children());

        EXPECT_TRUE(nd.is_lexical());
        EXPECT_FALSE(nd.is_syntactic());
        EXPECT_TRUE(nd.is_normal());
        EXPECT_FALSE(nd.is_failure());
        EXPECT_FALSE(nd.is_end());

        EXPECT_EQ(nd.id(), gram.lpr("lpr"_str).value().id());
        EXPECT_EQ(nd.pos(), 4);
        EXPECT_EQ(nd.len(), 1);
        EXPECT_EQ(nd.low_pos(), 4);
        EXPECT_EQ(nd.high_pos(), 5);
        EXPECT_EQ(nd.lpr(), gram.lpr("lpr"_str));
        EXPECT_EQ(nd.ppr(), std::nullopt);
        EXPECT_EQ(nd.str("abcdef"_str), "e"_str);
        EXPECT_EQ(nd.tkn(), std::make_optional(taul::token::normal(gram, "lpr"_str, 4, 1)));
    }
}

TEST_F(ParseTreeTests, Construction_SyntacticNodeAsRoot_Abort) {
    ASSERT_TRUE(ready);

    auto a =
        taul::parse_tree(gram)
        .syntactic(gram.ppr("ppr"_str).value(), 1)
        .abort()
        .close();

    ASSERT_TRUE(a.is_sealed());

    EXPECT_TRUE(a.is_aborted());

    constexpr size_t expected_nodes = 1;

    EXPECT_EQ(a.nodes(), expected_nodes);
    EXPECT_TRUE(a.has_nodes());

    EXPECT_EQ(std::distance(a.cbegin(), a.cend()), expected_nodes);
    EXPECT_EQ(std::distance(a.begin(), a.end()), expected_nodes);

    if (a.has_nodes()) {
        EXPECT_EQ(&(a.root()), &(a.at(0)));
    }

    EXPECT_THROW(a.at(expected_nodes), std::out_of_range);

    if (a.nodes() >= 1) {
        const auto& nd = a.at(0);
        EXPECT_EQ(nd.index(), 0);
        EXPECT_EQ(nd.level(), 0);

        EXPECT_EQ(nd.parent(), a.end());
        EXPECT_EQ(nd.left_sibling(), a.end());
        EXPECT_EQ(nd.right_sibling(), a.end());
        EXPECT_EQ(nd.left_child(), a.end());
        EXPECT_EQ(nd.right_child(), a.end());
        EXPECT_FALSE(nd.has_parent());
        EXPECT_FALSE(nd.has_left_sibling());
        EXPECT_FALSE(nd.has_right_sibling());

        EXPECT_EQ(nd.children(), 0);
        EXPECT_FALSE(nd.has_children());

        EXPECT_FALSE(nd.is_lexical());
        EXPECT_TRUE(nd.is_syntactic());
        EXPECT_TRUE(nd.is_normal());
        EXPECT_FALSE(nd.is_failure());
        EXPECT_FALSE(nd.is_end());

        EXPECT_EQ(nd.id(), gram.ppr("ppr"_str).value().id());
        EXPECT_EQ(nd.pos(), 1);
        EXPECT_EQ(nd.len(), 0);
        EXPECT_EQ(nd.low_pos(), 1);
        EXPECT_EQ(nd.high_pos(), 1);
        EXPECT_EQ(nd.lpr(), std::nullopt);
        EXPECT_EQ(nd.ppr(), gram.ppr("ppr"_str));
        EXPECT_EQ(nd.str("abc"_str), ""_str);
        EXPECT_EQ(nd.tkn(), std::nullopt);
    }
}

