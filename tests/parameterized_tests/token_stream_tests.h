

#pragma once


#include <gtest/gtest.h>

#include <taul/logger.h>
#include <taul/symbol_stream.h>


// these tests are for basic usage as a taul::token_stream, w/ each
// individual impl being expected to do additional testing regarding
// their own semantic particulars as needed


struct TokenStreamParam final {
    std::shared_ptr<taul::token_stream> stream; // the stream under test (nullptr aborts tests)
    std::size_t n_after_done; // the expected number of 'next' calls until done


    inline static TokenStreamParam init(
        std::shared_ptr<taul::token_stream> stream,
        std::size_t n_after_done) {
        return TokenStreamParam{
            stream,
            n_after_done,
        };
    }
};

class TokenStreamTests : public testing::TestWithParam<TokenStreamParam> {
public:

    std::shared_ptr<taul::logger> lgr;


    inline void SetUp() override final {
        lgr = taul::make_stderr_logger();
    }

    inline void TearDown() override final {}
};

