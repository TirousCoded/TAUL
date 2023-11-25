

#include <gtest/gtest.h>

#include <taul/all.h>


class TokenTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr;
    taul::grammar gram;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
        const auto spec =
            taul::spec_writer()
            .lpr_decl("lpr0")
            .lpr_decl("lpr1")
            .lpr("lpr0")
            .close()
            .lpr("lpr1")
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
    }
};


TEST_F(TokenTests, defaultCtor) {

    const taul::token tkn{};

    TAUL_LOG(lgr, "{}", tkn);

    EXPECT_TRUE(tkn.is_failure());
    EXPECT_FALSE((bool)tkn);

    EXPECT_THROW(tkn.lpr(), taul::failure_token_error);

    EXPECT_EQ(tkn.str(), "");
    EXPECT_EQ(tkn.pos(), 0);
}

TEST_F(TokenTests, initRegularToken) {

    const taul::token tkn0(gram.lpr("lpr0"), "abc", 14);
    const taul::token tkn1(gram.lpr("lpr0"), "def");

    TAUL_LOG(lgr, "{}\n{}", tkn0, tkn1);

    EXPECT_FALSE(tkn0.is_failure());
    EXPECT_FALSE(tkn1.is_failure());
    EXPECT_TRUE((bool)tkn0);
    EXPECT_TRUE((bool)tkn1);

    EXPECT_EQ(&(tkn0.lpr()), &(gram.lpr("lpr0")));
    EXPECT_EQ(&(tkn1.lpr()), &(gram.lpr("lpr0")));
    EXPECT_EQ(tkn0.str(), "abc");
    EXPECT_EQ(tkn1.str(), "def");
    EXPECT_EQ(tkn0.pos(), 14);
    EXPECT_EQ(tkn1.pos(), 0);
}

TEST_F(TokenTests, initFailureToken) {

    const taul::token tkn0 = taul::token::failure("abc", 14);
    const taul::token tkn1 = taul::token::failure("def");

    TAUL_LOG(lgr, "{}\n{}", tkn0, tkn1);

    EXPECT_TRUE(tkn0.is_failure());
    EXPECT_TRUE(tkn1.is_failure());
    EXPECT_FALSE((bool)tkn0);
    EXPECT_FALSE((bool)tkn1);

    EXPECT_THROW(tkn0.lpr(), taul::failure_token_error);
    EXPECT_THROW(tkn1.lpr(), taul::failure_token_error);

    EXPECT_EQ(tkn0.str(), "abc");
    EXPECT_EQ(tkn1.str(), "def");
    EXPECT_EQ(tkn0.pos(), 14);
    EXPECT_EQ(tkn1.pos(), 0);
}

TEST_F(TokenTests, copyCtor) {

    taul::token old_tkn0{};
    taul::token old_tkn1(gram.lpr("lpr0"), "abc", 15);
    taul::token old_tkn2 = taul::token::failure("def", 3);

    taul::token tkn0(old_tkn0);
    taul::token tkn1(old_tkn1);
    taul::token tkn2(old_tkn2);

    TAUL_LOG(lgr, "{}\n{}\n{}", tkn0, tkn1, tkn2);

    EXPECT_TRUE(tkn0.is_failure());
    EXPECT_FALSE(tkn1.is_failure());
    EXPECT_TRUE(tkn2.is_failure());
    EXPECT_FALSE((bool)tkn0);
    EXPECT_TRUE((bool)tkn1);
    EXPECT_FALSE((bool)tkn2);

    EXPECT_THROW(tkn0.lpr(), taul::failure_token_error);
    EXPECT_THROW(tkn2.lpr(), taul::failure_token_error);

    EXPECT_EQ(&(tkn1.lpr()), &(gram.lpr("lpr0")));

    EXPECT_EQ(tkn0.str(), "");
    EXPECT_EQ(tkn1.str(), "abc");
    EXPECT_EQ(tkn2.str(), "def");
    EXPECT_EQ(tkn0.pos(), 0);
    EXPECT_EQ(tkn1.pos(), 15);
    EXPECT_EQ(tkn2.pos(), 3);
}

TEST_F(TokenTests, moveCtor) {

    taul::token old_tkn0{};
    taul::token old_tkn1(gram.lpr("lpr0"), "abc", 15);
    taul::token old_tkn2 = taul::token::failure("def", 3);

    taul::token tkn0(std::move(old_tkn0));
    taul::token tkn1(std::move(old_tkn1));
    taul::token tkn2(std::move(old_tkn2));

    TAUL_LOG(lgr, "{}\n{}\n{}", tkn0, tkn1, tkn2);

    EXPECT_TRUE(tkn0.is_failure());
    EXPECT_FALSE(tkn1.is_failure());
    EXPECT_TRUE(tkn2.is_failure());
    EXPECT_FALSE((bool)tkn0);
    EXPECT_TRUE((bool)tkn1);
    EXPECT_FALSE((bool)tkn2);

    EXPECT_THROW(tkn0.lpr(), taul::failure_token_error);
    EXPECT_THROW(tkn2.lpr(), taul::failure_token_error);

    EXPECT_EQ(&(tkn1.lpr()), &(gram.lpr("lpr0")));

    EXPECT_EQ(tkn0.str(), "");
    EXPECT_EQ(tkn1.str(), "abc");
    EXPECT_EQ(tkn2.str(), "def");
    EXPECT_EQ(tkn0.pos(), 0);
    EXPECT_EQ(tkn1.pos(), 15);
    EXPECT_EQ(tkn2.pos(), 3);
}

TEST_F(TokenTests, copyAssign) {

    taul::token old_tkn0{};
    taul::token old_tkn1(gram.lpr("lpr0"), "abc", 15);
    taul::token old_tkn2 = taul::token::failure("def", 3);

    taul::token tkn0{};
    taul::token tkn1{};
    taul::token tkn2{};

    tkn0 = old_tkn0;
    tkn1 = old_tkn1;
    tkn2 = old_tkn2;

    TAUL_LOG(lgr, "{}\n{}\n{}", tkn0, tkn1, tkn2);

    EXPECT_TRUE(tkn0.is_failure());
    EXPECT_FALSE(tkn1.is_failure());
    EXPECT_TRUE(tkn2.is_failure());
    EXPECT_FALSE((bool)tkn0);
    EXPECT_TRUE((bool)tkn1);
    EXPECT_FALSE((bool)tkn2);

    EXPECT_THROW(tkn0.lpr(), taul::failure_token_error);
    EXPECT_THROW(tkn2.lpr(), taul::failure_token_error);

    EXPECT_EQ(&(tkn1.lpr()), &(gram.lpr("lpr0")));

    EXPECT_EQ(tkn0.str(), "");
    EXPECT_EQ(tkn1.str(), "abc");
    EXPECT_EQ(tkn2.str(), "def");
    EXPECT_EQ(tkn0.pos(), 0);
    EXPECT_EQ(tkn1.pos(), 15);
    EXPECT_EQ(tkn2.pos(), 3);
}

TEST_F(TokenTests, moveAssign) {

    taul::token old_tkn0{};
    taul::token old_tkn1(gram.lpr("lpr0"), "abc", 15);
    taul::token old_tkn2 = taul::token::failure("def", 3);

    taul::token tkn0{};
    taul::token tkn1{};
    taul::token tkn2{};

    tkn0 = std::move(old_tkn0);
    tkn1 = std::move(old_tkn1);
    tkn2 = std::move(old_tkn2);

    TAUL_LOG(lgr, "{}\n{}\n{}", tkn0, tkn1, tkn2);

    EXPECT_TRUE(tkn0.is_failure());
    EXPECT_FALSE(tkn1.is_failure());
    EXPECT_TRUE(tkn2.is_failure());
    EXPECT_FALSE((bool)tkn0);
    EXPECT_TRUE((bool)tkn1);
    EXPECT_FALSE((bool)tkn2);

    EXPECT_THROW(tkn0.lpr(), taul::failure_token_error);
    EXPECT_THROW(tkn2.lpr(), taul::failure_token_error);

    EXPECT_EQ(&(tkn1.lpr()), &(gram.lpr("lpr0")));

    EXPECT_EQ(tkn0.str(), "");
    EXPECT_EQ(tkn1.str(), "abc");
    EXPECT_EQ(tkn2.str(), "def");
    EXPECT_EQ(tkn0.pos(), 0);
    EXPECT_EQ(tkn1.pos(), 15);
    EXPECT_EQ(tkn2.pos(), 3);
}

TEST_F(TokenTests, moveAssign_ontoSelf) {

    taul::token tkn0{};
    taul::token tkn1(gram.lpr("lpr0"), "abc", 15);
    taul::token tkn2 = taul::token::failure("def", 3);

    tkn0 = std::move(tkn0);
    tkn1 = std::move(tkn1);
    tkn2 = std::move(tkn2);

    TAUL_LOG(lgr, "{}\n{}\n{}", tkn0, tkn1, tkn2);

    EXPECT_TRUE(tkn0.is_failure());
    EXPECT_FALSE(tkn1.is_failure());
    EXPECT_TRUE(tkn2.is_failure());
    EXPECT_FALSE((bool)tkn0);
    EXPECT_TRUE((bool)tkn1);
    EXPECT_FALSE((bool)tkn2);

    EXPECT_THROW(tkn0.lpr(), taul::failure_token_error);
    EXPECT_THROW(tkn2.lpr(), taul::failure_token_error);

    EXPECT_EQ(&(tkn1.lpr()), &(gram.lpr("lpr0")));

    EXPECT_EQ(tkn0.str(), "");
    EXPECT_EQ(tkn1.str(), "abc");
    EXPECT_EQ(tkn2.str(), "def");
    EXPECT_EQ(tkn0.pos(), 0);
    EXPECT_EQ(tkn1.pos(), 15);
    EXPECT_EQ(tkn2.pos(), 3);
}

TEST_F(TokenTests, equality) {

    // tokens are expected to be value types compared by value

    // success

    EXPECT_TRUE(taul::token{}.equal(taul::token{}));
    EXPECT_TRUE(taul::token(gram.lpr("lpr0"), "abc", 13).equal(taul::token(gram.lpr("lpr0"), "abc", 13)));
    EXPECT_TRUE(taul::token::failure("abc", 3).equal(taul::token::failure("abc", 3)));
    EXPECT_TRUE(taul::token{}.equal(taul::token::failure("")));
    EXPECT_TRUE(taul::token::failure("abc").equal(taul::token::failure("abc", 0)));

    EXPECT_TRUE(taul::token{} == taul::token{});
    EXPECT_TRUE(taul::token(gram.lpr("lpr0"), "abc", 13) == taul::token(gram.lpr("lpr0"), "abc", 13));
    EXPECT_TRUE(taul::token::failure("abc", 3) == taul::token::failure("abc", 3));
    EXPECT_TRUE(taul::token{} == taul::token::failure(""));
    EXPECT_TRUE(taul::token::failure("abc") == taul::token::failure("abc", 0));

    EXPECT_FALSE(taul::token{} != taul::token{});
    EXPECT_FALSE(taul::token(gram.lpr("lpr0"), "abc", 13) != taul::token(gram.lpr("lpr0"), "abc", 13));
    EXPECT_FALSE(taul::token::failure("abc", 3) != taul::token::failure("abc", 3));
    EXPECT_FALSE(taul::token{} != taul::token::failure(""));
    EXPECT_FALSE(taul::token::failure("abc") != taul::token::failure("abc", 0));

    // failure due to differing lpr() values (or lacking an lpr() value due to being a failure token)

    EXPECT_FALSE(taul::token(gram.lpr("lpr0"), "abc", 13).equal(taul::token(gram.lpr("lpr1"), "abc", 13)));
    EXPECT_FALSE(taul::token(gram.lpr("lpr0"), "abc", 13).equal(taul::token{}));
    EXPECT_FALSE(taul::token(gram.lpr("lpr0"), "abc", 13).equal(taul::token::failure("abc", 13)));

    EXPECT_FALSE(taul::token(gram.lpr("lpr0"), "abc", 13) == taul::token(gram.lpr("lpr1"), "abc", 13));
    EXPECT_FALSE(taul::token(gram.lpr("lpr0"), "abc", 13) == taul::token{});
    EXPECT_FALSE(taul::token(gram.lpr("lpr0"), "abc", 13) == taul::token::failure("abc", 13));

    EXPECT_TRUE(taul::token(gram.lpr("lpr0"), "abc", 13) != taul::token(gram.lpr("lpr1"), "abc", 13));
    EXPECT_TRUE(taul::token(gram.lpr("lpr0"), "abc", 13) != taul::token{});
    EXPECT_TRUE(taul::token(gram.lpr("lpr0"), "abc", 13) != taul::token::failure("abc", 13));

    // failue due to differing str() values

    EXPECT_FALSE(taul::token(gram.lpr("lpr0"), "abc", 13).equal(taul::token(gram.lpr("lpr0"), "def", 13)));
    EXPECT_FALSE(taul::token(gram.lpr("lpr0"), "abc").equal(taul::token(gram.lpr("lpr0"), "def")));
    EXPECT_FALSE(taul::token::failure("abc", 13).equal(taul::token::failure("def", 13)));
    EXPECT_FALSE(taul::token::failure("abc").equal(taul::token::failure("def")));

    EXPECT_FALSE(taul::token(gram.lpr("lpr0"), "abc", 13) == taul::token(gram.lpr("lpr0"), "def", 13));
    EXPECT_FALSE(taul::token(gram.lpr("lpr0"), "abc") == taul::token(gram.lpr("lpr0"), "def"));
    EXPECT_FALSE(taul::token::failure("abc", 13) == taul::token::failure("def", 13));
    EXPECT_FALSE(taul::token::failure("abc") == taul::token::failure("def"));

    EXPECT_TRUE(taul::token(gram.lpr("lpr0"), "abc", 13) != taul::token(gram.lpr("lpr0"), "def", 13));
    EXPECT_TRUE(taul::token(gram.lpr("lpr0"), "abc") != taul::token(gram.lpr("lpr0"), "def"));
    EXPECT_TRUE(taul::token::failure("abc", 13) != taul::token::failure("def", 13));
    EXPECT_TRUE(taul::token::failure("abc") != taul::token::failure("def"));

    // failure due to differing pos() values

    EXPECT_FALSE(taul::token(gram.lpr("lpr0"), "abc", 13).equal(taul::token(gram.lpr("lpr0"), "abc", 14)));
    EXPECT_FALSE(taul::token(gram.lpr("lpr0"), "abc").equal(taul::token(gram.lpr("lpr0"), "abc", 10)));
    EXPECT_FALSE(taul::token::failure("abc", 13).equal(taul::token::failure("abc", 14)));
    EXPECT_FALSE(taul::token::failure("abc").equal(taul::token::failure("abc", 10)));

    EXPECT_FALSE(taul::token(gram.lpr("lpr0"), "abc", 13) == taul::token(gram.lpr("lpr0"), "abc", 14));
    EXPECT_FALSE(taul::token(gram.lpr("lpr0"), "abc") == taul::token(gram.lpr("lpr0"), "abc", 10));
    EXPECT_FALSE(taul::token::failure("abc", 13) == taul::token::failure("abc", 14));
    EXPECT_FALSE(taul::token::failure("abc") == taul::token::failure("abc", 10));

    EXPECT_TRUE(taul::token(gram.lpr("lpr0"), "abc", 13) != taul::token(gram.lpr("lpr0"), "abc", 14));
    EXPECT_TRUE(taul::token(gram.lpr("lpr0"), "abc") != taul::token(gram.lpr("lpr0"), "abc", 10));
    EXPECT_TRUE(taul::token::failure("abc", 13) != taul::token::failure("abc", 14));
    EXPECT_TRUE(taul::token::failure("abc") != taul::token::failure("abc", 10));
}

