

#pragma once


#include <gtest/gtest.h>

#include <taul/symbol_filter.h>


// TODO: at present, these tests only really cover reset propagation, nothing else


// these tests are for basic usage as a taul::glyph_filter, w/ each individual
// impl being expected to do additional testing regarding their own 
// semantic particulars as needed


struct GlyphFilterParam final {
    std::function<std::shared_ptr<taul::glyph_filter>(std::shared_ptr<taul::logger>)> factory; // factory for filter under test


    inline static GlyphFilterParam init(
        std::function<std::shared_ptr<taul::glyph_filter>(std::shared_ptr<taul::logger>)> factory) {
        return GlyphFilterParam{
            std::move(factory),
        };
    }
};

class GlyphFilterTests : public testing::TestWithParam<GlyphFilterParam> {
public:

    std::shared_ptr<taul::logger> lgr;


    inline void SetUp() override final {
        lgr = taul::make_stderr_logger();
    }

    inline void TearDown() override final {}
};

