

#pragma once


#include <gtest/gtest.h>

#include <taul/error_handler.h>


// these tests are for basic usage as a taul::error_handler, w/ each individual
// impl being expected to do additional testing regarding their own 
// semantic particulars as needed


struct ErrorHandlerParam final {
    std::function<std::shared_ptr<taul::error_handler>(taul::grammar, std::shared_ptr<taul::logger>)> factory; // factory for parser under test


    inline static ErrorHandlerParam init(
        std::function<std::shared_ptr<taul::error_handler>(taul::grammar, std::shared_ptr<taul::logger>)> factory) {
        return ErrorHandlerParam{
            std::move(factory),
        };
    }
};

class ErrorHandlerTests : public testing::TestWithParam<ErrorHandlerParam> {
public:

    std::shared_ptr<taul::logger> lgr;


    inline void SetUp() override final {
        lgr = taul::make_stderr_logger();
    }

    inline void TearDown() override final {}
};

