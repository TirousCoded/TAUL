

#include <gtest/gtest.h>

#include <taul/logger.h>
#include <taul/str.h>
#include <taul/spec.h>
#include <taul/load.h>


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

    EXPECT_TRUE(taul::lpr_ref::equal(gram0->lpr_at(0), gram0->lpr_at(0)));
    EXPECT_TRUE(taul::lpr_ref::equal(gram0->lpr_at(0), gram1->lpr_at(0)));
    EXPECT_TRUE(taul::lpr_ref::equal(gram0->lpr_at(1), gram0->lpr_at(1)));
    EXPECT_TRUE(taul::lpr_ref::equal(gram0->lpr_at(1), gram1->lpr_at(1)));
    EXPECT_TRUE(taul::lpr_ref::equal(*gram0->lpr("lpr0"_str), *gram0->lpr("lpr0"_str)));
    EXPECT_TRUE(taul::lpr_ref::equal(*gram0->lpr("lpr0"_str), *gram1->lpr("lpr0"_str)));
    EXPECT_TRUE(taul::lpr_ref::equal(*gram0->lpr("lpr1"_str), *gram0->lpr("lpr1"_str)));
    EXPECT_TRUE(taul::lpr_ref::equal(*gram0->lpr("lpr1"_str), *gram1->lpr("lpr1"_str)));
    EXPECT_TRUE(taul::lpr_ref::equal(gram0->lpr_at(0), *gram0->lpr("lpr0"_str)));
    EXPECT_TRUE(taul::lpr_ref::equal(gram0->lpr_at(0), *gram1->lpr("lpr0"_str)));
    EXPECT_TRUE(taul::lpr_ref::equal(gram0->lpr_at(1), *gram0->lpr("lpr1"_str)));
    EXPECT_TRUE(taul::lpr_ref::equal(gram0->lpr_at(1), *gram1->lpr("lpr1"_str)));
    
    EXPECT_FALSE(taul::lpr_ref::equal(gram0->lpr_at(0), gram0->lpr_at(1)));
    EXPECT_FALSE(taul::lpr_ref::equal(gram0->lpr_at(0), gram1->lpr_at(1)));
    EXPECT_FALSE(taul::lpr_ref::equal(*gram0->lpr("lpr0"_str), *gram0->lpr("lpr1"_str)));
    EXPECT_FALSE(taul::lpr_ref::equal(*gram0->lpr("lpr0"_str), *gram1->lpr("lpr1"_str)));
    EXPECT_FALSE(taul::lpr_ref::equal(gram0->lpr_at(0), *gram0->lpr("lpr1"_str)));
    EXPECT_FALSE(taul::lpr_ref::equal(gram0->lpr_at(0), *gram1->lpr("lpr1"_str)));

    EXPECT_FALSE(taul::lpr_ref::equal(gram0->lpr_at(0), gram2->lpr_at(0)));
    EXPECT_FALSE(taul::lpr_ref::equal(gram0->lpr_at(1), gram2->lpr_at(1)));
    EXPECT_FALSE(taul::lpr_ref::equal(*gram0->lpr("lpr0"_str), *gram2->lpr("lpr0"_str)));
    EXPECT_FALSE(taul::lpr_ref::equal(*gram0->lpr("lpr1"_str), *gram2->lpr("lpr1"_str)));

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

    EXPECT_TRUE(taul::ppr_ref::equal(gram0->ppr_at(0), gram0->ppr_at(0)));
    EXPECT_TRUE(taul::ppr_ref::equal(gram0->ppr_at(0), gram1->ppr_at(0)));
    EXPECT_TRUE(taul::ppr_ref::equal(gram0->ppr_at(1), gram0->ppr_at(1)));
    EXPECT_TRUE(taul::ppr_ref::equal(gram0->ppr_at(1), gram1->ppr_at(1)));
    EXPECT_TRUE(taul::ppr_ref::equal(*gram0->ppr("ppr0"_str), *gram0->ppr("ppr0"_str)));
    EXPECT_TRUE(taul::ppr_ref::equal(*gram0->ppr("ppr0"_str), *gram1->ppr("ppr0"_str)));
    EXPECT_TRUE(taul::ppr_ref::equal(*gram0->ppr("ppr1"_str), *gram0->ppr("ppr1"_str)));
    EXPECT_TRUE(taul::ppr_ref::equal(*gram0->ppr("ppr1"_str), *gram1->ppr("ppr1"_str)));
    EXPECT_TRUE(taul::ppr_ref::equal(gram0->ppr_at(0), *gram0->ppr("ppr0"_str)));
    EXPECT_TRUE(taul::ppr_ref::equal(gram0->ppr_at(0), *gram1->ppr("ppr0"_str)));
    EXPECT_TRUE(taul::ppr_ref::equal(gram0->ppr_at(1), *gram0->ppr("ppr1"_str)));
    EXPECT_TRUE(taul::ppr_ref::equal(gram0->ppr_at(1), *gram1->ppr("ppr1"_str)));

    EXPECT_FALSE(taul::ppr_ref::equal(gram0->ppr_at(0), gram0->ppr_at(1)));
    EXPECT_FALSE(taul::ppr_ref::equal(gram0->ppr_at(0), gram1->ppr_at(1)));
    EXPECT_FALSE(taul::ppr_ref::equal(*gram0->ppr("ppr0"_str), *gram0->ppr("ppr1"_str)));
    EXPECT_FALSE(taul::ppr_ref::equal(*gram0->ppr("ppr0"_str), *gram1->ppr("ppr1"_str)));
    EXPECT_FALSE(taul::ppr_ref::equal(gram0->ppr_at(0), *gram0->ppr("ppr1"_str)));
    EXPECT_FALSE(taul::ppr_ref::equal(gram0->ppr_at(0), *gram1->ppr("ppr1"_str)));

    EXPECT_FALSE(taul::ppr_ref::equal(gram0->ppr_at(0), gram2->ppr_at(0)));
    EXPECT_FALSE(taul::ppr_ref::equal(gram0->ppr_at(1), gram2->ppr_at(1)));
    EXPECT_FALSE(taul::ppr_ref::equal(*gram0->ppr("ppr0"_str), *gram2->ppr("ppr0"_str)));
    EXPECT_FALSE(taul::ppr_ref::equal(*gram0->ppr("ppr1"_str), *gram2->ppr("ppr1"_str)));

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
        EXPECT_EQ(gram.lpr_at(1).name(), "lpr1"_str);
        EXPECT_EQ(gram.lpr_at(1).index(), 1);
        EXPECT_EQ(gram.lpr_at(1).qualifier(), taul::qualifier::skip);
    }
    else ADD_FAILURE();

    if (gram.pprs() == 2) {
        EXPECT_EQ(gram.ppr_at(0).name(), "ppr0"_str);
        EXPECT_EQ(gram.ppr_at(0).index(), 0);
        EXPECT_EQ(gram.ppr_at(1).name(), "ppr1"_str);
        EXPECT_EQ(gram.ppr_at(1).index(), 1);
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
        EXPECT_EQ(gram.lpr_at(1).name(), "lpr1"_str);
        EXPECT_EQ(gram.lpr_at(1).index(), 1);
        EXPECT_EQ(gram.lpr_at(1).qualifier(), taul::qualifier::skip);
    }
    else ADD_FAILURE();

    if (gram.pprs() == 2) {
        EXPECT_EQ(gram.ppr_at(0).name(), "ppr0"_str);
        EXPECT_EQ(gram.ppr_at(0).index(), 0);
        EXPECT_EQ(gram.ppr_at(1).name(), "ppr1"_str);
        EXPECT_EQ(gram.ppr_at(1).index(), 1);
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
        EXPECT_EQ(gram.lpr_at(1).name(), "lpr1"_str);
        EXPECT_EQ(gram.lpr_at(1).index(), 1);
        EXPECT_EQ(gram.lpr_at(1).qualifier(), taul::qualifier::skip);
    }
    else ADD_FAILURE();

    if (gram.pprs() == 2) {
        EXPECT_EQ(gram.ppr_at(0).name(), "ppr0"_str);
        EXPECT_EQ(gram.ppr_at(0).index(), 0);
        EXPECT_EQ(gram.ppr_at(1).name(), "ppr1"_str);
        EXPECT_EQ(gram.ppr_at(1).index(), 1);
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
        EXPECT_EQ(gram.lpr_at(1).name(), "lpr1"_str);
        EXPECT_EQ(gram.lpr_at(1).index(), 1);
        EXPECT_EQ(gram.lpr_at(1).qualifier(), taul::qualifier::skip);
    }
    else ADD_FAILURE();

    if (gram.pprs() == 2) {
        EXPECT_EQ(gram.ppr_at(0).name(), "ppr0"_str);
        EXPECT_EQ(gram.ppr_at(0).index(), 0);
        EXPECT_EQ(gram.ppr_at(1).name(), "ppr1"_str);
        EXPECT_EQ(gram.ppr_at(1).index(), 1);
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
        EXPECT_EQ(gram->lpr_at(1).name(), "lpr1"_str);
        EXPECT_EQ(gram->lpr_at(1).index(), 1);
        EXPECT_EQ(gram->lpr_at(1).qualifier(), taul::qualifier::skip);
    }
    else ADD_FAILURE();

    if (gram->pprs() == 2) {
        EXPECT_EQ(gram->ppr_at(0).name(), "ppr0"_str);
        EXPECT_EQ(gram->ppr_at(0).index(), 0);
        EXPECT_EQ(gram->ppr_at(1).name(), "ppr1"_str);
        EXPECT_EQ(gram->ppr_at(1).index(), 1);
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

TEST(GrammarTests, nonsupport_lprs) {
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

