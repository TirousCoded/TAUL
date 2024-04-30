

#include <gtest/gtest.h>

#include <taul/logger.h>
#include <taul/str.h>
#include <taul/symbols.h>
#include <taul/spec.h>
#include <taul/grammar.h>
#include <taul/load.h>


using namespace taul::string_literals;


class SymbolsTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr;
    taul::grammar gram;
    bool ready = false;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
        const auto spec =
            taul::spec_writer()
            .lpr_decl("lpr0"_str)
            .lpr_decl("lpr1"_str)
            .lpr("lpr0"_str)
            .close()
            .lpr("lpr1"_str)
            .close()
            .done();
        auto _gram = taul::load(spec, lgr);
        if (_gram) {
            gram = std::move(*_gram);
            TAUL_LOG(lgr, "{}", gram);
        }
        else {
            TAUL_LOG(lgr, "failed to load grammar for unit test!");
        }
        ready =
            _gram &&
            gram.has_lpr("lpr0"_str) &&
            gram.has_lpr("lpr1"_str);
    }
};


TEST_F(SymbolsTests, Glyph_DefaultCtor) {
    ASSERT_TRUE(ready);

    TAUL_LOG(lgr, "{}", taul::glyph{});

    EXPECT_EQ(taul::glyph{}.id, taul::end_cp_id);

    // we'll assume operator== and operator!= work if equal works

    EXPECT_TRUE(taul::glyph{}.equal(taul::glyph{}));
    EXPECT_TRUE(taul::glyph{}.equal(taul::glyph::end(0)));

    EXPECT_FALSE(taul::glyph{}.equal(taul::glyph::normal(U'&', 14)));
    EXPECT_FALSE(taul::glyph{}.equal(taul::glyph::end(14)));
}

TEST_F(SymbolsTests, Glyph_Normal) {
    ASSERT_TRUE(ready);

    const taul::glyph glyph0 = taul::glyph::normal(U'&', 14);
    const taul::glyph glyph1 = taul::glyph::normal(U'&');
    const taul::glyph glyph2 = taul::glyph::normal(U'\n', 15, 2);

    TAUL_LOG(lgr, "{}\n{}\n{}", glyph0, glyph1, glyph2);

    EXPECT_EQ(glyph0.id, taul::cp_id(U'&'));
    EXPECT_EQ(glyph0.pos, 14);
    EXPECT_EQ(glyph0.len, 1);

    EXPECT_TRUE(glyph0.cp().has_value());
    if (glyph0.cp()) EXPECT_EQ(glyph0.cp(), U'&');

    EXPECT_EQ(glyph1.id, taul::cp_id(U'&'));
    EXPECT_EQ(glyph1.pos, 0);
    EXPECT_EQ(glyph1.len, 1);
    
    EXPECT_TRUE(glyph1.cp().has_value());
    if (glyph1.cp()) EXPECT_EQ(glyph1.cp(), U'&');

    EXPECT_EQ(glyph2.id, taul::cp_id(U'\n'));
    EXPECT_EQ(glyph2.pos, 15);
    EXPECT_EQ(glyph2.len, 2);
    
    EXPECT_TRUE(glyph2.cp().has_value());
    if (glyph2.cp()) EXPECT_EQ(glyph2.cp(), U'\n');

    EXPECT_TRUE((bool)glyph0);
    EXPECT_TRUE((bool)glyph1);
    EXPECT_TRUE((bool)glyph2);

    // we'll assume operator== and operator!= work if equal works

    EXPECT_TRUE(taul::glyph::normal(U'&', 14).equal(taul::glyph::normal(U'&', 14)));
    EXPECT_TRUE(taul::glyph::normal(U'&', 14).equal(taul::glyph::normal(U'&', 14, 1)));

    EXPECT_FALSE(taul::glyph::normal(U'&', 14).equal(taul::glyph::normal(U'@', 14)));
    EXPECT_FALSE(taul::glyph::normal(U'&', 14).equal(taul::glyph::normal(U'&', 13)));
    EXPECT_FALSE(taul::glyph::normal(U'&', 14).equal(taul::glyph::normal(U'&', 14, 2)));

    EXPECT_FALSE(taul::glyph::normal(U'&', 14).equal(taul::glyph{}));
    EXPECT_FALSE(taul::glyph::normal(U'&', 14).equal(taul::glyph::end(14)));
}

TEST_F(SymbolsTests, Glyph_EndOfInput) {
    ASSERT_TRUE(ready);

    const taul::glyph glyph0 = taul::glyph::end(14);
    const taul::glyph glyph1 = taul::glyph::end();

    TAUL_LOG(lgr, "{}\n{}", glyph0, glyph1);

    EXPECT_EQ(glyph0.id, taul::end_cp_id);
    EXPECT_EQ(glyph0.pos, 14);
    EXPECT_EQ(glyph0.len, 0);
    
    EXPECT_FALSE(glyph0.cp().has_value());

    EXPECT_EQ(glyph1.id, taul::end_cp_id);
    EXPECT_EQ(glyph1.pos, 0);
    EXPECT_EQ(glyph1.len, 0);

    EXPECT_FALSE(glyph1.cp().has_value());

    EXPECT_FALSE((bool)glyph0);
    EXPECT_FALSE((bool)glyph1);

    // we'll assume operator== and operator!= work if equal works

    EXPECT_TRUE(taul::glyph::end(14).equal(taul::glyph::end(14)));

    EXPECT_TRUE(taul::glyph::end(0).equal(taul::glyph{}));

    EXPECT_FALSE(taul::glyph::end(14).equal(taul::glyph::end(13)));

    EXPECT_FALSE(taul::glyph::end(14).equal(taul::glyph{}));
    EXPECT_FALSE(taul::glyph::end(14).equal(taul::glyph::normal(U'&', 14)));
}

TEST_F(SymbolsTests, Token_DefaultCtor) {
    ASSERT_TRUE(ready);

    TAUL_LOG(lgr, "{}", taul::token{});

    EXPECT_EQ(taul::token{}.id, taul::end_lpr_id);

    // we'll assume operator== and operator!= work if equal works

    EXPECT_TRUE(taul::token{}.equal(taul::token{}));
    EXPECT_TRUE(taul::token{}.equal(taul::token::end(0)));

    EXPECT_FALSE(taul::token{}.equal(taul::token::normal(gram.lpr("lpr0"_str).value(), 14, 3)));
    EXPECT_FALSE(taul::token{}.equal(taul::token::end(14)));
    EXPECT_FALSE(taul::token{}.equal(taul::token::failure(14, 3)));
}

TEST_F(SymbolsTests, Token_Normal) {
    ASSERT_TRUE(ready);

    const taul::token tkn0 = taul::token::normal(*gram.lpr("lpr0"_str), 0, 3);
    const taul::token tkn1 = taul::token::normal(*gram.lpr("lpr0"_str), 3, 3);

    taul::str src = "abcdef"_str;

    TAUL_LOG(lgr, "{}\n{}", tkn0, tkn1);

    EXPECT_EQ(tkn0.id, taul::lpr_id(gram.lpr("lpr0"_str)->index()));
    EXPECT_EQ(tkn0.pos, 0);
    EXPECT_EQ(tkn0.len, 3);
    EXPECT_TRUE(tkn0.lpr);
    if (tkn0.lpr) EXPECT_EQ(tkn0.lpr.value(), gram.lpr("lpr0"_str).value());
    EXPECT_EQ(tkn0.str(src), "abc"_str);

    EXPECT_EQ(tkn1.id, taul::lpr_id(gram.lpr("lpr0"_str)->index()));
    EXPECT_EQ(tkn1.pos, 3);
    EXPECT_EQ(tkn1.len, 3);
    EXPECT_TRUE(tkn1.lpr);
    if (tkn0.lpr) EXPECT_EQ(tkn1.lpr.value(), gram.lpr("lpr0"_str).value());
    EXPECT_EQ(tkn1.str(src), "def"_str);

    EXPECT_TRUE((bool)tkn0);
    EXPECT_TRUE((bool)tkn1);

    // we'll assume operator== and operator!= work if equal works

    EXPECT_TRUE(taul::token::normal(*gram.lpr("lpr0"_str), 14, 3).equal(taul::token::normal(*gram.lpr("lpr0"_str), 14, 3)));

    EXPECT_FALSE(taul::token::normal(*gram.lpr("lpr0"_str), 14, 3).equal(taul::token::normal(*gram.lpr("lpr1"_str), 14, 3)));
    EXPECT_FALSE(taul::token::normal(*gram.lpr("lpr0"_str), 14, 3).equal(taul::token::normal(*gram.lpr("lpr0"_str), 13, 3)));
    EXPECT_FALSE(taul::token::normal(*gram.lpr("lpr0"_str), 14, 3).equal(taul::token::normal(*gram.lpr("lpr0"_str), 14, 4)));

    EXPECT_FALSE(taul::token::normal(*gram.lpr("lpr0"_str), 14, 3).equal(taul::token{}));
    EXPECT_FALSE(taul::token::normal(*gram.lpr("lpr0"_str), 14, 3).equal(taul::token::end(14)));
    EXPECT_FALSE(taul::token::normal(*gram.lpr("lpr0"_str), 14, 3).equal(taul::token::failure(14, 3)));
}

TEST_F(SymbolsTests, Token_EndOfInput) {
    ASSERT_TRUE(ready);

    const taul::token tkn0 = taul::token::end();
    const taul::token tkn1 = taul::token::end(3);

    taul::str src = "abcdef"_str;

    TAUL_LOG(lgr, "{}\n{}", tkn0, tkn1);

    EXPECT_EQ(tkn0.id, taul::end_lpr_id);
    EXPECT_EQ(tkn0.pos, 0);
    EXPECT_EQ(tkn0.len, 0);
    EXPECT_FALSE(tkn0.lpr);
    EXPECT_TRUE(tkn0.str(src).empty());

    EXPECT_EQ(tkn1.id, taul::end_lpr_id);
    EXPECT_EQ(tkn1.pos, 3);
    EXPECT_EQ(tkn1.len, 0);
    EXPECT_FALSE(tkn1.lpr);
    EXPECT_TRUE(tkn1.str(src).empty());

    EXPECT_FALSE((bool)tkn0);
    EXPECT_FALSE((bool)tkn1);

    // we'll assume operator== and operator!= work if equal works

    EXPECT_TRUE(taul::token::end(14).equal(taul::token::end(14)));

    EXPECT_TRUE(taul::token::end(0).equal(taul::token{}));

    EXPECT_FALSE(taul::token::end(14).equal(taul::token::end(13)));

    EXPECT_FALSE(taul::token::end(14).equal(taul::token{}));
    EXPECT_FALSE(taul::token::end(14).equal(taul::token::normal(gram.lpr("lpr0"_str).value(), 14, 3)));
    EXPECT_FALSE(taul::token::end(14).equal(taul::token::failure(14, 3)));
}

TEST_F(SymbolsTests, Token_Failure) {
    ASSERT_TRUE(ready);

    const taul::token tkn0 = taul::token::failure(0, 3);
    const taul::token tkn1 = taul::token::failure(3, 3);

    taul::str src = "abcdef"_str;

    TAUL_LOG(lgr, "{}\n{}", tkn0, tkn1);

    EXPECT_EQ(tkn0.id, taul::failure_lpr_id);
    EXPECT_EQ(tkn0.pos, 0);
    EXPECT_EQ(tkn0.len, 3);
    EXPECT_FALSE(tkn0.lpr);
    EXPECT_EQ(tkn0.str(src), "abc"_str);

    EXPECT_EQ(tkn1.id, taul::failure_lpr_id);
    EXPECT_EQ(tkn1.pos, 3);
    EXPECT_EQ(tkn1.len, 3);
    EXPECT_FALSE(tkn1.lpr);
    EXPECT_EQ(tkn1.str(src), "def"_str);

    EXPECT_FALSE((bool)tkn0);
    EXPECT_FALSE((bool)tkn1);

    // we'll assume operator== and operator!= work if equal works

    EXPECT_TRUE(taul::token::failure(14, 3).equal(taul::token::failure(14, 3)));

    EXPECT_FALSE(taul::token::failure(14, 3).equal(taul::token::failure(13, 3)));
    EXPECT_FALSE(taul::token::failure(14, 3).equal(taul::token::failure(14, 4)));

    EXPECT_FALSE(taul::token::failure(14, 3).equal(taul::token::normal(gram.lpr("lpr0"_str).value(), 14, 3)));
    EXPECT_FALSE(taul::token::failure(14, 3).equal(taul::token{}));
    EXPECT_FALSE(taul::token::failure(14, 3).equal(taul::token::end(14)));
}

