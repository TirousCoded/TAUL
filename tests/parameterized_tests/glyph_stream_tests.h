

#pragma once


#include <gtest/gtest.h>

#include <taul/logger.h>
#include <taul/symbol_stream.h>


// these tests are for basic usage as a taul::glyph_stream, w/ each
// individual impl being expected to do additional testing regarding
// their own semantic particulars as needed


struct GlyphStreamParam final {
    std::shared_ptr<taul::glyph_stream> stream; // the stream under test (nullptr aborts tests)
    std::size_t n_after_done; // the expected number of 'next' calls until done


    inline static GlyphStreamParam init(
        std::shared_ptr<taul::glyph_stream> stream,
        std::size_t n_after_done) {
        return GlyphStreamParam{
            stream,
            n_after_done,
        };
    }
};

class GlyphStreamTests : public testing::TestWithParam<GlyphStreamParam> {
public:

    std::shared_ptr<taul::logger> lgr;


    inline void SetUp() override final {
        lgr = taul::make_stderr_logger();
    }

    inline void TearDown() override final {}
};

