

#include "compile.h"

#include "string_reader.h"
#include "lexer.h"
#include "parser.h"
#include "regular_error_handler.h"
#include "load.h"
#include "taul_spec.h"

#include "internal/util.h"
#include "internal/compiler_backend.h"


using namespace taul::string_literals;


std::optional<taul::spec> taul::compile(const std::shared_ptr<source_code>& src, spec_error_counter& ec, const std::shared_ptr<logger>& lgr, bool dbgsyms) {
    if (!src) {
        taul::internal::raise_error(
            lgr, &ec, nullptr, 0,
            taul::spec_error::source_code_not_found,
            "passed src argument was nullptr!");
        return std::nullopt;
    }
    auto gram = taul::load(taul::taul_spec());
    TAUL_ASSERT(gram);
    taul::internal::compiler_backend backend(*src, ec, gram.value(), lgr, dbgsyms);
    taul::string_reader rdr(*src);
    taul::lexer lxr(gram.value());
    taul::parser psr(gram.value());
    taul::regular_error_handler eh{};
    lxr.bind_source(&rdr);
    psr.bind_source(&lxr);
    psr.bind_listener(&backend);
    psr.bind_error_handler(&eh);
    psr.reset();
    psr.parse_notree("Spec"_str);
    if (backend.result) backend.result->associate(src);
    return backend.result;
}

std::optional<taul::spec> taul::compile(const std::shared_ptr<source_code>& src, const std::shared_ptr<logger>& lgr, bool dbgsyms) {
    spec_error_counter ec{};
    return compile(src, ec, lgr, dbgsyms);
}

std::optional<taul::spec> taul::compile(const str& src, spec_error_counter& ec, const std::shared_ptr<logger>& lgr, bool dbgsyms) {
    auto src0 = std::make_shared<taul::source_code>();
    src0->add_str("<src>"_str, src);
    return compile(src0, ec, lgr, dbgsyms);
}

std::optional<taul::spec> taul::compile(const str& src, const std::shared_ptr<logger>& lgr, bool dbgsyms) {
    spec_error_counter ec{};
    return compile(src, ec, lgr, dbgsyms);
}

std::optional<taul::spec> taul::compile(const std::filesystem::path& src_path, spec_error_counter& ec, const std::shared_ptr<logger>& lgr, bool dbgsyms) {
    auto src0 = std::make_shared<taul::source_code>();
    if (!src0->add_file(src_path)) {
        taul::internal::raise_error(
            lgr, &ec, nullptr, 0,
            taul::spec_error::source_code_not_found,
            "failed to load source file at \"{}\"!",
            src_path.string());
        return std::nullopt;
    }
    return compile(src0, ec, lgr, dbgsyms);
}

std::optional<taul::spec> taul::compile(const std::filesystem::path& src_path, const std::shared_ptr<logger>& lgr, bool dbgsyms) {
    spec_error_counter ec{};
    return compile(src_path, ec, lgr, dbgsyms);
}

