

#pragma once


#include <gtest/gtest.h>

#include <taul/base_lexer.h>


// these tests are for basic usage as a taul::base_lexer, w/ each individual
// impl being expected to do additional testing regarding their own 
// semantic particulars as needed


struct BaseLexerParam final {
    std::function<std::shared_ptr<taul::base_lexer>(taul::grammar, std::shared_ptr<taul::logger>)> factory; // factory for lexer under test


    inline static BaseLexerParam init(
        std::function<std::shared_ptr<taul::base_lexer>(taul::grammar, std::shared_ptr<taul::logger>)> factory) {
        return BaseLexerParam{
            std::move(factory),
        };
    }
};

class BaseLexerTests : public testing::TestWithParam<BaseLexerParam> {
public:

    std::shared_ptr<taul::logger> lgr;


    inline void SetUp() override final {
        lgr = taul::make_stderr_logger();
    }

    inline void TearDown() override final {}
};

