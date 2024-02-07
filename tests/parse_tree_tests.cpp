

#include <gtest/gtest.h>

#include "taul/all.h"


class ParseTreeTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr;
    taul::grammar gram;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
        auto spec =
            taul::spec_writer()
            .lpr_decl("lpr")
            .ppr_decl("ppr")
            .lpr("lpr")
            .close()
            .ppr("ppr")
            .close()
            .done();
        auto loaded = taul::load(spec, lgr);
        TAUL_ASSERT(loaded);
        gram = std::move(*loaded);
    }
};


TEST_F(ParseTreeTests, DefaultCtor) {
    taul::parse_tree pt{};

    EXPECT_FALSE(pt.finished());

    EXPECT_EQ(pt.nodes(), 0);
    EXPECT_FALSE(pt.has_nodes());

    EXPECT_THROW(pt.at(0), std::out_of_range);
    EXPECT_THROW(pt.current(), std::out_of_range);
    EXPECT_THROW(pt.root(), std::out_of_range);

    EXPECT_EQ(pt.cbegin(), pt.cend());
    EXPECT_EQ(pt.begin(), pt.end());
}

TEST_F(ParseTreeTests, CopyCtor) {
    taul::parse_tree pt{};
    pt
        .open_syntactic(gram.ppr("ppr"), "a")
        .open_lexical(gram.lpr("lpr"), "a")
        .close()
        .close();

    taul::parse_tree copied(pt);
    
    taul::parse_tree expected{};
    expected
        .open_syntactic(gram.ppr("ppr"), "a")
        .open_lexical(gram.lpr("lpr"), "a")
        .close()
        .close();

    // this expects equality to work as expected

    EXPECT_EQ(copied, expected);
}

TEST_F(ParseTreeTests, MoveCtor) {
    taul::parse_tree pt{};
    pt
        .open_syntactic(gram.ppr("ppr"), "a")
        .open_lexical(gram.lpr("lpr"), "a")
        .close()
        .close();

    taul::parse_tree moved(std::move(pt));

    taul::parse_tree expected{};
    expected
        .open_syntactic(gram.ppr("ppr"), "a")
        .open_lexical(gram.lpr("lpr"), "a")
        .close()
        .close();

    // this expects equality to work as expected

    EXPECT_EQ(moved, expected);
}

TEST_F(ParseTreeTests, CopyAssign) {
    taul::parse_tree pt{};
    pt
        .open_syntactic(gram.ppr("ppr"), "a")
        .open_lexical(gram.lpr("lpr"), "a")
        .close()
        .close();

    taul::parse_tree copied_to{};
    copied_to
        .open_lexical(gram.lpr("lpr"), "a")
        .close();

    copied_to = pt;

    taul::parse_tree expected{};
    expected
        .open_syntactic(gram.ppr("ppr"), "a")
        .open_lexical(gram.lpr("lpr"), "a")
        .close()
        .close();

    // this expects equality to work as expected

    EXPECT_EQ(copied_to, expected);
}

TEST_F(ParseTreeTests, MoveAssign) {
    taul::parse_tree pt{};
    pt
        .open_syntactic(gram.ppr("ppr"), "a")
        .open_lexical(gram.lpr("lpr"), "a")
        .close()
        .close();

    taul::parse_tree moved_to{};
    moved_to
        .open_lexical(gram.lpr("lpr"), "a")
        .close();

    moved_to = std::move(pt);

    taul::parse_tree expected{};
    expected
        .open_syntactic(gram.ppr("ppr"), "a")
        .open_lexical(gram.lpr("lpr"), "a")
        .close()
        .close();

    // this expects equality to work as expected

    EXPECT_EQ(moved_to, expected);
}

TEST_F(ParseTreeTests, MoveAssignOntoSelf) {
    taul::parse_tree pt{};
    pt
        .open_syntactic(gram.ppr("ppr"), "a")
        .open_lexical(gram.lpr("lpr"), "a")
        .close()
        .close();

    pt = std::move(pt);

    taul::parse_tree expected{};
    expected
        .open_syntactic(gram.ppr("ppr"), "a")
        .open_lexical(gram.lpr("lpr"), "a")
        .close()
        .close();

    // this expects equality to work as expected

    EXPECT_EQ(pt, expected);
}

TEST_F(ParseTreeTests, Iterators) {
    taul::parse_tree pt{};
    pt
        .open_syntactic(gram.ppr("ppr"), "abc")
        .open_lexical(gram.lpr("lpr"), "a")
        .close()
        .open_syntactic(gram.ppr("ppr"), "bc")
        .open_syntactic(gram.ppr("ppr"), "b")
        .open_lexical(gram.lpr("lpr"), "b")
        .close()
        .close()
        .open_syntactic(gram.ppr("ppr"), "c")
        .open_lexical(gram.lpr("lpr"), "c")
        .close()
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

TEST_F(ParseTreeTests, Equality) {
    taul::parse_tree tree0{};
    tree0
        .open_syntactic(gram.ppr("ppr"), "abc")
        .open_lexical(gram.lpr("lpr"), "a")
        .close()
        .open_syntactic(gram.ppr("ppr"), "bc")
        .open_lexical(gram.lpr("lpr"), "b")
        .close()
        .open_lexical(gram.lpr("lpr"), "c")
        .close()
        .close()
        .close();
    // tree0 and tree1 are structurally equivalent
    taul::parse_tree tree1{};
    tree1
        .open_syntactic(gram.ppr("ppr"), "abc")
        .open_lexical(gram.lpr("lpr"), "a")
        .close()
        .open_syntactic(gram.ppr("ppr"), "bc")
        .open_lexical(gram.lpr("lpr"), "b")
        .close()
        .open_lexical(gram.lpr("lpr"), "c")
        .close()
        .close()
        .close();
    // tree0 and tree2 are structurally equivalent,
    // but w/ tree2 not being *finished* yet
    taul::parse_tree tree2{};
    tree2
        .open_syntactic(gram.ppr("ppr"), "abc")
        .open_lexical(gram.lpr("lpr"), "a")
        .close()
        .open_syntactic(gram.ppr("ppr"), "bc")
        .open_lexical(gram.lpr("lpr"), "b")
        .close()
        .open_lexical(gram.lpr("lpr"), "c")
        .close()
        .close()
        .close();
    taul::parse_tree tree3{};
    tree3
        .open_lexical(gram.lpr("lpr"), "a")
        .close();
    // tree3 and tree4 are structurally equivalent
    taul::parse_tree tree4{};
    tree4
        .open_lexical(gram.lpr("lpr"), "a")
        .close();

    EXPECT_TRUE(tree0.equal(tree0));
    EXPECT_TRUE(tree0.equal(tree1));
    EXPECT_TRUE(tree0.equal(tree2));
    EXPECT_FALSE(tree0.equal(tree3));
    EXPECT_FALSE(tree0.equal(tree4));

    EXPECT_TRUE(tree1.equal(tree0));
    EXPECT_TRUE(tree1.equal(tree1));
    EXPECT_TRUE(tree1.equal(tree2));
    EXPECT_FALSE(tree1.equal(tree3));
    EXPECT_FALSE(tree1.equal(tree4));

    EXPECT_TRUE(tree2.equal(tree0));
    EXPECT_TRUE(tree2.equal(tree1));
    EXPECT_TRUE(tree2.equal(tree2));
    EXPECT_FALSE(tree2.equal(tree3));
    EXPECT_FALSE(tree2.equal(tree4));

    EXPECT_FALSE(tree3.equal(tree0));
    EXPECT_FALSE(tree3.equal(tree1));
    EXPECT_FALSE(tree3.equal(tree2));
    EXPECT_TRUE(tree3.equal(tree3));
    EXPECT_TRUE(tree3.equal(tree4));

    EXPECT_FALSE(tree4.equal(tree0));
    EXPECT_FALSE(tree4.equal(tree1));
    EXPECT_FALSE(tree4.equal(tree2));
    EXPECT_TRUE(tree4.equal(tree3));
    EXPECT_TRUE(tree4.equal(tree4));

    EXPECT_TRUE(tree0 == tree0);
    EXPECT_TRUE(tree0 == tree1);
    EXPECT_TRUE(tree0 == tree2);
    EXPECT_FALSE(tree0 == tree3);
    EXPECT_FALSE(tree0 == tree4);

    EXPECT_TRUE(tree1 == tree0);
    EXPECT_TRUE(tree1 == tree1);
    EXPECT_TRUE(tree1 == tree2);
    EXPECT_FALSE(tree1 == tree3);
    EXPECT_FALSE(tree1 == tree4);

    EXPECT_TRUE(tree2 == tree0);
    EXPECT_TRUE(tree2 == tree1);
    EXPECT_TRUE(tree2 == tree2);
    EXPECT_FALSE(tree2 == tree3);
    EXPECT_FALSE(tree2 == tree4);

    EXPECT_FALSE(tree3 == tree0);
    EXPECT_FALSE(tree3 == tree1);
    EXPECT_FALSE(tree3 == tree2);
    EXPECT_TRUE(tree3 == tree3);
    EXPECT_TRUE(tree3 == tree4);

    EXPECT_FALSE(tree4 == tree0);
    EXPECT_FALSE(tree4 == tree1);
    EXPECT_FALSE(tree4 == tree2);
    EXPECT_TRUE(tree4 == tree3);
    EXPECT_TRUE(tree4 == tree4);

    EXPECT_FALSE(tree0 != tree0);
    EXPECT_FALSE(tree0 != tree1);
    EXPECT_FALSE(tree0 != tree2);
    EXPECT_TRUE(tree0 != tree3);
    EXPECT_TRUE(tree0 != tree4);

    EXPECT_FALSE(tree1 != tree0);
    EXPECT_FALSE(tree1 != tree1);
    EXPECT_FALSE(tree1 != tree2);
    EXPECT_TRUE(tree1 != tree3);
    EXPECT_TRUE(tree1 != tree4);

    EXPECT_FALSE(tree2 != tree0);
    EXPECT_FALSE(tree2 != tree1);
    EXPECT_FALSE(tree2 != tree2);
    EXPECT_TRUE(tree2 != tree3);
    EXPECT_TRUE(tree2 != tree4);

    EXPECT_TRUE(tree3 != tree0);
    EXPECT_TRUE(tree3 != tree1);
    EXPECT_TRUE(tree3 != tree2);
    EXPECT_FALSE(tree3 != tree3);
    EXPECT_FALSE(tree3 != tree4);

    EXPECT_TRUE(tree4 != tree0);
    EXPECT_TRUE(tree4 != tree1);
    EXPECT_TRUE(tree4 != tree2);
    EXPECT_FALSE(tree4 != tree3);
    EXPECT_FALSE(tree4 != tree4);
}

// this'll test move of our stuff regarding parse_tree state

TEST_F(ParseTreeTests, BasicTreeConstruction) {
    taul::parse_tree pt{};

    {
        EXPECT_FALSE(pt.finished());

        EXPECT_EQ(pt.nodes(), 0);
        EXPECT_FALSE(pt.has_nodes());

        EXPECT_THROW(pt.at(0), std::out_of_range);
        EXPECT_THROW(pt.current(), std::out_of_range);
        EXPECT_THROW(pt.root(), std::out_of_range);

        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 0);
    }

    pt.open_syntactic(gram.ppr("ppr"), "abc", 0);

    {
        EXPECT_FALSE(pt.finished());

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
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_THROW(pt.at(0).lpr(), taul::lpr_association_error);
                EXPECT_EQ(&(pt.at(0).ppr()), &(gram.ppr("ppr")));

                EXPECT_EQ(pt.at(0).str(), "abc");
                EXPECT_EQ(pt.at(0).pos(), 0);
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

    pt.open_lexical(gram.lpr("lpr"), "a", 0);

    {
        EXPECT_FALSE(pt.finished());

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
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_TRUE(pt.at(1).is_lexical());
                EXPECT_FALSE(pt.at(1).is_syntactic());
                EXPECT_FALSE(pt.at(1).is_failure());

                EXPECT_THROW(pt.at(0).lpr(), taul::lpr_association_error);
                EXPECT_EQ(&(pt.at(0).ppr()), &(gram.ppr("ppr")));

                EXPECT_EQ(&(pt.at(1).lpr()), &(gram.lpr("lpr")));
                EXPECT_THROW(pt.at(1).ppr(), taul::ppr_association_error);

                EXPECT_EQ(pt.at(0).str(), "abc");
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);

                EXPECT_EQ(pt.at(1).str(), "a");
                EXPECT_EQ(pt.at(1).pos(), 0);
                if (pt.at(1).tkn()) {
                    EXPECT_EQ(*pt.at(1).tkn(), taul::token(gram.lpr("lpr"), "a", 0));
                }
                else ADD_FAILURE();
            }
        }
        else ADD_FAILURE();

        EXPECT_EQ(&(pt.current()), &(pt.at(1)));
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(2), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 2);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 2);
    }

    pt.close();

    {
        EXPECT_FALSE(pt.finished());

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
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_TRUE(pt.at(1).is_lexical());
                EXPECT_FALSE(pt.at(1).is_syntactic());
                EXPECT_FALSE(pt.at(1).is_failure());

                EXPECT_THROW(pt.at(0).lpr(), taul::lpr_association_error);
                EXPECT_EQ(&(pt.at(0).ppr()), &(gram.ppr("ppr")));

                EXPECT_EQ(&(pt.at(1).lpr()), &(gram.lpr("lpr")));
                EXPECT_THROW(pt.at(1).ppr(), taul::ppr_association_error);

                EXPECT_EQ(pt.at(0).str(), "abc");
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);

                EXPECT_EQ(pt.at(1).str(), "a");
                EXPECT_EQ(pt.at(1).pos(), 0);
                if (pt.at(1).tkn()) {
                    EXPECT_EQ(*pt.at(1).tkn(), taul::token(gram.lpr("lpr"), "a", 0));
                }
                else ADD_FAILURE();
            }
        }
        else ADD_FAILURE();

        EXPECT_EQ(&(pt.current()), &(pt.at(0)));
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(2), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 2);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 2);
    }

    pt.open_lexical(taul::token(gram.lpr("lpr"), "b", 1));

    {
        EXPECT_FALSE(pt.finished());

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
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_TRUE(pt.at(1).is_lexical());
                EXPECT_FALSE(pt.at(1).is_syntactic());
                EXPECT_FALSE(pt.at(1).is_failure());

                EXPECT_TRUE(pt.at(2).is_lexical());
                EXPECT_FALSE(pt.at(2).is_syntactic());
                EXPECT_FALSE(pt.at(2).is_failure());

                EXPECT_THROW(pt.at(0).lpr(), taul::lpr_association_error);
                EXPECT_EQ(&(pt.at(0).ppr()), &(gram.ppr("ppr")));

                EXPECT_EQ(&(pt.at(1).lpr()), &(gram.lpr("lpr")));
                EXPECT_THROW(pt.at(1).ppr(), taul::ppr_association_error);

                EXPECT_EQ(&(pt.at(2).lpr()), &(gram.lpr("lpr")));
                EXPECT_THROW(pt.at(2).ppr(), taul::ppr_association_error);

                EXPECT_EQ(pt.at(0).str(), "abc");
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);

                EXPECT_EQ(pt.at(1).str(), "a");
                EXPECT_EQ(pt.at(1).pos(), 0);
                if (pt.at(1).tkn()) {
                    EXPECT_EQ(*pt.at(1).tkn(), taul::token(gram.lpr("lpr"), "a", 0));
                }
                else ADD_FAILURE();

                EXPECT_EQ(pt.at(2).str(), "b");
                EXPECT_EQ(pt.at(2).pos(), 1);
                if (pt.at(2).tkn()) {
                    EXPECT_EQ(*pt.at(2).tkn(), taul::token(gram.lpr("lpr"), "b", 1));
                }
                else ADD_FAILURE();
            }
        }
        else ADD_FAILURE();

        EXPECT_EQ(&(pt.current()), &(pt.at(2)));
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(3), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 3);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 3);
    }

    pt.close();

    {
        EXPECT_FALSE(pt.finished());

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
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_TRUE(pt.at(1).is_lexical());
                EXPECT_FALSE(pt.at(1).is_syntactic());
                EXPECT_FALSE(pt.at(1).is_failure());

                EXPECT_TRUE(pt.at(2).is_lexical());
                EXPECT_FALSE(pt.at(2).is_syntactic());
                EXPECT_FALSE(pt.at(2).is_failure());

                EXPECT_THROW(pt.at(0).lpr(), taul::lpr_association_error);
                EXPECT_EQ(&(pt.at(0).ppr()), &(gram.ppr("ppr")));

                EXPECT_EQ(&(pt.at(1).lpr()), &(gram.lpr("lpr")));
                EXPECT_THROW(pt.at(1).ppr(), taul::ppr_association_error);

                EXPECT_EQ(&(pt.at(2).lpr()), &(gram.lpr("lpr")));
                EXPECT_THROW(pt.at(2).ppr(), taul::ppr_association_error);

                EXPECT_EQ(pt.at(0).str(), "abc");
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);

                EXPECT_EQ(pt.at(1).str(), "a");
                EXPECT_EQ(pt.at(1).pos(), 0);
                if (pt.at(1).tkn()) {
                    EXPECT_EQ(*pt.at(1).tkn(), taul::token(gram.lpr("lpr"), "a", 0));
                }
                else ADD_FAILURE();

                EXPECT_EQ(pt.at(2).str(), "b");
                EXPECT_EQ(pt.at(2).pos(), 1);
                if (pt.at(2).tkn()) {
                    EXPECT_EQ(*pt.at(2).tkn(), taul::token(gram.lpr("lpr"), "b", 1));
                }
                else ADD_FAILURE();
            }
        }
        else ADD_FAILURE();

        EXPECT_EQ(&(pt.current()), &(pt.at(0)));
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(3), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 3);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 3);
    }

    pt.open_lexical(taul::token::failure("c", 2));

    {
        EXPECT_FALSE(pt.finished());

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
                EXPECT_EQ(pt.at(3).level(), 1);

                EXPECT_EQ(pt.at(0).parent(), pt.end());
                EXPECT_EQ(pt.at(0).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).left_child(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(0).right_child(), std::next(pt.begin(), 3));

                EXPECT_EQ(pt.at(1).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(1).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(1).right_sibling(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(1).left_child(), pt.end());
                EXPECT_EQ(pt.at(1).right_child(), pt.end());

                EXPECT_EQ(pt.at(2).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(2).left_sibling(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(2).right_sibling(), std::next(pt.begin(), 3));
                EXPECT_EQ(pt.at(2).left_child(), pt.end());
                EXPECT_EQ(pt.at(2).right_child(), pt.end());

                EXPECT_EQ(pt.at(3).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(3).left_sibling(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(3).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(3).left_child(), pt.end());
                EXPECT_EQ(pt.at(3).right_child(), pt.end());

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
                EXPECT_EQ(pt.at(2).children(), 0);
                EXPECT_FALSE(pt.at(2).has_children());

                EXPECT_TRUE(pt.at(3).has_parent());
                EXPECT_TRUE(pt.at(3).has_left_sibling());
                EXPECT_FALSE(pt.at(3).has_right_sibling());
                EXPECT_EQ(pt.at(3).children(), 0);
                EXPECT_FALSE(pt.at(3).has_children());

                EXPECT_FALSE(pt.at(0).is_lexical());
                EXPECT_TRUE(pt.at(0).is_syntactic());
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_TRUE(pt.at(1).is_lexical());
                EXPECT_FALSE(pt.at(1).is_syntactic());
                EXPECT_FALSE(pt.at(1).is_failure());

                EXPECT_TRUE(pt.at(2).is_lexical());
                EXPECT_FALSE(pt.at(2).is_syntactic());
                EXPECT_FALSE(pt.at(2).is_failure());

                EXPECT_TRUE(pt.at(3).is_lexical());
                EXPECT_FALSE(pt.at(3).is_syntactic());
                EXPECT_TRUE(pt.at(3).is_failure());

                EXPECT_THROW(pt.at(0).lpr(), taul::lpr_association_error);
                EXPECT_EQ(&(pt.at(0).ppr()), &(gram.ppr("ppr")));

                EXPECT_EQ(&(pt.at(1).lpr()), &(gram.lpr("lpr")));
                EXPECT_THROW(pt.at(1).ppr(), taul::ppr_association_error);

                EXPECT_EQ(&(pt.at(2).lpr()), &(gram.lpr("lpr")));
                EXPECT_THROW(pt.at(2).ppr(), taul::ppr_association_error);

                EXPECT_THROW(pt.at(3).lpr(), taul::lpr_association_error);
                EXPECT_THROW(pt.at(3).ppr(), taul::ppr_association_error);

                EXPECT_EQ(pt.at(0).str(), "abc");
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);

                EXPECT_EQ(pt.at(1).str(), "a");
                EXPECT_EQ(pt.at(1).pos(), 0);
                if (pt.at(1).tkn()) {
                    EXPECT_EQ(*pt.at(1).tkn(), taul::token(gram.lpr("lpr"), "a", 0));
                }
                else ADD_FAILURE();

                EXPECT_EQ(pt.at(2).str(), "b");
                EXPECT_EQ(pt.at(2).pos(), 1);
                if (pt.at(2).tkn()) {
                    EXPECT_EQ(*pt.at(2).tkn(), taul::token(gram.lpr("lpr"), "b", 1));
                }
                else ADD_FAILURE();

                EXPECT_EQ(pt.at(3).str(), "c");
                EXPECT_EQ(pt.at(3).pos(), 2);
                if (pt.at(3).tkn()) {
                    EXPECT_EQ(*pt.at(3).tkn(), taul::token::failure("c", 2));
                }
                else ADD_FAILURE();
            }
        }
        else ADD_FAILURE();

        EXPECT_EQ(&(pt.current()), &(pt.at(3)));
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(4), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 4);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 4);
    }

    pt.close();

    {
        EXPECT_FALSE(pt.finished());

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
                EXPECT_EQ(pt.at(3).level(), 1);

                EXPECT_EQ(pt.at(0).parent(), pt.end());
                EXPECT_EQ(pt.at(0).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).left_child(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(0).right_child(), std::next(pt.begin(), 3));

                EXPECT_EQ(pt.at(1).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(1).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(1).right_sibling(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(1).left_child(), pt.end());
                EXPECT_EQ(pt.at(1).right_child(), pt.end());

                EXPECT_EQ(pt.at(2).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(2).left_sibling(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(2).right_sibling(), std::next(pt.begin(), 3));
                EXPECT_EQ(pt.at(2).left_child(), pt.end());
                EXPECT_EQ(pt.at(2).right_child(), pt.end());

                EXPECT_EQ(pt.at(3).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(3).left_sibling(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(3).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(3).left_child(), pt.end());
                EXPECT_EQ(pt.at(3).right_child(), pt.end());

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
                EXPECT_EQ(pt.at(2).children(), 0);
                EXPECT_FALSE(pt.at(2).has_children());

                EXPECT_TRUE(pt.at(3).has_parent());
                EXPECT_TRUE(pt.at(3).has_left_sibling());
                EXPECT_FALSE(pt.at(3).has_right_sibling());
                EXPECT_EQ(pt.at(3).children(), 0);
                EXPECT_FALSE(pt.at(3).has_children());

                EXPECT_FALSE(pt.at(0).is_lexical());
                EXPECT_TRUE(pt.at(0).is_syntactic());
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_TRUE(pt.at(1).is_lexical());
                EXPECT_FALSE(pt.at(1).is_syntactic());
                EXPECT_FALSE(pt.at(1).is_failure());

                EXPECT_TRUE(pt.at(2).is_lexical());
                EXPECT_FALSE(pt.at(2).is_syntactic());
                EXPECT_FALSE(pt.at(2).is_failure());

                EXPECT_TRUE(pt.at(3).is_lexical());
                EXPECT_FALSE(pt.at(3).is_syntactic());
                EXPECT_TRUE(pt.at(3).is_failure());

                EXPECT_THROW(pt.at(0).lpr(), taul::lpr_association_error);
                EXPECT_EQ(&(pt.at(0).ppr()), &(gram.ppr("ppr")));

                EXPECT_EQ(&(pt.at(1).lpr()), &(gram.lpr("lpr")));
                EXPECT_THROW(pt.at(1).ppr(), taul::ppr_association_error);

                EXPECT_EQ(&(pt.at(2).lpr()), &(gram.lpr("lpr")));
                EXPECT_THROW(pt.at(2).ppr(), taul::ppr_association_error);

                EXPECT_THROW(pt.at(3).lpr(), taul::lpr_association_error);
                EXPECT_THROW(pt.at(3).ppr(), taul::ppr_association_error);

                EXPECT_EQ(pt.at(0).str(), "abc");
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);

                EXPECT_EQ(pt.at(1).str(), "a");
                EXPECT_EQ(pt.at(1).pos(), 0);
                if (pt.at(1).tkn()) {
                    EXPECT_EQ(*pt.at(1).tkn(), taul::token(gram.lpr("lpr"), "a", 0));
                }
                else ADD_FAILURE();

                EXPECT_EQ(pt.at(2).str(), "b");
                EXPECT_EQ(pt.at(2).pos(), 1);
                if (pt.at(2).tkn()) {
                    EXPECT_EQ(*pt.at(2).tkn(), taul::token(gram.lpr("lpr"), "b", 1));
                }
                else ADD_FAILURE();

                EXPECT_EQ(pt.at(3).str(), "c");
                EXPECT_EQ(pt.at(3).pos(), 2);
                if (pt.at(3).tkn()) {
                    EXPECT_EQ(*pt.at(3).tkn(), taul::token::failure("c", 2));
                }
                else ADD_FAILURE();
            }
        }
        else ADD_FAILURE();

        EXPECT_EQ(&(pt.current()), &(pt.at(0)));
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(4), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 4);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 4);
    }

    pt.close();

    {
        EXPECT_TRUE(pt.finished());

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
                EXPECT_EQ(pt.at(3).level(), 1);

                EXPECT_EQ(pt.at(0).parent(), pt.end());
                EXPECT_EQ(pt.at(0).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(0).left_child(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(0).right_child(), std::next(pt.begin(), 3));

                EXPECT_EQ(pt.at(1).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(1).left_sibling(), pt.end());
                EXPECT_EQ(pt.at(1).right_sibling(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(1).left_child(), pt.end());
                EXPECT_EQ(pt.at(1).right_child(), pt.end());

                EXPECT_EQ(pt.at(2).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(2).left_sibling(), std::next(pt.begin(), 1));
                EXPECT_EQ(pt.at(2).right_sibling(), std::next(pt.begin(), 3));
                EXPECT_EQ(pt.at(2).left_child(), pt.end());
                EXPECT_EQ(pt.at(2).right_child(), pt.end());

                EXPECT_EQ(pt.at(3).parent(), std::next(pt.begin(), 0));
                EXPECT_EQ(pt.at(3).left_sibling(), std::next(pt.begin(), 2));
                EXPECT_EQ(pt.at(3).right_sibling(), pt.end());
                EXPECT_EQ(pt.at(3).left_child(), pt.end());
                EXPECT_EQ(pt.at(3).right_child(), pt.end());

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
                EXPECT_EQ(pt.at(2).children(), 0);
                EXPECT_FALSE(pt.at(2).has_children());

                EXPECT_TRUE(pt.at(3).has_parent());
                EXPECT_TRUE(pt.at(3).has_left_sibling());
                EXPECT_FALSE(pt.at(3).has_right_sibling());
                EXPECT_EQ(pt.at(3).children(), 0);
                EXPECT_FALSE(pt.at(3).has_children());

                EXPECT_FALSE(pt.at(0).is_lexical());
                EXPECT_TRUE(pt.at(0).is_syntactic());
                EXPECT_FALSE(pt.at(0).is_failure());

                EXPECT_TRUE(pt.at(1).is_lexical());
                EXPECT_FALSE(pt.at(1).is_syntactic());
                EXPECT_FALSE(pt.at(1).is_failure());

                EXPECT_TRUE(pt.at(2).is_lexical());
                EXPECT_FALSE(pt.at(2).is_syntactic());
                EXPECT_FALSE(pt.at(2).is_failure());

                EXPECT_TRUE(pt.at(3).is_lexical());
                EXPECT_FALSE(pt.at(3).is_syntactic());
                EXPECT_TRUE(pt.at(3).is_failure());

                EXPECT_THROW(pt.at(0).lpr(), taul::lpr_association_error);
                EXPECT_EQ(&(pt.at(0).ppr()), &(gram.ppr("ppr")));

                EXPECT_EQ(&(pt.at(1).lpr()), &(gram.lpr("lpr")));
                EXPECT_THROW(pt.at(1).ppr(), taul::ppr_association_error);

                EXPECT_EQ(&(pt.at(2).lpr()), &(gram.lpr("lpr")));
                EXPECT_THROW(pt.at(2).ppr(), taul::ppr_association_error);

                EXPECT_THROW(pt.at(3).lpr(), taul::lpr_association_error);
                EXPECT_THROW(pt.at(3).ppr(), taul::ppr_association_error);

                EXPECT_EQ(pt.at(0).str(), "abc");
                EXPECT_EQ(pt.at(0).pos(), 0);
                EXPECT_EQ(pt.at(0).tkn(), std::nullopt);

                EXPECT_EQ(pt.at(1).str(), "a");
                EXPECT_EQ(pt.at(1).pos(), 0);
                if (pt.at(1).tkn()) {
                    EXPECT_EQ(*pt.at(1).tkn(), taul::token(gram.lpr("lpr"), "a", 0));
                }
                else ADD_FAILURE();

                EXPECT_EQ(pt.at(2).str(), "b");
                EXPECT_EQ(pt.at(2).pos(), 1);
                if (pt.at(2).tkn()) {
                    EXPECT_EQ(*pt.at(2).tkn(), taul::token(gram.lpr("lpr"), "b", 1));
                }
                else ADD_FAILURE();

                EXPECT_EQ(pt.at(3).str(), "c");
                EXPECT_EQ(pt.at(3).pos(), 2);
                if (pt.at(3).tkn()) {
                    EXPECT_EQ(*pt.at(3).tkn(), taul::token::failure("c", 2));
                }
                else ADD_FAILURE();
            }
        }
        else ADD_FAILURE();

        EXPECT_THROW(pt.current(), std::out_of_range);
        EXPECT_EQ(&(pt.root()), &(pt.at(0)));

        EXPECT_THROW(pt.at(4), std::out_of_range);

        EXPECT_EQ(std::distance(pt.cbegin(), pt.cend()), 4);
        EXPECT_EQ(std::distance(pt.begin(), pt.end()), 4);
    }


    TAUL_LOG(lgr, "{}", pt);
}

