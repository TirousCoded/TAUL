

#include <gtest/gtest.h>

#include <taul/all.h>


// each of our tests will involve calling taul::parser::operator(), w/ us
// thusly performing tests for each ctor/assignment, but not operator() itself

class test_parser_state final : public taul::parser_state {
public:

    taul::grammar gram;

    // these parser state types *shouldn't* be mutable, but we'll make a dirty
    // little exception here for the sake of ease of testing

    // this is to make sure the parser function actually acknowledges the parser state,
    // nothing more

    const taul::token_seq* last = nullptr;
};

std::optional<taul::node> text_parser_fn(
    taul::node_ctx& ctx,
    const std::shared_ptr<taul::parser_state>& state,
    const taul::token_seq& tkns,
    std::size_t offset,
    const std::shared_ptr<taul::logger>& lgr) {
    TAUL_LOG(lgr, "test_parser_fn called w/ <{} tokens> w/ offset {}", tkns.size(), offset);
    assert(state);
    auto& _state = *static_cast<test_parser_state*>(state.get());
    _state.last = &tkns;
    TAUL_IN_BOUNDS(offset, 0, tkns.size() + 1); // the '+ 1' is due to allowing offset == tkns.size()
    return std::make_optional<taul::node>(std::move(ctx.create(tkns[offset])));
}


class ParserTests : public testing::Test {
protected:

    std::shared_ptr<taul::logger> lgr = nullptr;
    std::shared_ptr<test_parser_state> state = nullptr;
    std::shared_ptr<taul::node_ctx> ctx = nullptr;
    std::unique_ptr<taul::token_seq> tkns0;
    std::unique_ptr<taul::token_seq> tkns1;


    void SetUp() override final {
        lgr = taul::make_stderr_logger();
        state = std::make_shared<test_parser_state>();
        ctx = std::make_shared<taul::node_ctx>(lgr);
        auto spec =
            taul::spec_writer()
            .lpr_decl("lpr")
            .ppr_decl("ppr")
            .lpr("lpr")
            .close()
            .ppr("ppr")
            .close()
            .done();
        state->gram = std::move(*taul::load(spec, lgr));
        tkns0 = std::make_unique<taul::token_seq>("abcdef");
        tkns0->push(state->gram.lpr("lpr"), 2);
        tkns0->push_failure(2);
        tkns0->push(state->gram.lpr("lpr"), 2);
        tkns1 = std::make_unique<taul::token_seq>("123456");
        tkns1->push(state->gram.lpr("lpr"), 2);
        tkns1->push_failure(2);
        tkns1->push(state->gram.lpr("lpr"), 2);
    }
};


TEST_F(ParserTests, defaultCtor) {

    taul::parser psr{};

    auto result0 = psr(*ctx, (*tkns0), lgr);
    auto result1 = psr(*ctx, (*tkns1), 2, lgr);

    EXPECT_FALSE(result0);
    EXPECT_FALSE(result1);
}

TEST_F(ParserTests, directCtor) {

    taul::parser psr(text_parser_fn, state);


    EXPECT_EQ(state->last, nullptr);

    auto result0 = psr(*ctx, (*tkns0), lgr);

    EXPECT_EQ(state->last, &(*tkns0));

    auto result1 = psr(*ctx, (*tkns0), 2, lgr);

    EXPECT_EQ(state->last, &(*tkns0));

    auto result2 = psr(*ctx, (*tkns1), 2, lgr);

    EXPECT_EQ(state->last, &(*tkns1));


    ASSERT_TRUE(result0);

    TAUL_LOG(lgr, "{}", *result0);

    EXPECT_TRUE(result0->equiv(ctx->create((*tkns0)[0])));

    ASSERT_TRUE(result1);

    TAUL_LOG(lgr, "{}", *result1);

    EXPECT_TRUE(result1->equiv(ctx->create((*tkns0)[2])));

    ASSERT_TRUE(result2);

    TAUL_LOG(lgr, "{}", *result2);

    EXPECT_TRUE(result2->equiv(ctx->create((*tkns1)[2])));
}

TEST_F(ParserTests, copyCtor) {

    taul::parser old_psr(text_parser_fn, state);

    taul::parser psr(old_psr);


    EXPECT_EQ(state->last, nullptr);

    auto result0 = psr(*ctx, (*tkns0), lgr);

    EXPECT_EQ(state->last, &(*tkns0));

    auto result1 = psr(*ctx, (*tkns0), 2, lgr);

    EXPECT_EQ(state->last, &(*tkns0));

    auto result2 = psr(*ctx, (*tkns1), 2, lgr);

    EXPECT_EQ(state->last, &(*tkns1));


    ASSERT_TRUE(result0);

    TAUL_LOG(lgr, "{}", *result0);

    EXPECT_TRUE(result0->equiv(ctx->create((*tkns0)[0])));

    ASSERT_TRUE(result1);

    TAUL_LOG(lgr, "{}", *result1);

    EXPECT_TRUE(result1->equiv(ctx->create((*tkns0)[2])));

    ASSERT_TRUE(result2);

    TAUL_LOG(lgr, "{}", *result2);

    EXPECT_TRUE(result2->equiv(ctx->create((*tkns1)[2])));
}

TEST_F(ParserTests, moveCtor) {

    taul::parser old_psr(text_parser_fn, state);

    taul::parser psr(std::move(old_psr));


    EXPECT_EQ(state->last, nullptr);

    auto result0 = psr(*ctx, (*tkns0), lgr);

    EXPECT_EQ(state->last, &(*tkns0));

    auto result1 = psr(*ctx, (*tkns0), 2, lgr);

    EXPECT_EQ(state->last, &(*tkns0));

    auto result2 = psr(*ctx, (*tkns1), 2, lgr);

    EXPECT_EQ(state->last, &(*tkns1));


    ASSERT_TRUE(result0);

    TAUL_LOG(lgr, "{}", *result0);

    EXPECT_TRUE(result0->equiv(ctx->create((*tkns0)[0])));

    ASSERT_TRUE(result1);

    TAUL_LOG(lgr, "{}", *result1);

    EXPECT_TRUE(result1->equiv(ctx->create((*tkns0)[2])));

    ASSERT_TRUE(result2);

    TAUL_LOG(lgr, "{}", *result2);

    EXPECT_TRUE(result2->equiv(ctx->create((*tkns1)[2])));
}

TEST_F(ParserTests, copyAssign) {

    taul::parser old_psr(text_parser_fn, state);

    taul::parser psr{};

    psr = old_psr;


    EXPECT_EQ(state->last, nullptr);

    auto result0 = psr(*ctx, (*tkns0), lgr);

    EXPECT_EQ(state->last, &(*tkns0));

    auto result1 = psr(*ctx, (*tkns0), 2, lgr);

    EXPECT_EQ(state->last, &(*tkns0));

    auto result2 = psr(*ctx, (*tkns1), 2, lgr);

    EXPECT_EQ(state->last, &(*tkns1));


    ASSERT_TRUE(result0);

    TAUL_LOG(lgr, "{}", *result0);

    EXPECT_TRUE(result0->equiv(ctx->create((*tkns0)[0])));

    ASSERT_TRUE(result1);

    TAUL_LOG(lgr, "{}", *result1);

    EXPECT_TRUE(result1->equiv(ctx->create((*tkns0)[2])));

    ASSERT_TRUE(result2);

    TAUL_LOG(lgr, "{}", *result2);

    EXPECT_TRUE(result2->equiv(ctx->create((*tkns1)[2])));
}

TEST_F(ParserTests, moveAssign) {

    taul::parser old_psr(text_parser_fn, state);

    taul::parser psr{};

    psr = std::move(old_psr);


    EXPECT_EQ(state->last, nullptr);

    auto result0 = psr(*ctx, (*tkns0), lgr);

    EXPECT_EQ(state->last, &(*tkns0));

    auto result1 = psr(*ctx, (*tkns0), 2, lgr);

    EXPECT_EQ(state->last, &(*tkns0));

    auto result2 = psr(*ctx, (*tkns1), 2, lgr);

    EXPECT_EQ(state->last, &(*tkns1));


    ASSERT_TRUE(result0);

    TAUL_LOG(lgr, "{}", *result0);

    EXPECT_TRUE(result0->equiv(ctx->create((*tkns0)[0])));

    ASSERT_TRUE(result1);

    TAUL_LOG(lgr, "{}", *result1);

    EXPECT_TRUE(result1->equiv(ctx->create((*tkns0)[2])));

    ASSERT_TRUE(result2);

    TAUL_LOG(lgr, "{}", *result2);

    EXPECT_TRUE(result2->equiv(ctx->create((*tkns1)[2])));
}

TEST_F(ParserTests, moveAssign_ontoSelf) {

    taul::parser psr(text_parser_fn, state);

    psr = std::move(psr);


    EXPECT_EQ(state->last, nullptr);

    auto result0 = psr(*ctx, (*tkns0), lgr);

    EXPECT_EQ(state->last, &(*tkns0));

    auto result1 = psr(*ctx, (*tkns0), 2, lgr);

    EXPECT_EQ(state->last, &(*tkns0));

    auto result2 = psr(*ctx, (*tkns1), 2, lgr);

    EXPECT_EQ(state->last, &(*tkns1));


    ASSERT_TRUE(result0);

    TAUL_LOG(lgr, "{}", *result0);

    EXPECT_TRUE(result0->equiv(ctx->create((*tkns0)[0])));

    ASSERT_TRUE(result1);

    TAUL_LOG(lgr, "{}", *result1);

    EXPECT_TRUE(result1->equiv(ctx->create((*tkns0)[2])));

    ASSERT_TRUE(result2);

    TAUL_LOG(lgr, "{}", *result2);

    EXPECT_TRUE(result2->equiv(ctx->create((*tkns1)[2])));
}

