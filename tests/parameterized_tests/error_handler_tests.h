

#pragma once


#include <gtest/gtest.h>

#include <taul/str.h>
#include <taul/spec.h>
#include <taul/error_handler.h>
#include <taul/load.h>


// these tests are for basic usage as a taul::error_handler, w/ each individual
// impl being expected to do additional testing regarding their own 
// semantic particulars as needed


// in particular, take note that the tests herein that cover recoverable errors
// only test cases where a single token is in error, and so recover should be
// fairly simple

// each error handler impl should have additional tests covering more nuanced
// behaviour unique to them, as well as covering any cases where the impl may
// end up *giving up* on error recovery


struct ErrorHandlerParam final {
    std::function<std::shared_ptr<taul::error_handler>(taul::grammar, std::shared_ptr<taul::logger>)> factory; // factory for error handler under test
    bool can_recover; // if the error handler under test can even recover from basic errors at all


    inline static ErrorHandlerParam init(
        std::function<std::shared_ptr<taul::error_handler>(taul::grammar, std::shared_ptr<taul::logger>)> factory,
        bool can_recover) {
        return ErrorHandlerParam{
            std::move(factory),
            can_recover,
        };
    }
};

class ErrorHandlerTests : public testing::TestWithParam<ErrorHandlerParam> {
public:

    std::shared_ptr<taul::logger> lgr;
    std::optional<taul::grammar> gram;


    inline void SetUp() override final {
        using namespace taul::string_literals;
        lgr = taul::make_stderr_logger();
        auto spec =
            taul::spec_writer()
            .lpr_decl("A"_str)
            .lpr_decl("B"_str)
            .lpr_decl("C"_str)
            .lpr_decl("GAP"_str)
            .ppr_decl("ABC"_str)
            .ppr_decl("ABCABC"_str)
            .lpr("A"_str)
            .string("a"_str)
            .close()
            .lpr("B"_str)
            .string("b"_str)
            .close()
            .lpr("C"_str)
            .string("c"_str)
            .close()
            .lpr("GAP"_str)
            .string("_"_str)
            .close()
            .ppr("ABC"_str)
            .name("A"_str)
            .name("B"_str)
            .name("C"_str)
            .close()
            .ppr("ABCABC"_str)
            .name("ABC"_str)
            .name("ABC"_str)
            .close()
            .done();
        gram = taul::load(spec, lgr);
    }

    inline void TearDown() override final {}
};

