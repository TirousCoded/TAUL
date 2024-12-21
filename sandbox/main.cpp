

#include <cstdint>
#include <iostream>
#include <fstream>

#include <taul/all.h>

#define TAUL_SRCGEN_IMPLEMENTATION 1
#include "abc_fetcher.h"


using namespace taul::string_literals;


int32_t main(int32_t argc, const char** argv) {
    auto lgr = taul::make_stderr_logger();
    if (const auto loaded = taul::load(std::filesystem::current_path() / "abc.taul", lgr)) {
        taul::export_fetcher(*loaded, "abc").to_file(std::filesystem::current_path() / "abc_fetcher.h");
    }
    return EXIT_SUCCESS;
}

int32_t main_disabled2(int32_t argc, const char** argv) {
    const auto gram = taul::taul_gram();
    std::cerr << std::format("{}\n", gram.fmt_internals());
    return EXIT_SUCCESS;
}

int32_t main_disabled1(int32_t argc, const char** argv) {
    auto lgr = taul::make_stderr_logger();
    //auto loaded = taul::load(std::filesystem::current_path() / "abc.taul", lgr);
    //if (!loaded) return EXIT_FAILURE;
    //taul::grammar gram = loaded.value();
    taul::grammar gram = taul::fetchers::abc();
    while (true) {
        std::string input{};
        std::cin >> input;
        if (input == "exit") break;
        taul::source_code src{};
        src.add_str(""_str, taul::str(input));
        taul::source_reader rdr(src);
        taul::lexer lxr(gram, lgr);
        taul::parser psr(gram, lgr);
        taul::regular_error_handler eh(lgr);
        lxr.bind_source(&rdr);
        psr.bind_source(&lxr);
        psr.bind_error_handler(&eh);
        psr.reset();
        auto result = psr.parse("Start"_str);
        TAUL_LOG(lgr, "{}", result);
        if (result.is_aborted()) TAUL_LOG(lgr, "aborted!");
    }
    return EXIT_SUCCESS;
}

