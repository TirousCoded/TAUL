

#include "compile.h"

#include "string_reader.h"
#include "lexer.h"
#include "parser.h"
#include "load.h"
#include "taul_spec.h"

#include "internal/util.h"
#include "internal/compiler_backend.h"


using namespace taul::string_literals;


std::optional<taul::spec> taul::compile(const std::shared_ptr<source_code>& src, spec_error_counter& ec, const std::shared_ptr<logger>& lgr) {
    if (!src) {
        internal::raise_spec_error(
            &ec,
            spec_error::source_code_not_found,
            lgr,
            "passed src argument was nullptr!");
        return std::nullopt;
    }
    auto gram = load(taul_spec());
    TAUL_ASSERT(gram);
    internal::compiler_backend backend(*src, ec, lgr);
    string_reader rdr(*src);
    lexer lxr(gram.value());
    parser psr(gram.value());
    lxr.bind_source(&rdr);
    psr.bind_source(&lxr);
    psr.bind_listener(&backend);
    psr.reset();
    psr.parse_notree("Spec"_str);
    if (backend.result) backend.result->associate(src);
    return backend.result;
}

std::optional<taul::spec> taul::compile(const std::shared_ptr<source_code>& src, const std::shared_ptr<logger>& lgr) {
    spec_error_counter ec{};
    return compile(src, ec, lgr);
}

std::optional<taul::spec> taul::compile(const str& src, spec_error_counter& ec, const std::shared_ptr<logger>& lgr) {
    auto src0 = std::make_shared<source_code>();
    src0->add_str("<src>"_str, src);
    return compile(src0, ec, lgr);
}

std::optional<taul::spec> taul::compile(const str& src, const std::shared_ptr<logger>& lgr) {
    spec_error_counter ec{};
    return compile(src, ec, lgr);
}

std::optional<taul::spec> taul::compile(const std::filesystem::path& src_path, spec_error_counter& ec, const std::shared_ptr<logger>& lgr) {
    auto src0 = std::make_shared<source_code>();
    if (!src0->add_file(src_path)) {
        internal::raise_spec_error(
            &ec,
            spec_error::source_code_not_found,
            lgr,
            "failed to load source file at \"{}\"!",
            src_path.string());
        return std::nullopt;
    }
    return compile(src0, ec, lgr);
}

std::optional<taul::spec> taul::compile(const std::filesystem::path& src_path, const std::shared_ptr<logger>& lgr) {
    spec_error_counter ec{};
    return compile(src_path, ec, lgr);
}

