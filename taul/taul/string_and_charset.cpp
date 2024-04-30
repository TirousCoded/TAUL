

#include "string_and_charset.h"

#include <vector>

#include "asserts.h"

#include "internal/util.h"


std::optional<char> taul::parse_taul_char(std::string_view x, std::size_t* chars) {
    /*
        \0              <- Null
        \a				<- Bell (Alert)
        \b				<- Backspace
        \f				<- Form Feed
        \n				<- New Line
        \r				<- Carriage Return
        \t				<- Horizontal Tab
        \v				<- Vertical Tab
        \'				<- Single Quotation
        \]				<- Right Square Bracket
        \-				<- Minus
        \\				<- Backslash
        \xhh			<- Hex Literal (8-bit)
    */
    if (x.empty()) {
        return std::nullopt;
    }
    else if (x[0] == '\\') {
        if (x.length() == 1) {
            if (chars) {
                *chars = 1;
            }
            return std::make_optional<char>(x.front());
        }
        else {
            if (chars) {
                *chars = 2;
            }
            char result{};
            switch (x[1]) {
            case '0':   result = '\0';  break;
            case 'a':   result = '\a';  break;
            case 'b':   result = '\b';  break;
            case 'f':   result = '\f';  break;
            case 'n':   result = '\n';  break;
            case 'r':   result = '\r';  break;
            case 't':   result = '\t';  break;
            case 'v':   result = '\v';  break;
            case '\'':  result = '\'';  break;
            case ']':   result = ']';   break;
            case '-':   result = '-';   break;
            case '\\':  result = '\\';  break;
            case 'x':
            {
                if (x.length() >= 4) {
                    std::size_t chr0 = internal::hex_digit(x[2]);
                    std::size_t chr1 = internal::hex_digit(x[3]);
                    if (chr0 == 16 || chr1 == 16) {
                        result = 'x';
                    }
                    else {
                        if (chars) {
                            *chars = 4;
                        }
                        result = char(chr0 * 16 + chr1);
                    }
                }
                else {
                    result = 'x';
                }
            }
            break;
            default:    result = x[1];  break;
            }
            return std::make_optional<char>(result);
        }
    }
    else {
        if (chars) {
            *chars = 1;
        }
        return std::make_optional<char>(x.front());
    }
}

std::string taul::fmt_taul_char(char x, bool string_not_charset) {
    /*
        \0              <- Null
        \a				<- Bell (Alert)
        \b				<- Backspace
        \f				<- Form Feed
        \n				<- New Line
        \r				<- Carriage Return
        \t				<- Horizontal Tab
        \v				<- Vertical Tab
        \'				<- Single Quotation
        \]				<- Right Square Bracket
        \-				<- Minus
        \\				<- Backslash
        \xhh			<- Hex Literal (8-bit)
    */
    switch (x) {
    case '\0': return "\\0"; break;
    case '\a': return "\\a"; break;
    case '\b': return "\\b"; break;
    case '\f': return "\\f"; break;
    case '\n': return "\\n"; break;
    case '\r': return "\\r"; break;
    case '\t': return "\\t"; break;
    case '\v': return "\\v"; break;
    case '\'':
    {
        return
            string_not_charset
            ? "\\'"
            : "'";
    }
    break;
    case ']':
    {
        return
            string_not_charset
            ? "]"
            : "\\]";
    }
    break;
    case '-':
    {
        return
            string_not_charset
            ? "-"
            : "\\-";
    }
    break;
    case '\\': return "\\\\"; break;
    default:
    {
        // visible ASCII range
        if (x >= ' ' && x <= '~') {
            return std::string(1, x);
        }
        else {
            constexpr const char* hex_digit = "0123456789abcdef";
            std::size_t low = std::size_t(x) % 16;
            std::size_t high = (std::size_t(x) - low) / 16;
            return std::format("\\x{}{}", hex_digit[high], hex_digit[low]);
        }
    }
    break;
    }
}

std::string taul::parse_taul_string(std::string_view x) {
    std::string result{};
    result.reserve(x.length());
    for (std::size_t i = 0; i < x.length();) {
        std::size_t chars{};
        auto chr = parse_taul_char(x.substr(i), &chars);
        TAUL_ASSERT(chr);
        result += *chr;
        i += chars;
    }
    return result;
}

std::string taul::fmt_taul_string(std::string_view x) {
    std::string result{};
    result.reserve(x.length());
    for (const auto& I : x) {
        result += fmt_taul_char(I);
    }
    return result;
}

std::string taul::parse_taul_charset(std::string_view x) {
    std::string result{};
    result.reserve(x.length());
    std::array<std::optional<char>, 3> matches{};
    std::array<std::size_t, 3> match_chars{};
    std::size_t position = 0;
    std::size_t processed = 0;
    auto _initial =
        [&]() {
        for (std::size_t i = 0; i < 3; i++) {
            std::size_t chars{};
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
        std::size_t chars{};
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
        [&](std::size_t n) {
        TAUL_ASSERT(n <= 3);
        for (std::size_t i = 0; i < n; i++) {
            _advance_once();
        }
        };
    auto _match_char_range =
        [&]() -> bool {
        if (!matches[0]) return false;
        if (!matches[1]) return false;
        if (!matches[2]) return false;
        if (*matches[1] != '-') return false;
        if (match_chars[1] != 1) return false; // can't be escaped
        if (*matches[0] <= *matches[2]) {
            result += *matches[0];
            result += *matches[2];
        }
        else {
            result += *matches[2];
            result += *matches[0];
        }
        _advance(3);
        return true;
        };
    auto _match_char =
        [&]() {
        if (!matches[0]) return;
        result += *matches[0];
        result += *matches[0];
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
    return result;
}

std::string taul::fmt_taul_charset(std::string_view x) {
    TAUL_ASSERT(x.length() % 2 == 0);
    auto _fmt = 
        [&](char x, bool at_start_or_end) -> std::string {
        if (x != '-') {
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
    for (std::size_t i = 0; i < x.length(); i += 2) {
        const bool at_start = i == 0;
        const bool at_end = i == x.length() - 2;
        const bool at_start_or_end = at_start || at_end;
        const auto& low = x[i];
        const auto& high = x[i + 1];
        TAUL_ASSERT(low <= high);
        result += _fmt(low, at_start_or_end);
        if (low != high) {
            result += '-';
            result += _fmt(high, at_start_or_end);
        }
    }
    return result;
}

bool taul::in_charset_str(char x, std::string_view charset) noexcept {
    return internal::where_in_charset_str(x, charset) < charset.length();
}

bool taul::internal::in_char_range(char x, char low, char high) noexcept {
    TAUL_ASSERT(low <= high);
    return x >= low && x <= high;
}

std::size_t taul::internal::where_in_charset_str(char x, std::string_view charset) noexcept {
    TAUL_ASSERT(charset.length() % 2 == 0);
    for (std::size_t i = 0; i < charset.length(); i += 2) {
        if (in_char_range(x, charset[i], charset[i + 1])) {
            return i;
        }
    }
    return charset.length();
}

