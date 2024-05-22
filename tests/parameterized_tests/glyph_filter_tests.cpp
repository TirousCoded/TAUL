

#include "glyph_filter_tests.h"

#include "../helpers/test_glyph_stream_for_reset.h"


GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(GlyphFilterTests);


TEST_P(GlyphFilterTests, ResetPropagation) {
    auto filter = GetParam().factory(lgr);
    test_glyph_stream_for_reset upstream{};

    filter->bind_source(&upstream);

    ASSERT_FALSE(upstream.flag);

    filter->reset();

    ASSERT_TRUE(upstream.flag);
}

