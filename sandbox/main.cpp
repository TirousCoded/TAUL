

#include <cstdint>
#include <iostream>

#include <taul/all.h>


using namespace taul::string_literals;


std::int32_t main(std::int32_t argc, const char** argv) {
    auto lgr = taul::make_stderr_logger();
    auto loaded = taul::load(std::filesystem::current_path() / "abc.taul", lgr);
    if (!loaded) return EXIT_FAILURE;
    while (true) {
        std::string input{};
        std::cin >> input;
        if (input == "exit") break;
        taul::source_code src{};
        src.add_str(""_str, taul::str(input));
        taul::string_reader rdr(src);
        taul::lexer lxr(loaded.value(), lgr);
        taul::parser psr(loaded.value(), lgr);
        lxr.bind_source(&rdr);
        psr.bind_source(&lxr);
        psr.reset();
        auto result = psr.parse("Start"_str);
        TAUL_LOG(lgr, "{}", result);
    }
    return EXIT_SUCCESS;
}

