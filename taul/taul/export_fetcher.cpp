

#include "export_fetcher.h"

#include "asserts.h"


using namespace taul::string_literals;


static std::string _fmt_include_txt(
    const char* taul_include_name,
    const char* taul_include_path_prefix,
    bool taul_includes_should_use_triangle_brackets) {
    TAUL_ASSERT(taul_include_name);
    TAUL_ASSERT(taul_include_path_prefix);
    return
        taul_includes_should_use_triangle_brackets
        ? std::format("<{0}/{1}>", taul_include_path_prefix, taul_include_name)
        : std::format("\"{0}/{1}\"", taul_include_path_prefix, taul_include_name);
}


taul::source_code taul::export_fetcher(
    const grammar& gram,
    const char* fetcher_name,
    const char* taul_include_path_prefix,
    bool taul_includes_should_use_triangle_brackets,
    const char* tab) {
    TAUL_ASSERT(fetcher_name);
    TAUL_ASSERT(taul_include_path_prefix);
    TAUL_ASSERT(tab);

    auto _include_txt =
        [&](const char* name) -> std::string {
        return _fmt_include_txt(name, taul_include_path_prefix, taul_includes_should_use_triangle_brackets);
        };
    
    std::string src{};
    src += std::format("#pragma once\n");
    src += std::format("#include <string_view>\n");
    src += std::format("#include <optional>\n");
    src += std::format("#include {0}\n", _include_txt("grammar.h"));
    src += std::format("#include {0}\n", _include_txt("export_fetcher.h"));
    src += std::format("\n");
    src += std::format("namespace taul::fetchers {{\n");

    // write our internal::export_fetcher_singleton_traits impl
    src += std::format("{0}namespace details {{\n", tab);
    src += std::format("{0}{0}struct {1}_singleton_traits final {{\n", tab, fetcher_name);
    src += std::format("{0}{0}{0}static constexpr std::string_view get_serial_str() noexcept {{\n", tab);
    src += std::format("{0}{0}{0}{0}return \"{1}\";\n", tab, gram.serialize());
    src += std::format("{0}{0}{0}}}\n", tab);
    src += std::format("{0}{0}}};\n", tab);
    src += std::format("{0}}}\n", tab);

    // write taul::fetchers::* fn
    src += std::format("{0}inline taul::grammar {1}() {{\n", tab, fetcher_name);
    src += std::format("{0}{0}return taul::internal::export_fetcher_singleton<details::{1}_singleton_traits>::fetch();\n", tab, fetcher_name);
    src += std::format("{0}}}\n", tab);

    src += std::format("}}");

    taul::source_code result{};
    result.add_str("<src>"_str, str(src));
    return result;
}

