

#include "compile.h"

#include "internal/util.h"


std::optional<taul::spec> taul::compile(
    node_ctx& ctx, 
    const std::shared_ptr<source_code>& src, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    if (!src) {
        internal::raise_spec_error(
            &ec, 
            spec_error::source_code_not_found,
            lgr,
            "passed (std::shared_ptr<source_code>) src argument was nullptr!");
        return std::nullopt;
    }
    return std::nullopt;
}

std::optional<taul::spec> taul::compile(
    node_ctx& ctx, 
    const std::shared_ptr<source_code>& src, 
    const std::shared_ptr<logger>& lgr) {
    spec_error_counter ec{};
    return compile(ctx, src, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    const std::shared_ptr<source_code>& src, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    node_ctx ctx{};
    return compile(ctx, src, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    const std::shared_ptr<source_code>& src, 
    const std::shared_ptr<logger>& lgr) {
    node_ctx ctx{};
    spec_error_counter ec{};
    return compile(ctx, src, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    node_ctx& ctx, 
    std::string src, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    auto sc = std::make_shared<source_code>();
    sc->add_str("<src>", src);
    return compile(ctx, sc, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    node_ctx& ctx, 
    std::string src, 
    const std::shared_ptr<logger>& lgr) {
    spec_error_counter ec{};
    return compile(ctx, src, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    std::string src, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    node_ctx ctx{};
    return compile(ctx, src, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    std::string src, 
    const std::shared_ptr<logger>& lgr) {
    node_ctx ctx{};
    spec_error_counter ec{};
    return compile(ctx, src, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    node_ctx& ctx, 
    const std::filesystem::path& src_path, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    auto sc = std::make_shared<source_code>();
    bool success = sc->add_file(src_path);
    if (!success) {
        internal::raise_spec_error(
            &ec,
            spec_error::source_code_not_found,
            lgr,
            "failed to load source file at \"{}\"!",
            src_path.string());
        return std::nullopt;
    }
    return compile(ctx, sc, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    node_ctx& ctx, 
    const std::filesystem::path& src_path, 
    const std::shared_ptr<logger>& lgr) {
    spec_error_counter ec{};
    return compile(ctx, src_path, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    const std::filesystem::path& src_path, 
    spec_error_counter& ec, 
    const std::shared_ptr<logger>& lgr) {
    node_ctx ctx{};
    return compile(ctx, src_path, ec, lgr);
}

std::optional<taul::spec> taul::compile(
    const std::filesystem::path& src_path, 
    const std::shared_ptr<logger>& lgr) {
    node_ctx ctx{};
    spec_error_counter ec{};
    return compile(ctx, src_path, ec, lgr);
}

