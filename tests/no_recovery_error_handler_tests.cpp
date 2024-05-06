

#include <gtest/gtest.h>

#include <taul/no_recovery_error_handler.h>

#include "parameterized_tests/error_handler_tests.h"


using namespace taul::string_literals;


static ErrorHandlerParam _make_param_1() {
    auto factory = [](taul::grammar gram, std::shared_ptr<taul::logger> lgr) -> std::shared_ptr<taul::error_handler> {
        return std::make_shared<taul::no_recovery_error_handler>(lgr);
        };
    return ErrorHandlerParam::init(factory, false);
}

INSTANTIATE_TEST_SUITE_P(
    NoRecoveryErrorHandler,
    ErrorHandlerTests,
    testing::Values(_make_param_1()));

