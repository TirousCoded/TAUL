

#include <gtest/gtest.h>

#include <taul/all.h>


TEST(grammar_tests, defaultCtor) {

    const auto lgr = taul::make_stderr_logger();

    taul::grammar gram{};


    TAUL_LOG(lgr, "{}", gram);

    EXPECT_TRUE(gram.lprs().empty());
    EXPECT_TRUE(gram.pprs().empty());

    EXPECT_THROW(gram.lpr(std::string("missing")), taul::lpr_not_found_error);
    EXPECT_THROW(gram.lpr(std::string_view("missing")), taul::lpr_not_found_error);
    EXPECT_THROW(gram.lpr("missing"), taul::lpr_not_found_error);

    EXPECT_THROW(gram.ppr(std::string("missing")), taul::ppr_not_found_error);
    EXPECT_THROW(gram.ppr(std::string_view("missing")), taul::ppr_not_found_error);
    EXPECT_THROW(gram.ppr("missing"), taul::ppr_not_found_error);

    EXPECT_FALSE(gram.contains(std::string("missing")));
    EXPECT_FALSE(gram.contains(std::string_view("missing")));
    EXPECT_FALSE(gram.contains("missing"));
    
    EXPECT_FALSE(gram.contains_lpr(std::string("missing")));
    EXPECT_FALSE(gram.contains_lpr(std::string_view("missing")));
    EXPECT_FALSE(gram.contains_lpr("missing"));
    
    EXPECT_FALSE(gram.contains_ppr(std::string("missing")));
    EXPECT_FALSE(gram.contains_ppr(std::string_view("missing")));
    EXPECT_FALSE(gram.contains_ppr("missing"));
}

TEST(grammar_tests, copyCtor) {

    const auto lgr = taul::make_stderr_logger();

    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .close()
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .close()
        .done();

    const auto gram0 = taul::load(spec, lgr);

    ASSERT_TRUE(gram0);

    taul::grammar gram(*gram0); // copy ctor


    {
        ASSERT_TRUE(gram0);

        TAUL_LOG(lgr, "{}", *gram0);

        if (gram0->lprs().size() == 2) {
            EXPECT_EQ(gram0->lprs()[0].name, "lpr0");
            EXPECT_EQ(gram0->lprs()[0].index, 0);
            EXPECT_EQ(gram0->lprs()[1].name, "lpr1");
            EXPECT_EQ(gram0->lprs()[1].index, 1);
        }
        else ADD_FAILURE();

        if (gram0->pprs().size() == 2) {
            EXPECT_EQ(gram0->pprs()[0].name, "ppr0");
            EXPECT_EQ(gram0->pprs()[0].index, 0);
            EXPECT_EQ(gram0->pprs()[1].name, "ppr1");
            EXPECT_EQ(gram0->pprs()[1].index, 1);
        }
        else ADD_FAILURE();

        EXPECT_THROW(gram0->lpr(std::string("missing")), taul::lpr_not_found_error);
        EXPECT_THROW(gram0->lpr(std::string_view("missing")), taul::lpr_not_found_error);
        EXPECT_THROW(gram0->lpr("missing"), taul::lpr_not_found_error);

        EXPECT_THROW(gram0->ppr(std::string("missing")), taul::ppr_not_found_error);
        EXPECT_THROW(gram0->ppr(std::string_view("missing")), taul::ppr_not_found_error);
        EXPECT_THROW(gram0->ppr("missing"), taul::ppr_not_found_error);

        const auto& _lpr0_a = gram0->lpr(std::string("lpr0"));
        const auto& _lpr0_b = gram0->lpr(std::string_view("lpr0"));
        const auto& _lpr0_c = gram0->lpr("lpr0");
        const auto& _lpr1_a = gram0->lpr(std::string("lpr1"));
        const auto& _lpr1_b = gram0->lpr(std::string_view("lpr1"));
        const auto& _lpr1_c = gram0->lpr("lpr1");

        const auto& _ppr0_a = gram0->ppr(std::string("ppr0"));
        const auto& _ppr0_b = gram0->ppr(std::string_view("ppr0"));
        const auto& _ppr0_c = gram0->ppr("ppr0");
        const auto& _ppr1_a = gram0->ppr(std::string("ppr1"));
        const auto& _ppr1_b = gram0->ppr(std::string_view("ppr1"));
        const auto& _ppr1_c = gram0->ppr("ppr1");

        EXPECT_EQ(&_lpr0_a, &(gram0->lprs()[0]));
        EXPECT_EQ(&_lpr0_b, &(gram0->lprs()[0]));
        EXPECT_EQ(&_lpr0_c, &(gram0->lprs()[0]));
        EXPECT_EQ(&_lpr1_a, &(gram0->lprs()[1]));
        EXPECT_EQ(&_lpr1_b, &(gram0->lprs()[1]));
        EXPECT_EQ(&_lpr1_c, &(gram0->lprs()[1]));

        EXPECT_EQ(&_ppr0_a, &(gram0->pprs()[0]));
        EXPECT_EQ(&_ppr0_b, &(gram0->pprs()[0]));
        EXPECT_EQ(&_ppr0_c, &(gram0->pprs()[0]));
        EXPECT_EQ(&_ppr1_a, &(gram0->pprs()[1]));
        EXPECT_EQ(&_ppr1_b, &(gram0->pprs()[1]));
        EXPECT_EQ(&_ppr1_c, &(gram0->pprs()[1]));

        EXPECT_TRUE(gram0->contains(std::string("lpr0")));
        EXPECT_TRUE(gram0->contains(std::string("lpr1")));
        EXPECT_TRUE(gram0->contains(std::string("ppr0")));
        EXPECT_TRUE(gram0->contains(std::string("ppr1")));
        EXPECT_TRUE(gram0->contains(std::string_view("lpr0")));
        EXPECT_TRUE(gram0->contains(std::string_view("lpr1")));
        EXPECT_TRUE(gram0->contains(std::string_view("ppr0")));
        EXPECT_TRUE(gram0->contains(std::string_view("ppr1")));
        EXPECT_TRUE(gram0->contains("lpr0"));
        EXPECT_TRUE(gram0->contains("lpr1"));
        EXPECT_TRUE(gram0->contains("ppr0"));
        EXPECT_TRUE(gram0->contains("ppr1"));
        EXPECT_FALSE(gram0->contains("missing"));

        EXPECT_TRUE(gram0->contains_lpr(std::string("lpr0")));
        EXPECT_TRUE(gram0->contains_lpr(std::string("lpr1")));
        EXPECT_FALSE(gram0->contains_lpr(std::string("ppr0")));
        EXPECT_FALSE(gram0->contains_lpr(std::string("ppr1")));
        EXPECT_TRUE(gram0->contains_lpr(std::string_view("lpr0")));
        EXPECT_TRUE(gram0->contains_lpr(std::string_view("lpr1")));
        EXPECT_FALSE(gram0->contains_lpr(std::string_view("ppr0")));
        EXPECT_FALSE(gram0->contains_lpr(std::string_view("ppr1")));
        EXPECT_TRUE(gram0->contains_lpr("lpr0"));
        EXPECT_TRUE(gram0->contains_lpr("lpr1"));
        EXPECT_FALSE(gram0->contains_lpr("ppr0"));
        EXPECT_FALSE(gram0->contains_lpr("ppr1"));
        EXPECT_FALSE(gram0->contains_lpr("missing"));

        EXPECT_FALSE(gram0->contains_ppr(std::string("lpr0")));
        EXPECT_FALSE(gram0->contains_ppr(std::string("lpr1")));
        EXPECT_TRUE(gram0->contains_ppr(std::string("ppr0")));
        EXPECT_TRUE(gram0->contains_ppr(std::string("ppr1")));
        EXPECT_FALSE(gram0->contains_ppr(std::string_view("lpr0")));
        EXPECT_FALSE(gram0->contains_ppr(std::string_view("lpr1")));
        EXPECT_TRUE(gram0->contains_ppr(std::string_view("ppr0")));
        EXPECT_TRUE(gram0->contains_ppr(std::string_view("ppr1")));
        EXPECT_FALSE(gram0->contains_ppr("lpr0"));
        EXPECT_FALSE(gram0->contains_ppr("lpr1"));
        EXPECT_TRUE(gram0->contains_ppr("ppr0"));
        EXPECT_TRUE(gram0->contains_ppr("ppr1"));
        EXPECT_FALSE(gram0->contains_ppr("missing"));
    }

    {
        TAUL_LOG(lgr, "{}", gram);

        if (gram.lprs().size() == 2) {
            EXPECT_EQ(gram.lprs()[0].name, "lpr0");
            EXPECT_EQ(gram.lprs()[0].index, 0);
            EXPECT_EQ(gram.lprs()[1].name, "lpr1");
            EXPECT_EQ(gram.lprs()[1].index, 1);
        }
        else ADD_FAILURE();

        if (gram.pprs().size() == 2) {
            EXPECT_EQ(gram.pprs()[0].name, "ppr0");
            EXPECT_EQ(gram.pprs()[0].index, 0);
            EXPECT_EQ(gram.pprs()[1].name, "ppr1");
            EXPECT_EQ(gram.pprs()[1].index, 1);
        }
        else ADD_FAILURE();

        EXPECT_THROW(gram.lpr(std::string("missing")), taul::lpr_not_found_error);
        EXPECT_THROW(gram.lpr(std::string_view("missing")), taul::lpr_not_found_error);
        EXPECT_THROW(gram.lpr("missing"), taul::lpr_not_found_error);

        EXPECT_THROW(gram.ppr(std::string("missing")), taul::ppr_not_found_error);
        EXPECT_THROW(gram.ppr(std::string_view("missing")), taul::ppr_not_found_error);
        EXPECT_THROW(gram.ppr("missing"), taul::ppr_not_found_error);

        const auto& _lpr0_a = gram.lpr(std::string("lpr0"));
        const auto& _lpr0_b = gram.lpr(std::string_view("lpr0"));
        const auto& _lpr0_c = gram.lpr("lpr0");
        const auto& _lpr1_a = gram.lpr(std::string("lpr1"));
        const auto& _lpr1_b = gram.lpr(std::string_view("lpr1"));
        const auto& _lpr1_c = gram.lpr("lpr1");

        const auto& _ppr0_a = gram.ppr(std::string("ppr0"));
        const auto& _ppr0_b = gram.ppr(std::string_view("ppr0"));
        const auto& _ppr0_c = gram.ppr("ppr0");
        const auto& _ppr1_a = gram.ppr(std::string("ppr1"));
        const auto& _ppr1_b = gram.ppr(std::string_view("ppr1"));
        const auto& _ppr1_c = gram.ppr("ppr1");

        EXPECT_EQ(&_lpr0_a, &(gram.lprs()[0]));
        EXPECT_EQ(&_lpr0_b, &(gram.lprs()[0]));
        EXPECT_EQ(&_lpr0_c, &(gram.lprs()[0]));
        EXPECT_EQ(&_lpr1_a, &(gram.lprs()[1]));
        EXPECT_EQ(&_lpr1_b, &(gram.lprs()[1]));
        EXPECT_EQ(&_lpr1_c, &(gram.lprs()[1]));

        EXPECT_EQ(&_ppr0_a, &(gram.pprs()[0]));
        EXPECT_EQ(&_ppr0_b, &(gram.pprs()[0]));
        EXPECT_EQ(&_ppr0_c, &(gram.pprs()[0]));
        EXPECT_EQ(&_ppr1_a, &(gram.pprs()[1]));
        EXPECT_EQ(&_ppr1_b, &(gram.pprs()[1]));
        EXPECT_EQ(&_ppr1_c, &(gram.pprs()[1]));

        EXPECT_TRUE(gram.contains(std::string("lpr0")));
        EXPECT_TRUE(gram.contains(std::string("lpr1")));
        EXPECT_TRUE(gram.contains(std::string("ppr0")));
        EXPECT_TRUE(gram.contains(std::string("ppr1")));
        EXPECT_TRUE(gram.contains(std::string_view("lpr0")));
        EXPECT_TRUE(gram.contains(std::string_view("lpr1")));
        EXPECT_TRUE(gram.contains(std::string_view("ppr0")));
        EXPECT_TRUE(gram.contains(std::string_view("ppr1")));
        EXPECT_TRUE(gram.contains("lpr0"));
        EXPECT_TRUE(gram.contains("lpr1"));
        EXPECT_TRUE(gram.contains("ppr0"));
        EXPECT_TRUE(gram.contains("ppr1"));
        EXPECT_FALSE(gram.contains("missing"));

        EXPECT_TRUE(gram.contains_lpr(std::string("lpr0")));
        EXPECT_TRUE(gram.contains_lpr(std::string("lpr1")));
        EXPECT_FALSE(gram.contains_lpr(std::string("ppr0")));
        EXPECT_FALSE(gram.contains_lpr(std::string("ppr1")));
        EXPECT_TRUE(gram.contains_lpr(std::string_view("lpr0")));
        EXPECT_TRUE(gram.contains_lpr(std::string_view("lpr1")));
        EXPECT_FALSE(gram.contains_lpr(std::string_view("ppr0")));
        EXPECT_FALSE(gram.contains_lpr(std::string_view("ppr1")));
        EXPECT_TRUE(gram.contains_lpr("lpr0"));
        EXPECT_TRUE(gram.contains_lpr("lpr1"));
        EXPECT_FALSE(gram.contains_lpr("ppr0"));
        EXPECT_FALSE(gram.contains_lpr("ppr1"));
        EXPECT_FALSE(gram.contains_lpr("missing"));

        EXPECT_FALSE(gram.contains_ppr(std::string("lpr0")));
        EXPECT_FALSE(gram.contains_ppr(std::string("lpr1")));
        EXPECT_TRUE(gram.contains_ppr(std::string("ppr0")));
        EXPECT_TRUE(gram.contains_ppr(std::string("ppr1")));
        EXPECT_FALSE(gram.contains_ppr(std::string_view("lpr0")));
        EXPECT_FALSE(gram.contains_ppr(std::string_view("lpr1")));
        EXPECT_TRUE(gram.contains_ppr(std::string_view("ppr0")));
        EXPECT_TRUE(gram.contains_ppr(std::string_view("ppr1")));
        EXPECT_FALSE(gram.contains_ppr("lpr0"));
        EXPECT_FALSE(gram.contains_ppr("lpr1"));
        EXPECT_TRUE(gram.contains_ppr("ppr0"));
        EXPECT_TRUE(gram.contains_ppr("ppr1"));
        EXPECT_FALSE(gram.contains_ppr("missing"));
    }
}

TEST(grammar_tests, moveCtor) {

    const auto lgr = taul::make_stderr_logger();

    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .close()
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .close()
        .done();

    auto gram0 = taul::load(spec, lgr);

    ASSERT_TRUE(gram0);

    taul::grammar gram(std::move(*gram0)); // move ctor


    TAUL_LOG(lgr, "{}", gram);

    if (gram.lprs().size() == 2) {
        EXPECT_EQ(gram.lprs()[0].name, "lpr0");
        EXPECT_EQ(gram.lprs()[0].index, 0);
        EXPECT_EQ(gram.lprs()[1].name, "lpr1");
        EXPECT_EQ(gram.lprs()[1].index, 1);
    }
    else ADD_FAILURE();

    if (gram.pprs().size() == 2) {
        EXPECT_EQ(gram.pprs()[0].name, "ppr0");
        EXPECT_EQ(gram.pprs()[0].index, 0);
        EXPECT_EQ(gram.pprs()[1].name, "ppr1");
        EXPECT_EQ(gram.pprs()[1].index, 1);
    }
    else ADD_FAILURE();

    EXPECT_THROW(gram.lpr(std::string("missing")), taul::lpr_not_found_error);
    EXPECT_THROW(gram.lpr(std::string_view("missing")), taul::lpr_not_found_error);
    EXPECT_THROW(gram.lpr("missing"), taul::lpr_not_found_error);

    EXPECT_THROW(gram.ppr(std::string("missing")), taul::ppr_not_found_error);
    EXPECT_THROW(gram.ppr(std::string_view("missing")), taul::ppr_not_found_error);
    EXPECT_THROW(gram.ppr("missing"), taul::ppr_not_found_error);

    const auto& _lpr0_a = gram.lpr(std::string("lpr0"));
    const auto& _lpr0_b = gram.lpr(std::string_view("lpr0"));
    const auto& _lpr0_c = gram.lpr("lpr0");
    const auto& _lpr1_a = gram.lpr(std::string("lpr1"));
    const auto& _lpr1_b = gram.lpr(std::string_view("lpr1"));
    const auto& _lpr1_c = gram.lpr("lpr1");

    const auto& _ppr0_a = gram.ppr(std::string("ppr0"));
    const auto& _ppr0_b = gram.ppr(std::string_view("ppr0"));
    const auto& _ppr0_c = gram.ppr("ppr0");
    const auto& _ppr1_a = gram.ppr(std::string("ppr1"));
    const auto& _ppr1_b = gram.ppr(std::string_view("ppr1"));
    const auto& _ppr1_c = gram.ppr("ppr1");

    EXPECT_EQ(&_lpr0_a, &(gram.lprs()[0]));
    EXPECT_EQ(&_lpr0_b, &(gram.lprs()[0]));
    EXPECT_EQ(&_lpr0_c, &(gram.lprs()[0]));
    EXPECT_EQ(&_lpr1_a, &(gram.lprs()[1]));
    EXPECT_EQ(&_lpr1_b, &(gram.lprs()[1]));
    EXPECT_EQ(&_lpr1_c, &(gram.lprs()[1]));

    EXPECT_EQ(&_ppr0_a, &(gram.pprs()[0]));
    EXPECT_EQ(&_ppr0_b, &(gram.pprs()[0]));
    EXPECT_EQ(&_ppr0_c, &(gram.pprs()[0]));
    EXPECT_EQ(&_ppr1_a, &(gram.pprs()[1]));
    EXPECT_EQ(&_ppr1_b, &(gram.pprs()[1]));
    EXPECT_EQ(&_ppr1_c, &(gram.pprs()[1]));

    EXPECT_TRUE(gram.contains(std::string("lpr0")));
    EXPECT_TRUE(gram.contains(std::string("lpr1")));
    EXPECT_TRUE(gram.contains(std::string("ppr0")));
    EXPECT_TRUE(gram.contains(std::string("ppr1")));
    EXPECT_TRUE(gram.contains(std::string_view("lpr0")));
    EXPECT_TRUE(gram.contains(std::string_view("lpr1")));
    EXPECT_TRUE(gram.contains(std::string_view("ppr0")));
    EXPECT_TRUE(gram.contains(std::string_view("ppr1")));
    EXPECT_TRUE(gram.contains("lpr0"));
    EXPECT_TRUE(gram.contains("lpr1"));
    EXPECT_TRUE(gram.contains("ppr0"));
    EXPECT_TRUE(gram.contains("ppr1"));
    EXPECT_FALSE(gram.contains("missing"));

    EXPECT_TRUE(gram.contains_lpr(std::string("lpr0")));
    EXPECT_TRUE(gram.contains_lpr(std::string("lpr1")));
    EXPECT_FALSE(gram.contains_lpr(std::string("ppr0")));
    EXPECT_FALSE(gram.contains_lpr(std::string("ppr1")));
    EXPECT_TRUE(gram.contains_lpr(std::string_view("lpr0")));
    EXPECT_TRUE(gram.contains_lpr(std::string_view("lpr1")));
    EXPECT_FALSE(gram.contains_lpr(std::string_view("ppr0")));
    EXPECT_FALSE(gram.contains_lpr(std::string_view("ppr1")));
    EXPECT_TRUE(gram.contains_lpr("lpr0"));
    EXPECT_TRUE(gram.contains_lpr("lpr1"));
    EXPECT_FALSE(gram.contains_lpr("ppr0"));
    EXPECT_FALSE(gram.contains_lpr("ppr1"));
    EXPECT_FALSE(gram.contains_lpr("missing"));

    EXPECT_FALSE(gram.contains_ppr(std::string("lpr0")));
    EXPECT_FALSE(gram.contains_ppr(std::string("lpr1")));
    EXPECT_TRUE(gram.contains_ppr(std::string("ppr0")));
    EXPECT_TRUE(gram.contains_ppr(std::string("ppr1")));
    EXPECT_FALSE(gram.contains_ppr(std::string_view("lpr0")));
    EXPECT_FALSE(gram.contains_ppr(std::string_view("lpr1")));
    EXPECT_TRUE(gram.contains_ppr(std::string_view("ppr0")));
    EXPECT_TRUE(gram.contains_ppr(std::string_view("ppr1")));
    EXPECT_FALSE(gram.contains_ppr("lpr0"));
    EXPECT_FALSE(gram.contains_ppr("lpr1"));
    EXPECT_TRUE(gram.contains_ppr("ppr0"));
    EXPECT_TRUE(gram.contains_ppr("ppr1"));
    EXPECT_FALSE(gram.contains_ppr("missing"));
}

TEST(grammar_tests, copyAssign) {

    const auto lgr = taul::make_stderr_logger();

    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .close()
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .close()
        .done();

    const auto gram0 = taul::load(spec, lgr);

    ASSERT_TRUE(gram0);

    taul::grammar gram{};

    gram = *gram0; // copy assign


    {
        ASSERT_TRUE(gram0);

        TAUL_LOG(lgr, "{}", *gram0);

        if (gram0->lprs().size() == 2) {
            EXPECT_EQ(gram0->lprs()[0].name, "lpr0");
            EXPECT_EQ(gram0->lprs()[0].index, 0);
            EXPECT_EQ(gram0->lprs()[1].name, "lpr1");
            EXPECT_EQ(gram0->lprs()[1].index, 1);
        }
        else ADD_FAILURE();

        if (gram0->pprs().size() == 2) {
            EXPECT_EQ(gram0->pprs()[0].name, "ppr0");
            EXPECT_EQ(gram0->pprs()[0].index, 0);
            EXPECT_EQ(gram0->pprs()[1].name, "ppr1");
            EXPECT_EQ(gram0->pprs()[1].index, 1);
        }
        else ADD_FAILURE();

        EXPECT_THROW(gram0->lpr(std::string("missing")), taul::lpr_not_found_error);
        EXPECT_THROW(gram0->lpr(std::string_view("missing")), taul::lpr_not_found_error);
        EXPECT_THROW(gram0->lpr("missing"), taul::lpr_not_found_error);

        EXPECT_THROW(gram0->ppr(std::string("missing")), taul::ppr_not_found_error);
        EXPECT_THROW(gram0->ppr(std::string_view("missing")), taul::ppr_not_found_error);
        EXPECT_THROW(gram0->ppr("missing"), taul::ppr_not_found_error);

        const auto& _lpr0_a = gram0->lpr(std::string("lpr0"));
        const auto& _lpr0_b = gram0->lpr(std::string_view("lpr0"));
        const auto& _lpr0_c = gram0->lpr("lpr0");
        const auto& _lpr1_a = gram0->lpr(std::string("lpr1"));
        const auto& _lpr1_b = gram0->lpr(std::string_view("lpr1"));
        const auto& _lpr1_c = gram0->lpr("lpr1");

        const auto& _ppr0_a = gram0->ppr(std::string("ppr0"));
        const auto& _ppr0_b = gram0->ppr(std::string_view("ppr0"));
        const auto& _ppr0_c = gram0->ppr("ppr0");
        const auto& _ppr1_a = gram0->ppr(std::string("ppr1"));
        const auto& _ppr1_b = gram0->ppr(std::string_view("ppr1"));
        const auto& _ppr1_c = gram0->ppr("ppr1");

        EXPECT_EQ(&_lpr0_a, &(gram0->lprs()[0]));
        EXPECT_EQ(&_lpr0_b, &(gram0->lprs()[0]));
        EXPECT_EQ(&_lpr0_c, &(gram0->lprs()[0]));
        EXPECT_EQ(&_lpr1_a, &(gram0->lprs()[1]));
        EXPECT_EQ(&_lpr1_b, &(gram0->lprs()[1]));
        EXPECT_EQ(&_lpr1_c, &(gram0->lprs()[1]));

        EXPECT_EQ(&_ppr0_a, &(gram0->pprs()[0]));
        EXPECT_EQ(&_ppr0_b, &(gram0->pprs()[0]));
        EXPECT_EQ(&_ppr0_c, &(gram0->pprs()[0]));
        EXPECT_EQ(&_ppr1_a, &(gram0->pprs()[1]));
        EXPECT_EQ(&_ppr1_b, &(gram0->pprs()[1]));
        EXPECT_EQ(&_ppr1_c, &(gram0->pprs()[1]));

        EXPECT_TRUE(gram0->contains(std::string("lpr0")));
        EXPECT_TRUE(gram0->contains(std::string("lpr1")));
        EXPECT_TRUE(gram0->contains(std::string("ppr0")));
        EXPECT_TRUE(gram0->contains(std::string("ppr1")));
        EXPECT_TRUE(gram0->contains(std::string_view("lpr0")));
        EXPECT_TRUE(gram0->contains(std::string_view("lpr1")));
        EXPECT_TRUE(gram0->contains(std::string_view("ppr0")));
        EXPECT_TRUE(gram0->contains(std::string_view("ppr1")));
        EXPECT_TRUE(gram0->contains("lpr0"));
        EXPECT_TRUE(gram0->contains("lpr1"));
        EXPECT_TRUE(gram0->contains("ppr0"));
        EXPECT_TRUE(gram0->contains("ppr1"));
        EXPECT_FALSE(gram0->contains("missing"));

        EXPECT_TRUE(gram0->contains_lpr(std::string("lpr0")));
        EXPECT_TRUE(gram0->contains_lpr(std::string("lpr1")));
        EXPECT_FALSE(gram0->contains_lpr(std::string("ppr0")));
        EXPECT_FALSE(gram0->contains_lpr(std::string("ppr1")));
        EXPECT_TRUE(gram0->contains_lpr(std::string_view("lpr0")));
        EXPECT_TRUE(gram0->contains_lpr(std::string_view("lpr1")));
        EXPECT_FALSE(gram0->contains_lpr(std::string_view("ppr0")));
        EXPECT_FALSE(gram0->contains_lpr(std::string_view("ppr1")));
        EXPECT_TRUE(gram0->contains_lpr("lpr0"));
        EXPECT_TRUE(gram0->contains_lpr("lpr1"));
        EXPECT_FALSE(gram0->contains_lpr("ppr0"));
        EXPECT_FALSE(gram0->contains_lpr("ppr1"));
        EXPECT_FALSE(gram0->contains_lpr("missing"));

        EXPECT_FALSE(gram0->contains_ppr(std::string("lpr0")));
        EXPECT_FALSE(gram0->contains_ppr(std::string("lpr1")));
        EXPECT_TRUE(gram0->contains_ppr(std::string("ppr0")));
        EXPECT_TRUE(gram0->contains_ppr(std::string("ppr1")));
        EXPECT_FALSE(gram0->contains_ppr(std::string_view("lpr0")));
        EXPECT_FALSE(gram0->contains_ppr(std::string_view("lpr1")));
        EXPECT_TRUE(gram0->contains_ppr(std::string_view("ppr0")));
        EXPECT_TRUE(gram0->contains_ppr(std::string_view("ppr1")));
        EXPECT_FALSE(gram0->contains_ppr("lpr0"));
        EXPECT_FALSE(gram0->contains_ppr("lpr1"));
        EXPECT_TRUE(gram0->contains_ppr("ppr0"));
        EXPECT_TRUE(gram0->contains_ppr("ppr1"));
        EXPECT_FALSE(gram0->contains_ppr("missing"));
    }

    {
        TAUL_LOG(lgr, "{}", gram);

        if (gram.lprs().size() == 2) {
            EXPECT_EQ(gram.lprs()[0].name, "lpr0");
            EXPECT_EQ(gram.lprs()[0].index, 0);
            EXPECT_EQ(gram.lprs()[1].name, "lpr1");
            EXPECT_EQ(gram.lprs()[1].index, 1);
        }
        else ADD_FAILURE();

        if (gram.pprs().size() == 2) {
            EXPECT_EQ(gram.pprs()[0].name, "ppr0");
            EXPECT_EQ(gram.pprs()[0].index, 0);
            EXPECT_EQ(gram.pprs()[1].name, "ppr1");
            EXPECT_EQ(gram.pprs()[1].index, 1);
        }
        else ADD_FAILURE();

        EXPECT_THROW(gram.lpr(std::string("missing")), taul::lpr_not_found_error);
        EXPECT_THROW(gram.lpr(std::string_view("missing")), taul::lpr_not_found_error);
        EXPECT_THROW(gram.lpr("missing"), taul::lpr_not_found_error);

        EXPECT_THROW(gram.ppr(std::string("missing")), taul::ppr_not_found_error);
        EXPECT_THROW(gram.ppr(std::string_view("missing")), taul::ppr_not_found_error);
        EXPECT_THROW(gram.ppr("missing"), taul::ppr_not_found_error);

        const auto& _lpr0_a = gram.lpr(std::string("lpr0"));
        const auto& _lpr0_b = gram.lpr(std::string_view("lpr0"));
        const auto& _lpr0_c = gram.lpr("lpr0");
        const auto& _lpr1_a = gram.lpr(std::string("lpr1"));
        const auto& _lpr1_b = gram.lpr(std::string_view("lpr1"));
        const auto& _lpr1_c = gram.lpr("lpr1");

        const auto& _ppr0_a = gram.ppr(std::string("ppr0"));
        const auto& _ppr0_b = gram.ppr(std::string_view("ppr0"));
        const auto& _ppr0_c = gram.ppr("ppr0");
        const auto& _ppr1_a = gram.ppr(std::string("ppr1"));
        const auto& _ppr1_b = gram.ppr(std::string_view("ppr1"));
        const auto& _ppr1_c = gram.ppr("ppr1");

        EXPECT_EQ(&_lpr0_a, &(gram.lprs()[0]));
        EXPECT_EQ(&_lpr0_b, &(gram.lprs()[0]));
        EXPECT_EQ(&_lpr0_c, &(gram.lprs()[0]));
        EXPECT_EQ(&_lpr1_a, &(gram.lprs()[1]));
        EXPECT_EQ(&_lpr1_b, &(gram.lprs()[1]));
        EXPECT_EQ(&_lpr1_c, &(gram.lprs()[1]));

        EXPECT_EQ(&_ppr0_a, &(gram.pprs()[0]));
        EXPECT_EQ(&_ppr0_b, &(gram.pprs()[0]));
        EXPECT_EQ(&_ppr0_c, &(gram.pprs()[0]));
        EXPECT_EQ(&_ppr1_a, &(gram.pprs()[1]));
        EXPECT_EQ(&_ppr1_b, &(gram.pprs()[1]));
        EXPECT_EQ(&_ppr1_c, &(gram.pprs()[1]));

        EXPECT_TRUE(gram.contains(std::string("lpr0")));
        EXPECT_TRUE(gram.contains(std::string("lpr1")));
        EXPECT_TRUE(gram.contains(std::string("ppr0")));
        EXPECT_TRUE(gram.contains(std::string("ppr1")));
        EXPECT_TRUE(gram.contains(std::string_view("lpr0")));
        EXPECT_TRUE(gram.contains(std::string_view("lpr1")));
        EXPECT_TRUE(gram.contains(std::string_view("ppr0")));
        EXPECT_TRUE(gram.contains(std::string_view("ppr1")));
        EXPECT_TRUE(gram.contains("lpr0"));
        EXPECT_TRUE(gram.contains("lpr1"));
        EXPECT_TRUE(gram.contains("ppr0"));
        EXPECT_TRUE(gram.contains("ppr1"));
        EXPECT_FALSE(gram.contains("missing"));

        EXPECT_TRUE(gram.contains_lpr(std::string("lpr0")));
        EXPECT_TRUE(gram.contains_lpr(std::string("lpr1")));
        EXPECT_FALSE(gram.contains_lpr(std::string("ppr0")));
        EXPECT_FALSE(gram.contains_lpr(std::string("ppr1")));
        EXPECT_TRUE(gram.contains_lpr(std::string_view("lpr0")));
        EXPECT_TRUE(gram.contains_lpr(std::string_view("lpr1")));
        EXPECT_FALSE(gram.contains_lpr(std::string_view("ppr0")));
        EXPECT_FALSE(gram.contains_lpr(std::string_view("ppr1")));
        EXPECT_TRUE(gram.contains_lpr("lpr0"));
        EXPECT_TRUE(gram.contains_lpr("lpr1"));
        EXPECT_FALSE(gram.contains_lpr("ppr0"));
        EXPECT_FALSE(gram.contains_lpr("ppr1"));
        EXPECT_FALSE(gram.contains_lpr("missing"));

        EXPECT_FALSE(gram.contains_ppr(std::string("lpr0")));
        EXPECT_FALSE(gram.contains_ppr(std::string("lpr1")));
        EXPECT_TRUE(gram.contains_ppr(std::string("ppr0")));
        EXPECT_TRUE(gram.contains_ppr(std::string("ppr1")));
        EXPECT_FALSE(gram.contains_ppr(std::string_view("lpr0")));
        EXPECT_FALSE(gram.contains_ppr(std::string_view("lpr1")));
        EXPECT_TRUE(gram.contains_ppr(std::string_view("ppr0")));
        EXPECT_TRUE(gram.contains_ppr(std::string_view("ppr1")));
        EXPECT_FALSE(gram.contains_ppr("lpr0"));
        EXPECT_FALSE(gram.contains_ppr("lpr1"));
        EXPECT_TRUE(gram.contains_ppr("ppr0"));
        EXPECT_TRUE(gram.contains_ppr("ppr1"));
        EXPECT_FALSE(gram.contains_ppr("missing"));
    }
}

TEST(grammar_tests, moveAssign) {

    const auto lgr = taul::make_stderr_logger();

    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .close()
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .close()
        .done();

    auto gram0 = taul::load(spec, lgr);

    ASSERT_TRUE(gram0);

    taul::grammar gram{};

    gram = std::move(*gram0); // move assign


    TAUL_LOG(lgr, "{}", gram);

    if (gram.lprs().size() == 2) {
        EXPECT_EQ(gram.lprs()[0].name, "lpr0");
        EXPECT_EQ(gram.lprs()[0].index, 0);
        EXPECT_EQ(gram.lprs()[1].name, "lpr1");
        EXPECT_EQ(gram.lprs()[1].index, 1);
    }
    else ADD_FAILURE();

    if (gram.pprs().size() == 2) {
        EXPECT_EQ(gram.pprs()[0].name, "ppr0");
        EXPECT_EQ(gram.pprs()[0].index, 0);
        EXPECT_EQ(gram.pprs()[1].name, "ppr1");
        EXPECT_EQ(gram.pprs()[1].index, 1);
    }
    else ADD_FAILURE();

    EXPECT_THROW(gram.lpr(std::string("missing")), taul::lpr_not_found_error);
    EXPECT_THROW(gram.lpr(std::string_view("missing")), taul::lpr_not_found_error);
    EXPECT_THROW(gram.lpr("missing"), taul::lpr_not_found_error);

    EXPECT_THROW(gram.ppr(std::string("missing")), taul::ppr_not_found_error);
    EXPECT_THROW(gram.ppr(std::string_view("missing")), taul::ppr_not_found_error);
    EXPECT_THROW(gram.ppr("missing"), taul::ppr_not_found_error);

    const auto& _lpr0_a = gram.lpr(std::string("lpr0"));
    const auto& _lpr0_b = gram.lpr(std::string_view("lpr0"));
    const auto& _lpr0_c = gram.lpr("lpr0");
    const auto& _lpr1_a = gram.lpr(std::string("lpr1"));
    const auto& _lpr1_b = gram.lpr(std::string_view("lpr1"));
    const auto& _lpr1_c = gram.lpr("lpr1");

    const auto& _ppr0_a = gram.ppr(std::string("ppr0"));
    const auto& _ppr0_b = gram.ppr(std::string_view("ppr0"));
    const auto& _ppr0_c = gram.ppr("ppr0");
    const auto& _ppr1_a = gram.ppr(std::string("ppr1"));
    const auto& _ppr1_b = gram.ppr(std::string_view("ppr1"));
    const auto& _ppr1_c = gram.ppr("ppr1");

    EXPECT_EQ(&_lpr0_a, &(gram.lprs()[0]));
    EXPECT_EQ(&_lpr0_b, &(gram.lprs()[0]));
    EXPECT_EQ(&_lpr0_c, &(gram.lprs()[0]));
    EXPECT_EQ(&_lpr1_a, &(gram.lprs()[1]));
    EXPECT_EQ(&_lpr1_b, &(gram.lprs()[1]));
    EXPECT_EQ(&_lpr1_c, &(gram.lprs()[1]));

    EXPECT_EQ(&_ppr0_a, &(gram.pprs()[0]));
    EXPECT_EQ(&_ppr0_b, &(gram.pprs()[0]));
    EXPECT_EQ(&_ppr0_c, &(gram.pprs()[0]));
    EXPECT_EQ(&_ppr1_a, &(gram.pprs()[1]));
    EXPECT_EQ(&_ppr1_b, &(gram.pprs()[1]));
    EXPECT_EQ(&_ppr1_c, &(gram.pprs()[1]));

    EXPECT_TRUE(gram.contains(std::string("lpr0")));
    EXPECT_TRUE(gram.contains(std::string("lpr1")));
    EXPECT_TRUE(gram.contains(std::string("ppr0")));
    EXPECT_TRUE(gram.contains(std::string("ppr1")));
    EXPECT_TRUE(gram.contains(std::string_view("lpr0")));
    EXPECT_TRUE(gram.contains(std::string_view("lpr1")));
    EXPECT_TRUE(gram.contains(std::string_view("ppr0")));
    EXPECT_TRUE(gram.contains(std::string_view("ppr1")));
    EXPECT_TRUE(gram.contains("lpr0"));
    EXPECT_TRUE(gram.contains("lpr1"));
    EXPECT_TRUE(gram.contains("ppr0"));
    EXPECT_TRUE(gram.contains("ppr1"));
    EXPECT_FALSE(gram.contains("missing"));

    EXPECT_TRUE(gram.contains_lpr(std::string("lpr0")));
    EXPECT_TRUE(gram.contains_lpr(std::string("lpr1")));
    EXPECT_FALSE(gram.contains_lpr(std::string("ppr0")));
    EXPECT_FALSE(gram.contains_lpr(std::string("ppr1")));
    EXPECT_TRUE(gram.contains_lpr(std::string_view("lpr0")));
    EXPECT_TRUE(gram.contains_lpr(std::string_view("lpr1")));
    EXPECT_FALSE(gram.contains_lpr(std::string_view("ppr0")));
    EXPECT_FALSE(gram.contains_lpr(std::string_view("ppr1")));
    EXPECT_TRUE(gram.contains_lpr("lpr0"));
    EXPECT_TRUE(gram.contains_lpr("lpr1"));
    EXPECT_FALSE(gram.contains_lpr("ppr0"));
    EXPECT_FALSE(gram.contains_lpr("ppr1"));
    EXPECT_FALSE(gram.contains_lpr("missing"));

    EXPECT_FALSE(gram.contains_ppr(std::string("lpr0")));
    EXPECT_FALSE(gram.contains_ppr(std::string("lpr1")));
    EXPECT_TRUE(gram.contains_ppr(std::string("ppr0")));
    EXPECT_TRUE(gram.contains_ppr(std::string("ppr1")));
    EXPECT_FALSE(gram.contains_ppr(std::string_view("lpr0")));
    EXPECT_FALSE(gram.contains_ppr(std::string_view("lpr1")));
    EXPECT_TRUE(gram.contains_ppr(std::string_view("ppr0")));
    EXPECT_TRUE(gram.contains_ppr(std::string_view("ppr1")));
    EXPECT_FALSE(gram.contains_ppr("lpr0"));
    EXPECT_FALSE(gram.contains_ppr("lpr1"));
    EXPECT_TRUE(gram.contains_ppr("ppr0"));
    EXPECT_TRUE(gram.contains_ppr("ppr1"));
    EXPECT_FALSE(gram.contains_ppr("missing"));
}

TEST(grammar_tests, grammarInitViaSpec) {

    const auto lgr = taul::make_stderr_logger();

    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .ppr_decl("ppr0")
        .ppr_decl("ppr1")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .close()
        .ppr("ppr0")
        .close()
        .ppr("ppr1")
        .close()
        .done();

    const auto gram = taul::load(spec, lgr);

    ASSERT_TRUE(gram);

    TAUL_LOG(lgr, "{}", *gram);


    if (gram->lprs().size() == 2) {
        EXPECT_EQ(gram->lprs()[0].name, "lpr0");
        EXPECT_EQ(gram->lprs()[0].index, 0);
        EXPECT_EQ(gram->lprs()[1].name, "lpr1");
        EXPECT_EQ(gram->lprs()[1].index, 1);
    }
    else ADD_FAILURE();

    if (gram->pprs().size() == 2) {
        EXPECT_EQ(gram->pprs()[0].name, "ppr0");
        EXPECT_EQ(gram->pprs()[0].index, 0);
        EXPECT_EQ(gram->pprs()[1].name, "ppr1");
        EXPECT_EQ(gram->pprs()[1].index, 1);
    }
    else ADD_FAILURE();

    EXPECT_THROW(gram->lpr(std::string("missing")), taul::lpr_not_found_error);
    EXPECT_THROW(gram->lpr(std::string_view("missing")), taul::lpr_not_found_error);
    EXPECT_THROW(gram->lpr("missing"), taul::lpr_not_found_error);

    EXPECT_THROW(gram->ppr(std::string("missing")), taul::ppr_not_found_error);
    EXPECT_THROW(gram->ppr(std::string_view("missing")), taul::ppr_not_found_error);
    EXPECT_THROW(gram->ppr("missing"), taul::ppr_not_found_error);

    const auto& _lpr0_a = gram->lpr(std::string("lpr0"));
    const auto& _lpr0_b = gram->lpr(std::string_view("lpr0"));
    const auto& _lpr0_c = gram->lpr("lpr0");
    const auto& _lpr1_a = gram->lpr(std::string("lpr1"));
    const auto& _lpr1_b = gram->lpr(std::string_view("lpr1"));
    const auto& _lpr1_c = gram->lpr("lpr1");

    const auto& _ppr0_a = gram->ppr(std::string("ppr0"));
    const auto& _ppr0_b = gram->ppr(std::string_view("ppr0"));
    const auto& _ppr0_c = gram->ppr("ppr0");
    const auto& _ppr1_a = gram->ppr(std::string("ppr1"));
    const auto& _ppr1_b = gram->ppr(std::string_view("ppr1"));
    const auto& _ppr1_c = gram->ppr("ppr1");

    EXPECT_EQ(&_lpr0_a, &(gram->lprs()[0]));
    EXPECT_EQ(&_lpr0_b, &(gram->lprs()[0]));
    EXPECT_EQ(&_lpr0_c, &(gram->lprs()[0]));
    EXPECT_EQ(&_lpr1_a, &(gram->lprs()[1]));
    EXPECT_EQ(&_lpr1_b, &(gram->lprs()[1]));
    EXPECT_EQ(&_lpr1_c, &(gram->lprs()[1]));

    EXPECT_EQ(&_ppr0_a, &(gram->pprs()[0]));
    EXPECT_EQ(&_ppr0_b, &(gram->pprs()[0]));
    EXPECT_EQ(&_ppr0_c, &(gram->pprs()[0]));
    EXPECT_EQ(&_ppr1_a, &(gram->pprs()[1]));
    EXPECT_EQ(&_ppr1_b, &(gram->pprs()[1]));
    EXPECT_EQ(&_ppr1_c, &(gram->pprs()[1]));

    EXPECT_TRUE(gram->contains(std::string("lpr0")));
    EXPECT_TRUE(gram->contains(std::string("lpr1")));
    EXPECT_TRUE(gram->contains(std::string("ppr0")));
    EXPECT_TRUE(gram->contains(std::string("ppr1")));
    EXPECT_TRUE(gram->contains(std::string_view("lpr0")));
    EXPECT_TRUE(gram->contains(std::string_view("lpr1")));
    EXPECT_TRUE(gram->contains(std::string_view("ppr0")));
    EXPECT_TRUE(gram->contains(std::string_view("ppr1")));
    EXPECT_TRUE(gram->contains("lpr0"));
    EXPECT_TRUE(gram->contains("lpr1"));
    EXPECT_TRUE(gram->contains("ppr0"));
    EXPECT_TRUE(gram->contains("ppr1"));
    EXPECT_FALSE(gram->contains("missing"));

    EXPECT_TRUE(gram->contains_lpr(std::string("lpr0")));
    EXPECT_TRUE(gram->contains_lpr(std::string("lpr1")));
    EXPECT_FALSE(gram->contains_lpr(std::string("ppr0")));
    EXPECT_FALSE(gram->contains_lpr(std::string("ppr1")));
    EXPECT_TRUE(gram->contains_lpr(std::string_view("lpr0")));
    EXPECT_TRUE(gram->contains_lpr(std::string_view("lpr1")));
    EXPECT_FALSE(gram->contains_lpr(std::string_view("ppr0")));
    EXPECT_FALSE(gram->contains_lpr(std::string_view("ppr1")));
    EXPECT_TRUE(gram->contains_lpr("lpr0"));
    EXPECT_TRUE(gram->contains_lpr("lpr1"));
    EXPECT_FALSE(gram->contains_lpr("ppr0"));
    EXPECT_FALSE(gram->contains_lpr("ppr1"));
    EXPECT_FALSE(gram->contains_lpr("missing"));

    EXPECT_FALSE(gram->contains_ppr(std::string("lpr0")));
    EXPECT_FALSE(gram->contains_ppr(std::string("lpr1")));
    EXPECT_TRUE(gram->contains_ppr(std::string("ppr0")));
    EXPECT_TRUE(gram->contains_ppr(std::string("ppr1")));
    EXPECT_FALSE(gram->contains_ppr(std::string_view("lpr0")));
    EXPECT_FALSE(gram->contains_ppr(std::string_view("lpr1")));
    EXPECT_TRUE(gram->contains_ppr(std::string_view("ppr0")));
    EXPECT_TRUE(gram->contains_ppr(std::string_view("ppr1")));
    EXPECT_FALSE(gram->contains_ppr("lpr0"));
    EXPECT_FALSE(gram->contains_ppr("lpr1"));
    EXPECT_TRUE(gram->contains_ppr("ppr0"));
    EXPECT_TRUE(gram->contains_ppr("ppr1"));
    EXPECT_FALSE(gram->contains_ppr("missing"));
}

TEST(grammar_tests, nonsupport_lprs) {

    const auto lgr = taul::make_stderr_logger();

    const auto spec =
        taul::spec_writer()
        .lpr_decl("lpr0")
        .lpr_decl("lpr1")
        .lpr_decl("lpr2")
        .lpr_decl("lpr3")
        .lpr_decl("lpr4")
        .lpr_decl("lpr5")
        .lpr_decl("lpr6")
        .lpr_decl("lpr7")
        .lpr_decl("lpr8")
        .lpr("lpr0")
        .close()
        .lpr("lpr1")
        .close()
        .lpr("lpr2")
        .close()
        .lpr("lpr3", taul::qualifier::skip)
        .close()
        .lpr("lpr4", taul::qualifier::skip)
        .close()
        .lpr("lpr5", taul::qualifier::support)
        .close()
        .lpr("lpr6", taul::qualifier::support)
        .close()
        .lpr("lpr7", taul::qualifier::support)
        .close()
        .lpr("lpr8", taul::qualifier::support)
        .close()
        .done();

    const auto gram = taul::load(spec, lgr);

    ASSERT_TRUE(gram);

    TAUL_LOG(lgr, "{}", *gram);


    EXPECT_EQ(gram->nonsupport_lprs(), 5);
}

