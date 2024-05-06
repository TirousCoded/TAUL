

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


TEST_F(ParseTreeTests, DefaultCtor) {
    ASSERT_TRUE(ready);
    taul::parse_tree pt(gram);

    EXPECT_FALSE(pt.is_sealed());
    EXPECT_FALSE(pt.is_aborted());

    EXPECT_EQ(pt.nodes(), 0);
    EXPECT_FALSE(pt.has_nodes());

    EXPECT_THROW(pt.at(0), std::out_of_range);
    EXPECT_THROW(pt.current(), std::out_of_range);
    EXPECT_THROW(pt.root(), std::out_of_range);

    EXPECT_EQ(pt.cbegin(), pt.cend());
    EXPECT_EQ(pt.begin(), pt.end());
}

TEST_F(ParseTreeTests, CopyCtor) {
    ASSERT_TRUE(ready);
    taul::parse_tree pt(gram);
    pt
        .syntactic("ppr"_str, 14)
        .lexical("lpr"_str, 14, 3)
        .close();

    taul::parse_tree copied(pt);
    
    taul::parse_tree expected(gram);
    expected
        .syntactic("ppr"_str, 14)
        .lexical("lpr"_str, 14, 3)
        .close();

    // this expects equality to work as expected

    EXPECT_EQ(copied, expected);
}

TEST_F(ParseTreeTests, MoveCtor) {
    ASSERT_TRUE(ready);
    taul::parse_tree pt(gram);
    pt
        .syntactic("ppr"_str, 14)
        .lexical("lpr"_str, 14, 3)
        .close();

    taul::parse_tree moved(std::move(pt));

    taul::parse_tree expected(gram);
    expected
        .syntactic("ppr"_str, 14)
        .lexical("lpr"_str, 14, 3)
        .close();

    // this expects equality to work as expected

    EXPECT_EQ(moved, expected);
}

TEST_F(ParseTreeTests, CopyAssign) {
    ASSERT_TRUE(ready);
    taul::parse_tree pt(gram);
    pt
        .syntactic("ppr"_str, 14)
        .lexical("lpr"_str, 14, 3)
        .close();

    taul::parse_tree copied_to(gram);
    copied_to
        .syntactic("ppr"_str, 2)
        .close();

    copied_to = pt;

    taul::parse_tree expected(gram);
    expected
        .syntactic("ppr"_str, 14)
        .lexical("lpr"_str, 14, 3)
        .close();

    // this expects equality to work as expected

    EXPECT_EQ(copied_to, expected);
}

TEST_F(ParseTreeTests, MoveAssign) {
    ASSERT_TRUE(ready);
    taul::parse_tree pt(gram);
    pt
        .syntactic("ppr"_str, 14)
        .lexical("lpr"_str, 14, 3)
        .close();

    taul::parse_tree moved_to(gram);
    moved_to
        .syntactic("ppr"_str, 2)
        .close();

    moved_to = std::move(pt);

    taul::parse_tree expected(gram);
    expected
        .syntactic("ppr"_str, 14)
        .lexical("lpr"_str, 14, 3)
        .close();

    // this expects equality to work as expected

    EXPECT_EQ(moved_to, expected);
}

TEST_F(ParseTreeTests, MoveAssignOntoSelf) {
    ASSERT_TRUE(ready);
    taul::parse_tree pt(gram);
    pt
        .syntactic("ppr"_str, 14)
        .lexical("lpr"_str, 14, 3)
        .close();

    pt = std::move(pt);

    taul::parse_tree expected(gram);
    expected
        .syntactic("ppr"_str, 14)
        .lexical("lpr"_str, 14, 3)
        .close();

    // this expects equality to work as expected

    EXPECT_EQ(pt, expected);
}

TEST_F(ParseTreeTests, Iterators) {
    ASSERT_TRUE(ready);
    taul::parse_tree pt(gram);
    pt
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .syntactic("ppr"_str, 1)
        .syntactic("ppr"_str, 1)
        .lexical("lpr"_str, 1, 1)
        .close()
        .syntactic("ppr"_str, 2)
        .lexical("lpr"_str, 2, 1)
        .close()
        .close()
        .close();

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
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .syntactic("ppr"_str, 1)
        .lexical("lpr"_str, 1, 1)
        .lexical("lpr"_str, 2, 1)
        .close()
        .close();
    // tree0 and tree1 are structurally equivalent
    taul::parse_tree tree1(gram);
    tree1
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .syntactic("ppr"_str, 1)
        .lexical("lpr"_str, 1, 1)
        .lexical("lpr"_str, 2, 1)
        .close()
        .close();
    taul::parse_tree tree2(gram);
    tree2
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .close();
    // tree2 and tree3 are structurally equivalent
    taul::parse_tree tree3(gram);
    tree3
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .close();

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

TEST_F(ParseTreeTests, Equality_NonIndifferenceToIsSealed) {
    ASSERT_TRUE(ready);
    taul::parse_tree tree0(gram);
    tree0
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .syntactic("ppr"_str, 1)
        .lexical("lpr"_str, 1, 1)
        .lexical("lpr"_str, 2, 1)
        .close()
        .close();
    // tree0 and tree1 are NOT structurally equivalent
    taul::parse_tree tree1(gram);
    tree1
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .syntactic("ppr"_str, 1)
        .lexical("lpr"_str, 1, 1)
        .lexical("lpr"_str, 2, 1);
        //.close() // <- shouldn't matter
        //.close(); // <- shouldn't matter

    ASSERT_TRUE(tree0.is_sealed());
    ASSERT_FALSE(tree1.is_sealed());

    EXPECT_FALSE(tree0.equal(tree1));
    EXPECT_FALSE(tree1.equal(tree0));

    EXPECT_FALSE(tree0 == tree1);
    EXPECT_FALSE(tree1 == tree0);

    EXPECT_TRUE(tree0 != tree1);
    EXPECT_TRUE(tree1 != tree0);
}

TEST_F(ParseTreeTests, Equality_IndifferenceToIsAborted) {
    ASSERT_TRUE(ready);
    taul::parse_tree tree0(gram);
    tree0
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .syntactic("ppr"_str, 1)
        .lexical("lpr"_str, 1, 1)
        .lexical("lpr"_str, 2, 1)
        .close()
        .close();
    // tree0 and tree1 are structurally equivalent
    taul::parse_tree tree1(gram);
    tree1
        .syntactic("ppr"_str, 0)
        .lexical("lpr"_str, 0, 1)
        .syntactic("ppr"_str, 1)
        .lexical("lpr"_str, 1, 1)
        .lexical("lpr"_str, 2, 1)
        .abort() // <- shouldn't matter
        .close()
        .close();

    ASSERT_FALSE(tree0.is_aborted());
    ASSERT_TRUE(tree1.is_aborted());

    EXPECT_TRUE(tree0.equal(tree1));
    EXPECT_TRUE(tree1.equal(tree0));

    EXPECT_TRUE(tree0 == tree1);
    EXPECT_TRUE(tree1 == tree0);

    EXPECT_FALSE(tree0 != tree1);
    EXPECT_FALSE(tree1 != tree0);
}

TEST_F(ParseTreeTests, LexicalNodeAsRoot) {
    ASSERT_TRUE(ready);

    taul::str src = "abcde"_str;

    taul::parse_tree pt(gram);

    {
        EXPECT_FALSE(pt.is_sealed());
        EXPECT_FALSE(pt.is_aborted());

        EXPECT_EQ(pt.nodes(), 0);
        EXPECT_FALSE(pt.has_nodes());

        EXPECT_THROW(pt.at(0), std::out_of_range);
        EXPECT_THROW(pt.current(), std::out_of_range);
        EXPECT_THROW(pt.root(), std::out_of_range);

        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 0);
    }

    pt.lexical("lpr"_str, 0, 1);

    {
        EXPECT_TRUE(pt.is_sealed());
        EXPECT_FALSE(pt.is_aborted());

        EXPECT_EQ(pt.nodes(), 1);
        EXPECT_TRUE(pt.has_nodes());

        if (pt.nodes() == 1) {
            EXPECT_EQ(pt.at(0).index(), 0);

            EXPECT_EQ(pt.at(0).level(), 0);

            EXPECT_EQ(pt.at(0).parent(), pt.end());
            EXPECT_EQ(pt.at(0).left_sibling(), pt.end());
            EXPECT_EQ(pt.at(0).right_sibling(), pt.end());
            EXPECT_EQ(pt.at(0).left_child(), pt.end());
            EXPECT_EQ(pt.at(0).right_child(), pt.end());

            EXPECT_FALSE(pt.at(0).has_parent());
            EXPECT_FALSE(pt.at(0).has_left_sibling());
            EXPECT_FALSE(pt.at(0).has_right_sibling());
            EXPECT_EQ(pt.at(0).children(), 0);
            EXPECT_FALSE(pt.at(0).has_children());

            EXPECT_TRUE(pt.at(0).is_lexical());
            EXPECT_FALSE(pt.at(0).is_syntactic());
            EXPECT_TRUE(pt.at(0).is_normal());
            EXPECT_FALSE(pt.at(0).is_failure());

            EXPECT_EQ(pt.at(0).id(), gram.lpr("lpr"_str).value().id());
            EXPECT_EQ(pt.at(0).pos(), 0);
            EXPECT_EQ(pt.at(0).len(), 1);

            EXPECT_TRUE(pt.at(0).lpr());
            EXPECT_FALSE(pt.at(0).ppr());
            if (pt.at(0).lpr()) EXPECT_EQ(pt.at(0).lpr().value(), gram.lpr("lpr"_str).value());

            EXPECT_EQ(pt.at(0).str(src), "a"_str);
            EXPECT_TRUE(pt.at(0).tkn());
            if (pt.at(0).tkn()) EXPECT_EQ(pt.at(0).tkn().value(), taul::token::normal(gram, "lpr"_str, 0, 1));
        }

        EXPECT_THROW(&(pt.current()), std::out_of_range);
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(1), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 1);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 1);
    }
}

// TODO: the below tests are *bloated*, and REALLY should be split up into
//       a collection of unit tests testing each small bit in isolation

// TODO: also, we don't really test 'abort'

TEST_F(ParseTreeTests, Skip) {
    ASSERT_TRUE(ready);

    taul::str src = "abcde"_str;

    taul::parse_tree pt(gram);

    {
        EXPECT_FALSE(pt.is_sealed());
        EXPECT_FALSE(pt.is_aborted());

        EXPECT_EQ(pt.nodes(), 0);
        EXPECT_FALSE(pt.has_nodes());

        EXPECT_THROW(pt.at(0), std::out_of_range);
        EXPECT_THROW(pt.current(), std::out_of_range);
        EXPECT_THROW(pt.root(), std::out_of_range);

        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 0);
    }

    pt.syntactic("ppr"_str, 0);

    {
        EXPECT_FALSE(pt.is_sealed());
        EXPECT_FALSE(pt.is_aborted());

        EXPECT_EQ(pt.nodes(), 1);
        EXPECT_TRUE(pt.has_nodes());

        if (pt.nodes() == 1) {
            {
                EXPECT_EQ(pt.at(0).index(), 0);

                EXPECT_EQ(pt.at(0).level(), 0);

                EXPECT_EQ(pt.at(0).parent(), pt.end());
                EXPECT_EQ(pt.at(0).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).left_child(), pt.end());
                EXPECT_EQ(pt.at(0).right_child(), pt.end());

                EXPECT_FALSE(pt.at(0).has_parent());
                EXPECT_FALSE(pt.at(0).has_left_sibling());
                EXPECT_FALSE(pt.at(0).has_right_sibling());
                EXPECT_EQ(pt.at(0).children(), 0);
                EXPECT_FALSE(pt.at(0).has_children());

                EXPECT_FALSE(pt.at(0).is_lexical());
                EXPECT_TRUE(pt.at(0).is_syntactic());
                EXPECT_TRUE(pt.at(0).is_normal());
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_EQ(pt.at(0).id(), gram.ppr("ppr"_str).value().id());
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).len(), 0);

                EXPECT_FALSE(pt.at(0).lpr());
                EXPECT_TRUE(pt.at(0).ppr());
                if (pt.at(0).ppr()) EXPECT_EQ(pt.at(0).ppr().value(), gram.ppr("ppr"_str).value());

                EXPECT_EQ(pt.at(0).str(src), ""_str);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);
            }
        }
        else ADD_FAILURE();

        EXPECT_EQ(&(pt.current()), &(pt.at(0)));
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(1), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 1);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 1);
    }

    pt.skip(5);

    {
        EXPECT_FALSE(pt.is_sealed());
        EXPECT_FALSE(pt.is_aborted());

        EXPECT_EQ(pt.nodes(), 1);
        EXPECT_TRUE(pt.has_nodes());

        if (pt.nodes() == 1) {
            {
                EXPECT_EQ(pt.at(0).index(), 0);

                EXPECT_EQ(pt.at(0).level(), 0);

                EXPECT_EQ(pt.at(0).parent(), pt.end());
                EXPECT_EQ(pt.at(0).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).left_child(), pt.end());
                EXPECT_EQ(pt.at(0).right_child(), pt.end());

                EXPECT_FALSE(pt.at(0).has_parent());
                EXPECT_FALSE(pt.at(0).has_left_sibling());
                EXPECT_FALSE(pt.at(0).has_right_sibling());
                EXPECT_EQ(pt.at(0).children(), 0);
                EXPECT_FALSE(pt.at(0).has_children());

                EXPECT_FALSE(pt.at(0).is_lexical());
                EXPECT_TRUE(pt.at(0).is_syntactic());
                EXPECT_TRUE(pt.at(0).is_normal());
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_EQ(pt.at(0).id(), gram.ppr("ppr"_str).value().id());
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).len(), 5);

                EXPECT_FALSE(pt.at(0).lpr());
                EXPECT_TRUE(pt.at(0).ppr());
                if (pt.at(0).ppr()) EXPECT_EQ(pt.at(0).ppr().value(), gram.ppr("ppr"_str).value());

                EXPECT_EQ(pt.at(0).str(src), "abcde"_str);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);
            }
        }
        else ADD_FAILURE();

        EXPECT_EQ(&(pt.current()), &(pt.at(0)));
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(1), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 1);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 1);
    }

    pt.close();

    {
        EXPECT_TRUE(pt.is_sealed());
        EXPECT_FALSE(pt.is_aborted());

        EXPECT_EQ(pt.nodes(), 1);
        EXPECT_TRUE(pt.has_nodes());

        if (pt.nodes() == 1) {
            {
                EXPECT_EQ(pt.at(0).index(), 0);

                EXPECT_EQ(pt.at(0).level(), 0);

                EXPECT_EQ(pt.at(0).parent(), pt.end());
                EXPECT_EQ(pt.at(0).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).left_child(), pt.end());
                EXPECT_EQ(pt.at(0).right_child(), pt.end());

                EXPECT_FALSE(pt.at(0).has_parent());
                EXPECT_FALSE(pt.at(0).has_left_sibling());
                EXPECT_FALSE(pt.at(0).has_right_sibling());
                EXPECT_EQ(pt.at(0).children(), 0);
                EXPECT_FALSE(pt.at(0).has_children());

                EXPECT_FALSE(pt.at(0).is_lexical());
                EXPECT_TRUE(pt.at(0).is_syntactic());
                EXPECT_TRUE(pt.at(0).is_normal());
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_EQ(pt.at(0).id(), gram.ppr("ppr"_str).value().id());
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).len(), 5);

                EXPECT_FALSE(pt.at(0).lpr());
                EXPECT_TRUE(pt.at(0).ppr());
                if (pt.at(0).ppr()) EXPECT_EQ(pt.at(0).ppr().value(), gram.ppr("ppr"_str).value());

                EXPECT_EQ(pt.at(0).str(src), "abcde"_str);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);
            }
        }
        else ADD_FAILURE();

        EXPECT_THROW(&(pt.current()), std::out_of_range);
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(1), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 1);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 1);
    }
}

TEST_F(ParseTreeTests, BasicTreeConstruction) {
    ASSERT_TRUE(ready);
    taul::parse_tree pt(gram);

    taul::str src = "abc"_str;

    {
        EXPECT_FALSE(pt.is_sealed());
        EXPECT_FALSE(pt.is_aborted());

        EXPECT_EQ(pt.nodes(), 0);
        EXPECT_FALSE(pt.has_nodes());

        EXPECT_THROW(pt.at(0), std::out_of_range);
        EXPECT_THROW(pt.current(), std::out_of_range);
        EXPECT_THROW(pt.root(), std::out_of_range);

        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 0);
    }

    pt.syntactic("ppr"_str, 0);

    {
        EXPECT_FALSE(pt.is_sealed());
        EXPECT_FALSE(pt.is_aborted());

        EXPECT_EQ(pt.nodes(), 1);
        EXPECT_TRUE(pt.has_nodes());

        if (pt.nodes() == 1) {
            {
                EXPECT_EQ(pt.at(0).index(), 0);

                EXPECT_EQ(pt.at(0).level(), 0);

                EXPECT_EQ(pt.at(0).parent(), pt.end());
                EXPECT_EQ(pt.at(0).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).left_child(), pt.end());
                EXPECT_EQ(pt.at(0).right_child(), pt.end());

                EXPECT_FALSE(pt.at(0).has_parent());
                EXPECT_FALSE(pt.at(0).has_left_sibling());
                EXPECT_FALSE(pt.at(0).has_right_sibling());
                EXPECT_EQ(pt.at(0).children(), 0);
                EXPECT_FALSE(pt.at(0).has_children());

                EXPECT_FALSE(pt.at(0).is_lexical());
                EXPECT_TRUE(pt.at(0).is_syntactic());
                EXPECT_TRUE(pt.at(0).is_normal());
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_EQ(pt.at(0).id(), gram.ppr("ppr"_str).value().id());
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).len(), 0);

                EXPECT_FALSE(pt.at(0).lpr());
                EXPECT_TRUE(pt.at(0).ppr());
                if (pt.at(0).ppr()) EXPECT_EQ(pt.at(0).ppr().value(), gram.ppr("ppr"_str).value());
                
                EXPECT_EQ(pt.at(0).str(src), ""_str);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);
            }
        }
        else ADD_FAILURE();

        EXPECT_EQ(&(pt.current()), &(pt.at(0)));
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(1), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 1);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 1);
    }

    pt.lexical("lpr"_str, 0, 1);

    {
        EXPECT_FALSE(pt.is_sealed());
        EXPECT_FALSE(pt.is_aborted());

        EXPECT_EQ(pt.nodes(), 2);
        EXPECT_TRUE(pt.has_nodes());

        if (pt.nodes() == 2) {
            {
                EXPECT_EQ(pt.at(0).index(), 0);
                EXPECT_EQ(pt.at(1).index(), 1);

                EXPECT_EQ(pt.at(0).level(), 0);
                EXPECT_EQ(pt.at(1).level(), 1);

                EXPECT_EQ(pt.at(0).parent(), pt.end());
                EXPECT_EQ(pt.at(0).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).left_child(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(0).right_child(), std::next(pt.begin(), 1));

                EXPECT_EQ(pt.at(1).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(1).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(1).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(1).left_child(), pt.end());
                EXPECT_EQ(pt.at(1).right_child(), pt.end());

                EXPECT_FALSE(pt.at(0).has_parent());
                EXPECT_FALSE(pt.at(0).has_left_sibling());
                EXPECT_FALSE(pt.at(0).has_right_sibling());
                EXPECT_EQ(pt.at(0).children(), 1);
                EXPECT_TRUE(pt.at(0).has_children());

                EXPECT_TRUE(pt.at(1).has_parent());
                EXPECT_FALSE(pt.at(1).has_left_sibling());
                EXPECT_FALSE(pt.at(1).has_right_sibling());
                EXPECT_EQ(pt.at(1).children(), 0);
                EXPECT_FALSE(pt.at(1).has_children());

                EXPECT_FALSE(pt.at(0).is_lexical());
                EXPECT_TRUE(pt.at(0).is_syntactic());
                EXPECT_TRUE(pt.at(0).is_normal());
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_TRUE(pt.at(1).is_lexical());
                EXPECT_FALSE(pt.at(1).is_syntactic());
                EXPECT_TRUE(pt.at(1).is_normal());
                EXPECT_FALSE(pt.at(1).is_failure());

                EXPECT_EQ(pt.at(0).id(), gram.ppr("ppr"_str).value().id());
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).len(), 1);

                EXPECT_FALSE(pt.at(0).lpr());
                EXPECT_TRUE(pt.at(0).ppr());
                if (pt.at(0).ppr()) EXPECT_EQ(pt.at(0).ppr().value(), gram.ppr("ppr"_str).value());

                EXPECT_EQ(pt.at(0).str(src), "a"_str);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);

                EXPECT_EQ(pt.at(1).id(), gram.lpr("lpr"_str).value().id());
                EXPECT_EQ(pt.at(1).pos(), 0);
                EXPECT_EQ(pt.at(1).len(), 1);

                EXPECT_TRUE(pt.at(1).lpr());
                EXPECT_FALSE(pt.at(1).ppr());
                if (pt.at(1).lpr()) EXPECT_EQ(pt.at(1).lpr().value(), gram.lpr("lpr"_str).value());

                EXPECT_EQ(pt.at(1).str(src), "a"_str);
                EXPECT_TRUE(pt.at(1).tkn());
                if (pt.at(1).tkn()) EXPECT_EQ(pt.at(1).tkn().value(), taul::token::normal(gram, "lpr"_str, 0, 1));
            }
        }
        else ADD_FAILURE();

        EXPECT_EQ(&(pt.current()), &(pt.at(0)));
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(2), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 2);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 2);
    }

    pt.syntactic("ppr"_str, 1);

    {
        EXPECT_FALSE(pt.is_sealed());
        EXPECT_FALSE(pt.is_aborted());

        EXPECT_EQ(pt.nodes(), 3);
        EXPECT_TRUE(pt.has_nodes());

        if (pt.nodes() == 3) {
            {
                EXPECT_EQ(pt.at(0).index(), 0);
                EXPECT_EQ(pt.at(1).index(), 1);
                EXPECT_EQ(pt.at(2).index(), 2);

                EXPECT_EQ(pt.at(0).level(), 0);
                EXPECT_EQ(pt.at(1).level(), 1);
                EXPECT_EQ(pt.at(2).level(), 1);

                EXPECT_EQ(pt.at(0).parent(), pt.end());
                EXPECT_EQ(pt.at(0).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).left_child(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(0).right_child(), std::next(pt.begin(), 2));

                EXPECT_EQ(pt.at(1).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(1).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(1).right_sibling(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(1).left_child(), pt.end());
                EXPECT_EQ(pt.at(1).right_child(), pt.end());
                
                EXPECT_EQ(pt.at(2).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(2).left_sibling(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(2).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(2).left_child(), pt.end());
                EXPECT_EQ(pt.at(2).right_child(), pt.end());

                EXPECT_FALSE(pt.at(0).has_parent());
                EXPECT_FALSE(pt.at(0).has_left_sibling());
                EXPECT_FALSE(pt.at(0).has_right_sibling());
                EXPECT_EQ(pt.at(0).children(), 2);
                EXPECT_TRUE(pt.at(0).has_children());

                EXPECT_TRUE(pt.at(1).has_parent());
                EXPECT_FALSE(pt.at(1).has_left_sibling());
                EXPECT_TRUE(pt.at(1).has_right_sibling());
                EXPECT_EQ(pt.at(1).children(), 0);
                EXPECT_FALSE(pt.at(1).has_children());
                
                EXPECT_TRUE(pt.at(2).has_parent());
                EXPECT_TRUE(pt.at(2).has_left_sibling());
                EXPECT_FALSE(pt.at(2).has_right_sibling());
                EXPECT_EQ(pt.at(2).children(), 0);
                EXPECT_FALSE(pt.at(2).has_children());

                EXPECT_FALSE(pt.at(0).is_lexical());
                EXPECT_TRUE(pt.at(0).is_syntactic());
                EXPECT_TRUE(pt.at(0).is_normal());
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_TRUE(pt.at(1).is_lexical());
                EXPECT_FALSE(pt.at(1).is_syntactic());
                EXPECT_TRUE(pt.at(1).is_normal());
                EXPECT_FALSE(pt.at(1).is_failure());
                
                EXPECT_FALSE(pt.at(2).is_lexical());
                EXPECT_TRUE(pt.at(2).is_syntactic());
                EXPECT_TRUE(pt.at(2).is_normal());
                EXPECT_FALSE(pt.at(2).is_failure());

                EXPECT_EQ(pt.at(0).id(), gram.ppr("ppr"_str).value().id());
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).len(), 1);

                EXPECT_FALSE(pt.at(0).lpr());
                EXPECT_TRUE(pt.at(0).ppr());
                if (pt.at(0).ppr()) EXPECT_EQ(pt.at(0).ppr().value(), gram.ppr("ppr"_str).value());

                EXPECT_EQ(pt.at(0).str(src), "a"_str);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);

                EXPECT_EQ(pt.at(1).id(), gram.lpr("lpr"_str).value().id());
                EXPECT_EQ(pt.at(1).pos(), 0);
                EXPECT_EQ(pt.at(1).len(), 1);

                EXPECT_TRUE(pt.at(1).lpr());
                EXPECT_FALSE(pt.at(1).ppr());
                if (pt.at(1).lpr()) EXPECT_EQ(pt.at(1).lpr().value(), gram.lpr("lpr"_str).value());

                EXPECT_EQ(pt.at(1).str(src), "a"_str);
                EXPECT_TRUE(pt.at(1).tkn());
                if (pt.at(1).tkn()) EXPECT_EQ(pt.at(1).tkn().value(), taul::token::normal(gram, "lpr"_str, 0, 1));

                EXPECT_EQ(pt.at(2).id(), gram.ppr("ppr"_str).value().id());
                EXPECT_EQ(pt.at(2).pos(), 1);
                EXPECT_EQ(pt.at(2).len(), 0);

                EXPECT_FALSE(pt.at(2).lpr());
                EXPECT_TRUE(pt.at(2).ppr());
                if (pt.at(2).ppr()) EXPECT_EQ(pt.at(2).ppr().value(), gram.ppr("ppr"_str).value());

                EXPECT_EQ(pt.at(2).str(src), ""_str);
                EXPECT_FALSE(pt.at(2).tkn());
            }
        }
        else ADD_FAILURE();

        EXPECT_EQ(&(pt.current()), &(pt.at(2)));
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(3), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 3);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 3);
    }

    pt.lexical(taul::token::normal(gram, "lpr"_str, 1, 1));

    {
        EXPECT_FALSE(pt.is_sealed());
        EXPECT_FALSE(pt.is_aborted());

        EXPECT_EQ(pt.nodes(), 4);
        EXPECT_TRUE(pt.has_nodes());

        if (pt.nodes() == 4) {
            {
                EXPECT_EQ(pt.at(0).index(), 0);
                EXPECT_EQ(pt.at(1).index(), 1);
                EXPECT_EQ(pt.at(2).index(), 2);
                EXPECT_EQ(pt.at(3).index(), 3);

                EXPECT_EQ(pt.at(0).level(), 0);
                EXPECT_EQ(pt.at(1).level(), 1);
                EXPECT_EQ(pt.at(2).level(), 1);
                EXPECT_EQ(pt.at(3).level(), 2);

                EXPECT_EQ(pt.at(0).parent(), pt.end());
                EXPECT_EQ(pt.at(0).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).left_child(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(0).right_child(), std::next(pt.begin(), 2));

                EXPECT_EQ(pt.at(1).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(1).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(1).right_sibling(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(1).left_child(), pt.end());
                EXPECT_EQ(pt.at(1).right_child(), pt.end());

                EXPECT_EQ(pt.at(2).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(2).left_sibling(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(2).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(2).left_child(), std::next(pt.begin(), 3));
                EXPECT_EQ(pt.at(2).right_child(), std::next(pt.begin(), 3));
                
                EXPECT_EQ(pt.at(3).parent(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(3).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(3).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(3).left_child(), pt.end());
                EXPECT_EQ(pt.at(3).right_child(), pt.end());

                EXPECT_FALSE(pt.at(0).has_parent());
                EXPECT_FALSE(pt.at(0).has_left_sibling());
                EXPECT_FALSE(pt.at(0).has_right_sibling());
                EXPECT_EQ(pt.at(0).children(), 2);
                EXPECT_TRUE(pt.at(0).has_children());

                EXPECT_TRUE(pt.at(1).has_parent());
                EXPECT_FALSE(pt.at(1).has_left_sibling());
                EXPECT_TRUE(pt.at(1).has_right_sibling());
                EXPECT_EQ(pt.at(1).children(), 0);
                EXPECT_FALSE(pt.at(1).has_children());

                EXPECT_TRUE(pt.at(2).has_parent());
                EXPECT_TRUE(pt.at(2).has_left_sibling());
                EXPECT_FALSE(pt.at(2).has_right_sibling());
                EXPECT_EQ(pt.at(2).children(), 1);
                EXPECT_TRUE(pt.at(2).has_children());

                EXPECT_TRUE(pt.at(3).has_parent());
                EXPECT_FALSE(pt.at(3).has_left_sibling());
                EXPECT_FALSE(pt.at(3).has_right_sibling());
                EXPECT_EQ(pt.at(3).children(), 0);
                EXPECT_FALSE(pt.at(3).has_children());

                EXPECT_FALSE(pt.at(0).is_lexical());
                EXPECT_TRUE(pt.at(0).is_syntactic());
                EXPECT_TRUE(pt.at(0).is_normal());
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_TRUE(pt.at(1).is_lexical());
                EXPECT_FALSE(pt.at(1).is_syntactic());
                EXPECT_TRUE(pt.at(1).is_normal());
                EXPECT_FALSE(pt.at(1).is_failure());

                EXPECT_FALSE(pt.at(2).is_lexical());
                EXPECT_TRUE(pt.at(2).is_syntactic());
                EXPECT_TRUE(pt.at(2).is_normal());
                EXPECT_FALSE(pt.at(2).is_failure());
                
                EXPECT_TRUE(pt.at(3).is_lexical());
                EXPECT_FALSE(pt.at(3).is_syntactic());
                EXPECT_TRUE(pt.at(3).is_normal());
                EXPECT_FALSE(pt.at(3).is_failure());

                EXPECT_EQ(pt.at(0).id(), gram.ppr("ppr"_str).value().id());
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).len(), 1);

                EXPECT_FALSE(pt.at(0).lpr());
                EXPECT_TRUE(pt.at(0).ppr());
                if (pt.at(0).ppr()) EXPECT_EQ(pt.at(0).ppr().value(), gram.ppr("ppr"_str).value());

                EXPECT_EQ(pt.at(0).str(src), "a"_str);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);

                EXPECT_EQ(pt.at(1).id(), gram.lpr("lpr"_str).value().id());
                EXPECT_EQ(pt.at(1).pos(), 0);
                EXPECT_EQ(pt.at(1).len(), 1);

                EXPECT_TRUE(pt.at(1).lpr());
                EXPECT_FALSE(pt.at(1).ppr());
                if (pt.at(1).lpr()) EXPECT_EQ(pt.at(1).lpr().value(), gram.lpr("lpr"_str).value());

                EXPECT_EQ(pt.at(1).str(src), "a"_str);
                EXPECT_TRUE(pt.at(1).tkn());
                if (pt.at(1).tkn()) EXPECT_EQ(pt.at(1).tkn().value(), taul::token::normal(gram, "lpr"_str, 0, 1));

                EXPECT_EQ(pt.at(2).id(), gram.ppr("ppr"_str).value().id());
                EXPECT_EQ(pt.at(2).pos(), 1);
                EXPECT_EQ(pt.at(2).len(), 1);

                EXPECT_FALSE(pt.at(2).lpr());
                EXPECT_TRUE(pt.at(2).ppr());
                if (pt.at(2).ppr()) EXPECT_EQ(pt.at(2).ppr().value(), gram.ppr("ppr"_str).value());

                EXPECT_EQ(pt.at(2).str(src), "b"_str);
                EXPECT_FALSE(pt.at(2).tkn());

                EXPECT_EQ(pt.at(3).id(), gram.lpr("lpr"_str).value().id());
                EXPECT_EQ(pt.at(3).pos(), 1);
                EXPECT_EQ(pt.at(3).len(), 1);

                EXPECT_TRUE(pt.at(3).lpr());
                EXPECT_FALSE(pt.at(3).ppr());
                if (pt.at(3).lpr()) EXPECT_EQ(pt.at(3).lpr().value(), gram.lpr("lpr"_str).value());

                EXPECT_EQ(pt.at(3).str(src), "b"_str);
                EXPECT_TRUE(pt.at(3).tkn());
                if (pt.at(3).tkn()) EXPECT_EQ(pt.at(3).tkn().value(), taul::token::normal(gram, "lpr"_str, 1, 1));
            }
        }
        else ADD_FAILURE();

        EXPECT_EQ(&(pt.current()), &(pt.at(2)));
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(4), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 4);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 4);
    }

    pt.close();

    {
        EXPECT_FALSE(pt.is_sealed());
        EXPECT_FALSE(pt.is_aborted());

        EXPECT_EQ(pt.nodes(), 4);
        EXPECT_TRUE(pt.has_nodes());

        if (pt.nodes() == 4) {
            {
                EXPECT_EQ(pt.at(0).index(), 0);
                EXPECT_EQ(pt.at(1).index(), 1);
                EXPECT_EQ(pt.at(2).index(), 2);
                EXPECT_EQ(pt.at(3).index(), 3);

                EXPECT_EQ(pt.at(0).level(), 0);
                EXPECT_EQ(pt.at(1).level(), 1);
                EXPECT_EQ(pt.at(2).level(), 1);
                EXPECT_EQ(pt.at(3).level(), 2);

                EXPECT_EQ(pt.at(0).parent(), pt.end());
                EXPECT_EQ(pt.at(0).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).left_child(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(0).right_child(), std::next(pt.begin(), 2));

                EXPECT_EQ(pt.at(1).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(1).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(1).right_sibling(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(1).left_child(), pt.end());
                EXPECT_EQ(pt.at(1).right_child(), pt.end());

                EXPECT_EQ(pt.at(2).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(2).left_sibling(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(2).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(2).left_child(), std::next(pt.begin(), 3));
                EXPECT_EQ(pt.at(2).right_child(), std::next(pt.begin(), 3));

                EXPECT_EQ(pt.at(3).parent(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(3).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(3).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(3).left_child(), pt.end());
                EXPECT_EQ(pt.at(3).right_child(), pt.end());

                EXPECT_FALSE(pt.at(0).has_parent());
                EXPECT_FALSE(pt.at(0).has_left_sibling());
                EXPECT_FALSE(pt.at(0).has_right_sibling());
                EXPECT_EQ(pt.at(0).children(), 2);
                EXPECT_TRUE(pt.at(0).has_children());

                EXPECT_TRUE(pt.at(1).has_parent());
                EXPECT_FALSE(pt.at(1).has_left_sibling());
                EXPECT_TRUE(pt.at(1).has_right_sibling());
                EXPECT_EQ(pt.at(1).children(), 0);
                EXPECT_FALSE(pt.at(1).has_children());

                EXPECT_TRUE(pt.at(2).has_parent());
                EXPECT_TRUE(pt.at(2).has_left_sibling());
                EXPECT_FALSE(pt.at(2).has_right_sibling());
                EXPECT_EQ(pt.at(2).children(), 1);
                EXPECT_TRUE(pt.at(2).has_children());

                EXPECT_TRUE(pt.at(3).has_parent());
                EXPECT_FALSE(pt.at(3).has_left_sibling());
                EXPECT_FALSE(pt.at(3).has_right_sibling());
                EXPECT_EQ(pt.at(3).children(), 0);
                EXPECT_FALSE(pt.at(3).has_children());

                EXPECT_FALSE(pt.at(0).is_lexical());
                EXPECT_TRUE(pt.at(0).is_syntactic());
                EXPECT_TRUE(pt.at(0).is_normal());
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_TRUE(pt.at(1).is_lexical());
                EXPECT_FALSE(pt.at(1).is_syntactic());
                EXPECT_TRUE(pt.at(1).is_normal());
                EXPECT_FALSE(pt.at(1).is_failure());

                EXPECT_FALSE(pt.at(2).is_lexical());
                EXPECT_TRUE(pt.at(2).is_syntactic());
                EXPECT_TRUE(pt.at(2).is_normal());
                EXPECT_FALSE(pt.at(2).is_failure());

                EXPECT_TRUE(pt.at(3).is_lexical());
                EXPECT_FALSE(pt.at(3).is_syntactic());
                EXPECT_TRUE(pt.at(3).is_normal());
                EXPECT_FALSE(pt.at(3).is_failure());

                EXPECT_EQ(pt.at(0).id(), gram.ppr("ppr"_str).value().id());
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).len(), 2);

                EXPECT_FALSE(pt.at(0).lpr());
                EXPECT_TRUE(pt.at(0).ppr());
                if (pt.at(0).ppr()) EXPECT_EQ(pt.at(0).ppr().value(), gram.ppr("ppr"_str).value());

                EXPECT_EQ(pt.at(0).str(src), "ab"_str);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);

                EXPECT_EQ(pt.at(1).id(), gram.lpr("lpr"_str).value().id());
                EXPECT_EQ(pt.at(1).pos(), 0);
                EXPECT_EQ(pt.at(1).len(), 1);

                EXPECT_TRUE(pt.at(1).lpr());
                EXPECT_FALSE(pt.at(1).ppr());
                if (pt.at(1).lpr()) EXPECT_EQ(pt.at(1).lpr().value(), gram.lpr("lpr"_str).value());

                EXPECT_EQ(pt.at(1).str(src), "a"_str);
                EXPECT_TRUE(pt.at(1).tkn());
                if (pt.at(1).tkn()) EXPECT_EQ(pt.at(1).tkn().value(), taul::token::normal(gram, "lpr"_str, 0, 1));

                EXPECT_EQ(pt.at(2).id(), gram.ppr("ppr"_str).value().id());
                EXPECT_EQ(pt.at(2).pos(), 1);
                EXPECT_EQ(pt.at(2).len(), 1);

                EXPECT_FALSE(pt.at(2).lpr());
                EXPECT_TRUE(pt.at(2).ppr());
                if (pt.at(2).ppr()) EXPECT_EQ(pt.at(2).ppr().value(), gram.ppr("ppr"_str).value());

                EXPECT_EQ(pt.at(2).str(src), "b"_str);
                EXPECT_FALSE(pt.at(2).tkn());

                EXPECT_EQ(pt.at(3).id(), gram.lpr("lpr"_str).value().id());
                EXPECT_EQ(pt.at(3).pos(), 1);
                EXPECT_EQ(pt.at(3).len(), 1);

                EXPECT_TRUE(pt.at(3).lpr());
                EXPECT_FALSE(pt.at(3).ppr());
                if (pt.at(3).lpr()) EXPECT_EQ(pt.at(3).lpr().value(), gram.lpr("lpr"_str).value());

                EXPECT_EQ(pt.at(3).str(src), "b"_str);
                EXPECT_TRUE(pt.at(3).tkn());
                if (pt.at(3).tkn()) EXPECT_EQ(pt.at(3).tkn().value(), taul::token::normal(gram, "lpr"_str, 1, 1));
            }
        }
        else ADD_FAILURE();

        EXPECT_EQ(&(pt.current()), &(pt.at(0)));
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(4), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 4);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 4);
    }

    pt.lexical(taul::token::failure(2, 1));

    {
        EXPECT_FALSE(pt.is_sealed());
        EXPECT_FALSE(pt.is_aborted());

        EXPECT_EQ(pt.nodes(), 5);
        EXPECT_TRUE(pt.has_nodes());

        if (pt.nodes() == 5) {
            {
                EXPECT_EQ(pt.at(0).index(), 0);
                EXPECT_EQ(pt.at(1).index(), 1);
                EXPECT_EQ(pt.at(2).index(), 2);
                EXPECT_EQ(pt.at(3).index(), 3);
                EXPECT_EQ(pt.at(4).index(), 4);

                EXPECT_EQ(pt.at(0).level(), 0);
                EXPECT_EQ(pt.at(1).level(), 1);
                EXPECT_EQ(pt.at(2).level(), 1);
                EXPECT_EQ(pt.at(3).level(), 2);
                EXPECT_EQ(pt.at(4).level(), 1);

                EXPECT_EQ(pt.at(0).parent(), pt.end());
                EXPECT_EQ(pt.at(0).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).left_child(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(0).right_child(), std::next(pt.begin(), 4));

                EXPECT_EQ(pt.at(1).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(1).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(1).right_sibling(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(1).left_child(), pt.end());
                EXPECT_EQ(pt.at(1).right_child(), pt.end());

                EXPECT_EQ(pt.at(2).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(2).left_sibling(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(2).right_sibling(), std::next(pt.begin(), 4));
                EXPECT_EQ(pt.at(2).left_child(), std::next(pt.begin(), 3));
                EXPECT_EQ(pt.at(2).right_child(), std::next(pt.begin(), 3));

                EXPECT_EQ(pt.at(3).parent(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(3).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(3).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(3).left_child(), pt.end());
                EXPECT_EQ(pt.at(3).right_child(), pt.end());

                EXPECT_EQ(pt.at(4).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(4).left_sibling(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(4).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(4).left_child(), pt.end());
                EXPECT_EQ(pt.at(4).right_child(), pt.end());

                EXPECT_FALSE(pt.at(0).has_parent());
                EXPECT_FALSE(pt.at(0).has_left_sibling());
                EXPECT_FALSE(pt.at(0).has_right_sibling());
                EXPECT_EQ(pt.at(0).children(), 3);
                EXPECT_TRUE(pt.at(0).has_children());

                EXPECT_TRUE(pt.at(1).has_parent());
                EXPECT_FALSE(pt.at(1).has_left_sibling());
                EXPECT_TRUE(pt.at(1).has_right_sibling());
                EXPECT_EQ(pt.at(1).children(), 0);
                EXPECT_FALSE(pt.at(1).has_children());

                EXPECT_TRUE(pt.at(2).has_parent());
                EXPECT_TRUE(pt.at(2).has_left_sibling());
                EXPECT_TRUE(pt.at(2).has_right_sibling());
                EXPECT_EQ(pt.at(2).children(), 1);
                EXPECT_TRUE(pt.at(2).has_children());

                EXPECT_TRUE(pt.at(3).has_parent());
                EXPECT_FALSE(pt.at(3).has_left_sibling());
                EXPECT_FALSE(pt.at(3).has_right_sibling());
                EXPECT_EQ(pt.at(3).children(), 0);
                EXPECT_FALSE(pt.at(3).has_children());
                
                EXPECT_TRUE(pt.at(4).has_parent());
                EXPECT_TRUE(pt.at(4).has_left_sibling());
                EXPECT_FALSE(pt.at(4).has_right_sibling());
                EXPECT_EQ(pt.at(4).children(), 0);
                EXPECT_FALSE(pt.at(4).has_children());

                EXPECT_FALSE(pt.at(0).is_lexical());
                EXPECT_TRUE(pt.at(0).is_syntactic());
                EXPECT_TRUE(pt.at(0).is_normal());
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_TRUE(pt.at(1).is_lexical());
                EXPECT_FALSE(pt.at(1).is_syntactic());
                EXPECT_TRUE(pt.at(1).is_normal());
                EXPECT_FALSE(pt.at(1).is_failure());

                EXPECT_FALSE(pt.at(2).is_lexical());
                EXPECT_TRUE(pt.at(2).is_syntactic());
                EXPECT_TRUE(pt.at(2).is_normal());
                EXPECT_FALSE(pt.at(2).is_failure());

                EXPECT_TRUE(pt.at(3).is_lexical());
                EXPECT_FALSE(pt.at(3).is_syntactic());
                EXPECT_TRUE(pt.at(3).is_normal());
                EXPECT_FALSE(pt.at(3).is_failure());
                
                EXPECT_TRUE(pt.at(4).is_lexical());
                EXPECT_FALSE(pt.at(4).is_syntactic());
                EXPECT_FALSE(pt.at(4).is_normal());
                EXPECT_TRUE(pt.at(4).is_failure());

                EXPECT_EQ(pt.at(0).id(), gram.ppr("ppr"_str).value().id());
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).len(), 3);

                EXPECT_FALSE(pt.at(0).lpr());
                EXPECT_TRUE(pt.at(0).ppr());
                if (pt.at(0).ppr()) EXPECT_EQ(pt.at(0).ppr().value(), gram.ppr("ppr"_str).value());

                EXPECT_EQ(pt.at(0).str(src), "abc"_str);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);

                EXPECT_EQ(pt.at(1).id(), gram.lpr("lpr"_str).value().id());
                EXPECT_EQ(pt.at(1).pos(), 0);
                EXPECT_EQ(pt.at(1).len(), 1);

                EXPECT_TRUE(pt.at(1).lpr());
                EXPECT_FALSE(pt.at(1).ppr());
                if (pt.at(1).lpr()) EXPECT_EQ(pt.at(1).lpr().value(), gram.lpr("lpr"_str).value());

                EXPECT_EQ(pt.at(1).str(src), "a"_str);
                EXPECT_TRUE(pt.at(1).tkn());
                if (pt.at(1).tkn()) EXPECT_EQ(pt.at(1).tkn().value(), taul::token::normal(gram, "lpr"_str, 0, 1));

                EXPECT_EQ(pt.at(2).id(), gram.ppr("ppr"_str).value().id());
                EXPECT_EQ(pt.at(2).pos(), 1);
                EXPECT_EQ(pt.at(2).len(), 1);

                EXPECT_FALSE(pt.at(2).lpr());
                EXPECT_TRUE(pt.at(2).ppr());
                if (pt.at(2).ppr()) EXPECT_EQ(pt.at(2).ppr().value(), gram.ppr("ppr"_str).value());

                EXPECT_EQ(pt.at(2).str(src), "b"_str);
                EXPECT_FALSE(pt.at(2).tkn());

                EXPECT_EQ(pt.at(3).id(), gram.lpr("lpr"_str).value().id());
                EXPECT_EQ(pt.at(3).pos(), 1);
                EXPECT_EQ(pt.at(3).len(), 1);

                EXPECT_TRUE(pt.at(3).lpr());
                EXPECT_FALSE(pt.at(3).ppr());
                if (pt.at(3).lpr()) EXPECT_EQ(pt.at(3).lpr().value(), gram.lpr("lpr"_str).value());

                EXPECT_EQ(pt.at(3).str(src), "b"_str);
                EXPECT_TRUE(pt.at(3).tkn());
                if (pt.at(3).tkn()) EXPECT_EQ(pt.at(3).tkn().value(), taul::token::normal(gram, "lpr"_str, 1, 1));

                EXPECT_EQ(pt.at(4).id(), taul::failure_lpr_id);
                EXPECT_EQ(pt.at(4).pos(), 2);
                EXPECT_EQ(pt.at(4).len(), 1);

                EXPECT_FALSE(pt.at(4).lpr());
                EXPECT_FALSE(pt.at(4).ppr());

                EXPECT_EQ(pt.at(4).str(src), "c"_str);
                EXPECT_TRUE(pt.at(4).tkn());
                if (pt.at(4).tkn()) EXPECT_EQ(pt.at(4).tkn().value(), taul::token::failure(2, 1));
            }
        }
        else ADD_FAILURE();

        EXPECT_EQ(&(pt.current()), &(pt.at(0)));
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(5), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 5);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 5);
    }

    pt.close();

    {
        EXPECT_TRUE(pt.is_sealed());
        EXPECT_FALSE(pt.is_aborted());

        EXPECT_EQ(pt.nodes(), 5);
        EXPECT_TRUE(pt.has_nodes());

        if (pt.nodes() == 5) {
            {
                EXPECT_EQ(pt.at(0).index(), 0);
                EXPECT_EQ(pt.at(1).index(), 1);
                EXPECT_EQ(pt.at(2).index(), 2);
                EXPECT_EQ(pt.at(3).index(), 3);
                EXPECT_EQ(pt.at(4).index(), 4);

                EXPECT_EQ(pt.at(0).level(), 0);
                EXPECT_EQ(pt.at(1).level(), 1);
                EXPECT_EQ(pt.at(2).level(), 1);
                EXPECT_EQ(pt.at(3).level(), 2);
                EXPECT_EQ(pt.at(4).level(), 1);

                EXPECT_EQ(pt.at(0).parent(), pt.end());
                EXPECT_EQ(pt.at(0).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).left_child(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(0).right_child(), std::next(pt.begin(), 4));

                EXPECT_EQ(pt.at(1).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(1).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(1).right_sibling(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(1).left_child(), pt.end());
                EXPECT_EQ(pt.at(1).right_child(), pt.end());

                EXPECT_EQ(pt.at(2).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(2).left_sibling(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(2).right_sibling(), std::next(pt.begin(), 4));
                EXPECT_EQ(pt.at(2).left_child(), std::next(pt.begin(), 3));
                EXPECT_EQ(pt.at(2).right_child(), std::next(pt.begin(), 3));

                EXPECT_EQ(pt.at(3).parent(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(3).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(3).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(3).left_child(), pt.end());
                EXPECT_EQ(pt.at(3).right_child(), pt.end());

                EXPECT_EQ(pt.at(4).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(4).left_sibling(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(4).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(4).left_child(), pt.end());
                EXPECT_EQ(pt.at(4).right_child(), pt.end());

                EXPECT_FALSE(pt.at(0).has_parent());
                EXPECT_FALSE(pt.at(0).has_left_sibling());
                EXPECT_FALSE(pt.at(0).has_right_sibling());
                EXPECT_EQ(pt.at(0).children(), 3);
                EXPECT_TRUE(pt.at(0).has_children());

                EXPECT_TRUE(pt.at(1).has_parent());
                EXPECT_FALSE(pt.at(1).has_left_sibling());
                EXPECT_TRUE(pt.at(1).has_right_sibling());
                EXPECT_EQ(pt.at(1).children(), 0);
                EXPECT_FALSE(pt.at(1).has_children());

                EXPECT_TRUE(pt.at(2).has_parent());
                EXPECT_TRUE(pt.at(2).has_left_sibling());
                EXPECT_TRUE(pt.at(2).has_right_sibling());
                EXPECT_EQ(pt.at(2).children(), 1);
                EXPECT_TRUE(pt.at(2).has_children());

                EXPECT_TRUE(pt.at(3).has_parent());
                EXPECT_FALSE(pt.at(3).has_left_sibling());
                EXPECT_FALSE(pt.at(3).has_right_sibling());
                EXPECT_EQ(pt.at(3).children(), 0);
                EXPECT_FALSE(pt.at(3).has_children());

                EXPECT_TRUE(pt.at(4).has_parent());
                EXPECT_TRUE(pt.at(4).has_left_sibling());
                EXPECT_FALSE(pt.at(4).has_right_sibling());
                EXPECT_EQ(pt.at(4).children(), 0);
                EXPECT_FALSE(pt.at(4).has_children());

                EXPECT_FALSE(pt.at(0).is_lexical());
                EXPECT_TRUE(pt.at(0).is_syntactic());
                EXPECT_TRUE(pt.at(0).is_normal());
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_TRUE(pt.at(1).is_lexical());
                EXPECT_FALSE(pt.at(1).is_syntactic());
                EXPECT_TRUE(pt.at(1).is_normal());
                EXPECT_FALSE(pt.at(1).is_failure());

                EXPECT_FALSE(pt.at(2).is_lexical());
                EXPECT_TRUE(pt.at(2).is_syntactic());
                EXPECT_TRUE(pt.at(2).is_normal());
                EXPECT_FALSE(pt.at(2).is_failure());

                EXPECT_TRUE(pt.at(3).is_lexical());
                EXPECT_FALSE(pt.at(3).is_syntactic());
                EXPECT_TRUE(pt.at(3).is_normal());
                EXPECT_FALSE(pt.at(3).is_failure());

                EXPECT_TRUE(pt.at(4).is_lexical());
                EXPECT_FALSE(pt.at(4).is_syntactic());
                EXPECT_FALSE(pt.at(4).is_normal());
                EXPECT_TRUE(pt.at(4).is_failure());

                EXPECT_EQ(pt.at(0).id(), gram.ppr("ppr"_str).value().id());
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).len(), 3);

                EXPECT_FALSE(pt.at(0).lpr());
                EXPECT_TRUE(pt.at(0).ppr());
                if (pt.at(0).ppr()) EXPECT_EQ(pt.at(0).ppr().value(), gram.ppr("ppr"_str).value());

                EXPECT_EQ(pt.at(0).str(src), "abc"_str);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);

                EXPECT_EQ(pt.at(1).id(), gram.lpr("lpr"_str).value().id());
                EXPECT_EQ(pt.at(1).pos(), 0);
                EXPECT_EQ(pt.at(1).len(), 1);

                EXPECT_TRUE(pt.at(1).lpr());
                EXPECT_FALSE(pt.at(1).ppr());
                if (pt.at(1).lpr()) EXPECT_EQ(pt.at(1).lpr().value(), gram.lpr("lpr"_str).value());

                EXPECT_EQ(pt.at(1).str(src), "a"_str);
                EXPECT_TRUE(pt.at(1).tkn());
                if (pt.at(1).tkn()) EXPECT_EQ(pt.at(1).tkn().value(), taul::token::normal(gram, "lpr"_str, 0, 1));

                EXPECT_EQ(pt.at(2).id(), gram.ppr("ppr"_str).value().id());
                EXPECT_EQ(pt.at(2).pos(), 1);
                EXPECT_EQ(pt.at(2).len(), 1);

                EXPECT_FALSE(pt.at(2).lpr());
                EXPECT_TRUE(pt.at(2).ppr());
                if (pt.at(2).ppr()) EXPECT_EQ(pt.at(2).ppr().value(), gram.ppr("ppr"_str).value());

                EXPECT_EQ(pt.at(2).str(src), "b"_str);
                EXPECT_FALSE(pt.at(2).tkn());

                EXPECT_EQ(pt.at(3).id(), gram.lpr("lpr"_str).value().id());
                EXPECT_EQ(pt.at(3).pos(), 1);
                EXPECT_EQ(pt.at(3).len(), 1);

                EXPECT_TRUE(pt.at(3).lpr());
                EXPECT_FALSE(pt.at(3).ppr());
                if (pt.at(3).lpr()) EXPECT_EQ(pt.at(3).lpr().value(), gram.lpr("lpr"_str).value());

                EXPECT_EQ(pt.at(3).str(src), "b"_str);
                EXPECT_TRUE(pt.at(3).tkn());
                if (pt.at(3).tkn()) EXPECT_EQ(pt.at(3).tkn().value(), taul::token::normal(gram, "lpr"_str, 1, 1));

                EXPECT_EQ(pt.at(4).id(), taul::failure_lpr_id);
                EXPECT_EQ(pt.at(4).pos(), 2);
                EXPECT_EQ(pt.at(4).len(), 1);

                EXPECT_FALSE(pt.at(4).lpr());
                EXPECT_FALSE(pt.at(4).ppr());

                EXPECT_EQ(pt.at(4).str(src), "c"_str);
                EXPECT_TRUE(pt.at(4).tkn());
                if (pt.at(4).tkn()) EXPECT_EQ(pt.at(4).tkn().value(), taul::token::failure(2, 1));
            }
        }
        else ADD_FAILURE();

        EXPECT_THROW(&(pt.current()), std::out_of_range);
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(5), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 5);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 5);
    }


    TAUL_LOG(lgr, "{}", pt);
}

TEST_F(ParseTreeTests, DeepNesting) {
    ASSERT_TRUE(ready);

    taul::str src = "abcd"_str;

    taul::parse_tree pt(gram);

    pt
        .syntactic("ppr"_str, 0)
        .syntactic("ppr"_str, 0) // test contiguous PPRs (as I had impl issues in those cases)
        .lexical("lpr"_str, 0, 1)
        .syntactic("ppr"_str, 1)
        .lexical("lpr"_str, 1, 1)
        .close()
        .lexical("lpr"_str, 2, 1)
        .close()
        .lexical("lpr"_str, 3, 1)
        .close();

    EXPECT_TRUE(pt.is_sealed());
    EXPECT_FALSE(pt.is_aborted());

    EXPECT_EQ(pt.nodes(), 7);
    EXPECT_TRUE(pt.has_nodes());

    if (pt.nodes() == 7) {
        EXPECT_EQ(pt.at(0).index(), 0);
        EXPECT_EQ(pt.at(1).index(), 1);
        EXPECT_EQ(pt.at(2).index(), 2);
        EXPECT_EQ(pt.at(3).index(), 3);
        EXPECT_EQ(pt.at(4).index(), 4);
        EXPECT_EQ(pt.at(5).index(), 5);
        EXPECT_EQ(pt.at(6).index(), 6);

        EXPECT_EQ(pt.at(0).level(), 0);
        EXPECT_EQ(pt.at(1).level(), 1);
        EXPECT_EQ(pt.at(2).level(), 2);
        EXPECT_EQ(pt.at(3).level(), 2);
        EXPECT_EQ(pt.at(4).level(), 3);
        EXPECT_EQ(pt.at(5).level(), 2);
        EXPECT_EQ(pt.at(6).level(), 1);

        EXPECT_EQ(pt.at(0).parent(), pt.end());
        EXPECT_EQ(pt.at(0).left_sibling(), pt.end());
        EXPECT_EQ(pt.at(0).right_sibling(), pt.end());
        EXPECT_EQ(pt.at(0).left_child(), std::next(pt.begin(), 1));
        EXPECT_EQ(pt.at(0).right_child(), std::next(pt.begin(), 6));

        EXPECT_EQ(pt.at(1).parent(), std::next(pt.begin(), 0));
        EXPECT_EQ(pt.at(1).left_sibling(), pt.end());
        EXPECT_EQ(pt.at(1).right_sibling(), std::next(pt.begin(), 6));
        EXPECT_EQ(pt.at(1).left_child(), std::next(pt.begin(), 2));
        EXPECT_EQ(pt.at(1).right_child(), std::next(pt.begin(), 5));

        EXPECT_EQ(pt.at(2).parent(), std::next(pt.begin(), 1));
        EXPECT_EQ(pt.at(2).left_sibling(), pt.end());
        EXPECT_EQ(pt.at(2).right_sibling(), std::next(pt.begin(), 3));
        EXPECT_EQ(pt.at(2).left_child(), pt.end());
        EXPECT_EQ(pt.at(2).right_child(), pt.end());

        EXPECT_EQ(pt.at(3).parent(), std::next(pt.begin(), 1));
        EXPECT_EQ(pt.at(3).left_sibling(), std::next(pt.begin(), 2));
        EXPECT_EQ(pt.at(3).right_sibling(), std::next(pt.begin(), 5));
        EXPECT_EQ(pt.at(3).left_child(), std::next(pt.begin(), 4));
        EXPECT_EQ(pt.at(3).right_child(), std::next(pt.begin(), 4));

        EXPECT_EQ(pt.at(4).parent(), std::next(pt.begin(), 3));
        EXPECT_EQ(pt.at(4).left_sibling(), pt.end());
        EXPECT_EQ(pt.at(4).right_sibling(), pt.end());
        EXPECT_EQ(pt.at(4).left_child(), pt.end());
        EXPECT_EQ(pt.at(4).right_child(), pt.end());

        EXPECT_EQ(pt.at(5).parent(), std::next(pt.begin(), 1));
        EXPECT_EQ(pt.at(5).left_sibling(), std::next(pt.begin(), 3));
        EXPECT_EQ(pt.at(5).right_sibling(), pt.end());
        EXPECT_EQ(pt.at(5).left_child(), pt.end());
        EXPECT_EQ(pt.at(5).right_child(), pt.end());

        EXPECT_EQ(pt.at(6).parent(), std::next(pt.begin(), 0));
        EXPECT_EQ(pt.at(6).left_sibling(), std::next(pt.begin(), 1));
        EXPECT_EQ(pt.at(6).right_sibling(), pt.end());
        EXPECT_EQ(pt.at(6).left_child(), pt.end());
        EXPECT_EQ(pt.at(6).right_child(), pt.end());

        EXPECT_FALSE(pt.at(0).has_parent());
        EXPECT_FALSE(pt.at(0).has_left_sibling());
        EXPECT_FALSE(pt.at(0).has_right_sibling());
        EXPECT_EQ(pt.at(0).children(), 2);
        EXPECT_TRUE(pt.at(0).has_children());

        EXPECT_TRUE(pt.at(1).has_parent());
        EXPECT_FALSE(pt.at(1).has_left_sibling());
        EXPECT_TRUE(pt.at(1).has_right_sibling());
        EXPECT_EQ(pt.at(1).children(), 3);
        EXPECT_TRUE(pt.at(1).has_children());

        EXPECT_TRUE(pt.at(2).has_parent());
        EXPECT_FALSE(pt.at(2).has_left_sibling());
        EXPECT_TRUE(pt.at(2).has_right_sibling());
        EXPECT_EQ(pt.at(2).children(), 0);
        EXPECT_FALSE(pt.at(2).has_children());

        EXPECT_TRUE(pt.at(3).has_parent());
        EXPECT_TRUE(pt.at(3).has_left_sibling());
        EXPECT_TRUE(pt.at(3).has_right_sibling());
        EXPECT_EQ(pt.at(3).children(), 1);
        EXPECT_TRUE(pt.at(3).has_children());

        EXPECT_TRUE(pt.at(4).has_parent());
        EXPECT_FALSE(pt.at(4).has_left_sibling());
        EXPECT_FALSE(pt.at(4).has_right_sibling());
        EXPECT_EQ(pt.at(4).children(), 0);
        EXPECT_FALSE(pt.at(4).has_children());

        EXPECT_TRUE(pt.at(5).has_parent());
        EXPECT_TRUE(pt.at(5).has_left_sibling());
        EXPECT_FALSE(pt.at(5).has_right_sibling());
        EXPECT_EQ(pt.at(5).children(), 0);
        EXPECT_FALSE(pt.at(5).has_children());

        EXPECT_TRUE(pt.at(6).has_parent());
        EXPECT_TRUE(pt.at(6).has_left_sibling());
        EXPECT_FALSE(pt.at(6).has_right_sibling());
        EXPECT_EQ(pt.at(6).children(), 0);
        EXPECT_FALSE(pt.at(6).has_children());

        EXPECT_FALSE(pt.at(0).is_lexical());
        EXPECT_TRUE(pt.at(0).is_syntactic());
        EXPECT_TRUE(pt.at(0).is_normal());
        EXPECT_FALSE(pt.at(0).is_failure());

        EXPECT_FALSE(pt.at(1).is_lexical());
        EXPECT_TRUE(pt.at(1).is_syntactic());
        EXPECT_TRUE(pt.at(1).is_normal());
        EXPECT_FALSE(pt.at(1).is_failure());

        EXPECT_TRUE(pt.at(2).is_lexical());
        EXPECT_FALSE(pt.at(2).is_syntactic());
        EXPECT_TRUE(pt.at(2).is_normal());
        EXPECT_FALSE(pt.at(2).is_failure());

        EXPECT_FALSE(pt.at(3).is_lexical());
        EXPECT_TRUE(pt.at(3).is_syntactic());
        EXPECT_TRUE(pt.at(3).is_normal());
        EXPECT_FALSE(pt.at(3).is_failure());

        EXPECT_TRUE(pt.at(4).is_lexical());
        EXPECT_FALSE(pt.at(4).is_syntactic());
        EXPECT_TRUE(pt.at(4).is_normal());
        EXPECT_FALSE(pt.at(4).is_failure());

        EXPECT_TRUE(pt.at(5).is_lexical());
        EXPECT_FALSE(pt.at(5).is_syntactic());
        EXPECT_TRUE(pt.at(5).is_normal());
        EXPECT_FALSE(pt.at(5).is_failure());

        EXPECT_TRUE(pt.at(6).is_lexical());
        EXPECT_FALSE(pt.at(6).is_syntactic());
        EXPECT_TRUE(pt.at(6).is_normal());
        EXPECT_FALSE(pt.at(6).is_failure());

        EXPECT_EQ(pt.at(0).id(), gram.ppr("ppr"_str).value().id());
        EXPECT_EQ(pt.at(1).id(), gram.ppr("ppr"_str).value().id());
        EXPECT_EQ(pt.at(2).id(), gram.lpr("lpr"_str).value().id());
        EXPECT_EQ(pt.at(3).id(), gram.ppr("ppr"_str).value().id());
        EXPECT_EQ(pt.at(4).id(), gram.lpr("lpr"_str).value().id());
        EXPECT_EQ(pt.at(5).id(), gram.lpr("lpr"_str).value().id());
        EXPECT_EQ(pt.at(6).id(), gram.lpr("lpr"_str).value().id());

        EXPECT_EQ(pt.at(0).pos(), 0);
        EXPECT_EQ(pt.at(1).pos(), 0);
        EXPECT_EQ(pt.at(2).pos(), 0);
        EXPECT_EQ(pt.at(3).pos(), 1);
        EXPECT_EQ(pt.at(4).pos(), 1);
        EXPECT_EQ(pt.at(5).pos(), 2);
        EXPECT_EQ(pt.at(6).pos(), 3);

        EXPECT_EQ(pt.at(0).len(), 4);
        EXPECT_EQ(pt.at(1).len(), 3);
        EXPECT_EQ(pt.at(2).len(), 1);
        EXPECT_EQ(pt.at(3).len(), 1);
        EXPECT_EQ(pt.at(4).len(), 1);
        EXPECT_EQ(pt.at(5).len(), 1);
        EXPECT_EQ(pt.at(6).len(), 1);

        EXPECT_FALSE(pt.at(0).lpr());
        EXPECT_FALSE(pt.at(1).lpr());
        EXPECT_TRUE(pt.at(2).lpr());
        EXPECT_FALSE(pt.at(3).lpr());
        EXPECT_TRUE(pt.at(4).lpr());
        EXPECT_TRUE(pt.at(5).lpr());
        EXPECT_TRUE(pt.at(6).lpr());

        EXPECT_TRUE(pt.at(0).ppr());
        EXPECT_TRUE(pt.at(1).ppr());
        EXPECT_FALSE(pt.at(2).ppr());
        EXPECT_TRUE(pt.at(3).ppr());
        EXPECT_FALSE(pt.at(4).ppr());
        EXPECT_FALSE(pt.at(5).ppr());
        EXPECT_FALSE(pt.at(6).ppr());

        if (pt.at(0).ppr()) EXPECT_EQ(pt.at(0).ppr().value(), gram.ppr("ppr"_str).value());
        if (pt.at(1).ppr()) EXPECT_EQ(pt.at(1).ppr().value(), gram.ppr("ppr"_str).value());
        if (pt.at(2).lpr()) EXPECT_EQ(pt.at(2).lpr().value(), gram.lpr("lpr"_str).value());
        if (pt.at(3).ppr()) EXPECT_EQ(pt.at(3).ppr().value(), gram.ppr("ppr"_str).value());
        if (pt.at(4).lpr()) EXPECT_EQ(pt.at(4).lpr().value(), gram.lpr("lpr"_str).value());
        if (pt.at(5).lpr()) EXPECT_EQ(pt.at(5).lpr().value(), gram.lpr("lpr"_str).value());
        if (pt.at(6).lpr()) EXPECT_EQ(pt.at(6).lpr().value(), gram.lpr("lpr"_str).value());

        EXPECT_EQ(pt.at(0).str(src), "abcd"_str);
        EXPECT_EQ(pt.at(1).str(src), "abc"_str);
        EXPECT_EQ(pt.at(2).str(src), "a"_str);
        EXPECT_EQ(pt.at(3).str(src), "b"_str);
        EXPECT_EQ(pt.at(4).str(src), "b"_str);
        EXPECT_EQ(pt.at(5).str(src), "c"_str);
        EXPECT_EQ(pt.at(6).str(src), "d"_str);

        EXPECT_EQ(pt.at(0).tkn(), std::nullopt);
        EXPECT_EQ(pt.at(1).tkn(), std::nullopt);
        EXPECT_EQ(pt.at(2).tkn(), taul::token::normal(gram, "lpr"_str, 0, 1));
        EXPECT_EQ(pt.at(3).tkn(), std::nullopt);
        EXPECT_EQ(pt.at(4).tkn(), taul::token::normal(gram, "lpr"_str, 1, 1));
        EXPECT_EQ(pt.at(5).tkn(), taul::token::normal(gram, "lpr"_str, 2, 1));
        EXPECT_EQ(pt.at(6).tkn(), taul::token::normal(gram, "lpr"_str, 3, 1));
    }
    else ADD_FAILURE();

    EXPECT_THROW(&(pt.current()), std::out_of_range);
    EXPECT_EQ(&(pt.root()), &(pt.at(0)));

    EXPECT_THROW(pt.at(7), std::out_of_range);

    EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 7);
    EXPECT_EQ(std::distance(pt.begin(), pt.end()), 7);
}

