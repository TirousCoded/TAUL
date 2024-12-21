

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
    const std::string& output_path,
    std::optional<std::string> include_path_value = std::nullopt,
    bool has_triangle_brackets = false);


int32_t main(int32_t argc, const char** argv) {
    args_reader args(argc, argv);
    if (args.expect("help")) {
        if (args.expect("help")) {
            message("Synopsis:");
            message("");
            message("    taulc help <command>");
            message("");
            message("Explains form and function of <command>.");
        }
        else if (args.expect("version")) {
            message("Synopsis:");
            message("");
            message("    taulc version");
            message("");
            message("Explains the TAUL library version that taulc was compiled under.");
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
            message("    taulc compile <fetcher> <source-path> <output-path> [[--include-path|-i]=<include-path> [--triangle-brackets|-t]]");
            message("");
            message("Compiles the TAUL spec file at <source-path>, if any, and if successful, outputs");
            message("a C++ header file to <output-path>, with this generated header file encapsulating");
            message("a TAUL fetcher function named <fetcher>.");
            message("");
            message("Any existing file at <output-path> is overwritten.");
            message("");
            message("No checks are made to ensure that <fetcher> is a valid C++ identifier.");
            message("");
            message("Any compilation errors which arise are reported.");
            message("All stages of TAUL spec compilation and loading are tested for errors.");
            message("");
            message("If provided, <include-path> tells taulc where it can find TAUL library header files,");
            message("for situations where generated C++ takes the form '#include \"<include-path>/<X>\"',");
            message("where <X> is the desired header file.");
            message("");
            message("If --triangle-brackets is provided, the above include directives will use");
            message("'#include <...>' instead of '#include \"...\"'.");
            message("");
            message("No checks are made to ensure <include-path> is syntactic in C++.");
            message("");
            message("If <include-path> is not provided, 'taul' will be presumed.");
        }
        else error("Cannot provide help for no or unrecognized command!");
    }
    else if (args.expect("version")) {
        message("{}", taul::api_version);
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
            if (const auto include_path_value = args.expect_ext({ "--include-path", "-i" })) {
                const bool has_triangle_brackets = args.expect({ "--triangle-brackets", "-t" });
                exec_compile(*fetcher, *source_path, *output_path, include_path_value, has_triangle_brackets);
            }
            else exec_compile(*fetcher, *source_path, *output_path);
        }
        else error("Missing or invalid command arguments!");
    }
    else error("No or invalid argument specifying driver command!");
    return EXIT_SUCCESS;
}


void exec_check(
    const std::string& source_path) {
    const auto path = std::filesystem::path(source_path);
    if (!std::filesystem::exists(path)) {
        error("No file found at \"{}\"!", source_path);
    }
    if ((bool)taul::load(path, lgr)) {
        message("TAUL spec file compilation succeeded!");
    }
    else {
        message("TAUL spec file compilation failed!");
    }
}

void exec_compile(
    const std::string& fetcher,
    const std::string& source_path,
    const std::string& output_path,
    std::optional<std::string> include_path_value,
    bool has_triangle_brackets) {
    const auto in_path = std::filesystem::path(source_path);
    const auto out_path = std::filesystem::path(output_path);
    if (!std::filesystem::exists(in_path)) {
        error("No file found at \"{}\"!", source_path);
    }
    if (!out_path.has_filename()) {
        error("Output path \"{}\" does not have filename component!", output_path);
    }
    if (const auto loaded = taul::load(in_path, lgr)) {
        message("TAUL spec file compilation succeeded!");
        const auto generated_code =
            include_path_value
            ? taul::export_fetcher(*loaded, fetcher.c_str(), include_path_value->c_str(), has_triangle_brackets)
            : taul::export_fetcher(*loaded, fetcher.c_str(), "taul", false);
        if (!generated_code.to_file(out_path)) {
            error("TAUL spec file output failed!");
        }
    }
    else error("TAUL spec file compilation failed!");
}

