

#include "load.h"

#include "asserts.h"
#include "string_and_charset.h"
#include "compile.h"

#include "internal/spec_load_interpreter.h"


std::optional<taul::grammar> taul::load(
    const taul::spec& s, 
    const std::shared_ptr<logger>& lgr) {
    spec_error_counter ec{};
    return load(s, ec, lgr);
}

std::optional<taul::grammar> taul::load(
    const taul::spec& s, 
    taul::spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    internal::spec_load_interpreter interp(s.src, ec, lgr);
    interp.interpret(s);
    return interp.get_result();
}

std::optional<taul::grammar> taul::load(
    const std::shared_ptr<source_code>& src, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr,
    bool dbgsyms) {
    const auto compiled = compile(src, ec, lgr, dbgsyms);
    return
        compiled
        ? load(*compiled, ec, lgr)
        : std::nullopt;
}

std::optional<taul::grammar> taul::load(
    const std::shared_ptr<source_code>& src, 
    const std::shared_ptr<logger>& lgr,
    bool dbgsyms) {
    const auto compiled = compile(src, lgr, dbgsyms);
    return
        compiled
        ? load(*compiled, lgr)
        : std::nullopt;
}

std::optional<taul::grammar> taul::load(
    const std::string& src, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr,
    bool dbgsyms) {
    const auto compiled = compile(src, lgr, dbgsyms);
    return
        compiled
        ? load(*compiled, lgr)
        : std::nullopt;
}

std::optional<taul::grammar> taul::load(
    const std::string& src, 
    const std::shared_ptr<logger>& lgr,
    bool dbgsyms) {
    const auto compiled = compile(src, lgr, dbgsyms);
    return
        compiled
        ? load(*compiled, lgr)
        : std::nullopt;
}

std::optional<taul::grammar> taul::load(
    const std::filesystem::path& src_path, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr,
    bool dbgsyms) {
    const auto compiled = compile(src_path, lgr, dbgsyms);
    return
        compiled
        ? load(*compiled, lgr)
        : std::nullopt;
}

std::optional<taul::grammar> taul::load(
    const std::filesystem::path& src_path, 
    const std::shared_ptr<logger>& lgr,
    bool dbgsyms) {
    const auto compiled = compile(src_path, lgr, dbgsyms);
    return
        compiled
        ? load(*compiled, lgr)
        : std::nullopt;
}

