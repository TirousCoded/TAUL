

#include "export_fetcher.h"


using namespace taul::string_literals;


taul::source_code taul::export_fetcher(
    const spec& s,
    const char* fetcher_name,
    const char* include_prefix,
    const char* tab) {
    TAUL_ASSERT(fetcher_name);
    TAUL_ASSERT(include_prefix);
    TAUL_ASSERT(tab);

    std::string src{};
    src += "#pragma once\n";
    src += "#include <optional>\n";
    src += std::format("#include <{0}/logger.h>\n", include_prefix);
    src += std::format("#include <{0}/grammar.h>\n", include_prefix);
    src += "\n";
    src += "// write \"#define TAUL_SRCGEN_IMPLEMENTATION 1\" in a *.cpp file to host the implementation\n";
    src += "\n";
    src += "namespace taul::fetchers {\n";
    src += std::format("{0}std::optional<taul::grammar> {1}(std::shared_ptr<taul::logger> lgr = nullptr);\n", tab, fetcher_name);
    src += std::format("{0}namespace internal {{\n", tab);
    src += std::format("{0}}}\n", tab);
    src += "}\n";
    src += "\n";
    src += "#if TAUL_SRCGEN_IMPLEMENTATION\n";
    src += std::format("std::optional<taul::grammar> taul::fetchers::{0}(std::shared_ptr<taul::logger> lgr) {{\n", fetcher_name);
    src += "}\n";
    src += "#endif";

    taul::source_code result{};
    result.add_str("<src>"_str, str(src));
    return result;
}

