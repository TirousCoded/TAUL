

#include "string_and_charset.h"

#include <vector>

#include "asserts.h"
#include "hex.h"
#include "encoding.h"

#include "internal/util.h"


std::optional<taul::unicode_t> taul::parse_taul_char(std::string_view x, size_t* chars) {
    taul::decoder<char> decoder(utf8, x);
    auto first_chr = decoder.next();
    if (!first_chr) {
        return std::nullopt;
    }
    else if (first_chr.value().cp != U'\\') {
        if (chars) {
            *chars = first_chr.value().bytes;
        }
        return std::make_optional<unicode_t>(first_chr.value().cp);
    }
    else {
        auto second_chr = decoder.next();
        if (!second_chr) {
            if (chars) {
                *chars = first_chr.value().bytes;
            }
            return std::make_optional<unicode_t>(first_chr.value().cp);
        }
        else {
            // *chars is default set to valid value for when we're processing
            // a 'basic escape seq', or a case of 'literalization', w/ *chars
            // value being updated later if not one of these cases (ie. a hex
            // escape seq)
            if (chars) {
                *chars = first_chr.value().bytes + second_chr.value().bytes;
            }
            unicode_t result{};
            switch (second_chr.value().cp) {
                // basic escape seqs
            case U'0':  result = U'\0'; break;
            case U'a':  result = U'\a'; break;
            case U'b':  result = U'\b'; break;
            case U'f':  result = U'\f'; break;
            case U'n':  result = U'\n'; break;
            case U'r':  result = U'\r'; break;
            case U't':  result = U'\t'; break;
            case U'v':  result = U'\v'; break;
            case U'\'': result = U'\''; break;
            case U']':  result = U']';  break;
            case U'-':  result = U'-';  break;
            case U'\\': result = U'\\'; break;
                // non-basic escape seqs
            case U'x':
            {
                auto temp = parse_hex_u8(x.substr(2));
                if (temp) {
                    if (chars) {
                        *chars = 4; // update *chars here
                    }
                    result = unicode_t(temp.value());
                }
                else {
                    result = U'x';
                }
            }
            break;
            case U'u':
            {
                auto temp = parse_hex_u16(x.substr(2));
                if (temp) {
                    if (chars) {
                        *chars = 6; // update *chars here
                    }
                    result = unicode_t(temp.value());
                }
                else {
                    result = U'u';
                }
            }
            break;
            case U'U':
            {
                auto temp = parse_hex_u32(x.substr(2));
                if (temp) {
                    if (chars) {
                        *chars = 10; // update *chars here
                    }
                    result = unicode_t(temp.value());
                }
                else {
                    result = U'U';
                }
            }
            break;
            default:
            {
                // not a recognized escape seq -> literalize
                result = second_chr.value().cp;
            }
            break;
            }
            return std::make_optional<unicode_t>(result);
        }
    }
}

std::string taul::fmt_taul_char(unicode_t x, bool string_not_charset) {
    switch (x) {
    case U'\0': return "\\0"; break;
    case U'\a': return "\\a"; break;
    case U'\b': return "\\b"; break;
    case U'\f': return "\\f"; break;
    case U'\n': return "\\n"; break;
    case U'\r': return "\\r"; break;
    case U'\t': return "\\t"; break;
    case U'\v': return "\\v"; break;
    case U'\'':
    {
        return
            string_not_charset
            ? "\\'"
            : "'";
    }
    break;
    case U']':
    {
        return
            string_not_charset
            ? "]"
            : "\\]";
    }
    break;
    case U'-':
    {
        return
            string_not_charset
            ? "-"
            : "\\-";
    }
    break;
    case U'\\': return "\\\\"; break;
    default:
    {
        if (is_visible_ascii(x)) {
            return std::string(1, char(x));
        }
        else if (x <= 0xff) {
            return std::format("\\x{}", fmt_hex_u8(uint8_t(x)));
        }
        else if (x <= 0xffff) {
            return std::format("\\u{}", fmt_hex_u16(uint16_t(x)));
        }
        else {
            return std::format("\\U{}", fmt_hex_u32(uint32_t(x)));
        }
    }
    break;
    }
}

std::string taul::parse_taul_string(std::string_view x) {
    encoder<char> encoder(utf8);
    for (size_t i = 0; i < x.length();) {
        size_t chars{};
        auto chr = parse_taul_char(x.substr(i), &chars);
        TAUL_ASSERT(chr);
        encoder.next(chr.value());
        i += chars;
    }
    return encoder.result();
}

std::string taul::fmt_taul_string(std::string_view x) {
    std::string result{};
    result.reserve(x.length());
    decoder<char> decoder(utf8, x);
    while (!decoder.done()) {
        result += fmt_taul_char(decoder.next().value().cp);
    }
    return result;
}

std::string taul::parse_taul_charset(std::string_view x) {
    // NOTE: basically, the below impls a kind of micro-parser w/ a cache
    //       of up to 3 decoded codepoint which it *matches* against
    //
    //       _initial inits the system, populating it initially, and then
    //       it proceeds until it's done, consuming input and matching
    // TODO: write doc comments to better explain what's going on below
    //       w/ regard to its particulars
    encoder<char> encoder(utf8);
    std::array<std::optional<unicode_t>, 3> matches{};
    std::array<size_t, 3> match_chars{};
    size_t position = 0;
    size_t processed = 0;
    auto _initial =
        [&]() {
        for (size_t i = 0; i < 3; i++) {
            size_t chars{};
            auto rr = parse_taul_char(x.substr(position), &chars);
            if (rr) {
                matches[i] = rr;
                match_chars[i] = chars;
                position += chars;
            }
            else {
                matches[i] = std::nullopt;
                match_chars[i] = 0;
            }
        }
        };
    auto _advance_once =
        [&]() {
        size_t chars{};
        auto rr = parse_taul_char(x.substr(position), &chars);
        if (rr) {
            position += chars;
        }
        if (matches[0]) {
            processed += match_chars[0];
        }
        matches[0] = std::move(matches[1]);
        matches[1] = std::move(matches[2]);
        matches[2] = std::move(rr);
        match_chars[0] = std::move(match_chars[1]);
        match_chars[1] = std::move(match_chars[2]);
        match_chars[2] = std::move(chars);
        };
    auto _advance =
        [&](size_t n) {
        TAUL_ASSERT(n <= 3);
        for (size_t i = 0; i < n; i++) {
            _advance_once();
        }
        };
    auto _match_char_range =
        [&]() -> bool {
        if (!matches[0]) return false;
        if (!matches[1]) return false;
        if (!matches[2]) return false;
        if (*matches[1] != U'-') return false;
        if (match_chars[1] != 1) return false; // can't be escaped
        if (*matches[0] <= *matches[2]) {
            encoder.next(*matches[0]);
            encoder.next(*matches[2]);
        }
        else {
            encoder.next(*matches[2]);
            encoder.next(*matches[0]);
        }
        _advance(3);
        return true;
        };
    auto _match_char =
        [&]() {
        if (!matches[0]) return;
        encoder.next(*matches[0]);
        encoder.next(*matches[0]);
        _advance(1);
        };
    auto _match =
        [&]() {
        if (!_match_char_range()) {
            _match_char();
        }
        };
    _initial();
    while (processed < position) {
        _match();
    }
    return encoder.result();
}

std::string taul::fmt_taul_charset(std::string_view x) {
    auto _fmt = 
        [&](unicode_t x, bool at_start_or_end) -> std::string {
        if (x != U'-') {
            return fmt_taul_char(x, false);
        }
        else if (!at_start_or_end) {
            return fmt_taul_char(x, false);
        }
        else {
            return "-";
        }
        };
    std::string result{};
    decoder<char> decoder(utf8, x);
    bool is_at_start = true;
    while (!decoder.done()) {
        const auto low = decoder.next().value();
        // charset string is malformed if not composed of perfect pairs
        TAUL_ASSERT(!decoder.done());
        const auto high = decoder.next().value();

        const bool at_start = is_at_start;
        const bool at_end = decoder.done();
        const bool at_start_or_end = at_start || at_end;

        TAUL_ASSERT(low.cp <= high.cp);
        result += _fmt(low.cp, at_start_or_end);
        if (low.cp != high.cp) {
            result += '-';
            result += _fmt(high.cp, at_start_or_end);
        }

        is_at_start = false;
    }
    return result;
}

bool taul::in_charset_str(unicode_t x, std::string_view charset) noexcept {
    return internal::where_in_charset_str(x, charset) < charset.length();
}

bool taul::internal::in_char_range(unicode_t x, unicode_t low, unicode_t high) noexcept {
    TAUL_ASSERT(low <= high);
    return x >= low && x <= high;
}

size_t taul::internal::where_in_charset_str(unicode_t x, std::string_view charset) noexcept {
    decoder<char> decoder(utf8, charset);
    while (!decoder.done()) {
        const auto pos = decoder.pos(); // position of start of low/high pair
        const auto low = decoder.next().value();
        // charset string is malformed if not composed of perfect pairs
        TAUL_ASSERT(!decoder.done());
        const auto high = decoder.next().value();
        if (in_char_range(x, low.cp, high.cp)) return pos; // return pair pos on success
    }
    return charset.length(); // length of charset indicates fail
}

