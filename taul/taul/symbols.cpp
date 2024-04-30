

#include "symbols.h"

#include "asserts.h"


using namespace taul::string_literals;


std::optional<taul::unicode_t> taul::glyph::cp() const noexcept {
    return symbol_traits<glyph>::preferred(id);
}

bool taul::glyph::equal(const glyph& other) const noexcept {
    return
        id == other.id &&
        pos == other.pos &&
        len == other.len;
}

std::string taul::glyph::fmt() const {
    return std::format("{} {}", fmt_pos_and_len(pos, len), id);
}

taul::glyph taul::glyph::normal(unicode_t cp, source_pos pos, source_len len) noexcept {
    glyph result{};
    result.id = cp_id(cp);
    result.pos = pos;
    result.len = len;
    return result;
}

taul::glyph taul::glyph::end(source_pos pos) noexcept {
    glyph result{};
    result.id = end_cp_id;
    result.pos = pos;
    result.len = 0;
    return result;
}

taul::str taul::token::str(taul::str src) const noexcept {
    TAUL_ASSERT(pos <= src.length());
    TAUL_ASSERT(len <= src.length() - pos);
    return src.substr(pos, len);
}

bool taul::token::equal(const token& other) const noexcept {
    return
        id == other.id &&
        pos == other.pos &&
        len == other.len;
}

std::string taul::token::fmt() const {
    return
        is_normal()
        ? std::format("{} {} {}", fmt_pos_and_len(pos, len), id, lpr.value().name())
        : std::format("{} {}", fmt_pos_and_len(pos, len), id);
}

taul::token taul::token::normal(lpr_ref lpr, source_pos pos, source_len len) noexcept {
    token result{};
    result.id = lpr_id(lpr.index());
    result.pos = pos;
    result.len = len;
    result.lpr = std::make_optional(lpr);
    return result;
}

taul::token taul::token::normal(const grammar& gram, taul::str name, source_pos pos, source_len len) {
    TAUL_ASSERT(gram.has_lpr(name));
    return normal(gram.lpr(name).value(), pos, len);
}

taul::token taul::token::end(source_pos pos) noexcept {
    token result{};
    result.id = end_lpr_id;
    result.pos = pos;
    result.len = 0;
    return result;
}

taul::token taul::token::failure(source_pos pos, source_len len) noexcept {
    token result{};
    result.id = failure_lpr_id;
    result.pos = pos;
    result.len = len;
    return result;
}

