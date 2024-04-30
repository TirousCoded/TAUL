

#include "glyph_stream_tests.h"


TEST_P(GlyphStreamTests, Usage) {
    auto& stream = GetParam().stream;
    const auto& n_after_done = GetParam().n_after_done;
    ASSERT_TRUE(stream); // abort

    TAUL_LOG(lgr, "n_after_done=={}", n_after_done);

    taul::glyph last_peek{}, last_next{};
    std::size_t n = 0;

    while (!stream->done()) {
        TAUL_LOG(lgr, "n=={}", n);
        n++;

        last_peek = stream->peek();
        last_next = stream->next();
        TAUL_LOG(lgr, "last_peek=={}", last_peek);
        TAUL_LOG(lgr, "last_next=={}", last_next);

        EXPECT_EQ(last_peek, last_next);

        if (!stream->done()) {
            EXPECT_FALSE(last_peek.is_end());
            EXPECT_FALSE(last_next.is_end());
        }
    }

    EXPECT_EQ(n, n_after_done);

    last_peek = stream->peek();
    last_next = stream->next();
    EXPECT_TRUE(last_peek.is_end());
    EXPECT_TRUE(last_next.is_end());
}

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(GlyphStreamTests);

