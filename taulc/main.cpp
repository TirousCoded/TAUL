

#include <span>
#include <format>

#include <taul/all.h>

#include "args_reader.h"


using namespace taul::string_literals;


const auto lgr = taul::make_stdout_logger();


template<typename... Args>
void message(std::format_string<Args...> fmt, Args&&... args) {
    TAUL_LOG(lgr, fmt, std::forward<Args&&>(args)...);
}

template<typename... Args>
void error(std::format_string<Args...> fmt, Args&&... args) {
    message(fmt, std::forward<Args&&>(args)...);
    exit(0); // exit successful, even if use was invalid
}


void exec_check(
    const std::string& source_path);

void exec_compile(
    const std::string& fetcher,
    const std::string& source_path,
    const std::string& output_path);


int32_t main(int32_t argc, const char** argv) {
    args_reader args(argc, argv);
    if (args.expect("help")) {
        if (args.expect("help")) {
            message("Synopsis:");
            message("");
            message("    taulc help <command>");
            message("");
            message("Explains form and function of CLI driver app command <command>.");
        }
        else if (args.expect("version")) {
            message("Synopsis:");
            message("");
            message("    taulc version");
            message("");
            message("Explains the TAUL library version the CLI driver app was compiled under.");
        }
        else if (args.expect("check")) {
            message("Synopsis:");
            message("");
            message("    taulc check <source-path>");
            message("");
            message("Checks if TAUL spec file at <source-path>, if any, compiles correctly.");
            message("");
            message("Any compilation errors which arise are reported.");
            message("All stages of TAUL spec compilation and loading are tested for errors.");
        }
        else if (args.expect("compile")) {
            message("Synopsis:");
            message("");
            message("    taulc compile <fetcher> <source-path> <output-path>");
            message("");
            message("Compiles the TAUL spec file at <source-path>, if any, and if successful,");
            message("outputs a C++ header file at <output-path>, with this generated header file");
            message("encapsulating a TAUL fetcher function named <fetcher>.");
            message("");
            message("Any existing file at <output-path> is overwritten.");
            message("");
            message("No checks are made to ensure that <fetcher> is a valid C++ identifier.");
            message("");
            message("Any compilation errors which arise are reported.");
            message("All stages of TAUL spec compilation and loading are tested for errors.");
        }
        else error("Cannot provide help for no or unrecognized command!");
    }
    else if (args.expect("version")) {
        message(taul::api_version);
    }
    else if (args.expect("check")) {
        const auto output_path = args.next();
        if (output_path) {
            exec_check(*output_path);
        }
        else error("Missing or invalid command arguments!");
    }
    else if (args.expect("compile")) {
        const auto fetcher = args.next();
        const auto source_path = args.next();
        const auto output_path = args.next();
        if (fetcher && source_path && output_path) {
            exec_compile(*fetcher, *source_path, *output_path);
        }
        else error("Missing or invalid command arguments!");
    }
    else error("No or invalid argument specifying driver command!");
    return EXIT_SUCCESS;
}


void exec_check(const std::string& source_path) {
    const auto path = std::filesystem::path(source_path);
    if (!std::filesystem::exists(path)) {
        error("No file found at \"{}\"!", source_path);
    }
    const auto loaded = taul::load(path, lgr);
    if (loaded) {
        message("TAUL spec file compilation succeeded!");
    }
    else {
        message("TAUL spec file compilation failed!");
    }
}

void exec_compile(const std::string& fetcher, const std::string& source_path, const std::string& output_path) {
    const auto in_path = std::filesystem::path(source_path);
    const auto out_path = std::filesystem::path(output_path);
    if (!std::filesystem::exists(in_path)) {
        error("No file found at \"{}\"!", source_path);
    }
    if (!out_path.has_filename()) {
        error("Output path \"{}\" does not have filename component!", output_path);
    }
    const auto compiled = taul::compile(in_path, lgr);
    const auto loaded = compiled ? taul::load(compiled.value(), lgr) : std::nullopt;
    if (compiled && loaded) {
        message("TAUL spec file compilation succeeded!");
        if (!taul::export_fetcher(*compiled, fetcher.c_str()).to_file(out_path)) {
            error("TAUL spec file output failed!");
        }
    }
    else error("TAUL spec file compilation failed!");
}

