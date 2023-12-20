

#include <gtest/gtest.h>

#include <taul/all.h>


class NodeTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr;
    taul::grammar gram;


    void SetUp() override final {

        lgr = taul::make_stderr_logger();

        auto spec =
            taul::spec_writer()
            .lpr_decl("lpr")
            .lpr_decl("lpr2")
            .ppr_decl("ppr")
            .ppr_decl("ppr2")
            .lpr("lpr")
            .close()
            .lpr("lpr2")
            .close()
            .ppr("ppr")
            .close()
            .ppr("ppr2")
            .close()
            .done();

        gram = std::move(*taul::load(spec, lgr));
    }
};


TEST_F(NodeTests, NodeCtx_Basics) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, NodeCtx_Clone) {

    auto nc0 = std::make_shared<taul::node_ctx>(lgr);
    auto nc1 = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc0);
    ASSERT_TRUE((bool)nc1);

    EXPECT_EQ(nc0->count(), 0);
    EXPECT_EQ(nc1->count(), 0);

    {
        auto node0a = nc0->create(gram.ppr("ppr"), "abc", 10);
        auto node1a = nc0->create(gram.lpr("lpr"), "def", 11);
        auto node2a = nc0->create(taul::token::failure("ghi", 3));
        auto node3a = nc0->create(gram.lpr("lpr"), "ggg", 31);
        auto node4a = nc0->create(gram.ppr("ppr"), "hh", 444);

        node1a.attach(std::move(node2a));
        node1a.attach(std::move(node3a));
        node0a.attach(std::move(node1a));
        node0a.attach(std::move(node4a));

        EXPECT_EQ(nc0->count(), 5);
        EXPECT_EQ(nc1->count(), 0);

        TAUL_LOG(lgr, "original:\n{}", node0a.fmt_tree());

        auto node0b = nc1->clone(node0a);

        EXPECT_EQ(nc0->count(), 5);
        EXPECT_EQ(nc1->count(), 5);

        TAUL_LOG(lgr, "clone:\n{}", node0b.fmt_tree());

        // obvious this presumes that equiv works as expected

        EXPECT_TRUE(node0a.equiv(node0b));
    }

    EXPECT_EQ(nc0->count(), 0);
    EXPECT_EQ(nc1->count(), 0);
}

TEST_F(NodeTests, Node_Basics) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    EXPECT_EQ(nc->count(), 0);

    ASSERT_TRUE((bool)nc);

    {
        auto node0 = nc->create(gram.lpr("lpr"), "abc", 3);
        auto node1 = nc->create(taul::token(gram.lpr("lpr"), "abc", 3));
        auto node2 = nc->create(taul::token::failure("abc", 3));
        auto node3 = nc->create(gram.ppr("ppr"), "abc", 3);

        EXPECT_EQ(nc->count(), 4);

        {
            EXPECT_EQ(node0.ctx(), nc.get());

            EXPECT_TRUE(node0.is_lexical());
            EXPECT_FALSE(node0.is_syntactic());
            EXPECT_FALSE(node0.is_failure());

            EXPECT_NO_THROW(node0.lpr());
            EXPECT_THROW(node0.ppr(), taul::ppr_association_error);

            try {
                EXPECT_EQ(&(node0.lpr()), &(gram.lpr("lpr")));
            }
            catch (...) {}

            EXPECT_EQ(node0.str(), "abc");
            EXPECT_EQ(node0.pos(), 3);

            if ((bool)node0.tkn()) {
                EXPECT_EQ(*node0.tkn(), taul::token(gram.lpr("lpr"), "abc", 3));
            }
            else ADD_FAILURE();

            const auto& cnst = (const decltype(node0)&)node0;

            EXPECT_EQ(node0.parent(), node0.end());
            EXPECT_EQ(node0.left_child(), node0.end());
            EXPECT_EQ(node0.right_child(), node0.end());
            EXPECT_EQ(node0.left_sibling(), node0.end());
            EXPECT_EQ(node0.right_sibling(), node0.end());
            
            EXPECT_EQ(cnst.parent(), cnst.end());
            EXPECT_EQ(cnst.left_child(), cnst.end());
            EXPECT_EQ(cnst.right_child(), cnst.end());
            EXPECT_EQ(cnst.left_sibling(), cnst.end());
            EXPECT_EQ(cnst.right_sibling(), cnst.end());

            EXPECT_EQ(node0.children(), 0);

            EXPECT_EQ(node0.begin(), node0.end());
            EXPECT_EQ(node0.cbegin(), node0.cend());
            EXPECT_EQ(cnst.begin(), cnst.end());

            EXPECT_EQ(node0.rbegin(), node0.rend());
            EXPECT_EQ(node0.crbegin(), node0.crend());
            EXPECT_EQ(cnst.rbegin(), cnst.rend());

            EXPECT_FALSE(node0.end().is_decrementable_pte());
            EXPECT_FALSE(node0.cend().is_decrementable_pte());
            EXPECT_FALSE(cnst.end().is_decrementable_pte());
        }

        {
            EXPECT_EQ(node1.ctx(), nc.get());

            EXPECT_TRUE(node1.is_lexical());
            EXPECT_FALSE(node1.is_syntactic());
            EXPECT_FALSE(node1.is_failure());

            EXPECT_NO_THROW(node1.lpr());
            EXPECT_THROW(node1.ppr(), taul::ppr_association_error);

            try {
                EXPECT_EQ(&(node1.lpr()), &(gram.lpr("lpr")));
            }
            catch (...) {}

            EXPECT_EQ(node1.str(), "abc");
            EXPECT_EQ(node1.pos(), 3);

            if ((bool)node1.tkn()) {
                EXPECT_EQ(*node1.tkn(), taul::token(gram.lpr("lpr"), "abc", 3));
            }
            else ADD_FAILURE();

            const auto& cnst = (const decltype(node1)&)node1;

            EXPECT_EQ(node1.parent(), node1.end());
            EXPECT_EQ(node1.left_child(), node1.end());
            EXPECT_EQ(node1.right_child(), node1.end());
            EXPECT_EQ(node1.left_sibling(), node1.end());
            EXPECT_EQ(node1.right_sibling(), node1.end());

            EXPECT_EQ(cnst.parent(), cnst.end());
            EXPECT_EQ(cnst.left_child(), cnst.end());
            EXPECT_EQ(cnst.right_child(), cnst.end());
            EXPECT_EQ(cnst.left_sibling(), cnst.end());
            EXPECT_EQ(cnst.right_sibling(), cnst.end());

            EXPECT_EQ(node1.children(), 0);

            EXPECT_EQ(node1.begin(), node1.end());
            EXPECT_EQ(node1.cbegin(), node1.cend());
            EXPECT_EQ(cnst.begin(), cnst.end());

            EXPECT_EQ(node1.rbegin(), node1.rend());
            EXPECT_EQ(node1.crbegin(), node1.crend());
            EXPECT_EQ(cnst.rbegin(), cnst.rend());

            EXPECT_FALSE(node1.end().is_decrementable_pte());
            EXPECT_FALSE(node1.cend().is_decrementable_pte());
            EXPECT_FALSE(cnst.end().is_decrementable_pte());
        }

        {
            EXPECT_EQ(node2.ctx(), nc.get());

            EXPECT_TRUE(node2.is_lexical());
            EXPECT_FALSE(node2.is_syntactic());
            EXPECT_TRUE(node2.is_failure());

            EXPECT_THROW(node2.lpr(), taul::lpr_association_error);
            EXPECT_THROW(node2.ppr(), taul::ppr_association_error);

            EXPECT_EQ(node2.str(), "abc");
            EXPECT_EQ(node2.pos(), 3);

            if ((bool)node2.tkn()) {
                EXPECT_EQ(*node2.tkn(), taul::token::failure("abc", 3));
            }
            else ADD_FAILURE();

            const auto& cnst = (const decltype(node2)&)node2;

            EXPECT_EQ(node2.parent(), node2.end());
            EXPECT_EQ(node2.left_child(), node2.end());
            EXPECT_EQ(node2.right_child(), node2.end());
            EXPECT_EQ(node2.left_sibling(), node2.end());
            EXPECT_EQ(node2.right_sibling(), node2.end());

            EXPECT_EQ(cnst.parent(), cnst.end());
            EXPECT_EQ(cnst.left_child(), cnst.end());
            EXPECT_EQ(cnst.right_child(), cnst.end());
            EXPECT_EQ(cnst.left_sibling(), cnst.end());
            EXPECT_EQ(cnst.right_sibling(), cnst.end());

            EXPECT_EQ(node2.children(), 0);

            EXPECT_EQ(node2.begin(), node2.end());
            EXPECT_EQ(node2.cbegin(), node2.cend());
            EXPECT_EQ(cnst.begin(), cnst.end());

            EXPECT_EQ(node2.rbegin(), node2.rend());
            EXPECT_EQ(node2.crbegin(), node2.crend());
            EXPECT_EQ(cnst.rbegin(), cnst.rend());

            EXPECT_FALSE(node2.end().is_decrementable_pte());
            EXPECT_FALSE(node2.cend().is_decrementable_pte());
            EXPECT_FALSE(cnst.end().is_decrementable_pte());
        }

        {
            EXPECT_EQ(node3.ctx(), nc.get());

            EXPECT_FALSE(node3.is_lexical());
            EXPECT_TRUE(node3.is_syntactic());
            EXPECT_FALSE(node3.is_failure());

            EXPECT_THROW(node3.lpr(), taul::lpr_association_error);
            EXPECT_NO_THROW(node3.ppr());

            try {
                EXPECT_EQ(&(node3.ppr()), &(gram.ppr("ppr")));
            }
            catch (...) {}

            EXPECT_EQ(node3.str(), "abc");
            EXPECT_EQ(node3.pos(), 3);

            EXPECT_FALSE((bool)node3.tkn());

            const auto& cnst = (const decltype(node3)&)node3;

            EXPECT_EQ(node3.parent(), node3.end());
            EXPECT_EQ(node3.left_child(), node3.end());
            EXPECT_EQ(node3.right_child(), node3.end());
            EXPECT_EQ(node3.left_sibling(), node3.end());
            EXPECT_EQ(node3.right_sibling(), node3.end());

            EXPECT_EQ(cnst.parent(), cnst.end());
            EXPECT_EQ(cnst.left_child(), cnst.end());
            EXPECT_EQ(cnst.right_child(), cnst.end());
            EXPECT_EQ(cnst.left_sibling(), cnst.end());
            EXPECT_EQ(cnst.right_sibling(), cnst.end());

            EXPECT_EQ(node3.children(), 0);

            EXPECT_EQ(node3.begin(), node3.end());
            EXPECT_EQ(node3.cbegin(), node3.cend());
            EXPECT_EQ(cnst.begin(), cnst.end());

            EXPECT_EQ(node3.rbegin(), node3.rend());
            EXPECT_EQ(node3.crbegin(), node3.crend());
            EXPECT_EQ(cnst.rbegin(), cnst.rend());

            EXPECT_FALSE(node3.end().is_decrementable_pte());
            EXPECT_FALSE(node3.cend().is_decrementable_pte());
            EXPECT_FALSE(cnst.end().is_decrementable_pte());
        }
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, Node_MoveCtor) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        auto node0 = nc->create(gram.lpr("lpr"), "abc", 3);

        EXPECT_EQ(nc->count(), 1);

        {
            taul::node node1(std::move(node0));

            EXPECT_EQ(nc->count(), 1);

            EXPECT_EQ(node1.ctx(), nc.get());

            EXPECT_TRUE(node1.is_lexical());
            EXPECT_FALSE(node1.is_syntactic());
            EXPECT_FALSE(node1.is_failure());

            EXPECT_NO_THROW(node1.lpr());
            EXPECT_THROW(node1.ppr(), taul::ppr_association_error);

            try {
                EXPECT_EQ(&(node1.lpr()), &(gram.lpr("lpr")));
            }
            catch (...) {}

            EXPECT_EQ(node1.str(), "abc");
            EXPECT_EQ(node1.pos(), 3);

            if ((bool)node1.tkn()) {
                EXPECT_EQ(*node1.tkn(), taul::token(gram.lpr("lpr"), "abc", 3));
            }
            else ADD_FAILURE();

            const auto& cnst = (const decltype(node1)&)node1;

            EXPECT_EQ(node1.parent(), node1.end());
            EXPECT_EQ(node1.left_child(), node1.end());
            EXPECT_EQ(node1.right_child(), node1.end());
            EXPECT_EQ(node1.left_sibling(), node1.end());
            EXPECT_EQ(node1.right_sibling(), node1.end());

            EXPECT_EQ(cnst.parent(), cnst.end());
            EXPECT_EQ(cnst.left_child(), cnst.end());
            EXPECT_EQ(cnst.right_child(), cnst.end());
            EXPECT_EQ(cnst.left_sibling(), cnst.end());
            EXPECT_EQ(cnst.right_sibling(), cnst.end());

            EXPECT_EQ(node1.children(), 0);

            EXPECT_EQ(node1.begin(), node1.end());
            EXPECT_EQ(node1.cbegin(), node1.cend());
            EXPECT_EQ(cnst.begin(), cnst.end());

            EXPECT_EQ(node1.rbegin(), node1.rend());
            EXPECT_EQ(node1.crbegin(), node1.crend());
            EXPECT_EQ(cnst.rbegin(), cnst.rend());

            EXPECT_FALSE(node1.end().is_decrementable_pte());
            EXPECT_FALSE(node1.cend().is_decrementable_pte());
            EXPECT_FALSE(cnst.end().is_decrementable_pte());
        }

        EXPECT_EQ(nc->count(), 0);
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, Node_MoveCtor_WithChildNodes) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        taul::node node0 = nc->create(gram.lpr("lpr"), "abc", 3);

        taul::node child0 = nc->create(gram.lpr("lpr"), "bb", 1);
        taul::node child1 = nc->create(gram.lpr("lpr"), "cc", 2);
        taul::node child2 = nc->create(gram.lpr("lpr"), "dd", 3);

        taul::node_iter child_iter0 = child0.iter();
        taul::node_iter child_iter1 = child1.iter();
        taul::node_iter child_iter2 = child2.iter();

        node0.attach(std::move(child0));
        node0.attach(std::move(child1));
        node0.attach(std::move(child2));

        EXPECT_EQ(nc->count(), 4);

        {
            taul::node node1(std::move(node0));

            EXPECT_EQ(nc->count(), 4);

            TAUL_LOG(lgr, "{}", node1.fmt_tree());

            EXPECT_EQ(node1.ctx(), nc.get());

            EXPECT_TRUE(node1.is_lexical());
            EXPECT_FALSE(node1.is_syntactic());
            EXPECT_FALSE(node1.is_failure());

            EXPECT_NO_THROW(node1.lpr());
            EXPECT_THROW(node1.ppr(), taul::ppr_association_error);

            try {
                EXPECT_EQ(&(node1.lpr()), &(gram.lpr("lpr")));
            }
            catch (...) {}

            EXPECT_EQ(node1.str(), "abc");
            EXPECT_EQ(node1.pos(), 3);

            if ((bool)node1.tkn()) {
                EXPECT_EQ(*node1.tkn(), taul::token(gram.lpr("lpr"), "abc", 3));
            }
            else ADD_FAILURE();

            const auto& cnst = (const decltype(node1)&)node1;

            EXPECT_EQ(node1.parent(), node1.end());
            EXPECT_EQ(node1.left_child(),  child_iter0);
            EXPECT_EQ(node1.right_child(), child_iter2);
            EXPECT_EQ(node1.left_sibling(), node1.end());
            EXPECT_EQ(node1.right_sibling(), node1.end());

            EXPECT_EQ(cnst.parent(), cnst.end());
            EXPECT_EQ(cnst.left_child(), child_iter0.to_const());
            EXPECT_EQ(cnst.right_child(), child_iter2.to_const());
            EXPECT_EQ(cnst.left_sibling(), cnst.end());
            EXPECT_EQ(cnst.right_sibling(), cnst.end());

            EXPECT_EQ(node1.children(), 3);

            ASSERT_EQ(std::next(node1.begin(), 0), child_iter0);
            ASSERT_EQ(std::next(node1.begin(), 1), child_iter1);
            ASSERT_EQ(std::next(node1.begin(), 2), child_iter2);
            ASSERT_EQ(std::next(node1.begin(), 3), node1.end());

            ASSERT_EQ(std::next(node1.cbegin(), 0), child_iter0);
            ASSERT_EQ(std::next(node1.cbegin(), 1), child_iter1);
            ASSERT_EQ(std::next(node1.cbegin(), 2), child_iter2);
            ASSERT_EQ(std::next(node1.cbegin(), 3), node1.cend());

            ASSERT_EQ(std::next(cnst.begin(), 0), child_iter0);
            ASSERT_EQ(std::next(cnst.begin(), 1), child_iter1);
            ASSERT_EQ(std::next(cnst.begin(), 2), child_iter2);
            ASSERT_EQ(std::next(cnst.begin(), 3), cnst.end());

            ASSERT_EQ(std::next(node1.rbegin(), 0).base(), node1.end());
            ASSERT_EQ(std::next(node1.rbegin(), 1).base(), child_iter2);
            ASSERT_EQ(std::next(node1.rbegin(), 2).base(), child_iter1);
            ASSERT_EQ(std::next(node1.rbegin(), 3).base(), child_iter0);

            ASSERT_EQ(std::next(node1.crbegin(), 0).base(), node1.cend());
            ASSERT_EQ(std::next(node1.crbegin(), 1).base(), child_iter2.to_const());
            ASSERT_EQ(std::next(node1.crbegin(), 2).base(), child_iter1.to_const());
            ASSERT_EQ(std::next(node1.crbegin(), 3).base(), child_iter0.to_const());

            ASSERT_EQ(std::next(cnst.rbegin(), 0).base(), cnst.end());
            ASSERT_EQ(std::next(cnst.rbegin(), 1).base(), child_iter2.to_const());
            ASSERT_EQ(std::next(cnst.rbegin(), 2).base(), child_iter1.to_const());
            ASSERT_EQ(std::next(cnst.rbegin(), 3).base(), child_iter0.to_const());
        }

        EXPECT_EQ(nc->count(), 0);
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, Node_MoveAssign) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        taul::node node0 = nc->create(gram.ppr("ppr"), "12345");
        taul::node node1 = nc->create(gram.lpr("lpr"), "abc", 3);

        EXPECT_EQ(nc->count(), 2);

        taul::node node2(std::move(node0));

        EXPECT_EQ(nc->count(), 2);

        {
            node2 = std::move(node1); // move assign

            EXPECT_EQ(nc->count(), 1);

            EXPECT_EQ(node2.ctx(), nc.get());

            EXPECT_TRUE(node2.is_lexical());
            EXPECT_FALSE(node2.is_syntactic());
            EXPECT_FALSE(node2.is_failure());

            EXPECT_NO_THROW(node2.lpr());
            EXPECT_THROW(node2.ppr(), taul::ppr_association_error);

            try {
                EXPECT_EQ(&(node2.lpr()), &(gram.lpr("lpr")));
            }
            catch (...) {}

            EXPECT_EQ(node2.str(), "abc");
            EXPECT_EQ(node2.pos(), 3);

            if ((bool)node2.tkn()) {
                EXPECT_EQ(*node2.tkn(), taul::token(gram.lpr("lpr"), "abc", 3));
            }
            else ADD_FAILURE();

            const auto& cnst = (const decltype(node2)&)node2;

            EXPECT_EQ(node2.parent(), node2.end());
            EXPECT_EQ(node2.left_child(), node2.end());
            EXPECT_EQ(node2.right_child(), node2.end());
            EXPECT_EQ(node2.left_sibling(), node2.end());
            EXPECT_EQ(node2.right_sibling(), node2.end());

            EXPECT_EQ(cnst.parent(), cnst.end());
            EXPECT_EQ(cnst.left_child(), cnst.end());
            EXPECT_EQ(cnst.right_child(), cnst.end());
            EXPECT_EQ(cnst.left_sibling(), cnst.end());
            EXPECT_EQ(cnst.right_sibling(), cnst.end());

            EXPECT_EQ(node2.children(), 0);

            EXPECT_EQ(node2.begin(), node2.end());
            EXPECT_EQ(node2.cbegin(), node2.cend());
            EXPECT_EQ(cnst.begin(), cnst.end());

            EXPECT_EQ(node2.rbegin(), node2.rend());
            EXPECT_EQ(node2.crbegin(), node2.crend());
            EXPECT_EQ(cnst.rbegin(), cnst.rend());

            EXPECT_FALSE(node2.end().is_decrementable_pte());
            EXPECT_FALSE(node2.cend().is_decrementable_pte());
            EXPECT_FALSE(cnst.end().is_decrementable_pte());
        }
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, Node_MoveAssign_WithChildNodes) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        taul::node node0 = nc->create(gram.ppr("ppr"), "12345");
        taul::node node1 = nc->create(gram.lpr("lpr"), "abc", 3);

        taul::node child0 = nc->create(gram.lpr("lpr"), "bb", 1);
        taul::node child1 = nc->create(gram.lpr("lpr"), "cc", 2);
        taul::node child2 = nc->create(gram.lpr("lpr"), "dd", 3);

        taul::node_iter child_iter0 = child0.iter();
        taul::node_iter child_iter1 = child1.iter();
        taul::node_iter child_iter2 = child2.iter();

        node1.attach(std::move(child0));
        node1.attach(std::move(child1));
        node1.attach(std::move(child2));

        EXPECT_EQ(nc->count(), 5);

        taul::node node2(std::move(node0));

        EXPECT_EQ(nc->count(), 5);

        {
            node2 = std::move(node1); // move assign

            EXPECT_EQ(nc->count(), 4);

            TAUL_LOG(lgr, "{}", node2.fmt_tree());

            //TAUL_LOG(lgr, "node2.children()=={}", node2.children());
            //TAUL_LOG(lgr, "contents");
            //for (const auto& I : node2) {
            //    TAUL_LOG(lgr, "    I=={}", *(I.tkn()));
            //}
            //TAUL_LOG(lgr, "contents (reversed)");
            //for (auto it = node2.rbegin(); it != node2.rend(); it++) {
            //    TAUL_LOG(lgr, "    I=={}", *(it->tkn()));
            //}

            EXPECT_EQ(node2.ctx(), nc.get());

            EXPECT_TRUE(node2.is_lexical());
            EXPECT_FALSE(node2.is_syntactic());
            EXPECT_FALSE(node2.is_failure());

            EXPECT_NO_THROW(node2.lpr());
            EXPECT_THROW(node2.ppr(), taul::ppr_association_error);

            try {
                EXPECT_EQ(&(node2.lpr()), &(gram.lpr("lpr")));
            }
            catch (...) {}

            EXPECT_EQ(node2.str(), "abc");
            EXPECT_EQ(node2.pos(), 3);

            if ((bool)node2.tkn()) {
                EXPECT_EQ(*node2.tkn(), taul::token(gram.lpr("lpr"), "abc", 3));
            }
            else ADD_FAILURE();

            const auto& cnst = (const decltype(node2)&)node2;

            EXPECT_EQ(node2.parent(), node2.end());
            EXPECT_EQ(node2.left_child(), child_iter0);
            EXPECT_EQ(node2.right_child(), child_iter2);
            EXPECT_EQ(node2.left_sibling(), node2.end());
            EXPECT_EQ(node2.right_sibling(), node2.end());

            EXPECT_EQ(cnst.parent(), cnst.end());
            EXPECT_EQ(cnst.left_child(), child_iter0.to_const());
            EXPECT_EQ(cnst.right_child(), child_iter2.to_const());
            EXPECT_EQ(cnst.left_sibling(), cnst.end());
            EXPECT_EQ(cnst.right_sibling(), cnst.end());

            EXPECT_EQ(node2.children(), 3);

            ASSERT_EQ(std::next(node2.begin(), 0), child_iter0);
            ASSERT_EQ(std::next(node2.begin(), 1), child_iter1);
            ASSERT_EQ(std::next(node2.begin(), 2), child_iter2);
            ASSERT_EQ(std::next(node2.begin(), 3), node2.end());

            ASSERT_EQ(std::next(node2.cbegin(), 0), child_iter0);
            ASSERT_EQ(std::next(node2.cbegin(), 1), child_iter1);
            ASSERT_EQ(std::next(node2.cbegin(), 2), child_iter2);
            ASSERT_EQ(std::next(node2.cbegin(), 3), node2.cend());

            ASSERT_EQ(std::next(cnst.begin(), 0), child_iter0);
            ASSERT_EQ(std::next(cnst.begin(), 1), child_iter1);
            ASSERT_EQ(std::next(cnst.begin(), 2), child_iter2);
            ASSERT_EQ(std::next(cnst.begin(), 3), cnst.end());

            ASSERT_EQ(std::next(node2.rbegin(), 0).base(), std::next(child_iter2));
            ASSERT_EQ(std::next(node2.rbegin(), 1).base(), std::next(child_iter1));
            ASSERT_EQ(std::next(node2.rbegin(), 2).base(), std::next(child_iter0));
            ASSERT_EQ(std::next(node2.rbegin(), 3), node2.rend());

            ASSERT_EQ(std::next(node2.crbegin(), 0).base(), std::next(child_iter2));
            ASSERT_EQ(std::next(node2.crbegin(), 1).base(), std::next(child_iter1));
            ASSERT_EQ(std::next(node2.crbegin(), 2).base(), std::next(child_iter0));
            ASSERT_EQ(std::next(node2.crbegin(), 3), node2.crend());

            ASSERT_EQ(std::next(cnst.rbegin(), 0).base(), std::next(child_iter2));
            ASSERT_EQ(std::next(cnst.rbegin(), 1).base(), std::next(child_iter1));
            ASSERT_EQ(std::next(cnst.rbegin(), 2).base(), std::next(child_iter0));
            ASSERT_EQ(std::next(cnst.rbegin(), 3), cnst.rend());
        }
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, Node_Equiv) {

    auto nc0 = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc0);

    EXPECT_EQ(nc0->count(), 0);

    {
        auto node0a = nc0->create(gram.ppr("ppr"), "abc", 10);
        auto node1a = nc0->create(gram.lpr("lpr"), "def", 11);
        auto node2a = nc0->create(taul::token::failure("ghi", 3));
        auto node3a = nc0->create(gram.lpr("lpr"), "ggg", 31);
        auto node4a = nc0->create(gram.ppr("ppr"), "hh", 444);

        node1a.attach(std::move(node2a));
        node1a.attach(std::move(node3a));
        node0a.attach(std::move(node1a));
        node0a.attach(std::move(node4a));

        EXPECT_EQ(nc0->count(), 5);

        TAUL_LOG(lgr, "node0a:\n{}", node0a.fmt_tree());

        auto node0b = nc0->create(gram.ppr("ppr"), "abc", 10);
        auto node1b = nc0->create(gram.lpr("lpr"), "def", 11);
        auto node2b = nc0->create(taul::token::failure("ghi", 3));
        auto node3b = nc0->create(gram.lpr("lpr"), "ggg", 31);
        auto node4b = nc0->create(gram.ppr("ppr"), "hh", 444);

        node1b.attach(std::move(node2b));
        node1b.attach(std::move(node3b));
        node0b.attach(std::move(node1b));
        node0b.attach(std::move(node4b));

        EXPECT_EQ(nc0->count(), 10);

        TAUL_LOG(lgr, "node0b:\n{}", node0b.fmt_tree());

        auto node0c = nc0->create(gram.lpr("lpr"), "def", 11);
        auto node1c = nc0->create(taul::token::failure("ghi", 3));
        auto node2c = nc0->create(gram.lpr("lpr"), "ggg", 31);

        node0c.attach(std::move(node1c));
        node0c.attach(std::move(node2c));

        EXPECT_EQ(nc0->count(), 13);

        TAUL_LOG(lgr, "node0c:\n{}", node0c.fmt_tree());

        auto node0d = nc0->create(gram.lpr("lpr"), "123", 11);
        auto node1d = nc0->create(taul::token::failure("ghi", 3));
        auto node2d = nc0->create(gram.lpr("lpr"), "ggg", 31);

        node0d.attach(std::move(node1d));
        node0d.attach(std::move(node2d));

        EXPECT_EQ(nc0->count(), 16);

        TAUL_LOG(lgr, "node0d:\n{}", node0d.fmt_tree());

        auto node0e = nc0->create(gram.ppr("ppr"), "123", 11);

        EXPECT_EQ(nc0->count(), 17);

        TAUL_LOG(lgr, "node0e:\n{}", node0e.fmt_tree());

        auto node0f = nc0->create(gram.ppr("ppr"), "123", 11);

        EXPECT_EQ(nc0->count(), 18);

        TAUL_LOG(lgr, "node0f:\n{}", node0f.fmt_tree());

        auto node0g = nc0->create(gram.lpr("lpr"), "000", 414);

        EXPECT_EQ(nc0->count(), 19);

        TAUL_LOG(lgr, "node0g:\n{}", node0g.fmt_tree());

        auto node0h = nc0->create(gram.lpr("lpr2"), "000", 414);

        EXPECT_EQ(nc0->count(), 20);

        TAUL_LOG(lgr, "node0h:\n{}", node0h.fmt_tree());

        auto node0i = nc0->create(gram.ppr("ppr"), "000", 414);

        EXPECT_EQ(nc0->count(), 21);

        TAUL_LOG(lgr, "node0i:\n{}", node0i.fmt_tree());

        auto node0j = nc0->create(gram.ppr("ppr2"), "000", 414);

        EXPECT_EQ(nc0->count(), 22);

        TAUL_LOG(lgr, "node0j:\n{}", node0j.fmt_tree());


        EXPECT_TRUE(node0a.equiv(node0a));
        EXPECT_TRUE(node0a.equiv(node0b));
        EXPECT_FALSE(node0a.equiv(*node0a.begin()));
        EXPECT_FALSE(node0a.equiv(*node0b.begin()));
        EXPECT_FALSE(node0a.equiv(node0c));
        EXPECT_FALSE(node0a.equiv(node0d));
        EXPECT_FALSE(node0a.equiv(node0e));
        EXPECT_FALSE(node0a.equiv(node0f));
        EXPECT_FALSE(node0a.equiv(node0g));
        EXPECT_FALSE(node0a.equiv(node0h));
        EXPECT_FALSE(node0a.equiv(node0i));
        EXPECT_FALSE(node0a.equiv(node0j));

        EXPECT_TRUE(node0b.equiv(node0a));
        EXPECT_TRUE(node0b.equiv(node0b));
        EXPECT_FALSE(node0b.equiv(*node0a.begin()));
        EXPECT_FALSE(node0b.equiv(*node0b.begin()));
        EXPECT_FALSE(node0b.equiv(node0c));
        EXPECT_FALSE(node0b.equiv(node0d));
        EXPECT_FALSE(node0b.equiv(node0e));
        EXPECT_FALSE(node0b.equiv(node0f));
        EXPECT_FALSE(node0b.equiv(node0g));
        EXPECT_FALSE(node0b.equiv(node0h));
        EXPECT_FALSE(node0b.equiv(node0i));
        EXPECT_FALSE(node0b.equiv(node0j));

        EXPECT_FALSE(node0c.equiv(node0a));
        EXPECT_FALSE(node0c.equiv(node0b));
        EXPECT_TRUE(node0c.equiv(*node0a.begin()));
        EXPECT_TRUE(node0c.equiv(*node0b.begin()));
        EXPECT_TRUE(node0c.equiv(node0c));
        EXPECT_FALSE(node0c.equiv(node0d));
        EXPECT_FALSE(node0c.equiv(node0e));
        EXPECT_FALSE(node0c.equiv(node0f));
        EXPECT_FALSE(node0c.equiv(node0g));
        EXPECT_FALSE(node0c.equiv(node0h));
        EXPECT_FALSE(node0c.equiv(node0i));
        EXPECT_FALSE(node0c.equiv(node0j));

        EXPECT_FALSE(node0d.equiv(node0a));
        EXPECT_FALSE(node0d.equiv(node0b));
        EXPECT_FALSE(node0d.equiv(*node0a.begin()));
        EXPECT_FALSE(node0d.equiv(*node0b.begin()));
        EXPECT_FALSE(node0d.equiv(node0c));
        EXPECT_TRUE(node0d.equiv(node0d));
        EXPECT_FALSE(node0d.equiv(node0e));
        EXPECT_FALSE(node0d.equiv(node0f));
        EXPECT_FALSE(node0d.equiv(node0g));
        EXPECT_FALSE(node0d.equiv(node0h));
        EXPECT_FALSE(node0d.equiv(node0i));
        EXPECT_FALSE(node0d.equiv(node0j));

        EXPECT_FALSE(node0e.equiv(node0a));
        EXPECT_FALSE(node0e.equiv(node0b));
        EXPECT_FALSE(node0e.equiv(*node0a.begin()));
        EXPECT_FALSE(node0e.equiv(*node0b.begin()));
        EXPECT_FALSE(node0e.equiv(node0c));
        EXPECT_FALSE(node0e.equiv(node0d));
        EXPECT_TRUE(node0e.equiv(node0e));
        EXPECT_TRUE(node0e.equiv(node0f));
        EXPECT_FALSE(node0e.equiv(node0g));
        EXPECT_FALSE(node0e.equiv(node0h));
        EXPECT_FALSE(node0e.equiv(node0i));
        EXPECT_FALSE(node0e.equiv(node0j));

        EXPECT_FALSE(node0f.equiv(node0a));
        EXPECT_FALSE(node0f.equiv(node0b));
        EXPECT_FALSE(node0f.equiv(*node0a.begin()));
        EXPECT_FALSE(node0f.equiv(*node0b.begin()));
        EXPECT_FALSE(node0f.equiv(node0c));
        EXPECT_FALSE(node0f.equiv(node0d));
        EXPECT_TRUE(node0f.equiv(node0e));
        EXPECT_TRUE(node0f.equiv(node0f));
        EXPECT_FALSE(node0f.equiv(node0g));
        EXPECT_FALSE(node0f.equiv(node0h));
        EXPECT_FALSE(node0f.equiv(node0i));
        EXPECT_FALSE(node0f.equiv(node0j));

        EXPECT_FALSE(node0g.equiv(node0a));
        EXPECT_FALSE(node0g.equiv(node0b));
        EXPECT_FALSE(node0g.equiv(*node0a.begin()));
        EXPECT_FALSE(node0g.equiv(*node0b.begin()));
        EXPECT_FALSE(node0g.equiv(node0c));
        EXPECT_FALSE(node0g.equiv(node0d));
        EXPECT_FALSE(node0g.equiv(node0e));
        EXPECT_FALSE(node0g.equiv(node0f));
        EXPECT_TRUE(node0g.equiv(node0g));
        EXPECT_FALSE(node0g.equiv(node0h));
        EXPECT_FALSE(node0g.equiv(node0i));
        EXPECT_FALSE(node0g.equiv(node0j));

        EXPECT_FALSE(node0h.equiv(node0a));
        EXPECT_FALSE(node0h.equiv(node0b));
        EXPECT_FALSE(node0h.equiv(*node0a.begin()));
        EXPECT_FALSE(node0h.equiv(*node0b.begin()));
        EXPECT_FALSE(node0h.equiv(node0c));
        EXPECT_FALSE(node0h.equiv(node0d));
        EXPECT_FALSE(node0h.equiv(node0e));
        EXPECT_FALSE(node0h.equiv(node0f));
        EXPECT_FALSE(node0h.equiv(node0g));
        EXPECT_TRUE(node0h.equiv(node0h));
        EXPECT_FALSE(node0h.equiv(node0i));
        EXPECT_FALSE(node0h.equiv(node0j));

        EXPECT_FALSE(node0i.equiv(node0a));
        EXPECT_FALSE(node0i.equiv(node0b));
        EXPECT_FALSE(node0i.equiv(*node0a.begin()));
        EXPECT_FALSE(node0i.equiv(*node0b.begin()));
        EXPECT_FALSE(node0i.equiv(node0c));
        EXPECT_FALSE(node0i.equiv(node0d));
        EXPECT_FALSE(node0i.equiv(node0e));
        EXPECT_FALSE(node0i.equiv(node0f));
        EXPECT_FALSE(node0i.equiv(node0g));
        EXPECT_FALSE(node0i.equiv(node0h));
        EXPECT_TRUE(node0i.equiv(node0i));
        EXPECT_FALSE(node0i.equiv(node0j));

        EXPECT_FALSE(node0j.equiv(node0a));
        EXPECT_FALSE(node0j.equiv(node0b));
        EXPECT_FALSE(node0j.equiv(*node0a.begin()));
        EXPECT_FALSE(node0j.equiv(*node0b.begin()));
        EXPECT_FALSE(node0j.equiv(node0c));
        EXPECT_FALSE(node0j.equiv(node0d));
        EXPECT_FALSE(node0j.equiv(node0e));
        EXPECT_FALSE(node0j.equiv(node0f));
        EXPECT_FALSE(node0j.equiv(node0g));
        EXPECT_FALSE(node0j.equiv(node0h));
        EXPECT_FALSE(node0j.equiv(node0i));
        EXPECT_TRUE(node0j.equiv(node0j));
    }

    EXPECT_EQ(nc0->count(), 0);
}

TEST_F(NodeTests, Node_Equiv_WithDiffCtx) {

    // w/ me being a tad lazy, these tests only test successful
    // equivalence, and only testing one case, lol

    auto nc0 = std::make_shared<taul::node_ctx>(lgr);
    auto nc1 = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc0);
    ASSERT_TRUE((bool)nc1);

    EXPECT_EQ(nc0->count(), 0);
    EXPECT_EQ(nc1->count(), 0);

    {
        auto node0a = nc0->create(gram.ppr("ppr"), "abc", 10);
        auto node1a = nc0->create(gram.lpr("lpr"), "def", 11);
        auto node2a = nc0->create(taul::token::failure("ghi", 3));
        auto node3a = nc0->create(gram.lpr("lpr"), "ggg", 31);
        auto node4a = nc0->create(gram.ppr("ppr"), "hh", 444);

        node1a.attach(std::move(node2a));
        node1a.attach(std::move(node3a));
        node0a.attach(std::move(node1a));
        node0a.attach(std::move(node4a));

        EXPECT_EQ(nc0->count(), 5);
        EXPECT_EQ(nc1->count(), 0);

        TAUL_LOG(lgr, "node0a:\n{}", node0a.fmt_tree());

        auto node0b = nc1->create(gram.ppr("ppr"), "abc", 10);
        auto node1b = nc1->create(gram.lpr("lpr"), "def", 11);
        auto node2b = nc1->create(taul::token::failure("ghi", 3));
        auto node3b = nc1->create(gram.lpr("lpr"), "ggg", 31);
        auto node4b = nc1->create(gram.ppr("ppr"), "hh", 444);

        node1b.attach(std::move(node2b));
        node1b.attach(std::move(node3b));
        node0b.attach(std::move(node1b));
        node0b.attach(std::move(node4b));

        EXPECT_EQ(nc0->count(), 5);
        EXPECT_EQ(nc1->count(), 5);

        TAUL_LOG(lgr, "node0b:\n{}", node0b.fmt_tree());

        EXPECT_TRUE(node0a.equiv(node0b));
        EXPECT_TRUE(node0b.equiv(node0a));
    }

    EXPECT_EQ(nc0->count(), 0);
    EXPECT_EQ(nc1->count(), 0);
}

TEST_F(NodeTests, Attach) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        taul::node node0 = nc->create(gram.lpr("lpr"), "aa");
        taul::node node1 = nc->create(gram.lpr("lpr"), "bb", 1);
        taul::node node2 = nc->create(gram.lpr("lpr"), "cc", 2);
        taul::node node3 = nc->create(gram.lpr("lpr"), "dd", 3);
        taul::node node4 = nc->create(gram.lpr("lpr"), "ee", 4);

        const auto& cnst0 = (const decltype(node0)&)node0;
        const auto& cnst1 = (const decltype(node1)&)node1;
        const auto& cnst2 = (const decltype(node2)&)node2;
        const auto& cnst3 = (const decltype(node3)&)node3;
        const auto& cnst4 = (const decltype(node4)&)node4;

        EXPECT_EQ(nc->count(), 5);

        taul::node_iter iter1 = node1.iter();
        taul::node_iter iter2 = node2.iter();
        taul::node_iter iter3 = node3.iter();
        taul::node_iter iter4 = node4.iter();

        node0.attach(std::move(node1));
        node0.attach(std::move(node2));
        node0.attach(std::move(node3));
        node0.attach(std::move(node4));

        EXPECT_EQ(nc->count(), 5);

        TAUL_LOG(lgr, "{}", node0.fmt_tree());

        //TAUL_LOG(lgr, "node0.children()=={}", node0.children());
        //TAUL_LOG(lgr, "contents");
        //for (const auto& I : node0) {
        //    TAUL_LOG(lgr, "    I=={}", *(I.tkn()));
        //}
        //TAUL_LOG(lgr, "contents (reversed)");
        //for (auto it = node0.rbegin(); it != node0.rend(); it++) {
        //    TAUL_LOG(lgr, "    I=={}", *(it->tkn()));
        //}

        ASSERT_EQ(std::next(node0.begin(), 0), iter1);
        ASSERT_EQ(std::next(node0.begin(), 1), iter2);
        ASSERT_EQ(std::next(node0.begin(), 2), iter3);
        ASSERT_EQ(std::next(node0.begin(), 3), iter4);
        ASSERT_EQ(std::next(node0.begin(), 4), node0.end());

        ASSERT_EQ(std::next(node0.cbegin(), 0), iter1);
        ASSERT_EQ(std::next(node0.cbegin(), 1), iter2);
        ASSERT_EQ(std::next(node0.cbegin(), 2), iter3);
        ASSERT_EQ(std::next(node0.cbegin(), 3), iter4);
        ASSERT_EQ(std::next(node0.cbegin(), 4), node0.cend());

        ASSERT_EQ(std::next(cnst0.begin(), 0), iter1);
        ASSERT_EQ(std::next(cnst0.begin(), 1), iter2);
        ASSERT_EQ(std::next(cnst0.begin(), 2), iter3);
        ASSERT_EQ(std::next(cnst0.begin(), 3), iter4);
        ASSERT_EQ(std::next(cnst0.begin(), 4), cnst0.end());

        ASSERT_EQ(std::next(node0.rbegin(), 0).base(), std::next(iter4));
        ASSERT_EQ(std::next(node0.rbegin(), 1).base(), std::next(iter3));
        ASSERT_EQ(std::next(node0.rbegin(), 2).base(), std::next(iter2));
        ASSERT_EQ(std::next(node0.rbegin(), 3).base(), std::next(iter1));
        ASSERT_EQ(std::next(node0.rbegin(), 4), node0.rend());

        ASSERT_EQ(std::next(node0.crbegin(), 0).base(), std::next(iter4).to_const());
        ASSERT_EQ(std::next(node0.crbegin(), 1).base(), std::next(iter3).to_const());
        ASSERT_EQ(std::next(node0.crbegin(), 2).base(), std::next(iter2).to_const());
        ASSERT_EQ(std::next(node0.crbegin(), 3).base(), std::next(iter1).to_const());
        ASSERT_EQ(std::next(node0.crbegin(), 4), node0.crend());

        ASSERT_EQ(std::next(cnst0.rbegin(), 0).base(), std::next(iter4).to_const());
        ASSERT_EQ(std::next(cnst0.rbegin(), 1).base(), std::next(iter3).to_const());
        ASSERT_EQ(std::next(cnst0.rbegin(), 2).base(), std::next(iter2).to_const());
        ASSERT_EQ(std::next(cnst0.rbegin(), 3).base(), std::next(iter1).to_const());
        ASSERT_EQ(std::next(cnst0.rbegin(), 4), cnst0.rend());

        EXPECT_EQ(node0.children(), 4);

        EXPECT_EQ(iter1->parent(), node0.iter());
        EXPECT_EQ(iter1->left_child(), node0.end());
        EXPECT_EQ(iter1->right_child(), node0.end());
        EXPECT_EQ(iter1->left_sibling(), node0.end());
        EXPECT_EQ(iter1->right_sibling(), iter2);
        
        EXPECT_EQ(iter2->parent(), node0.iter());
        EXPECT_EQ(iter2->left_child(), node0.end());
        EXPECT_EQ(iter2->right_child(), node0.end());
        EXPECT_EQ(iter2->left_sibling(), iter1);
        EXPECT_EQ(iter2->right_sibling(), iter3);

        EXPECT_EQ(iter3->parent(), node0.iter());
        EXPECT_EQ(iter3->left_child(), node0.end());
        EXPECT_EQ(iter3->right_child(), node0.end());
        EXPECT_EQ(iter3->left_sibling(), iter2);
        EXPECT_EQ(iter3->right_sibling(), iter4);

        EXPECT_EQ(iter4->parent(), node0.iter());
        EXPECT_EQ(iter4->left_child(), node0.end());
        EXPECT_EQ(iter4->right_child(), node0.end());
        EXPECT_EQ(iter4->left_sibling(), iter3);
        EXPECT_EQ(iter4->right_sibling(), node0.end());

        EXPECT_EQ(iter1.to_const()->parent(), cnst0.iter());
        EXPECT_EQ(iter1.to_const()->left_child(), node0.cend());
        EXPECT_EQ(iter1.to_const()->right_child(), node0.cend());
        EXPECT_EQ(iter1.to_const()->left_sibling(), node0.cend());
        EXPECT_EQ(iter1.to_const()->right_sibling(), iter2.to_const());
        
        EXPECT_EQ(iter2.to_const()->parent(), cnst0.iter());
        EXPECT_EQ(iter2.to_const()->left_child(), node0.cend());
        EXPECT_EQ(iter2.to_const()->right_child(), node0.cend());
        EXPECT_EQ(iter2.to_const()->left_sibling(), iter1.to_const());
        EXPECT_EQ(iter2.to_const()->right_sibling(), iter3.to_const());

        EXPECT_EQ(iter3.to_const()->parent(), cnst0.iter());
        EXPECT_EQ(iter3.to_const()->left_child(), node0.cend());
        EXPECT_EQ(iter3.to_const()->right_child(), node0.cend());
        EXPECT_EQ(iter3.to_const()->left_sibling(), iter2.to_const());
        EXPECT_EQ(iter3.to_const()->right_sibling(), iter4.to_const());

        EXPECT_EQ(iter4.to_const()->parent(), cnst0.iter());
        EXPECT_EQ(iter4.to_const()->left_child(), node0.cend());
        EXPECT_EQ(iter4.to_const()->right_child(), node0.cend());
        EXPECT_EQ(iter4.to_const()->left_sibling(), iter3.to_const());
        EXPECT_EQ(iter4.to_const()->right_sibling(), node0.cend());
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, Attach_PlaceBefore) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        taul::node node0 = nc->create(gram.lpr("lpr"), "aa");
        taul::node node1 = nc->create(gram.lpr("lpr"), "bb", 1);
        taul::node node2 = nc->create(gram.lpr("lpr"), "cc", 2);
        taul::node node3 = nc->create(gram.lpr("lpr"), "dd", 3);
        taul::node node4 = nc->create(gram.lpr("lpr"), "ee", 4);

        const auto& cnst0 = (const decltype(node0)&)node0;
        const auto& cnst1 = (const decltype(node1)&)node1;
        const auto& cnst2 = (const decltype(node2)&)node2;
        const auto& cnst3 = (const decltype(node3)&)node3;
        const auto& cnst4 = (const decltype(node4)&)node4;

        EXPECT_EQ(nc->count(), 5);

        taul::node_iter iter1 = node1.iter();
        taul::node_iter iter2 = node2.iter();
        taul::node_iter iter3 = node3.iter();
        taul::node_iter iter4 = node4.iter();

        node0.attach(std::move(node2));
        node0.attach(std::move(node4));

        node0.attach(std::move(node1), iter2);
        node0.attach(std::move(node3), iter4);

        EXPECT_EQ(nc->count(), 5);

        TAUL_LOG(lgr, "{}", node0.fmt_tree());

        ASSERT_EQ(std::next(node0.begin(), 0), iter1);
        ASSERT_EQ(std::next(node0.begin(), 1), iter2);
        ASSERT_EQ(std::next(node0.begin(), 2), iter3);
        ASSERT_EQ(std::next(node0.begin(), 3), iter4);
        ASSERT_EQ(std::next(node0.begin(), 4), node0.end());

        ASSERT_EQ(std::next(node0.cbegin(), 0), iter1);
        ASSERT_EQ(std::next(node0.cbegin(), 1), iter2);
        ASSERT_EQ(std::next(node0.cbegin(), 2), iter3);
        ASSERT_EQ(std::next(node0.cbegin(), 3), iter4);
        ASSERT_EQ(std::next(node0.cbegin(), 4), node0.cend());

        ASSERT_EQ(std::next(cnst0.begin(), 0), iter1);
        ASSERT_EQ(std::next(cnst0.begin(), 1), iter2);
        ASSERT_EQ(std::next(cnst0.begin(), 2), iter3);
        ASSERT_EQ(std::next(cnst0.begin(), 3), iter4);
        ASSERT_EQ(std::next(cnst0.begin(), 4), cnst0.end());

        ASSERT_EQ(std::next(node0.rbegin(), 0).base(), std::next(iter4));
        ASSERT_EQ(std::next(node0.rbegin(), 1).base(), std::next(iter3));
        ASSERT_EQ(std::next(node0.rbegin(), 2).base(), std::next(iter2));
        ASSERT_EQ(std::next(node0.rbegin(), 3).base(), std::next(iter1));
        ASSERT_EQ(std::next(node0.rbegin(), 4), node0.rend());

        ASSERT_EQ(std::next(node0.crbegin(), 0).base(), std::next(iter4).to_const());
        ASSERT_EQ(std::next(node0.crbegin(), 1).base(), std::next(iter3).to_const());
        ASSERT_EQ(std::next(node0.crbegin(), 2).base(), std::next(iter2).to_const());
        ASSERT_EQ(std::next(node0.crbegin(), 3).base(), std::next(iter1).to_const());
        ASSERT_EQ(std::next(node0.crbegin(), 4), node0.crend());

        ASSERT_EQ(std::next(cnst0.rbegin(), 0).base(), std::next(iter4).to_const());
        ASSERT_EQ(std::next(cnst0.rbegin(), 1).base(), std::next(iter3).to_const());
        ASSERT_EQ(std::next(cnst0.rbegin(), 2).base(), std::next(iter2).to_const());
        ASSERT_EQ(std::next(cnst0.rbegin(), 3).base(), std::next(iter1).to_const());
        ASSERT_EQ(std::next(cnst0.rbegin(), 4), cnst0.rend());

        EXPECT_EQ(node0.children(), 4);
        EXPECT_EQ(iter1->parent(), node0.iter());
        EXPECT_EQ(iter1->left_child(), node0.end());
        EXPECT_EQ(iter1->right_child(), node0.end());
        EXPECT_EQ(iter1->left_sibling(), node0.end());
        EXPECT_EQ(iter1->right_sibling(), iter2);

        EXPECT_EQ(iter2->parent(), node0.iter());
        EXPECT_EQ(iter2->left_child(), node0.end());
        EXPECT_EQ(iter2->right_child(), node0.end());
        EXPECT_EQ(iter2->left_sibling(), iter1);
        EXPECT_EQ(iter2->right_sibling(), iter3);

        EXPECT_EQ(iter3->parent(), node0.iter());
        EXPECT_EQ(iter3->left_child(), node0.end());
        EXPECT_EQ(iter3->right_child(), node0.end());
        EXPECT_EQ(iter3->left_sibling(), iter2);
        EXPECT_EQ(iter3->right_sibling(), iter4);

        EXPECT_EQ(iter4->parent(), node0.iter());
        EXPECT_EQ(iter4->left_child(), node0.end());
        EXPECT_EQ(iter4->right_child(), node0.end());
        EXPECT_EQ(iter4->left_sibling(), iter3);
        EXPECT_EQ(iter4->right_sibling(), node0.end());

        EXPECT_EQ(iter1.to_const()->parent(), cnst0.iter());
        EXPECT_EQ(iter1.to_const()->left_child(), node0.cend());
        EXPECT_EQ(iter1.to_const()->right_child(), node0.cend());
        EXPECT_EQ(iter1.to_const()->left_sibling(), node0.cend());
        EXPECT_EQ(iter1.to_const()->right_sibling(), iter2.to_const());

        EXPECT_EQ(iter2.to_const()->parent(), cnst0.iter());
        EXPECT_EQ(iter2.to_const()->left_child(), node0.cend());
        EXPECT_EQ(iter2.to_const()->right_child(), node0.cend());
        EXPECT_EQ(iter2.to_const()->left_sibling(), iter1.to_const());
        EXPECT_EQ(iter2.to_const()->right_sibling(), iter3.to_const());

        EXPECT_EQ(iter3.to_const()->parent(), cnst0.iter());
        EXPECT_EQ(iter3.to_const()->left_child(), node0.cend());
        EXPECT_EQ(iter3.to_const()->right_child(), node0.cend());
        EXPECT_EQ(iter3.to_const()->left_sibling(), iter2.to_const());
        EXPECT_EQ(iter3.to_const()->right_sibling(), iter4.to_const());

        EXPECT_EQ(iter4.to_const()->parent(), cnst0.iter());
        EXPECT_EQ(iter4.to_const()->left_child(), node0.cend());
        EXPECT_EQ(iter4.to_const()->right_child(), node0.cend());
        EXPECT_EQ(iter4.to_const()->left_sibling(), iter3.to_const());
        EXPECT_EQ(iter4.to_const()->right_sibling(), node0.cend());
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, Detach) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        taul::node node0 = nc->create(gram.lpr("lpr"), "aa");
        taul::node node1 = nc->create(gram.lpr("lpr"), "bb", 1);
        taul::node node2 = nc->create(gram.lpr("lpr"), "cc", 2);
        taul::node node3 = nc->create(gram.lpr("lpr"), "dd", 3);
        taul::node node4 = nc->create(gram.lpr("lpr"), "ee", 4);

        const auto& cnst0 = (const decltype(node0)&)node0;
        const auto& cnst1 = (const decltype(node1)&)node1;
        const auto& cnst2 = (const decltype(node2)&)node2;
        const auto& cnst3 = (const decltype(node3)&)node3;
        const auto& cnst4 = (const decltype(node4)&)node4;

        EXPECT_EQ(nc->count(), 5);

        taul::node_iter iter1 = node1.iter();
        taul::node_iter iter2 = node2.iter();
        taul::node_iter iter3 = node3.iter();
        taul::node_iter iter4 = node4.iter();

        node0.attach(std::move(node1));
        node0.attach(std::move(node2));
        node0.attach(std::move(node3));
        node0.attach(std::move(node4));

        TAUL_LOG(lgr, "before:\n{}", node0.fmt_tree());

        taul::node node2a = node0.detach(iter2);
        taul::node node4a = node0.detach(iter4);

        EXPECT_EQ(nc->count(), 5);

        TAUL_LOG(lgr, "after:\n{}", node0.fmt_tree());
        TAUL_LOG(lgr, "{}", node2a.fmt_tree());
        TAUL_LOG(lgr, "{}", node4a.fmt_tree());

        ASSERT_EQ(std::next(node0.begin(), 0), iter1);
        ASSERT_EQ(std::next(node0.begin(), 1), iter3);
        ASSERT_EQ(std::next(node0.begin(), 2), node0.end());

        ASSERT_EQ(std::next(node0.cbegin(), 0), iter1);
        ASSERT_EQ(std::next(node0.cbegin(), 1), iter3);
        ASSERT_EQ(std::next(node0.cbegin(), 2), node0.cend());

        ASSERT_EQ(std::next(cnst0.begin(), 0), iter1);
        ASSERT_EQ(std::next(cnst0.begin(), 1), iter3);
        ASSERT_EQ(std::next(cnst0.begin(), 2), cnst0.end());

        ASSERT_EQ(std::next(node0.rbegin(), 0).base(), std::next(iter3));
        ASSERT_EQ(std::next(node0.rbegin(), 1).base(), std::next(iter1));
        ASSERT_EQ(std::next(node0.rbegin(), 2), node0.rend());

        ASSERT_EQ(std::next(node0.crbegin(), 0).base(), std::next(iter3).to_const());
        ASSERT_EQ(std::next(node0.crbegin(), 1).base(), std::next(iter1).to_const());
        ASSERT_EQ(std::next(node0.crbegin(), 2), node0.crend());

        ASSERT_EQ(std::next(cnst0.rbegin(), 0).base(), std::next(iter3).to_const());
        ASSERT_EQ(std::next(cnst0.rbegin(), 1).base(), std::next(iter1).to_const());
        ASSERT_EQ(std::next(cnst0.rbegin(), 2), cnst0.rend());

        EXPECT_EQ(node0.children(), 2);

        EXPECT_EQ(iter1->parent(), node0.iter());
        EXPECT_EQ(iter1->left_child(), node0.end());
        EXPECT_EQ(iter1->right_child(), node0.end());
        EXPECT_EQ(iter1->left_sibling(), node0.end());
        EXPECT_EQ(iter1->right_sibling(), iter3);

        EXPECT_EQ(iter3->parent(), node0.iter());
        EXPECT_EQ(iter3->left_child(), node0.end());
        EXPECT_EQ(iter3->right_child(), node0.end());
        EXPECT_EQ(iter3->left_sibling(), iter1);
        EXPECT_EQ(iter3->right_sibling(), node0.end());

        EXPECT_EQ(iter1.to_const()->parent(), cnst0.iter());
        EXPECT_EQ(iter1.to_const()->left_child(), node0.cend());
        EXPECT_EQ(iter1.to_const()->right_child(), node0.cend());
        EXPECT_EQ(iter1.to_const()->left_sibling(), node0.cend());
        EXPECT_EQ(iter1.to_const()->right_sibling(), iter3.to_const());

        EXPECT_EQ(iter3.to_const()->parent(), cnst0.iter());
        EXPECT_EQ(iter3.to_const()->left_child(), node0.cend());
        EXPECT_EQ(iter3.to_const()->right_child(), node0.cend());
        EXPECT_EQ(iter3.to_const()->left_sibling(), iter1.to_const());
        EXPECT_EQ(iter3.to_const()->right_sibling(), node0.cend());

        EXPECT_EQ(node2a.iter(), iter2);
        EXPECT_EQ(node2a.iter().to_const(), iter2.to_const());

        EXPECT_EQ(node2a.parent(), node0.end());
        EXPECT_EQ(node2a.left_child(), node0.end());
        EXPECT_EQ(node2a.right_child(), node0.end());
        EXPECT_EQ(node2a.left_sibling(), node0.end());
        EXPECT_EQ(node2a.right_sibling(), node0.end());

        EXPECT_EQ(node4a.iter(), iter4);
        EXPECT_EQ(node4a.iter().to_const(), iter4.to_const());

        EXPECT_EQ(node4a.parent(), node0.end());
        EXPECT_EQ(node4a.left_child(), node0.end());
        EXPECT_EQ(node4a.right_child(), node0.end());
        EXPECT_EQ(node4a.left_sibling(), node0.end());
        EXPECT_EQ(node4a.right_sibling(), node0.end());
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, Detach_Range) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        taul::node node0 = nc->create(gram.lpr("lpr"), "aa");
        taul::node node1 = nc->create(gram.lpr("lpr"), "bb", 1);
        taul::node node2 = nc->create(gram.lpr("lpr"), "cc", 2);
        taul::node node3 = nc->create(gram.lpr("lpr"), "dd", 3);
        taul::node node4 = nc->create(gram.lpr("lpr"), "ee", 4);
        taul::node node5 = nc->create(gram.lpr("lpr"), "ff", 5);

        const auto& cnst0 = (const decltype(node0)&)node0;
        const auto& cnst1 = (const decltype(node1)&)node1;
        const auto& cnst2 = (const decltype(node2)&)node2;
        const auto& cnst3 = (const decltype(node3)&)node3;
        const auto& cnst4 = (const decltype(node4)&)node4;
        const auto& cnst5 = (const decltype(node5)&)node5;

        EXPECT_EQ(nc->count(), 6);

        taul::node_iter iter1 = node1.iter();
        taul::node_iter iter2 = node2.iter();
        taul::node_iter iter3 = node3.iter();
        taul::node_iter iter4 = node4.iter();
        taul::node_iter iter5 = node5.iter();

        node0.attach(std::move(node1));
        node0.attach(std::move(node2));
        node0.attach(std::move(node3));
        node0.attach(std::move(node4));
        node0.attach(std::move(node5));

        TAUL_LOG(lgr, "before:\n{}", node0.fmt_tree());

        node0.detach_range(iter2, std::next(iter4));

        EXPECT_EQ(nc->count(), 3);

        TAUL_LOG(lgr, "after:\n{}", node0.fmt_tree());

        ASSERT_EQ(std::next(node0.begin(), 0), iter1);
        ASSERT_EQ(std::next(node0.begin(), 1), iter5);
        ASSERT_EQ(std::next(node0.begin(), 2), node0.end());

        ASSERT_EQ(std::next(node0.cbegin(), 0), iter1);
        ASSERT_EQ(std::next(node0.cbegin(), 1), iter5);
        ASSERT_EQ(std::next(node0.cbegin(), 2), node0.cend());

        ASSERT_EQ(std::next(cnst0.begin(), 0), iter1);
        ASSERT_EQ(std::next(cnst0.begin(), 1), iter5);
        ASSERT_EQ(std::next(cnst0.begin(), 2), cnst0.end());

        ASSERT_EQ(std::next(node0.rbegin(), 0).base(), std::next(iter5));
        ASSERT_EQ(std::next(node0.rbegin(), 1).base(), std::next(iter1));
        ASSERT_EQ(std::next(node0.rbegin(), 2), node0.rend());

        ASSERT_EQ(std::next(node0.crbegin(), 0).base(), std::next(iter5).to_const());
        ASSERT_EQ(std::next(node0.crbegin(), 1).base(), std::next(iter1).to_const());
        ASSERT_EQ(std::next(node0.crbegin(), 2), node0.crend());

        ASSERT_EQ(std::next(cnst0.rbegin(), 0).base(), std::next(iter5).to_const());
        ASSERT_EQ(std::next(cnst0.rbegin(), 1).base(), std::next(iter1).to_const());
        ASSERT_EQ(std::next(cnst0.rbegin(), 2), cnst0.rend());

        EXPECT_EQ(node0.children(), 2);

        EXPECT_EQ(iter1->parent(), node0.iter());
        EXPECT_EQ(iter1->left_child(), node0.end());
        EXPECT_EQ(iter1->right_child(), node0.end());
        EXPECT_EQ(iter1->left_sibling(), node0.end());
        EXPECT_EQ(iter1->right_sibling(), iter5);

        EXPECT_EQ(iter5->parent(), node0.iter());
        EXPECT_EQ(iter5->left_child(), node0.end());
        EXPECT_EQ(iter5->right_child(), node0.end());
        EXPECT_EQ(iter5->left_sibling(), iter1);
        EXPECT_EQ(iter5->right_sibling(), node0.end());

        EXPECT_EQ(iter1.to_const()->parent(), cnst0.iter());
        EXPECT_EQ(iter1.to_const()->left_child(), node0.cend());
        EXPECT_EQ(iter1.to_const()->right_child(), node0.cend());
        EXPECT_EQ(iter1.to_const()->left_sibling(), node0.cend());
        EXPECT_EQ(iter1.to_const()->right_sibling(), iter5.to_const());

        EXPECT_EQ(iter5.to_const()->parent(), cnst0.iter());
        EXPECT_EQ(iter5.to_const()->left_child(), node0.cend());
        EXPECT_EQ(iter5.to_const()->right_child(), node0.cend());
        EXPECT_EQ(iter5.to_const()->left_sibling(), iter1.to_const());
        EXPECT_EQ(iter5.to_const()->right_sibling(), node0.cend());
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, Detach_All) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        taul::node node0 = nc->create(gram.lpr("lpr"), "aa");
        taul::node node1 = nc->create(gram.lpr("lpr"), "bb", 1);
        taul::node node2 = nc->create(gram.lpr("lpr"), "cc", 2);
        taul::node node3 = nc->create(gram.lpr("lpr"), "dd", 3);
        taul::node node4 = nc->create(gram.lpr("lpr"), "ee", 4);

        const auto& cnst0 = (const decltype(node0)&)node0;

        EXPECT_EQ(nc->count(), 5);

        node0.attach(std::move(node1));
        node0.attach(std::move(node2));
        node0.attach(std::move(node3));
        node0.attach(std::move(node4));

        TAUL_LOG(lgr, "before:\n{}", node0.fmt_tree());

        node0.detach_all();

        TAUL_LOG(lgr, "after:\n{}", node0.fmt_tree());

        EXPECT_EQ(nc->count(), 1);

        ASSERT_EQ(node0.begin(), node0.end());
        ASSERT_EQ(node0.cbegin(), node0.cend());
        ASSERT_EQ(cnst0.begin(), cnst0.end());

        ASSERT_EQ(node0.rbegin(), node0.rend());
        ASSERT_EQ(node0.crbegin(), node0.crend());
        ASSERT_EQ(cnst0.rbegin(), cnst0.rend());

        EXPECT_EQ(node0.children(), 0);
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, NodeIter_DefaultCtor) {

    auto iter = taul::node_iter<false>();
    auto citer0 = iter.to_const();
    auto citer1 = (taul::node_iter<true>)iter;

    
    EXPECT_FALSE(iter.is_const());
    EXPECT_TRUE(iter.is_past_the_end());
    EXPECT_FALSE(iter.is_decrementable_pte());

    EXPECT_TRUE(citer0.is_const());
    EXPECT_TRUE(citer0.is_past_the_end());
    EXPECT_FALSE(citer0.is_decrementable_pte());

    EXPECT_TRUE(citer1.is_const());
    EXPECT_TRUE(citer1.is_past_the_end());
    EXPECT_FALSE(citer1.is_decrementable_pte());
}

TEST_F(NodeTests, NodeIter_CopyCtor) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        taul::node node0 = nc->create(gram.lpr("lpr"), "a");
        taul::node node1 = nc->create(gram.lpr("lpr"), "b", 1);
        taul::node node2 = nc->create(gram.lpr("lpr"), "c", 2);

        auto node1_iter = node1.iter();
        auto node2_iter = node2.iter();

        node0.attach(std::move(node1));
        node0.attach(std::move(node2));

        EXPECT_EQ(nc->count(), 3);

        taul::node_iter<false> iter0_old = taul::node_iter<false>();
        taul::node_iter<false> iter1_old = node1_iter;
        taul::node_iter<false> iter2_old = node2_iter;
        taul::node_iter<false> iter3_old = std::next(iter2_old);

        taul::node_iter<true> iter4_old = taul::node_iter<true>();
        taul::node_iter<true> iter5_old = node1_iter.to_const();
        taul::node_iter<true> iter6_old = node2_iter.to_const();
        taul::node_iter<true> iter7_old = std::next(iter2_old).to_const();

        taul::node_iter<false> iter0 = taul::node_iter<false>(iter0_old);
        taul::node_iter<false> iter1 = taul::node_iter<false>(iter1_old);
        taul::node_iter<false> iter2 = taul::node_iter<false>(iter2_old);
        taul::node_iter<false> iter3 = taul::node_iter<false>(iter3_old);

        taul::node_iter<true> iter4 = taul::node_iter<true>(iter4_old);
        taul::node_iter<true> iter5 = taul::node_iter<true>(iter5_old);
        taul::node_iter<true> iter6 = taul::node_iter<true>(iter6_old);
        taul::node_iter<true> iter7 = taul::node_iter<true>(iter7_old);

        ASSERT_FALSE(iter0.is_const());
        ASSERT_TRUE(iter0.is_past_the_end());
        ASSERT_FALSE(iter0.is_decrementable_pte());

        ASSERT_FALSE(iter1.is_const());
        ASSERT_FALSE(iter1.is_past_the_end());
        ASSERT_FALSE(iter1.is_decrementable_pte());

        if (!iter1.is_past_the_end()) {
            ASSERT_EQ(iter1->str(), "b");
            ASSERT_EQ((*iter1).str(), "b");
        }

        ASSERT_EQ(iter1, node1_iter);

        ASSERT_FALSE(iter2.is_const());
        ASSERT_FALSE(iter2.is_past_the_end());
        ASSERT_FALSE(iter2.is_decrementable_pte());

        if (!iter2.is_past_the_end()) {
            ASSERT_EQ(iter2->str(), "c");
            ASSERT_EQ((*iter2).str(), "c");
        }

        ASSERT_EQ(iter2, node2_iter);

        ASSERT_FALSE(iter3.is_const());
        ASSERT_TRUE(iter3.is_past_the_end());
        ASSERT_TRUE(iter3.is_decrementable_pte());

        ASSERT_TRUE(iter4.is_const());
        ASSERT_TRUE(iter4.is_past_the_end());
        ASSERT_FALSE(iter4.is_decrementable_pte());

        ASSERT_TRUE(iter5.is_const());
        ASSERT_FALSE(iter5.is_past_the_end());
        ASSERT_FALSE(iter5.is_decrementable_pte());

        if (!iter5.is_past_the_end()) {
            ASSERT_EQ(iter5->str(), "b");
            ASSERT_EQ((*iter5).str(), "b");
        }

        ASSERT_EQ(iter5, node1_iter);

        ASSERT_TRUE(iter6.is_const());
        ASSERT_FALSE(iter6.is_past_the_end());
        ASSERT_FALSE(iter6.is_decrementable_pte());

        if (!iter6.is_past_the_end()) {
            ASSERT_EQ(iter6->str(), "c");
            ASSERT_EQ((*iter6).str(), "c");
        }

        ASSERT_EQ(iter6, node2_iter);

        ASSERT_TRUE(iter7.is_const());
        ASSERT_TRUE(iter7.is_past_the_end());
        ASSERT_TRUE(iter7.is_decrementable_pte());
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, NodeIter_MoveCtor) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        taul::node node0 = nc->create(gram.lpr("lpr"), "a");
        taul::node node1 = nc->create(gram.lpr("lpr"), "b", 1);
        taul::node node2 = nc->create(gram.lpr("lpr"), "c", 2);

        auto node1_iter = node1.iter();
        auto node2_iter = node2.iter();

        node0.attach(std::move(node1));
        node0.attach(std::move(node2));

        EXPECT_EQ(nc->count(), 3);

        taul::node_iter<false> iter0_old = taul::node_iter<false>();
        taul::node_iter<false> iter1_old = node1_iter;
        taul::node_iter<false> iter2_old = node2_iter;
        taul::node_iter<false> iter3_old = std::next(iter2_old);

        taul::node_iter<true> iter4_old = taul::node_iter<true>();
        taul::node_iter<true> iter5_old = node1_iter.to_const();
        taul::node_iter<true> iter6_old = node2_iter.to_const();
        taul::node_iter<true> iter7_old = std::next(iter2_old).to_const();

        taul::node_iter<false> iter0 = taul::node_iter<false>(std::move(iter0_old));
        taul::node_iter<false> iter1 = taul::node_iter<false>(std::move(iter1_old));
        taul::node_iter<false> iter2 = taul::node_iter<false>(std::move(iter2_old));
        taul::node_iter<false> iter3 = taul::node_iter<false>(std::move(iter3_old));

        taul::node_iter<true> iter4 = taul::node_iter<true>(std::move(iter4_old));
        taul::node_iter<true> iter5 = taul::node_iter<true>(std::move(iter5_old));
        taul::node_iter<true> iter6 = taul::node_iter<true>(std::move(iter6_old));
        taul::node_iter<true> iter7 = taul::node_iter<true>(std::move(iter7_old));

        ASSERT_FALSE(iter0.is_const());
        ASSERT_TRUE(iter0.is_past_the_end());
        ASSERT_FALSE(iter0.is_decrementable_pte());

        ASSERT_FALSE(iter1.is_const());
        ASSERT_FALSE(iter1.is_past_the_end());
        ASSERT_FALSE(iter1.is_decrementable_pte());

        if (!iter1.is_past_the_end()) {
            ASSERT_EQ(iter1->str(), "b");
            ASSERT_EQ((*iter1).str(), "b");
        }

        ASSERT_EQ(iter1, node1_iter);

        ASSERT_FALSE(iter2.is_const());
        ASSERT_FALSE(iter2.is_past_the_end());
        ASSERT_FALSE(iter2.is_decrementable_pte());

        if (!iter2.is_past_the_end()) {
            ASSERT_EQ(iter2->str(), "c");
            ASSERT_EQ((*iter2).str(), "c");
        }

        ASSERT_EQ(iter2, node2_iter);

        ASSERT_FALSE(iter3.is_const());
        ASSERT_TRUE(iter3.is_past_the_end());
        ASSERT_TRUE(iter3.is_decrementable_pte());

        ASSERT_TRUE(iter4.is_const());
        ASSERT_TRUE(iter4.is_past_the_end());
        ASSERT_FALSE(iter4.is_decrementable_pte());

        ASSERT_TRUE(iter5.is_const());
        ASSERT_FALSE(iter5.is_past_the_end());
        ASSERT_FALSE(iter5.is_decrementable_pte());

        if (!iter5.is_past_the_end()) {
            ASSERT_EQ(iter5->str(), "b");
            ASSERT_EQ((*iter5).str(), "b");
        }

        ASSERT_EQ(iter5, node1_iter);

        ASSERT_TRUE(iter6.is_const());
        ASSERT_FALSE(iter6.is_past_the_end());
        ASSERT_FALSE(iter6.is_decrementable_pte());

        if (!iter6.is_past_the_end()) {
            ASSERT_EQ(iter6->str(), "c");
            ASSERT_EQ((*iter6).str(), "c");
        }

        ASSERT_EQ(iter6, node2_iter);

        ASSERT_TRUE(iter7.is_const());
        ASSERT_TRUE(iter7.is_past_the_end());
        ASSERT_TRUE(iter7.is_decrementable_pte());
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, NodeIter_CopyAssign) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        taul::node node0 = nc->create(gram.lpr("lpr"), "a");
        taul::node node1 = nc->create(gram.lpr("lpr"), "b", 1);
        taul::node node2 = nc->create(gram.lpr("lpr"), "c", 2);

        auto node1_iter = node1.iter();
        auto node2_iter = node2.iter();

        node0.attach(std::move(node1));
        node0.attach(std::move(node2));

        EXPECT_EQ(nc->count(), 3);

        taul::node_iter<false> iter0_old = taul::node_iter<false>();
        taul::node_iter<false> iter1_old = node1_iter;
        taul::node_iter<false> iter2_old = node2_iter;
        taul::node_iter<false> iter3_old = std::next(iter2_old);

        taul::node_iter<true> iter4_old = taul::node_iter<true>();
        taul::node_iter<true> iter5_old = node1_iter.to_const();
        taul::node_iter<true> iter6_old = node2_iter.to_const();
        taul::node_iter<true> iter7_old = std::next(iter2_old).to_const();

        taul::node_iter<false> iter0{}, iter1{}, iter2{}, iter3{};
        taul::node_iter<true> iter4{}, iter5{}, iter6{}, iter7{};

        iter0 = iter0_old;
        iter1 = iter1_old;
        iter2 = iter2_old;
        iter3 = iter3_old;

        iter4 = iter4_old;
        iter5 = iter5_old;
        iter6 = iter6_old;
        iter7 = iter7_old;

        ASSERT_FALSE(iter0.is_const());
        ASSERT_TRUE(iter0.is_past_the_end());
        ASSERT_FALSE(iter0.is_decrementable_pte());

        ASSERT_FALSE(iter1.is_const());
        ASSERT_FALSE(iter1.is_past_the_end());
        ASSERT_FALSE(iter1.is_decrementable_pte());

        if (!iter1.is_past_the_end()) {
            ASSERT_EQ(iter1->str(), "b");
            ASSERT_EQ((*iter1).str(), "b");
        }

        ASSERT_EQ(iter1, node1_iter);

        ASSERT_FALSE(iter2.is_const());
        ASSERT_FALSE(iter2.is_past_the_end());
        ASSERT_FALSE(iter2.is_decrementable_pte());

        if (!iter2.is_past_the_end()) {
            ASSERT_EQ(iter2->str(), "c");
            ASSERT_EQ((*iter2).str(), "c");
        }

        ASSERT_EQ(iter2, node2_iter);

        ASSERT_FALSE(iter3.is_const());
        ASSERT_TRUE(iter3.is_past_the_end());
        ASSERT_TRUE(iter3.is_decrementable_pte());

        ASSERT_TRUE(iter4.is_const());
        ASSERT_TRUE(iter4.is_past_the_end());
        ASSERT_FALSE(iter4.is_decrementable_pte());

        ASSERT_TRUE(iter5.is_const());
        ASSERT_FALSE(iter5.is_past_the_end());
        ASSERT_FALSE(iter5.is_decrementable_pte());

        if (!iter5.is_past_the_end()) {
            ASSERT_EQ(iter5->str(), "b");
            ASSERT_EQ((*iter5).str(), "b");
        }

        ASSERT_EQ(iter5, node1_iter);

        ASSERT_TRUE(iter6.is_const());
        ASSERT_FALSE(iter6.is_past_the_end());
        ASSERT_FALSE(iter6.is_decrementable_pte());

        if (!iter6.is_past_the_end()) {
            ASSERT_EQ(iter6->str(), "c");
            ASSERT_EQ((*iter6).str(), "c");
        }

        ASSERT_EQ(iter6, node2_iter);

        ASSERT_TRUE(iter7.is_const());
        ASSERT_TRUE(iter7.is_past_the_end());
        ASSERT_TRUE(iter7.is_decrementable_pte());
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, NodeIter_MoveAssign) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        taul::node node0 = nc->create(gram.lpr("lpr"), "a");
        taul::node node1 = nc->create(gram.lpr("lpr"), "b", 1);
        taul::node node2 = nc->create(gram.lpr("lpr"), "c", 2);

        auto node1_iter = node1.iter();
        auto node2_iter = node2.iter();

        node0.attach(std::move(node1));
        node0.attach(std::move(node2));

        EXPECT_EQ(nc->count(), 3);

        taul::node_iter<false> iter0_old = taul::node_iter<false>();
        taul::node_iter<false> iter1_old = node1_iter;
        taul::node_iter<false> iter2_old = node2_iter;
        taul::node_iter<false> iter3_old = std::next(iter2_old);

        taul::node_iter<true> iter4_old = taul::node_iter<true>();
        taul::node_iter<true> iter5_old = node1_iter.to_const();
        taul::node_iter<true> iter6_old = node2_iter.to_const();
        taul::node_iter<true> iter7_old = std::next(iter2_old).to_const();

        taul::node_iter<false> iter0{}, iter1{}, iter2{}, iter3{};
        taul::node_iter<true> iter4{}, iter5{}, iter6{}, iter7{};

        iter0 = std::move(iter0_old);
        iter1 = std::move(iter1_old);
        iter2 = std::move(iter2_old);
        iter3 = std::move(iter3_old);

        iter4 = std::move(iter4_old);
        iter5 = std::move(iter5_old);
        iter6 = std::move(iter6_old);
        iter7 = std::move(iter7_old);

        ASSERT_FALSE(iter0.is_const());
        ASSERT_TRUE(iter0.is_past_the_end());
        ASSERT_FALSE(iter0.is_decrementable_pte());

        ASSERT_FALSE(iter1.is_const());
        ASSERT_FALSE(iter1.is_past_the_end());
        ASSERT_FALSE(iter1.is_decrementable_pte());

        if (!iter1.is_past_the_end()) {
            ASSERT_EQ(iter1->str(), "b");
            ASSERT_EQ((*iter1).str(), "b");
        }

        ASSERT_EQ(iter1, node1_iter);

        ASSERT_FALSE(iter2.is_const());
        ASSERT_FALSE(iter2.is_past_the_end());
        ASSERT_FALSE(iter2.is_decrementable_pte());

        if (!iter2.is_past_the_end()) {
            ASSERT_EQ(iter2->str(), "c");
            ASSERT_EQ((*iter2).str(), "c");
        }

        ASSERT_EQ(iter2, node2_iter);

        ASSERT_FALSE(iter3.is_const());
        ASSERT_TRUE(iter3.is_past_the_end());
        ASSERT_TRUE(iter3.is_decrementable_pte());

        ASSERT_TRUE(iter4.is_const());
        ASSERT_TRUE(iter4.is_past_the_end());
        ASSERT_FALSE(iter4.is_decrementable_pte());

        ASSERT_TRUE(iter5.is_const());
        ASSERT_FALSE(iter5.is_past_the_end());
        ASSERT_FALSE(iter5.is_decrementable_pte());

        if (!iter5.is_past_the_end()) {
            ASSERT_EQ(iter5->str(), "b");
            ASSERT_EQ((*iter5).str(), "b");
        }

        ASSERT_EQ(iter5, node1_iter);

        ASSERT_TRUE(iter6.is_const());
        ASSERT_FALSE(iter6.is_past_the_end());
        ASSERT_FALSE(iter6.is_decrementable_pte());

        if (!iter6.is_past_the_end()) {
            ASSERT_EQ(iter6->str(), "c");
            ASSERT_EQ((*iter6).str(), "c");
        }

        ASSERT_EQ(iter6, node2_iter);

        ASSERT_TRUE(iter7.is_const());
        ASSERT_TRUE(iter7.is_past_the_end());
        ASSERT_TRUE(iter7.is_decrementable_pte());
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, NodeIter_PreIncrAndDecr) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        taul::node node0 = nc->create(gram.lpr("lpr"), "a");
        taul::node node1 = nc->create(gram.lpr("lpr"), "b", 1);
        taul::node node2 = nc->create(gram.lpr("lpr"), "c", 2);

        auto node1_iter = node1.iter();
        auto node2_iter = node2.iter();

        node0.attach(std::move(node1));
        node0.attach(std::move(node2));

        EXPECT_EQ(nc->count(), 3);

        taul::node::iterator iter = node1_iter;

        ASSERT_FALSE(iter.is_const());
        ASSERT_FALSE(iter.is_past_the_end());
        ASSERT_FALSE(iter.is_decrementable_pte());

        if (!iter.is_past_the_end()) {
            ASSERT_EQ(iter->str(), "b");
            ASSERT_EQ((*iter).str(), "b");
        }

        ASSERT_EQ(iter, node1_iter);

        taul::node::iterator iter0 = ++iter;

        ASSERT_FALSE(iter.is_const());
        ASSERT_FALSE(iter.is_past_the_end());
        ASSERT_FALSE(iter.is_decrementable_pte());

        if (!iter.is_past_the_end()) {
            ASSERT_EQ(iter->str(), "c");
            ASSERT_EQ((*iter).str(), "c");
        }

        ASSERT_EQ(iter, node2_iter);

        ASSERT_FALSE(iter0.is_const());
        ASSERT_FALSE(iter0.is_past_the_end());
        ASSERT_FALSE(iter0.is_decrementable_pte());

        if (!iter.is_past_the_end()) {
            ASSERT_EQ(iter0->str(), "c");
            ASSERT_EQ((*iter0).str(), "c");
        }

        ASSERT_EQ(iter0, node2_iter);

        ASSERT_EQ(iter, iter0);

        taul::node::iterator iter1 = ++iter;

        ASSERT_FALSE(iter.is_const());
        ASSERT_TRUE(iter.is_past_the_end());
        ASSERT_TRUE(iter.is_decrementable_pte());

        ASSERT_FALSE(iter1.is_const());
        ASSERT_TRUE(iter1.is_past_the_end());
        ASSERT_TRUE(iter1.is_decrementable_pte());

        ASSERT_EQ(iter, iter1);

        taul::node::iterator iter2 = --iter;

        ASSERT_FALSE(iter.is_const());
        ASSERT_FALSE(iter.is_past_the_end());
        ASSERT_FALSE(iter.is_decrementable_pte());

        if (!iter.is_past_the_end()) {
            ASSERT_EQ(iter->str(), "c");
            ASSERT_EQ((*iter).str(), "c");
        }

        ASSERT_EQ(iter, node2_iter);

        ASSERT_FALSE(iter2.is_const());
        ASSERT_FALSE(iter2.is_past_the_end());
        ASSERT_FALSE(iter2.is_decrementable_pte());

        if (!iter2.is_past_the_end()) {
            ASSERT_EQ(iter2->str(), "c");
            ASSERT_EQ((*iter2).str(), "c");
        }

        ASSERT_EQ(iter2, node2_iter);

        ASSERT_EQ(iter, iter2);

        taul::node::iterator iter3 = --iter;

        ASSERT_FALSE(iter.is_const());
        ASSERT_FALSE(iter.is_past_the_end());
        ASSERT_FALSE(iter.is_decrementable_pte());

        if (!iter.is_past_the_end()) {
            ASSERT_EQ(iter->str(), "b");
            ASSERT_EQ((*iter).str(), "b");
        }

        ASSERT_EQ(iter, node1_iter);

        ASSERT_FALSE(iter3.is_const());
        ASSERT_FALSE(iter3.is_past_the_end());
        ASSERT_FALSE(iter3.is_decrementable_pte());

        if (!iter3.is_past_the_end()) {
            ASSERT_EQ(iter3->str(), "b");
            ASSERT_EQ((*iter3).str(), "b");
        }

        ASSERT_EQ(iter3, node1_iter);

        ASSERT_EQ(iter, iter3);
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, NodeIter_PostIncrAndDecr) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        taul::node node0 = nc->create(gram.lpr("lpr"), "a");
        taul::node node1 = nc->create(gram.lpr("lpr"), "b", 1);
        taul::node node2 = nc->create(gram.lpr("lpr"), "c", 2);

        auto node1_iter = node1.iter();
        auto node2_iter = node2.iter();

        node0.attach(std::move(node1));
        node0.attach(std::move(node2));

        EXPECT_EQ(nc->count(), 3);

        taul::node::iterator iter = node1_iter;

        ASSERT_FALSE(iter.is_const());
        ASSERT_FALSE(iter.is_past_the_end());
        ASSERT_FALSE(iter.is_decrementable_pte());

        if (!iter.is_past_the_end()) {
            ASSERT_EQ(iter->str(), "b");
            ASSERT_EQ((*iter).str(), "b");
        }

        ASSERT_EQ(iter, node1_iter);

        taul::node::iterator iter0 = iter++;

        ASSERT_FALSE(iter.is_const());
        ASSERT_FALSE(iter.is_past_the_end());
        ASSERT_FALSE(iter.is_decrementable_pte());

        if (!iter.is_past_the_end()) {
            ASSERT_EQ(iter->str(), "c");
            ASSERT_EQ((*iter).str(), "c");
        }

        ASSERT_EQ(iter, node2_iter);

        ASSERT_FALSE(iter0.is_const());
        ASSERT_FALSE(iter0.is_past_the_end());
        ASSERT_FALSE(iter0.is_decrementable_pte());

        if (!iter0.is_past_the_end()) {
            ASSERT_EQ(iter0->str(), "b");
            ASSERT_EQ((*iter0).str(), "b");
        }

        ASSERT_EQ(iter0, node1_iter);

        taul::node::iterator iter1 = iter++;

        ASSERT_FALSE(iter.is_const());
        ASSERT_TRUE(iter.is_past_the_end());
        ASSERT_TRUE(iter.is_decrementable_pte());

        ASSERT_FALSE(iter1.is_const());
        ASSERT_FALSE(iter1.is_past_the_end());
        ASSERT_FALSE(iter1.is_decrementable_pte());

        if (!iter1.is_past_the_end()) {
            ASSERT_EQ(iter1->str(), "c");
            ASSERT_EQ((*iter1).str(), "c");
        }

        taul::node::iterator iter2 = iter--;

        ASSERT_FALSE(iter.is_const());
        ASSERT_FALSE(iter.is_past_the_end());
        ASSERT_FALSE(iter.is_decrementable_pte());

        if (!iter.is_past_the_end()) {
            ASSERT_EQ(iter->str(), "c");
            ASSERT_EQ((*iter).str(), "c");
        }

        ASSERT_EQ(iter, node2_iter);

        ASSERT_FALSE(iter2.is_const());
        ASSERT_TRUE(iter2.is_past_the_end());
        ASSERT_TRUE(iter2.is_decrementable_pte());

        taul::node::iterator iter3 = iter--;
        
        ASSERT_FALSE(iter.is_const());
        ASSERT_FALSE(iter.is_past_the_end());
        ASSERT_FALSE(iter.is_decrementable_pte());

        if (!iter.is_past_the_end()) {
            ASSERT_EQ(iter->str(), "b");
            ASSERT_EQ((*iter).str(), "b");
        }

        ASSERT_EQ(iter, node1_iter);

        ASSERT_FALSE(iter3.is_const());
        ASSERT_FALSE(iter3.is_past_the_end());
        ASSERT_FALSE(iter3.is_decrementable_pte());

        if (!iter3.is_past_the_end()) {
            ASSERT_EQ(iter3->str(), "c");
            ASSERT_EQ((*iter3).str(), "c");
        }

        ASSERT_EQ(iter3, node2_iter);
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, NodeIter_ToConst) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        taul::node node0 = nc->create(gram.lpr("lpr"), "a");
        taul::node node1 = nc->create(gram.lpr("lpr"), "b", 1);
        taul::node node2 = nc->create(gram.lpr("lpr"), "c", 2);

        auto node1_iter = node1.iter();
        auto node2_iter = node2.iter();

        node0.attach(std::move(node1));
        node0.attach(std::move(node2));

        EXPECT_EQ(nc->count(), 3);

        auto iter0 = taul::node_iter<false>().to_const();
        auto iter1 = node1_iter.to_const();
        auto iter2 = node2_iter.to_const();
        auto iter3 = std::next(iter2).to_const();

        ASSERT_TRUE(iter0.is_const());
        ASSERT_TRUE(iter0.is_past_the_end());
        ASSERT_FALSE(iter0.is_decrementable_pte());

        ASSERT_TRUE(iter1.is_const());
        ASSERT_FALSE(iter1.is_past_the_end());
        ASSERT_FALSE(iter1.is_decrementable_pte());

        if (!iter1.is_past_the_end()) {
            ASSERT_EQ(iter1->str(), "b");
            ASSERT_EQ((*iter1).str(), "b");
        }

        ASSERT_EQ(iter1, node1_iter);

        ASSERT_TRUE(iter2.is_const());
        ASSERT_FALSE(iter2.is_past_the_end());
        ASSERT_FALSE(iter2.is_decrementable_pte());

        if (!iter2.is_past_the_end()) {
            ASSERT_EQ(iter2->str(), "c");
            ASSERT_EQ((*iter2).str(), "c");
        }

        ASSERT_EQ(iter2, node2_iter);

        ASSERT_TRUE(iter3.is_const());
        ASSERT_TRUE(iter3.is_past_the_end());
        ASSERT_TRUE(iter3.is_decrementable_pte());

        // implicit convert on assign

        taul::node_iter<true> iter4{}, iter5{}, iter6{}, iter7{};

        iter4 = taul::node_iter<false>();
        iter5 = node1_iter;
        iter6 = node2_iter;
        iter7 = std::next(iter6);

        ASSERT_TRUE(iter4.is_const());
        ASSERT_TRUE(iter4.is_past_the_end());
        ASSERT_FALSE(iter4.is_decrementable_pte());

        ASSERT_TRUE(iter5.is_const());
        ASSERT_FALSE(iter5.is_past_the_end());
        ASSERT_FALSE(iter5.is_decrementable_pte());

        if (!iter5.is_past_the_end()) {
            ASSERT_EQ(iter5->str(), "b");
            ASSERT_EQ((*iter5).str(), "b");
        }

        ASSERT_EQ(iter5, node1_iter);

        ASSERT_TRUE(iter6.is_const());
        ASSERT_FALSE(iter6.is_past_the_end());
        ASSERT_FALSE(iter6.is_decrementable_pte());

        if (!iter6.is_past_the_end()) {
            ASSERT_EQ(iter6->str(), "c");
            ASSERT_EQ((*iter6).str(), "c");
        }

        ASSERT_EQ(iter6, node2_iter);

        ASSERT_TRUE(iter7.is_const());
        ASSERT_TRUE(iter7.is_past_the_end());
        ASSERT_TRUE(iter7.is_decrementable_pte());
    }

    EXPECT_EQ(nc->count(), 0);
}

TEST_F(NodeTests, NodeIter_Equality) {

    auto nc = std::make_shared<taul::node_ctx>(lgr);

    ASSERT_TRUE((bool)nc);

    EXPECT_EQ(nc->count(), 0);

    {
        taul::node node0a = nc->create(gram.lpr("lpr"), "a");
        taul::node node1a = nc->create(gram.lpr("lpr"), "b", 1);
        taul::node node2a = nc->create(gram.lpr("lpr"), "c", 2);

        auto node1a_iter = node1a.iter();
        auto node2a_iter = node2a.iter();

        node0a.attach(std::move(node1a));
        node0a.attach(std::move(node2a));

        taul::node node0b = nc->create(gram.lpr("lpr"), "a");
        taul::node node1b = nc->create(gram.lpr("lpr"), "b", 1);
        taul::node node2b = nc->create(gram.lpr("lpr"), "c", 2);

        auto node1b_iter = node1b.iter();
        auto node2b_iter = node2b.iter();

        node0b.attach(std::move(node1b));
        node0b.attach(std::move(node2b));

        EXPECT_EQ(nc->count(), 6);

        taul::node::iterator iter0 = taul::node_iter<false>();
        taul::node::iterator iter1 = node1a_iter;
        taul::node::iterator iter2 = node2a_iter;
        taul::node::iterator iter3 = std::next(iter2);
        taul::node::iterator iter4 = node1b_iter;
        taul::node::iterator iter5 = node2b_iter;
        taul::node::iterator iter6 = std::next(iter5);


        EXPECT_EQ(iter0, iter0);
        EXPECT_NE(iter0, iter1);
        EXPECT_NE(iter0, iter2);
        EXPECT_EQ(iter0, iter3);
        EXPECT_NE(iter0, iter4);
        EXPECT_NE(iter0, iter5);
        EXPECT_EQ(iter0, iter6);
        
        EXPECT_NE(iter1, iter0);
        EXPECT_EQ(iter1, iter1);
        EXPECT_NE(iter1, iter2);
        EXPECT_NE(iter1, iter3);
        EXPECT_NE(iter1, iter4);
        EXPECT_NE(iter1, iter5);
        EXPECT_NE(iter1, iter6);
        
        EXPECT_NE(iter2, iter0);
        EXPECT_NE(iter2, iter1);
        EXPECT_EQ(iter2, iter2);
        EXPECT_NE(iter2, iter3);
        EXPECT_NE(iter2, iter4);
        EXPECT_NE(iter2, iter5);
        EXPECT_NE(iter2, iter6);
        
        EXPECT_EQ(iter3, iter0);
        EXPECT_NE(iter3, iter1);
        EXPECT_NE(iter3, iter2);
        EXPECT_EQ(iter3, iter3);
        EXPECT_NE(iter3, iter4);
        EXPECT_NE(iter3, iter5);
        EXPECT_EQ(iter3, iter6);

        EXPECT_NE(iter4, iter0);
        EXPECT_NE(iter4, iter1);
        EXPECT_NE(iter4, iter2);
        EXPECT_NE(iter4, iter3);
        EXPECT_EQ(iter4, iter4);
        EXPECT_NE(iter4, iter5);
        EXPECT_NE(iter4, iter6);
        
        EXPECT_NE(iter5, iter0);
        EXPECT_NE(iter5, iter1);
        EXPECT_NE(iter5, iter2);
        EXPECT_NE(iter5, iter3);
        EXPECT_NE(iter5, iter4);
        EXPECT_EQ(iter5, iter5);
        EXPECT_NE(iter5, iter6);
        
        EXPECT_EQ(iter6, iter0);
        EXPECT_NE(iter6, iter1);
        EXPECT_NE(iter6, iter2);
        EXPECT_EQ(iter6, iter3);
        EXPECT_NE(iter6, iter4);
        EXPECT_NE(iter6, iter5);
        EXPECT_EQ(iter6, iter6);


        EXPECT_TRUE(iter0.equal(iter0));
        EXPECT_FALSE(iter0.equal(iter1));
        EXPECT_FALSE(iter0.equal(iter2));
        EXPECT_TRUE(iter0.equal(iter3));
        EXPECT_FALSE(iter0.equal(iter4));
        EXPECT_FALSE(iter0.equal(iter5));
        EXPECT_TRUE(iter0.equal(iter6));
        
        EXPECT_FALSE(iter1.equal(iter0));
        EXPECT_TRUE(iter1.equal(iter1));
        EXPECT_FALSE(iter1.equal(iter2));
        EXPECT_FALSE(iter1.equal(iter3));
        EXPECT_FALSE(iter1.equal(iter4));
        EXPECT_FALSE(iter1.equal(iter5));
        EXPECT_FALSE(iter1.equal(iter6));
        
        EXPECT_FALSE(iter2.equal(iter0));
        EXPECT_FALSE(iter2.equal(iter1));
        EXPECT_TRUE(iter2.equal(iter2));
        EXPECT_FALSE(iter2.equal(iter3));
        EXPECT_FALSE(iter2.equal(iter4));
        EXPECT_FALSE(iter2.equal(iter5));
        EXPECT_FALSE(iter2.equal(iter6));
        
        EXPECT_TRUE(iter3.equal(iter0));
        EXPECT_FALSE(iter3.equal(iter1));
        EXPECT_FALSE(iter3.equal(iter2));
        EXPECT_TRUE(iter3.equal(iter3));
        EXPECT_FALSE(iter3.equal(iter4));
        EXPECT_FALSE(iter3.equal(iter5));
        EXPECT_TRUE(iter3.equal(iter6));

        EXPECT_FALSE(iter4.equal(iter0));
        EXPECT_FALSE(iter4.equal(iter1));
        EXPECT_FALSE(iter4.equal(iter2));
        EXPECT_FALSE(iter4.equal(iter3));
        EXPECT_TRUE(iter4.equal(iter4));
        EXPECT_FALSE(iter4.equal(iter5));
        EXPECT_FALSE(iter4.equal(iter6));
        
        EXPECT_FALSE(iter5.equal(iter0));
        EXPECT_FALSE(iter5.equal(iter1));
        EXPECT_FALSE(iter5.equal(iter2));
        EXPECT_FALSE(iter5.equal(iter3));
        EXPECT_FALSE(iter5.equal(iter4));
        EXPECT_TRUE(iter5.equal(iter5));
        EXPECT_FALSE(iter5.equal(iter6));
        
        EXPECT_TRUE(iter6.equal(iter0));
        EXPECT_FALSE(iter6.equal(iter1));
        EXPECT_FALSE(iter6.equal(iter2));
        EXPECT_TRUE(iter6.equal(iter3));
        EXPECT_FALSE(iter6.equal(iter4));
        EXPECT_FALSE(iter6.equal(iter5));
        EXPECT_TRUE(iter6.equal(iter6));


        EXPECT_TRUE(iter0 == iter0);
        EXPECT_FALSE(iter0 == iter1);
        EXPECT_FALSE(iter0 == iter2);
        EXPECT_TRUE(iter0 == iter3);
        EXPECT_FALSE(iter0 == iter4);
        EXPECT_FALSE(iter0 == iter5);
        EXPECT_TRUE(iter0 == iter6);
        
        EXPECT_FALSE(iter1 == iter0);
        EXPECT_TRUE(iter1 == iter1);
        EXPECT_FALSE(iter1 == iter2);
        EXPECT_FALSE(iter1 == iter3);
        EXPECT_FALSE(iter1 == iter4);
        EXPECT_FALSE(iter1 == iter5);
        EXPECT_FALSE(iter1 == iter6);
        
        EXPECT_FALSE(iter2 == iter0);
        EXPECT_FALSE(iter2 == iter1);
        EXPECT_TRUE(iter2 == iter2);
        EXPECT_FALSE(iter2 == iter3);
        EXPECT_FALSE(iter2 == iter4);
        EXPECT_FALSE(iter2 == iter5);
        EXPECT_FALSE(iter2 == iter6);
        
        EXPECT_TRUE(iter3 == iter0);
        EXPECT_FALSE(iter3 == iter1);
        EXPECT_FALSE(iter3 == iter2);
        EXPECT_TRUE(iter3 == iter3);
        EXPECT_FALSE(iter3 == iter4);
        EXPECT_FALSE(iter3 == iter5);
        EXPECT_TRUE(iter3 == iter6);

        EXPECT_FALSE(iter4 == iter0);
        EXPECT_FALSE(iter4 == iter1);
        EXPECT_FALSE(iter4 == iter2);
        EXPECT_FALSE(iter4 == iter3);
        EXPECT_TRUE(iter4 == iter4);
        EXPECT_FALSE(iter4 == iter5);
        EXPECT_FALSE(iter4 == iter6);
        
        EXPECT_FALSE(iter5 == iter0);
        EXPECT_FALSE(iter5 == iter1);
        EXPECT_FALSE(iter5 == iter2);
        EXPECT_FALSE(iter5 == iter3);
        EXPECT_FALSE(iter5 == iter4);
        EXPECT_TRUE(iter5 == iter5);
        EXPECT_FALSE(iter5 == iter6);
        
        EXPECT_TRUE(iter6 == iter0);
        EXPECT_FALSE(iter6 == iter1);
        EXPECT_FALSE(iter6 == iter2);
        EXPECT_TRUE(iter6 == iter3);
        EXPECT_FALSE(iter6 == iter4);
        EXPECT_FALSE(iter6 == iter5);
        EXPECT_TRUE(iter6 == iter6);


        EXPECT_FALSE(iter0 != iter0);
        EXPECT_TRUE(iter0 != iter1);
        EXPECT_TRUE(iter0 != iter2);
        EXPECT_FALSE(iter0 != iter3);
        EXPECT_TRUE(iter0 != iter4);
        EXPECT_TRUE(iter0 != iter5);
        EXPECT_FALSE(iter0 != iter6);
        
        EXPECT_TRUE(iter1 != iter0);
        EXPECT_FALSE(iter1 != iter1);
        EXPECT_TRUE(iter1 != iter2);
        EXPECT_TRUE(iter1 != iter3);
        EXPECT_TRUE(iter1 != iter4);
        EXPECT_TRUE(iter1 != iter5);
        EXPECT_TRUE(iter1 != iter6);
        
        EXPECT_TRUE(iter2 != iter0);
        EXPECT_TRUE(iter2 != iter1);
        EXPECT_FALSE(iter2 != iter2);
        EXPECT_TRUE(iter2 != iter3);
        EXPECT_TRUE(iter2 != iter4);
        EXPECT_TRUE(iter2 != iter5);
        EXPECT_TRUE(iter2 != iter6);
        
        EXPECT_FALSE(iter3 != iter0);
        EXPECT_TRUE(iter3 != iter1);
        EXPECT_TRUE(iter3 != iter2);
        EXPECT_FALSE(iter3 != iter3);
        EXPECT_TRUE(iter3 != iter4);
        EXPECT_TRUE(iter3 != iter5);
        EXPECT_FALSE(iter3 != iter6);

        EXPECT_TRUE(iter4 != iter0);
        EXPECT_TRUE(iter4 != iter1);
        EXPECT_TRUE(iter4 != iter2);
        EXPECT_TRUE(iter4 != iter3);
        EXPECT_FALSE(iter4 != iter4);
        EXPECT_TRUE(iter4 != iter5);
        EXPECT_TRUE(iter4 != iter6);
        
        EXPECT_TRUE(iter5 != iter0);
        EXPECT_TRUE(iter5 != iter1);
        EXPECT_TRUE(iter5 != iter2);
        EXPECT_TRUE(iter5 != iter3);
        EXPECT_TRUE(iter5 != iter4);
        EXPECT_FALSE(iter5 != iter5);
        EXPECT_TRUE(iter5 != iter6);
        
        EXPECT_FALSE(iter6 != iter0);
        EXPECT_TRUE(iter6 != iter1);
        EXPECT_TRUE(iter6 != iter2);
        EXPECT_FALSE(iter6 != iter3);
        EXPECT_TRUE(iter6 != iter4);
        EXPECT_TRUE(iter6 != iter5);
        EXPECT_FALSE(iter6 != iter6);
    }

    EXPECT_EQ(nc->count(), 0);
}

