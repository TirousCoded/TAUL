

#include <cstdint>

#include <taul/all.h>


using namespace taul::string_literals; // TAUL uses a custom string type


int32_t main(int32_t argc, char** argv) {
    // logger for TAUL diagnostics (including grammar compile/load errors)
    auto lgr = taul::make_stderr_logger();

    // load our grammar
    auto loaded = taul::load(std::filesystem::current_path() / "grammar.taul", lgr);
    if (!loaded) return EXIT_FAILURE;
    taul::grammar gram = loaded.value();

    // build parsing pipeline
    taul::source_reader rdr(""_str); // <- just pass empty string for now, we'll change it out later
    taul::lexer lxr(gram, lgr);
    taul::parser psr(gram, lgr);
    taul::regular_error_handler eh(lgr);
    lxr.bind_source(&rdr);
    psr.bind_source(&lxr);
    psr.bind_error_handler(&eh);

    while (true) { // begin our REPL

        // get user input to parse
        std::string input{};
        std::getline(std::cin, input);
        if (input == "exit") break;

        // build source code we're gonna parse
        taul::source_code src{};
        src.add_str(""_str, taul::str(input));

        // make src new input to pipeline
        rdr.change_input(src);
        psr.reset(); // <- reset pipeline after structural changes

        // parse our user input
        auto result = psr.parse("Start"_str);
        TAUL_LOG(lgr, "{}", result);
        if (result.is_aborted()) TAUL_LOG(lgr, "aborted!");
    }

    return EXIT_SUCCESS;
}