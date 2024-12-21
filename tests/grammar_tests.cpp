

#include <gtest/gtest.h>

#include <taul/logger.h>
#include <taul/str.h>
#include <taul/spec.h>
#include <taul/load.h>

#include <taul/source_pos_counter.h>
#include <taul/parse_tree_pattern.h>
#include <taul/source_reader.h>
#include <taul/lexer.h>
#include <taul/parser.h>
#include <taul/no_recovery_error_handler.h>


using namespace taul::string_literals;


TEST(LPRRefTests, Basics) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str, taul::qualifier::skip)
        .close()
        .done();
    const auto gram = taul::load(spec, lgr);
    ASSERT_TRUE(gram);
    TAUL_LOG(lgr, "{}", *gram);
    const auto ref = gram->lpr("lpr1"_str);
    ASSERT_TRUE(ref);

    EXPECT_EQ(ref->name(), "lpr1"_str);
    EXPECT_EQ(ref->index(), 1);
    EXPECT_EQ(ref->id(), taul::lpr_id(1));
    EXPECT_EQ(ref->qualifier(), taul::qualifier::skip);
}

TEST(LPRRefTests, CopyCtor) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str, taul::qualifier::skip)
        .close()
        .done();
    const auto gram = taul::load(spec, lgr);
    ASSERT_TRUE(gram);
    TAUL_LOG(lgr, "{}", *gram);
    const auto ref0 = gram->lpr("lpr1"_str);
    ASSERT_TRUE(ref0);
    const taul::lpr_ref ref1(*ref0); // copy ctor

    EXPECT_EQ(ref1.name(), "lpr1"_str);
    EXPECT_EQ(ref1.index(), 1);
    EXPECT_EQ(ref1.id(), taul::lpr_id(1));
    EXPECT_EQ(ref1.qualifier(), taul::qualifier::skip);

    EXPECT_EQ(ref1, *(gram->lpr("lpr1"_str)));
}

TEST(LPRRefTests, MoveCtor) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str, taul::qualifier::skip)
        .close()
        .done();
    const auto gram = taul::load(spec, lgr);
    ASSERT_TRUE(gram);
    TAUL_LOG(lgr, "{}", *gram);
    auto ref0 = gram->lpr("lpr1"_str);
    ASSERT_TRUE(ref0);
    const taul::lpr_ref ref1(std::move(*ref0)); // move ctor

    EXPECT_EQ(ref1.name(), "lpr1"_str);
    EXPECT_EQ(ref1.index(), 1);
    EXPECT_EQ(ref1.id(), taul::lpr_id(1));
    EXPECT_EQ(ref1.qualifier(), taul::qualifier::skip);

    EXPECT_EQ(ref1, *(gram->lpr("lpr1"_str)));
}

TEST(LPRRefTests, CopyAssign) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str, taul::qualifier::skip)
        .close()
        .done();
    const auto gram = taul::load(spec, lgr);
    ASSERT_TRUE(gram);
    TAUL_LOG(lgr, "{}", *gram);
    const auto ref0 = gram->lpr("lpr1"_str);
    ASSERT_TRUE(ref0);
    auto ref1 = gram->lpr("lpr0"_str);
    ASSERT_TRUE(ref1);
    *ref1 = *ref0; // copy assign

    EXPECT_EQ(ref1->name(), "lpr1"_str);
    EXPECT_EQ(ref1->index(), 1);
    EXPECT_EQ(ref1->id(), taul::lpr_id(1));
    EXPECT_EQ(ref1->qualifier(), taul::qualifier::skip);

    EXPECT_EQ(*ref1, *(gram->lpr("lpr1"_str)));
}

TEST(LPRRefTests, MoveAssign) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str, taul::qualifier::skip)
        .close()
        .done();
    const auto gram = taul::load(spec, lgr);
    ASSERT_TRUE(gram);
    TAUL_LOG(lgr, "{}", *gram);
    auto ref0 = gram->lpr("lpr1"_str);
    ASSERT_TRUE(ref0);
    auto ref1 = gram->lpr("lpr0"_str);
    ASSERT_TRUE(ref1);
    *ref1 = std::move(*ref0); // move assign

    EXPECT_EQ(ref1->name(), "lpr1"_str);
    EXPECT_EQ(ref1->index(), 1);
    EXPECT_EQ(ref1->id(), taul::lpr_id(1));
    EXPECT_EQ(ref1->qualifier(), taul::qualifier::skip);

    EXPECT_EQ(*ref1, *(gram->lpr("lpr1"_str)));
}

TEST(LPRRefTests, Equality) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str, taul::qualifier::skip)
        .close()
        .done();
    const auto gram0 = taul::load(spec, lgr);
    ASSERT_TRUE(gram0);
    const std::optional<taul::grammar> gram1 = gram0;
    ASSERT_TRUE(gram1);
    const auto gram2 = taul::load(spec, lgr);
    ASSERT_TRUE(gram2);
    TAUL_LOG(lgr, "{}", *gram0);
    TAUL_LOG(lgr, "{}", *gram1);
    TAUL_LOG(lgr, "{}", *gram2);

    ASSERT_EQ(gram0->lprs(), 2);
    ASSERT_TRUE(gram0->lpr("lpr0"_str));
    ASSERT_TRUE(gram0->lpr("lpr1"_str));
    ASSERT_EQ(gram1->lprs(), 2);
    ASSERT_TRUE(gram1->lpr("lpr0"_str));
    ASSERT_TRUE(gram1->lpr("lpr1"_str));
    ASSERT_EQ(gram2->lprs(), 2);
    ASSERT_TRUE(gram2->lpr("lpr0"_str));
    ASSERT_TRUE(gram2->lpr("lpr1"_str));

    EXPECT_TRUE(gram0->lpr_at(0).equal(gram0->lpr_at(0)));
    EXPECT_TRUE(gram0->lpr_at(0).equal(gram1->lpr_at(0)));
    EXPECT_TRUE(gram0->lpr_at(1).equal(gram0->lpr_at(1)));
    EXPECT_TRUE(gram0->lpr_at(1).equal(gram1->lpr_at(1)));
    EXPECT_TRUE(gram0->lpr("lpr0"_str)->equal(*gram0->lpr("lpr0"_str)));
    EXPECT_TRUE(gram0->lpr("lpr0"_str)->equal(*gram1->lpr("lpr0"_str)));
    EXPECT_TRUE(gram0->lpr("lpr1"_str)->equal(*gram0->lpr("lpr1"_str)));
    EXPECT_TRUE(gram0->lpr("lpr1"_str)->equal(*gram1->lpr("lpr1"_str)));
    EXPECT_TRUE(gram0->lpr_at(0).equal(*gram0->lpr("lpr0"_str)));
    EXPECT_TRUE(gram0->lpr_at(0).equal(*gram1->lpr("lpr0"_str)));
    EXPECT_TRUE(gram0->lpr_at(1).equal(*gram0->lpr("lpr1"_str)));
    EXPECT_TRUE(gram0->lpr_at(1).equal(*gram1->lpr("lpr1"_str)));
    
    EXPECT_FALSE(gram0->lpr_at(0).equal(gram0->lpr_at(1)));
    EXPECT_FALSE(gram0->lpr_at(0).equal(gram1->lpr_at(1)));
    EXPECT_FALSE(gram0->lpr("lpr0"_str)->equal(*gram0->lpr("lpr1"_str)));
    EXPECT_FALSE(gram0->lpr("lpr0"_str)->equal(*gram1->lpr("lpr1"_str)));
    EXPECT_FALSE(gram0->lpr_at(0).equal(*gram0->lpr("lpr1"_str)));
    EXPECT_FALSE(gram0->lpr_at(0).equal(*gram1->lpr("lpr1"_str)));

    EXPECT_FALSE(gram0->lpr_at(0).equal(gram2->lpr_at(0)));
    EXPECT_FALSE(gram0->lpr_at(1).equal(gram2->lpr_at(1)));
    EXPECT_FALSE(gram0->lpr("lpr0"_str)->equal(*gram2->lpr("lpr0"_str)));
    EXPECT_FALSE(gram0->lpr("lpr1"_str)->equal(*gram2->lpr("lpr1"_str)));

    EXPECT_TRUE(gram0->lpr_at(0) == gram0->lpr_at(0));
    EXPECT_TRUE(gram0->lpr_at(0) == gram1->lpr_at(0));
    EXPECT_TRUE(gram0->lpr_at(1) == gram0->lpr_at(1));
    EXPECT_TRUE(gram0->lpr_at(1) == gram1->lpr_at(1));
    EXPECT_TRUE(*gram0->lpr("lpr0"_str) == *gram0->lpr("lpr0"_str));
    EXPECT_TRUE(*gram0->lpr("lpr0"_str) == *gram1->lpr("lpr0"_str));
    EXPECT_TRUE(*gram0->lpr("lpr1"_str) == *gram0->lpr("lpr1"_str));
    EXPECT_TRUE(*gram0->lpr("lpr1"_str) == *gram1->lpr("lpr1"_str));
    EXPECT_TRUE(gram0->lpr_at(0) == *gram0->lpr("lpr0"_str));
    EXPECT_TRUE(gram0->lpr_at(0) == *gram1->lpr("lpr0"_str));
    EXPECT_TRUE(gram0->lpr_at(1) == *gram0->lpr("lpr1"_str));
    EXPECT_TRUE(gram0->lpr_at(1) == *gram1->lpr("lpr1"_str));

    EXPECT_FALSE(gram0->lpr_at(0) == gram0->lpr_at(1));
    EXPECT_FALSE(gram0->lpr_at(0) == gram1->lpr_at(1));
    EXPECT_FALSE(*gram0->lpr("lpr0"_str) == *gram0->lpr("lpr1"_str));
    EXPECT_FALSE(*gram0->lpr("lpr0"_str) == *gram1->lpr("lpr1"_str));
    EXPECT_FALSE(gram0->lpr_at(0) == *gram0->lpr("lpr1"_str));
    EXPECT_FALSE(gram0->lpr_at(0) == *gram1->lpr("lpr1"_str));

    EXPECT_FALSE(gram0->lpr_at(0) == gram2->lpr_at(0));
    EXPECT_FALSE(gram0->lpr_at(1) == gram2->lpr_at(1));
    EXPECT_FALSE(*gram0->lpr("lpr0"_str) == *gram2->lpr("lpr0"_str));
    EXPECT_FALSE(*gram0->lpr("lpr1"_str) == *gram2->lpr("lpr1"_str));

    EXPECT_FALSE(gram0->lpr_at(0) != gram0->lpr_at(0));
    EXPECT_FALSE(gram0->lpr_at(0) != gram1->lpr_at(0));
    EXPECT_FALSE(gram0->lpr_at(1) != gram0->lpr_at(1));
    EXPECT_FALSE(gram0->lpr_at(1) != gram1->lpr_at(1));
    EXPECT_FALSE(*gram0->lpr("lpr0"_str) != *gram0->lpr("lpr0"_str));
    EXPECT_FALSE(*gram0->lpr("lpr0"_str) != *gram1->lpr("lpr0"_str));
    EXPECT_FALSE(*gram0->lpr("lpr1"_str) != *gram0->lpr("lpr1"_str));
    EXPECT_FALSE(*gram0->lpr("lpr1"_str) != *gram1->lpr("lpr1"_str));
    EXPECT_FALSE(gram0->lpr_at(0) != *gram0->lpr("lpr0"_str));
    EXPECT_FALSE(gram0->lpr_at(0) != *gram1->lpr("lpr0"_str));
    EXPECT_FALSE(gram0->lpr_at(1) != *gram0->lpr("lpr1"_str));
    EXPECT_FALSE(gram0->lpr_at(1) != *gram1->lpr("lpr1"_str));

    EXPECT_TRUE(gram0->lpr_at(0) != gram0->lpr_at(1));
    EXPECT_TRUE(gram0->lpr_at(0) != gram1->lpr_at(1));
    EXPECT_TRUE(*gram0->lpr("lpr0"_str) != *gram0->lpr("lpr1"_str));
    EXPECT_TRUE(*gram0->lpr("lpr0"_str) != *gram1->lpr("lpr1"_str));
    EXPECT_TRUE(gram0->lpr_at(0) != *gram0->lpr("lpr1"_str));
    EXPECT_TRUE(gram0->lpr_at(0) != *gram1->lpr("lpr1"_str));

    EXPECT_TRUE(gram0->lpr_at(0) != gram2->lpr_at(0));
    EXPECT_TRUE(gram0->lpr_at(1) != gram2->lpr_at(1));
    EXPECT_TRUE(*gram0->lpr("lpr0"_str) != *gram2->lpr("lpr0"_str));
    EXPECT_TRUE(*gram0->lpr("lpr1"_str) != *gram2->lpr("lpr1"_str));
}

TEST(LPRRefTests, Hash) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str, taul::qualifier::skip)
        .close()
        .done();
    const auto gram0 = taul::load(spec, lgr);
    ASSERT_TRUE(gram0);
    const std::optional<taul::grammar> gram1 = gram0;
    ASSERT_TRUE(gram1);
    const auto gram2 = taul::load(spec, lgr);
    ASSERT_TRUE(gram2);
    TAUL_LOG(lgr, "{}", *gram0);
    TAUL_LOG(lgr, "{}", *gram1);
    TAUL_LOG(lgr, "{}", *gram2);

    TAUL_LOG(lgr, "gram0->lpr_at(0).hash()=={}", gram0->lpr_at(0).hash());
    TAUL_LOG(lgr, "gram0->lpr_at(1).hash()=={}", gram0->lpr_at(1).hash());
    TAUL_LOG(lgr, "gram1->lpr_at(0).hash()=={}", gram1->lpr_at(0).hash());
    TAUL_LOG(lgr, "gram1->lpr_at(1).hash()=={}", gram1->lpr_at(1).hash());
    TAUL_LOG(lgr, "gram2->lpr_at(0).hash()=={}", gram2->lpr_at(0).hash());
    TAUL_LOG(lgr, "gram2->lpr_at(1).hash()=={}", gram2->lpr_at(1).hash());

    EXPECT_EQ(gram0->lpr_at(0).hash(), gram0->lpr_at(0).hash());
    EXPECT_EQ(gram0->lpr_at(0).hash(), gram1->lpr_at(0).hash());
    EXPECT_EQ(gram0->lpr_at(1).hash(), gram0->lpr_at(1).hash());
    EXPECT_EQ(gram0->lpr_at(1).hash(), gram1->lpr_at(1).hash());

    EXPECT_NE(gram0->lpr_at(0).hash(), gram0->lpr_at(1).hash());
    EXPECT_NE(gram0->lpr_at(0).hash(), gram2->lpr_at(0).hash());

    EXPECT_EQ(gram0->lpr("lpr0"_str)->hash(), gram0->lpr("lpr0"_str)->hash());
    EXPECT_EQ(gram0->lpr("lpr0"_str)->hash(), gram1->lpr("lpr0"_str)->hash());
    EXPECT_EQ(gram0->lpr("lpr1"_str)->hash(), gram0->lpr("lpr1"_str)->hash());
    EXPECT_EQ(gram0->lpr("lpr1"_str)->hash(), gram1->lpr("lpr1"_str)->hash());

    EXPECT_NE(gram0->lpr("lpr0"_str)->hash(), gram0->lpr("lpr1"_str)->hash());
    EXPECT_NE(gram0->lpr("lpr0"_str)->hash(), gram2->lpr("lpr0"_str)->hash());
}

TEST(PPRRefTests, Basics) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .close()
        .done();
    const auto gram = taul::load(spec, lgr);
    ASSERT_TRUE(gram);
    TAUL_LOG(lgr, "{}", *gram);
    const auto ref = gram->ppr("ppr1"_str);
    ASSERT_TRUE(ref);

    EXPECT_EQ(ref->name(), "ppr1"_str);
    EXPECT_EQ(ref->index(), 1);
    EXPECT_EQ(ref->id(), taul::ppr_id(1));
    EXPECT_EQ(ref->qualifier(), taul::qualifier::none);
}

TEST(PPRRefTests, CopyCtor) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .close()
        .done();
    const auto gram = taul::load(spec, lgr);
    ASSERT_TRUE(gram);
    TAUL_LOG(lgr, "{}", *gram);
    const auto ref0 = gram->ppr("ppr1"_str);
    ASSERT_TRUE(ref0);
    const taul::ppr_ref ref1(*ref0); // copy ctor

    EXPECT_EQ(ref1.name(), "ppr1"_str);
    EXPECT_EQ(ref1.index(), 1);
    EXPECT_EQ(ref1.id(), taul::ppr_id(1));
    EXPECT_EQ(ref1.qualifier(), taul::qualifier::none);

    EXPECT_EQ(ref1, *(gram->ppr("ppr1"_str)));
}

TEST(PPRRefTests, MoveCtor) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .close()
        .done();
    const auto gram = taul::load(spec, lgr);
    ASSERT_TRUE(gram);
    TAUL_LOG(lgr, "{}", *gram);
    auto ref0 = gram->ppr("ppr1"_str);
    ASSERT_TRUE(ref0);
    const taul::ppr_ref ref1(std::move(*ref0)); // move ctor

    EXPECT_EQ(ref1.name(), "ppr1"_str);
    EXPECT_EQ(ref1.index(), 1);
    EXPECT_EQ(ref1.id(), taul::ppr_id(1));
    EXPECT_EQ(ref1.qualifier(), taul::qualifier::none);

    EXPECT_EQ(ref1, *(gram->ppr("ppr1"_str)));
}

TEST(PPRRefTests, CopyAssign) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .close()
        .done();
    const auto gram = taul::load(spec, lgr);
    ASSERT_TRUE(gram);
    TAUL_LOG(lgr, "{}", *gram);
    const auto ref0 = gram->ppr("ppr1"_str);
    ASSERT_TRUE(ref0);
    auto ref1 = gram->ppr("ppr0"_str);
    ASSERT_TRUE(ref1);
    *ref1 = *ref0; // copy assign

    EXPECT_EQ(ref1->name(), "ppr1"_str);
    EXPECT_EQ(ref1->index(), 1);
    EXPECT_EQ(ref1->id(), taul::ppr_id(1));
    EXPECT_EQ(ref1->qualifier(), taul::qualifier::none);

    EXPECT_EQ(*ref1, *(gram->ppr("ppr1"_str)));
}

TEST(PPRRefTests, MoveAssign) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .close()
        .done();
    const auto gram = taul::load(spec, lgr);
    ASSERT_TRUE(gram);
    TAUL_LOG(lgr, "{}", *gram);
    auto ref0 = gram->ppr("ppr1"_str);
    ASSERT_TRUE(ref0);
    auto ref1 = gram->ppr("ppr0"_str);
    ASSERT_TRUE(ref1);
    *ref1 = std::move(*ref0); // move assign

    EXPECT_EQ(ref1->name(), "ppr1"_str);
    EXPECT_EQ(ref1->index(), 1);
    EXPECT_EQ(ref1->id(), taul::ppr_id(1));
    EXPECT_EQ(ref1->qualifier(), taul::qualifier::none);

    EXPECT_EQ(*ref1, *(gram->ppr("ppr1"_str)));
}

TEST(PPRRefTests, Equality) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .close()
        .done();
    const auto gram0 = taul::load(spec, lgr);
    ASSERT_TRUE(gram0);
    const std::optional<taul::grammar> gram1 = gram0;
    ASSERT_TRUE(gram1);
    const auto gram2 = taul::load(spec, lgr);
    ASSERT_TRUE(gram2);
    TAUL_LOG(lgr, "{}", *gram0);
    TAUL_LOG(lgr, "{}", *gram1);
    TAUL_LOG(lgr, "{}", *gram2);

    ASSERT_EQ(gram0->pprs(), 2);
    ASSERT_TRUE(gram0->ppr("ppr0"_str));
    ASSERT_TRUE(gram0->ppr("ppr1"_str));
    ASSERT_EQ(gram1->pprs(), 2);
    ASSERT_TRUE(gram1->ppr("ppr0"_str));
    ASSERT_TRUE(gram1->ppr("ppr1"_str));
    ASSERT_EQ(gram2->pprs(), 2);
    ASSERT_TRUE(gram2->ppr("ppr0"_str));
    ASSERT_TRUE(gram2->ppr("ppr1"_str));

    EXPECT_TRUE(gram0->ppr_at(0).equal(gram0->ppr_at(0)));
    EXPECT_TRUE(gram0->ppr_at(0).equal(gram1->ppr_at(0)));
    EXPECT_TRUE(gram0->ppr_at(1).equal(gram0->ppr_at(1)));
    EXPECT_TRUE(gram0->ppr_at(1).equal(gram1->ppr_at(1)));
    EXPECT_TRUE(gram0->ppr("ppr0"_str)->equal(*gram0->ppr("ppr0"_str)));
    EXPECT_TRUE(gram0->ppr("ppr0"_str)->equal(*gram1->ppr("ppr0"_str)));
    EXPECT_TRUE(gram0->ppr("ppr1"_str)->equal(*gram0->ppr("ppr1"_str)));
    EXPECT_TRUE(gram0->ppr("ppr1"_str)->equal(*gram1->ppr("ppr1"_str)));
    EXPECT_TRUE(gram0->ppr_at(0).equal(*gram0->ppr("ppr0"_str)));
    EXPECT_TRUE(gram0->ppr_at(0).equal(*gram1->ppr("ppr0"_str)));
    EXPECT_TRUE(gram0->ppr_at(1).equal(*gram0->ppr("ppr1"_str)));
    EXPECT_TRUE(gram0->ppr_at(1).equal(*gram1->ppr("ppr1"_str)));

    EXPECT_FALSE(gram0->ppr_at(0).equal(gram0->ppr_at(1)));
    EXPECT_FALSE(gram0->ppr_at(0).equal(gram1->ppr_at(1)));
    EXPECT_FALSE(gram0->ppr("ppr0"_str)->equal(*gram0->ppr("ppr1"_str)));
    EXPECT_FALSE(gram0->ppr("ppr0"_str)->equal(*gram1->ppr("ppr1"_str)));
    EXPECT_FALSE(gram0->ppr_at(0).equal(*gram0->ppr("ppr1"_str)));
    EXPECT_FALSE(gram0->ppr_at(0).equal(*gram1->ppr("ppr1"_str)));

    EXPECT_FALSE(gram0->ppr_at(0).equal(gram2->ppr_at(0)));
    EXPECT_FALSE(gram0->ppr_at(1).equal(gram2->ppr_at(1)));
    EXPECT_FALSE(gram0->ppr("ppr0"_str)->equal(*gram2->ppr("ppr0"_str)));
    EXPECT_FALSE(gram0->ppr("ppr1"_str)->equal(*gram2->ppr("ppr1"_str)));

    EXPECT_TRUE(gram0->ppr_at(0) == gram0->ppr_at(0));
    EXPECT_TRUE(gram0->ppr_at(0) == gram1->ppr_at(0));
    EXPECT_TRUE(gram0->ppr_at(1) == gram0->ppr_at(1));
    EXPECT_TRUE(gram0->ppr_at(1) == gram1->ppr_at(1));
    EXPECT_TRUE(*gram0->ppr("ppr0"_str) == *gram0->ppr("ppr0"_str));
    EXPECT_TRUE(*gram0->ppr("ppr0"_str) == *gram1->ppr("ppr0"_str));
    EXPECT_TRUE(*gram0->ppr("ppr1"_str) == *gram0->ppr("ppr1"_str));
    EXPECT_TRUE(*gram0->ppr("ppr1"_str) == *gram1->ppr("ppr1"_str));
    EXPECT_TRUE(gram0->ppr_at(0) == *gram0->ppr("ppr0"_str));
    EXPECT_TRUE(gram0->ppr_at(0) == *gram1->ppr("ppr0"_str));
    EXPECT_TRUE(gram0->ppr_at(1) == *gram0->ppr("ppr1"_str));
    EXPECT_TRUE(gram0->ppr_at(1) == *gram1->ppr("ppr1"_str));

    EXPECT_FALSE(gram0->ppr_at(0) == gram0->ppr_at(1));
    EXPECT_FALSE(gram0->ppr_at(0) == gram1->ppr_at(1));
    EXPECT_FALSE(*gram0->ppr("ppr0"_str) == *gram0->ppr("ppr1"_str));
    EXPECT_FALSE(*gram0->ppr("ppr0"_str) == *gram1->ppr("ppr1"_str));
    EXPECT_FALSE(gram0->ppr_at(0) == *gram0->ppr("ppr1"_str));
    EXPECT_FALSE(gram0->ppr_at(0) == *gram1->ppr("ppr1"_str));

    EXPECT_FALSE(gram0->ppr_at(0) == gram2->ppr_at(0));
    EXPECT_FALSE(gram0->ppr_at(1) == gram2->ppr_at(1));
    EXPECT_FALSE(*gram0->ppr("ppr0"_str) == *gram2->ppr("ppr0"_str));
    EXPECT_FALSE(*gram0->ppr("ppr1"_str) == *gram2->ppr("ppr1"_str));

    EXPECT_FALSE(gram0->ppr_at(0) != gram0->ppr_at(0));
    EXPECT_FALSE(gram0->ppr_at(0) != gram1->ppr_at(0));
    EXPECT_FALSE(gram0->ppr_at(1) != gram0->ppr_at(1));
    EXPECT_FALSE(gram0->ppr_at(1) != gram1->ppr_at(1));
    EXPECT_FALSE(*gram0->ppr("ppr0"_str) != *gram0->ppr("ppr0"_str));
    EXPECT_FALSE(*gram0->ppr("ppr0"_str) != *gram1->ppr("ppr0"_str));
    EXPECT_FALSE(*gram0->ppr("ppr1"_str) != *gram0->ppr("ppr1"_str));
    EXPECT_FALSE(*gram0->ppr("ppr1"_str) != *gram1->ppr("ppr1"_str));
    EXPECT_FALSE(gram0->ppr_at(0) != *gram0->ppr("ppr0"_str));
    EXPECT_FALSE(gram0->ppr_at(0) != *gram1->ppr("ppr0"_str));
    EXPECT_FALSE(gram0->ppr_at(1) != *gram0->ppr("ppr1"_str));
    EXPECT_FALSE(gram0->ppr_at(1) != *gram1->ppr("ppr1"_str));

    EXPECT_TRUE(gram0->ppr_at(0) != gram0->ppr_at(1));
    EXPECT_TRUE(gram0->ppr_at(0) != gram1->ppr_at(1));
    EXPECT_TRUE(*gram0->ppr("ppr0"_str) != *gram0->ppr("ppr1"_str));
    EXPECT_TRUE(*gram0->ppr("ppr0"_str) != *gram1->ppr("ppr1"_str));
    EXPECT_TRUE(gram0->ppr_at(0) != *gram0->ppr("ppr1"_str));
    EXPECT_TRUE(gram0->ppr_at(0) != *gram1->ppr("ppr1"_str));

    EXPECT_TRUE(gram0->ppr_at(0) != gram2->ppr_at(0));
    EXPECT_TRUE(gram0->ppr_at(1) != gram2->ppr_at(1));
    EXPECT_TRUE(*gram0->ppr("ppr0"_str) != *gram2->ppr("ppr0"_str));
    EXPECT_TRUE(*gram0->ppr("ppr1"_str) != *gram2->ppr("ppr1"_str));
}

TEST(PPRRefTests, Hash) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .close()
        .done();
    const auto gram0 = taul::load(spec, lgr);
    ASSERT_TRUE(gram0);
    const std::optional<taul::grammar> gram1 = gram0;
    ASSERT_TRUE(gram1);
    const auto gram2 = taul::load(spec, lgr);
    ASSERT_TRUE(gram2);
    TAUL_LOG(lgr, "{}", *gram0);
    TAUL_LOG(lgr, "{}", *gram1);
    TAUL_LOG(lgr, "{}", *gram2);

    TAUL_LOG(lgr, "gram0->ppr_at(0).hash()=={}", gram0->ppr_at(0).hash());
    TAUL_LOG(lgr, "gram0->ppr_at(1).hash()=={}", gram0->ppr_at(1).hash());
    TAUL_LOG(lgr, "gram1->ppr_at(0).hash()=={}", gram1->ppr_at(0).hash());
    TAUL_LOG(lgr, "gram1->ppr_at(1).hash()=={}", gram1->ppr_at(1).hash());
    TAUL_LOG(lgr, "gram2->ppr_at(0).hash()=={}", gram2->ppr_at(0).hash());
    TAUL_LOG(lgr, "gram2->ppr_at(1).hash()=={}", gram2->ppr_at(1).hash());

    EXPECT_EQ(gram0->ppr_at(0).hash(), gram0->ppr_at(0).hash());
    EXPECT_EQ(gram0->ppr_at(0).hash(), gram1->ppr_at(0).hash());
    EXPECT_EQ(gram0->ppr_at(1).hash(), gram0->ppr_at(1).hash());
    EXPECT_EQ(gram0->ppr_at(1).hash(), gram1->ppr_at(1).hash());

    EXPECT_NE(gram0->ppr_at(0).hash(), gram0->ppr_at(1).hash());
    EXPECT_NE(gram0->ppr_at(0).hash(), gram2->ppr_at(0).hash());

    EXPECT_EQ(gram0->ppr("ppr0"_str)->hash(), gram0->ppr("ppr0"_str)->hash());
    EXPECT_EQ(gram0->ppr("ppr0"_str)->hash(), gram1->ppr("ppr0"_str)->hash());
    EXPECT_EQ(gram0->ppr("ppr1"_str)->hash(), gram0->ppr("ppr1"_str)->hash());
    EXPECT_EQ(gram0->ppr("ppr1"_str)->hash(), gram1->ppr("ppr1"_str)->hash());

    EXPECT_NE(gram0->ppr("ppr0"_str)->hash(), gram0->ppr("ppr1"_str)->hash());
    EXPECT_NE(gram0->ppr("ppr0"_str)->hash(), gram2->ppr("ppr0"_str)->hash());
}

TEST(GrammarTests, DefaultCtor) {
    const auto lgr = taul::make_stderr_logger();
    taul::grammar gram{};

    TAUL_LOG(lgr, "{}", gram);

    EXPECT_EQ(gram.lprs(), 0);
    EXPECT_EQ(gram.pprs(), 0);

    EXPECT_THROW(gram.lpr_at(0), std::out_of_range);
    EXPECT_THROW(gram.ppr_at(0), std::out_of_range);

    EXPECT_EQ(gram.lpr("missing"_str), std::nullopt);
    EXPECT_EQ(gram.ppr("missing"_str), std::nullopt);

    EXPECT_FALSE(gram.has_rule("missing"_str));
    EXPECT_FALSE(gram.has_lpr("missing"_str));
    EXPECT_FALSE(gram.has_ppr("missing"_str));
}

TEST(GrammarTests, CopyCtor) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str, taul::qualifier::skip)
        .close()
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .close()
        .done();
    const auto gram0 = taul::load(spec, lgr);
    ASSERT_TRUE(gram0);
    taul::grammar gram(*gram0); // copy ctor

    TAUL_LOG(lgr, "{}", gram);

    EXPECT_EQ(gram.lprs(), 2);
    EXPECT_EQ(gram.pprs(), 2);

    if (gram.lprs() == 2) {
        EXPECT_EQ(gram.lpr_at(0).name(), "lpr0"_str);
        EXPECT_EQ(gram.lpr_at(0).index(), 0);
        EXPECT_EQ(gram.lpr_at(0).qualifier(), taul::qualifier::none);
        EXPECT_EQ(gram.lpr_at(0).id(), taul::lpr_id(0));
        EXPECT_EQ(gram.lpr_at(1).name(), "lpr1"_str);
        EXPECT_EQ(gram.lpr_at(1).index(), 1);
        EXPECT_EQ(gram.lpr_at(1).qualifier(), taul::qualifier::skip);
        EXPECT_EQ(gram.lpr_at(1).id(), taul::lpr_id(1));
    }
    else ADD_FAILURE();

    if (gram.pprs() == 2) {
        EXPECT_EQ(gram.ppr_at(0).name(), "ppr0"_str);
        EXPECT_EQ(gram.ppr_at(0).index(), 0);
        EXPECT_EQ(gram.ppr_at(0).id(), taul::ppr_id(0));
        EXPECT_EQ(gram.ppr_at(0).qualifier(), taul::qualifier::none);
        EXPECT_EQ(gram.ppr_at(1).name(), "ppr1"_str);
        EXPECT_EQ(gram.ppr_at(1).index(), 1);
        EXPECT_EQ(gram.ppr_at(1).id(), taul::ppr_id(1));
        EXPECT_EQ(gram.ppr_at(1).qualifier(), taul::qualifier::none);
    }
    else ADD_FAILURE();

    EXPECT_THROW(gram.lpr_at(2), std::out_of_range);
    EXPECT_THROW(gram.ppr_at(2), std::out_of_range);

    EXPECT_EQ(gram.lpr("missing"_str), std::nullopt);
    EXPECT_EQ(gram.ppr("missing"_str), std::nullopt);

    if (gram.has_lpr("lpr0"_str)) {
        EXPECT_EQ(gram.lpr_at(0), gram.lpr("lpr0"_str).value());
    }
    else ADD_FAILURE();
    if (gram.has_lpr("lpr1"_str)) {
        EXPECT_EQ(gram.lpr_at(1), gram.lpr("lpr1"_str).value());
    }
    else ADD_FAILURE();
    if (gram.has_ppr("ppr0"_str)) {
        EXPECT_EQ(gram.ppr_at(0), gram.ppr("ppr0"_str).value());
    }
    else ADD_FAILURE();
    if (gram.has_ppr("ppr1"_str)) {
        EXPECT_EQ(gram.ppr_at(1), gram.ppr("ppr1"_str).value());
    }
    else ADD_FAILURE();

    EXPECT_TRUE(gram.has_rule("lpr0"_str));
    EXPECT_TRUE(gram.has_rule("lpr1"_str));
    EXPECT_TRUE(gram.has_rule("ppr0"_str));
    EXPECT_TRUE(gram.has_rule("ppr1"_str));
    EXPECT_FALSE(gram.has_rule("missing"_str));

    EXPECT_TRUE(gram.has_lpr("lpr0"_str));
    EXPECT_TRUE(gram.has_lpr("lpr1"_str));
    EXPECT_FALSE(gram.has_lpr("ppr0"_str));
    EXPECT_FALSE(gram.has_lpr("ppr1"_str));
    EXPECT_FALSE(gram.has_lpr("missing"_str));

    EXPECT_FALSE(gram.has_ppr("lpr0"_str));
    EXPECT_FALSE(gram.has_ppr("lpr1"_str));
    EXPECT_TRUE(gram.has_ppr("ppr0"_str));
    EXPECT_TRUE(gram.has_ppr("ppr1"_str));
    EXPECT_FALSE(gram.has_ppr("missing"_str));
}

TEST(GrammarTests, MoveCtor) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str, taul::qualifier::skip)
        .close()
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .close()
        .done();
    auto gram0 = taul::load(spec, lgr);
    ASSERT_TRUE(gram0);
    taul::grammar gram(std::move(*gram0)); // move ctor

    TAUL_LOG(lgr, "{}", gram);

    EXPECT_EQ(gram.lprs(), 2);
    EXPECT_EQ(gram.pprs(), 2);

    if (gram.lprs() == 2) {
        EXPECT_EQ(gram.lpr_at(0).name(), "lpr0"_str);
        EXPECT_EQ(gram.lpr_at(0).index(), 0);
        EXPECT_EQ(gram.lpr_at(0).qualifier(), taul::qualifier::none);
        EXPECT_EQ(gram.lpr_at(0).id(), taul::lpr_id(0));
        EXPECT_EQ(gram.lpr_at(1).name(), "lpr1"_str);
        EXPECT_EQ(gram.lpr_at(1).index(), 1);
        EXPECT_EQ(gram.lpr_at(1).qualifier(), taul::qualifier::skip);
        EXPECT_EQ(gram.lpr_at(1).id(), taul::lpr_id(1));
    }
    else ADD_FAILURE();

    if (gram.pprs() == 2) {
        EXPECT_EQ(gram.ppr_at(0).name(), "ppr0"_str);
        EXPECT_EQ(gram.ppr_at(0).index(), 0);
        EXPECT_EQ(gram.ppr_at(0).id(), taul::ppr_id(0));
        EXPECT_EQ(gram.ppr_at(0).qualifier(), taul::qualifier::none);
        EXPECT_EQ(gram.ppr_at(1).name(), "ppr1"_str);
        EXPECT_EQ(gram.ppr_at(1).index(), 1);
        EXPECT_EQ(gram.ppr_at(1).id(), taul::ppr_id(1));
        EXPECT_EQ(gram.ppr_at(1).qualifier(), taul::qualifier::none);
    }
    else ADD_FAILURE();

    EXPECT_THROW(gram.lpr_at(2), std::out_of_range);
    EXPECT_THROW(gram.ppr_at(2), std::out_of_range);

    EXPECT_EQ(gram.lpr("missing"_str), std::nullopt);
    EXPECT_EQ(gram.ppr("missing"_str), std::nullopt);

    if (gram.has_lpr("lpr0"_str)) {
        EXPECT_EQ(gram.lpr_at(0), gram.lpr("lpr0"_str).value());
    }
    else ADD_FAILURE();
    if (gram.has_lpr("lpr1"_str)) {
        EXPECT_EQ(gram.lpr_at(1), gram.lpr("lpr1"_str).value());
    }
    else ADD_FAILURE();
    if (gram.has_ppr("ppr0"_str)) {
        EXPECT_EQ(gram.ppr_at(0), gram.ppr("ppr0"_str).value());
    }
    else ADD_FAILURE();
    if (gram.has_ppr("ppr1"_str)) {
        EXPECT_EQ(gram.ppr_at(1), gram.ppr("ppr1"_str).value());
    }
    else ADD_FAILURE();

    EXPECT_TRUE(gram.has_rule("lpr0"_str));
    EXPECT_TRUE(gram.has_rule("lpr1"_str));
    EXPECT_TRUE(gram.has_rule("ppr0"_str));
    EXPECT_TRUE(gram.has_rule("ppr1"_str));
    EXPECT_FALSE(gram.has_rule("missing"_str));

    EXPECT_TRUE(gram.has_lpr("lpr0"_str));
    EXPECT_TRUE(gram.has_lpr("lpr1"_str));
    EXPECT_FALSE(gram.has_lpr("ppr0"_str));
    EXPECT_FALSE(gram.has_lpr("ppr1"_str));
    EXPECT_FALSE(gram.has_lpr("missing"_str));

    EXPECT_FALSE(gram.has_ppr("lpr0"_str));
    EXPECT_FALSE(gram.has_ppr("lpr1"_str));
    EXPECT_TRUE(gram.has_ppr("ppr0"_str));
    EXPECT_TRUE(gram.has_ppr("ppr1"_str));
    EXPECT_FALSE(gram.has_ppr("missing"_str));
}

TEST(GrammarTests, CopyAssign) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str, taul::qualifier::skip)
        .close()
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .close()
        .done();
    const auto gram0 = taul::load(spec, lgr);
    ASSERT_TRUE(gram0);
    taul::grammar gram{};
    gram = *gram0; // copy assign

    TAUL_LOG(lgr, "{}", gram);

    EXPECT_EQ(gram.lprs(), 2);
    EXPECT_EQ(gram.pprs(), 2);

    if (gram.lprs() == 2) {
        EXPECT_EQ(gram.lpr_at(0).name(), "lpr0"_str);
        EXPECT_EQ(gram.lpr_at(0).index(), 0);
        EXPECT_EQ(gram.lpr_at(0).qualifier(), taul::qualifier::none);
        EXPECT_EQ(gram.lpr_at(0).id(), taul::lpr_id(0));
        EXPECT_EQ(gram.lpr_at(1).name(), "lpr1"_str);
        EXPECT_EQ(gram.lpr_at(1).index(), 1);
        EXPECT_EQ(gram.lpr_at(1).qualifier(), taul::qualifier::skip);
        EXPECT_EQ(gram.lpr_at(1).id(), taul::lpr_id(1));
    }
    else ADD_FAILURE();

    if (gram.pprs() == 2) {
        EXPECT_EQ(gram.ppr_at(0).name(), "ppr0"_str);
        EXPECT_EQ(gram.ppr_at(0).index(), 0);
        EXPECT_EQ(gram.ppr_at(0).id(), taul::ppr_id(0));
        EXPECT_EQ(gram.ppr_at(0).qualifier(), taul::qualifier::none);
        EXPECT_EQ(gram.ppr_at(1).name(), "ppr1"_str);
        EXPECT_EQ(gram.ppr_at(1).index(), 1);
        EXPECT_EQ(gram.ppr_at(1).id(), taul::ppr_id(1));
        EXPECT_EQ(gram.ppr_at(1).qualifier(), taul::qualifier::none);
    }
    else ADD_FAILURE();

    EXPECT_THROW(gram.lpr_at(2), std::out_of_range);
    EXPECT_THROW(gram.ppr_at(2), std::out_of_range);

    EXPECT_EQ(gram.lpr("missing"_str), std::nullopt);
    EXPECT_EQ(gram.ppr("missing"_str), std::nullopt);

    if (gram.has_lpr("lpr0"_str)) {
        EXPECT_EQ(gram.lpr_at(0), gram.lpr("lpr0"_str).value());
    }
    else ADD_FAILURE();
    if (gram.has_lpr("lpr1"_str)) {
        EXPECT_EQ(gram.lpr_at(1), gram.lpr("lpr1"_str).value());
    }
    else ADD_FAILURE();
    if (gram.has_ppr("ppr0"_str)) {
        EXPECT_EQ(gram.ppr_at(0), gram.ppr("ppr0"_str).value());
    }
    else ADD_FAILURE();
    if (gram.has_ppr("ppr1"_str)) {
        EXPECT_EQ(gram.ppr_at(1), gram.ppr("ppr1"_str).value());
    }
    else ADD_FAILURE();

    EXPECT_TRUE(gram.has_rule("lpr0"_str));
    EXPECT_TRUE(gram.has_rule("lpr1"_str));
    EXPECT_TRUE(gram.has_rule("ppr0"_str));
    EXPECT_TRUE(gram.has_rule("ppr1"_str));
    EXPECT_FALSE(gram.has_rule("missing"_str));

    EXPECT_TRUE(gram.has_lpr("lpr0"_str));
    EXPECT_TRUE(gram.has_lpr("lpr1"_str));
    EXPECT_FALSE(gram.has_lpr("ppr0"_str));
    EXPECT_FALSE(gram.has_lpr("ppr1"_str));
    EXPECT_FALSE(gram.has_lpr("missing"_str));

    EXPECT_FALSE(gram.has_ppr("lpr0"_str));
    EXPECT_FALSE(gram.has_ppr("lpr1"_str));
    EXPECT_TRUE(gram.has_ppr("ppr0"_str));
    EXPECT_TRUE(gram.has_ppr("ppr1"_str));
    EXPECT_FALSE(gram.has_ppr("missing"_str));
}

TEST(GrammarTests, MoveAssign) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str, taul::qualifier::skip)
        .close()
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .close()
        .done();
    auto gram0 = taul::load(spec, lgr);
    ASSERT_TRUE(gram0);
    taul::grammar gram{};
    gram = std::move(*gram0); // move assign

    TAUL_LOG(lgr, "{}", gram);

    EXPECT_EQ(gram.lprs(), 2);
    EXPECT_EQ(gram.pprs(), 2);

    if (gram.lprs() == 2) {
        EXPECT_EQ(gram.lpr_at(0).name(), "lpr0"_str);
        EXPECT_EQ(gram.lpr_at(0).index(), 0);
        EXPECT_EQ(gram.lpr_at(0).qualifier(), taul::qualifier::none);
        EXPECT_EQ(gram.lpr_at(0).id(), taul::lpr_id(0));
        EXPECT_EQ(gram.lpr_at(1).name(), "lpr1"_str);
        EXPECT_EQ(gram.lpr_at(1).index(), 1);
        EXPECT_EQ(gram.lpr_at(1).qualifier(), taul::qualifier::skip);
        EXPECT_EQ(gram.lpr_at(1).id(), taul::lpr_id(1));
    }
    else ADD_FAILURE();

    if (gram.pprs() == 2) {
        EXPECT_EQ(gram.ppr_at(0).name(), "ppr0"_str);
        EXPECT_EQ(gram.ppr_at(0).index(), 0);
        EXPECT_EQ(gram.ppr_at(0).id(), taul::ppr_id(0));
        EXPECT_EQ(gram.ppr_at(0).qualifier(), taul::qualifier::none);
        EXPECT_EQ(gram.ppr_at(1).name(), "ppr1"_str);
        EXPECT_EQ(gram.ppr_at(1).index(), 1);
        EXPECT_EQ(gram.ppr_at(1).id(), taul::ppr_id(1));
        EXPECT_EQ(gram.ppr_at(1).qualifier(), taul::qualifier::none);
    }
    else ADD_FAILURE();

    EXPECT_THROW(gram.lpr_at(2), std::out_of_range);
    EXPECT_THROW(gram.ppr_at(2), std::out_of_range);

    EXPECT_EQ(gram.lpr("missing"_str), std::nullopt);
    EXPECT_EQ(gram.ppr("missing"_str), std::nullopt);

    if (gram.has_lpr("lpr0"_str)) {
        EXPECT_EQ(gram.lpr_at(0), gram.lpr("lpr0"_str).value());
    }
    else ADD_FAILURE();
    if (gram.has_lpr("lpr1"_str)) {
        EXPECT_EQ(gram.lpr_at(1), gram.lpr("lpr1"_str).value());
    }
    else ADD_FAILURE();
    if (gram.has_ppr("ppr0"_str)) {
        EXPECT_EQ(gram.ppr_at(0), gram.ppr("ppr0"_str).value());
    }
    else ADD_FAILURE();
    if (gram.has_ppr("ppr1"_str)) {
        EXPECT_EQ(gram.ppr_at(1), gram.ppr("ppr1"_str).value());
    }
    else ADD_FAILURE();

    EXPECT_TRUE(gram.has_rule("lpr0"_str));
    EXPECT_TRUE(gram.has_rule("lpr1"_str));
    EXPECT_TRUE(gram.has_rule("ppr0"_str));
    EXPECT_TRUE(gram.has_rule("ppr1"_str));
    EXPECT_FALSE(gram.has_rule("missing"_str));

    EXPECT_TRUE(gram.has_lpr("lpr0"_str));
    EXPECT_TRUE(gram.has_lpr("lpr1"_str));
    EXPECT_FALSE(gram.has_lpr("ppr0"_str));
    EXPECT_FALSE(gram.has_lpr("ppr1"_str));
    EXPECT_FALSE(gram.has_lpr("missing"_str));

    EXPECT_FALSE(gram.has_ppr("lpr0"_str));
    EXPECT_FALSE(gram.has_ppr("lpr1"_str));
    EXPECT_TRUE(gram.has_ppr("ppr0"_str));
    EXPECT_TRUE(gram.has_ppr("ppr1"_str));
    EXPECT_FALSE(gram.has_ppr("missing"_str));
}

TEST(GrammarTests, GrammarInitViaSpec) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .ppr_decl("ppr0"_str)
        .ppr_decl("ppr1"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str, taul::qualifier::skip)
        .close()
        .ppr("ppr0"_str)
        .close()
        .ppr("ppr1"_str)
        .close()
        .done();
    const auto gram = taul::load(spec, lgr);
    ASSERT_TRUE(gram);

    TAUL_LOG(lgr, "{}", *gram);

    EXPECT_EQ(gram->lprs(), 2);
    EXPECT_EQ(gram->pprs(), 2);

    if (gram->lprs() == 2) {
        EXPECT_EQ(gram->lpr_at(0).name(), "lpr0"_str);
        EXPECT_EQ(gram->lpr_at(0).index(), 0);
        EXPECT_EQ(gram->lpr_at(0).qualifier(), taul::qualifier::none);
        EXPECT_EQ(gram->lpr_at(0).id(), taul::lpr_id(0));
        EXPECT_EQ(gram->lpr_at(1).name(), "lpr1"_str);
        EXPECT_EQ(gram->lpr_at(1).index(), 1);
        EXPECT_EQ(gram->lpr_at(1).qualifier(), taul::qualifier::skip);
        EXPECT_EQ(gram->lpr_at(1).id(), taul::lpr_id(1));
    }
    else ADD_FAILURE();

    if (gram->pprs() == 2) {
        EXPECT_EQ(gram->ppr_at(0).name(), "ppr0"_str);
        EXPECT_EQ(gram->ppr_at(0).index(), 0);
        EXPECT_EQ(gram->ppr_at(0).id(), taul::ppr_id(0));
        EXPECT_EQ(gram->ppr_at(0).qualifier(), taul::qualifier::none);
        EXPECT_EQ(gram->ppr_at(1).name(), "ppr1"_str);
        EXPECT_EQ(gram->ppr_at(1).index(), 1);
        EXPECT_EQ(gram->ppr_at(1).id(), taul::ppr_id(1));
        EXPECT_EQ(gram->ppr_at(1).qualifier(), taul::qualifier::none);
    }
    else ADD_FAILURE();

    EXPECT_THROW(gram->lpr_at(2), std::out_of_range);
    EXPECT_THROW(gram->ppr_at(2), std::out_of_range);

    EXPECT_EQ(gram->lpr("missing"_str), std::nullopt);
    EXPECT_EQ(gram->ppr("missing"_str), std::nullopt);

    if (gram->has_lpr("lpr0"_str)) {
        EXPECT_EQ(gram->lpr_at(0), gram->lpr("lpr0"_str).value());
    }
    else ADD_FAILURE();
    if (gram->has_lpr("lpr1"_str)) {
        EXPECT_EQ(gram->lpr_at(1), gram->lpr("lpr1"_str).value());
    }
    else ADD_FAILURE();
    if (gram->has_ppr("ppr0"_str)) {
        EXPECT_EQ(gram->ppr_at(0), gram->ppr("ppr0"_str).value());
    }
    else ADD_FAILURE();
    if (gram->has_ppr("ppr1"_str)) {
        EXPECT_EQ(gram->ppr_at(1), gram->ppr("ppr1"_str).value());
    }
    else ADD_FAILURE();

    EXPECT_TRUE(gram->has_rule("lpr0"_str));
    EXPECT_TRUE(gram->has_rule("lpr1"_str));
    EXPECT_TRUE(gram->has_rule("ppr0"_str));
    EXPECT_TRUE(gram->has_rule("ppr1"_str));
    EXPECT_FALSE(gram->has_rule("missing"_str));

    EXPECT_TRUE(gram->has_lpr("lpr0"_str));
    EXPECT_TRUE(gram->has_lpr("lpr1"_str));
    EXPECT_FALSE(gram->has_lpr("ppr0"_str));
    EXPECT_FALSE(gram->has_lpr("ppr1"_str));
    EXPECT_FALSE(gram->has_lpr("missing"_str));

    EXPECT_FALSE(gram->has_ppr("lpr0"_str));
    EXPECT_FALSE(gram->has_ppr("lpr1"_str));
    EXPECT_TRUE(gram->has_ppr("ppr0"_str));
    EXPECT_TRUE(gram->has_ppr("ppr1"_str));
    EXPECT_FALSE(gram->has_ppr("missing"_str));
}

TEST(GrammarTests, NonSupportLPRs) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .lpr_decl("lpr1"_str)
        .lpr_decl("lpr2"_str)
        .lpr_decl("lpr3"_str)
        .lpr_decl("lpr4"_str)
        .lpr_decl("lpr5"_str)
        .lpr_decl("lpr6"_str)
        .lpr_decl("lpr7"_str)
        .lpr_decl("lpr8"_str)
        .lpr("lpr0"_str)
        .close()
        .lpr("lpr1"_str)
        .close()
        .lpr("lpr2"_str)
        .close()
        .lpr("lpr3"_str, taul::qualifier::skip)
        .close()
        .lpr("lpr4"_str, taul::qualifier::skip)
        .close()
        .lpr("lpr5"_str, taul::qualifier::support)
        .close()
        .lpr("lpr6"_str, taul::qualifier::support)
        .close()
        .lpr("lpr7"_str, taul::qualifier::support)
        .close()
        .lpr("lpr8"_str, taul::qualifier::support)
        .close()
        .done();
    const auto gram = taul::load(spec, lgr);
    ASSERT_TRUE(gram);

    TAUL_LOG(lgr, "{}", *gram);

    EXPECT_EQ(gram->nonsupport_lprs(), 5);
}

TEST(GrammarTests, IsAssociated) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0"_str)
        .ppr_decl("ppr0"_str)
        .lpr("lpr0"_str)
        .close()
        .ppr("ppr0"_str)
        .close()
        .done();
    const auto gram0a = taul::load(spec, lgr);
    const auto gram0b = gram0a;
    const auto gram1 = taul::load(spec, lgr);
    ASSERT_TRUE(gram0a);
    ASSERT_TRUE(gram0b);
    ASSERT_TRUE(gram1);

    TAUL_LOG(lgr, "{}", *gram0a);

    EXPECT_TRUE(gram0a->is_associated(gram0a->lpr_at(0)));
    EXPECT_TRUE(gram0b->is_associated(gram0a->lpr_at(0)));
    EXPECT_FALSE(gram1->is_associated(gram0a->lpr_at(0)));

    EXPECT_TRUE(gram0a->is_associated(gram0b->lpr_at(0)));
    EXPECT_TRUE(gram0b->is_associated(gram0b->lpr_at(0)));
    EXPECT_FALSE(gram1->is_associated(gram0b->lpr_at(0)));

    EXPECT_FALSE(gram0a->is_associated(gram1->lpr_at(0)));
    EXPECT_FALSE(gram0b->is_associated(gram1->lpr_at(0)));
    EXPECT_TRUE(gram1->is_associated(gram1->lpr_at(0)));

    EXPECT_TRUE(gram0a->is_associated(gram0a->ppr_at(0)));
    EXPECT_TRUE(gram0b->is_associated(gram0a->ppr_at(0)));
    EXPECT_FALSE(gram1->is_associated(gram0a->ppr_at(0)));

    EXPECT_TRUE(gram0a->is_associated(gram0b->ppr_at(0)));
    EXPECT_TRUE(gram0b->is_associated(gram0b->ppr_at(0)));
    EXPECT_FALSE(gram1->is_associated(gram0b->ppr_at(0)));

    EXPECT_FALSE(gram0a->is_associated(gram1->ppr_at(0)));
    EXPECT_FALSE(gram0b->is_associated(gram1->ppr_at(0)));
    EXPECT_TRUE(gram1->is_associated(gram1->ppr_at(0)));
}

TEST(GrammarTests, Serialization) {
    const auto lgr = taul::make_stderr_logger();
    const auto spec =
        taul::spec_writer()
        .lpr_decl("PLUS"_str)
        .lpr_decl("A"_str)
        .lpr_decl("B"_str)
        .lpr_decl("WS"_str)
        .ppr_decl("Number"_str)
        .ppr_decl("Expr"_str)
        .lpr("PLUS"_str)
        .string("+")
        .close()
        .lpr("A"_str)
        .string("a")
        .close()
        .lpr("B"_str)
        .string("b")
        .close()
        .lpr("WS"_str, taul::skip)
        .charset(" \\t")
        .close()
        .ppr("Number"_str)
        .name("A"_str)
        .alternative()
        .name("B"_str)
        .close()
        .ppr("Expr"_str, taul::precedence)
        .name("Expr"_str)
        .name("PLUS"_str)
        .name("Expr"_str)
        .alternative()
        .name("Number"_str)
        .close()
        .done();
    const auto gram0 = taul::load(spec, lgr);
    ASSERT_TRUE(gram0);

    //TAUL_LOG(lgr, "{}", *gram0);

    ASSERT_TRUE(gram0->has_lpr("PLUS"_str));
    ASSERT_TRUE(gram0->has_lpr("A"_str));
    ASSERT_TRUE(gram0->has_lpr("B"_str));
    ASSERT_TRUE(gram0->has_lpr("WS"_str));
    ASSERT_TRUE(gram0->has_ppr("Number"_str));
    ASSERT_TRUE(gram0->has_ppr("Expr"_str));

    // perform our serialize/deserialize of gram0 to get gram1

    const auto serialized = gram0->serialize();

    std::cerr << std::format("serialized == {}\n", serialized);

    const auto gram1 = taul::grammar::deserialize(serialized);
    ASSERT_TRUE(gram1);

    TAUL_LOG(lgr, "{}", *gram1);

    ASSERT_TRUE(gram1->has_lpr("PLUS"_str));
    ASSERT_TRUE(gram1->has_lpr("A"_str));
    ASSERT_TRUE(gram1->has_lpr("B"_str));
    ASSERT_TRUE(gram1->has_lpr("WS"_str));
    ASSERT_TRUE(gram1->has_ppr("Number"_str));
    ASSERT_TRUE(gram1->has_ppr("Expr"_str));

    // test that deserialized grammar behaves as expected

    const auto input = "a + b + a + a + b"_str;

    taul::source_reader rdr(input);
    taul::lexer lxr(*gram1);
    taul::parser psr(*gram1);
    taul::no_recovery_error_handler eh(lgr);
    lxr.bind_source(&rdr);
    psr.bind_source(&lxr);
    psr.bind_error_handler(&eh);
    psr.reset();

    taul::source_pos_counter cntr{};
    const auto expected =
        taul::parse_tree_pattern(*gram1)
        .syntactic("Expr"_str, cntr())
        .syntactic("Number"_str, cntr())
        .lexical("A"_str, cntr(1, 1), 1)
        .close()
        .lexical("PLUS"_str, cntr(1, 1), 1)
        .syntactic("Expr"_str, cntr())
        .syntactic("Number"_str, cntr())
        .lexical("B"_str, cntr(1, 1), 1)
        .close()
        .close()
        .lexical("PLUS"_str, cntr(1, 1), 1)
        .syntactic("Expr"_str, cntr())
        .syntactic("Number"_str, cntr())
        .lexical("A"_str, cntr(1, 1), 1)
        .close()
        .close()
        .lexical("PLUS"_str, cntr(1, 1), 1)
        .syntactic("Expr"_str, cntr())
        .syntactic("Number"_str, cntr())
        .lexical("A"_str, cntr(1, 1), 1)
        .close()
        .close()
        .lexical("PLUS"_str, cntr(1, 1), 1)
        .syntactic("Expr"_str, cntr())
        .syntactic("Number"_str, cntr())
        .lexical("B"_str, cntr(1), 1)
        .close()
        .close()
        .close();

    auto actual = psr.parse("Expr"_str);

    EXPECT_TRUE(expected.match(actual));
}

