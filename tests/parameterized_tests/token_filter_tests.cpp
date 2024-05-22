

#include "token_filter_tests.h"

#include "../helpers/test_token_stream_for_reset.h"


GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(TokenFilterTests);


TEST_P(TokenFilterTests, ResetPropagation) {
    auto filter = GetParam().factory(lgr);
    test_token_stream_for_reset upstream{};

    filter->bind_source(&upstream);

    ASSERT_FALSE(upstream.flag);

    filter->reset();

    ASSERT_TRUE(upstream.flag);
}

