

#include <gtest/gtest.h>

#include <taul/spec.h>
#include <taul/grammar.h>
#include <taul/load.h>
#include <taul/source_reader.h>
#include <taul/parser.h>

#include "parameterized_tests/base_parser_tests.h"


using namespace taul::string_literals;


BaseParserParam _make_param_1() {
    auto factory = [](taul::grammar gram, std::shared_ptr<taul::logger> lgr) -> std::shared_ptr<taul::base_parser> {
        return std::make_shared<taul::parser>(gram, lgr);
        };
    return BaseParserParam::init(factory);
}

INSTANTIATE_TEST_SUITE_P(
    Parser,
    BaseParserTests,
    testing::Values(_make_param_1()));

