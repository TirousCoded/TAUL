

#include <cstdint>
#include <iostream>

#include <taul/all.h>

#define TAUL_SRCGEN_IMPLEMENTATION 1
#include "abc_fetcher.h"


using namespace taul::string_literals;


std::int32_t main_disabled1(std::int32_t argc, const char** argv) {
    /*auto spec =
        taul::spec_writer()
        .lpr_decl("a魂"_str)
        .lpr_decl(taul::str(taul::convert_encoding<char>(taul::utf8, taul::utf8, u8"a\r\n魂").value()))
        .lpr(taul::str(taul::convert_encoding<char>(taul::utf8, taul::utf8, u8"a\r\n元気ですか").value()))
        .pos(105)
        .close()
        .done();*/
    auto lgr = taul::make_stderr_logger();
    auto compiled = taul::compile(std::filesystem::current_path() / "abc.taul", lgr);
    if (!compiled) return EXIT_FAILURE;
    taul::export_fetcher(compiled.value(), "abc").to_file(std::filesystem::current_path() / "abc_fetcher.h");
    return EXIT_SUCCESS;
}

std::int32_t main(std::int32_t argc, const char** argv) {
    auto lgr = taul::make_stderr_logger();
    auto loaded = taul::load(std::filesystem::current_path() / "abc.taul", lgr);
    if (!loaded) return EXIT_FAILURE;
    taul::grammar gram = loaded.value();
    //taul::grammar gram = taul::fetchers::abc();
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

