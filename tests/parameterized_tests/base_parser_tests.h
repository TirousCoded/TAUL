

#pragma once


#include <gtest/gtest.h>

#include <taul/base_parser.h>


// these tests are for basic usage as a taul::base_parser, w/ each individual
// impl being expected to do additional testing regarding their own 
// semantic particulars as needed


struct BaseParserParam final {
    std::function<std::shared_ptr<taul::base_parser>(taul::grammar, std::shared_ptr<taul::logger>)> factory; // factory for parser under test


    inline static BaseParserParam init(
        std::function<std::shared_ptr<taul::base_parser>(taul::grammar, std::shared_ptr<taul::logger>)> factory) {
        return BaseParserParam{
            std::move(factory),
        };
    }
};

class BaseParserTests : public testing::TestWithParam<BaseParserParam> {
public:

    std::shared_ptr<taul::logger> lgr;


    inline void SetUp() override final {
        lgr = taul::make_stderr_logger();
    }

    inline void TearDown() override final {}
};

