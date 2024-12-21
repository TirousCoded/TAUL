

#include "grammar_data.h"


#define _DUMP_DESERIALIZE_LOG 0

#if _DUMP_DESERIALIZE_LOG
#include "../logger.h"
#endif


std::string taul::internal::lexer_rule::fmt() const {
    return
        qualifier != qualifier::none
        ? std::format("[{0}] {1} ({2})", index, name, qualifier)
        : std::format("[{0}] {1}", index, name);
}

void taul::internal::lexer_rule::serialize(buff& b) const {
    b.write(std::string_view(name), qualifier);
}

std::optional<taul::internal::lexer_rule> taul::internal::lexer_rule::deserialize(buff_reader& b) {
    auto rdr = b.fork();
    std::optional<lexer_rule> result{};
    if (const auto a = rdr.read<std::string_view, taul::qualifier>()) {
        b.commit(rdr);
        const auto& [name, qualifier] = *a;
#if _DUMP_DESERIALIZE_LOG
        taul::log(make_stderr_logger(), "lexer_rule::deserialize -> name: {}, qualifier: {}", std::string(name), qualifier);
#endif
        result = lexer_rule{ .name = str(name), .index = 0, .qualifier = qualifier };
    }
#if _DUMP_DESERIALIZE_LOG
    else {
        taul::log(make_stderr_logger(), "lexer_rule::deserialize -> failed!");
    }
#endif
    return result;
}

std::string taul::internal::parser_rule::fmt() const {
    return
        qualifier != qualifier::none
        ? std::format("[{0}] {1} ({2})", index, name, qualifier)
        : std::format("[{0}] {1}", index, name);
}

void taul::internal::parser_rule::serialize(buff& b) const {
    b.write(std::string_view(name), qualifier);
}

std::optional<taul::internal::parser_rule> taul::internal::parser_rule::deserialize(buff_reader& b) {
    auto rdr = b.fork();
    std::optional<parser_rule> result{};
    if (const auto a = rdr.read<std::string_view, taul::qualifier>()) {
        b.commit(rdr);
        const auto& [name, qualifier] = *a;
#if _DUMP_DESERIALIZE_LOG
        taul::log(make_stderr_logger(), "parser_rule::deserialize -> name: {}, qualifier: {}", std::string(name), qualifier);
#endif
        result = parser_rule{ .name = str(name), .index = 0, .qualifier = qualifier };
    }
#if _DUMP_DESERIALIZE_LOG
    else {
        taul::log(make_stderr_logger(), "parser_rule::deserialize -> failed!");
    }
#endif
    return result;
}

void taul::internal::grammar_data::build_lookup() {
    TAUL_ASSERT(_lookup.empty());
    for (const auto& I : _lprs) _lookup[I.name] = entry{ true, I.index };
    for (const auto& I : _pprs) _lookup[I.name] = entry{ false, I.index };
}

void taul::internal::grammar_data::serialize(buff& b) const {
    // TODO: the size_t(0) below is the TAUL internal API serialization version number, so that in the future
    //       we can impl replacements for how serialization works w/out necessarily breaking existing usages
    b.write(size_t(1), _lprs.size(), _pprs.size());
    for (const auto& I : _lprs) I.serialize(b);
    for (const auto& I : _pprs) I.serialize(b);
    _lpr_pt.serialize(b);
    _ppr_pt.serialize(b);
}

std::optional<taul::internal::grammar_data> taul::internal::grammar_data::deserialize(buff_reader& b) {
    auto rdr = b.fork();
    std::optional<grammar_data> result{};
    if (const auto aa = rdr.read<size_t, size_t, size_t>()) {
        // TODO: internal_api_version is what's described in the TODO in stated in serialize
        const auto& [internal_api_version, expected_lprs, expected_pprs] = *aa;
#if _DUMP_DESERIALIZE_LOG
        taul::log(make_stderr_logger(), "grammar_data::deserialize -> internal_api_version: {}, expected_lprs: {}, expected_pprs: {}", internal_api_version, expected_lprs, expected_pprs);
#endif
        grammar_data temp{};
        for (size_t i = 0; i < expected_lprs; i++) {
            if (auto bb = lexer_rule::deserialize(rdr)) {
                bb->index = i;
                temp._lprs.push_back(std::move(*bb));
            }
            else {
#if _DUMP_DESERIALIZE_LOG
                log(make_stderr_logger(), "grammar_data::deserialize -> failed! (expected LPR not found)");
#endif
                return std::nullopt;
            }
        }
        for (size_t i = 0; i < expected_pprs; i++) {
            if (auto bb = parser_rule::deserialize(rdr)) {
                bb->index = i;
                temp._pprs.push_back(std::move(*bb));
            }
            else {
#if _DUMP_DESERIALIZE_LOG
                log(make_stderr_logger(), "grammar_data::deserialize -> failed! (expected PPR not found)");
#endif
                return std::nullopt;
            }
        }
        temp._lpr_id_allocs.first_helper_id = lpr_id(expected_lprs);
        temp._ppr_id_allocs.first_helper_id = ppr_id(expected_pprs);
        if (auto lpr_pt = parse_table<glyph>::deserialize(rdr)) temp._lpr_pt = std::move(*lpr_pt);
        else {
#if _DUMP_DESERIALIZE_LOG
            log(make_stderr_logger(), "grammar_data::deserialize -> failed! (lexer parse_table not found)");
#endif
            return std::nullopt;
        }
        if (auto ppr_pt = parse_table<token>::deserialize(rdr)) temp._ppr_pt = std::move(*ppr_pt);
        else {
#if _DUMP_DESERIALIZE_LOG
            log(make_stderr_logger(), "grammar_data::deserialize -> failed! (parser parse_table not found)");
#endif
            return std::nullopt;
        }
        temp.build_lookup();
        // if didn't fail up to this point, assign result
        result = std::move(temp);
#if _DUMP_DESERIALIZE_LOG
        log(make_stderr_logger(), "grammar_data::deserialize -> done!");
#endif
    }
    if (result) b.commit(rdr);
#if _DUMP_DESERIALIZE_LOG
    else {
        taul::log(make_stderr_logger(), "grammar_data::deserialize -> failed!");
    }
#endif
    return result;
}

