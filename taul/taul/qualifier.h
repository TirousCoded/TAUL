

#pragma once


#include <cstdint>
#include <ostream>
#include <string>
#include <format>


namespace taul {


    enum class qualifier : std::uint8_t {
        none,
        skip,       // skip qualified lexers participates in grammar-wide usage, but is discarded during tokenization
        support,    // support qualified lexers does not participate in grammar-wide usage
    };

    std::string fmt_qualifier(qualifier x);
}


template<>
struct std::formatter<taul::qualifier> final : std::formatter<std::string> {
    auto format(taul::qualifier x, format_context& ctx) const {
        return formatter<string>::format(taul::fmt_qualifier(x), ctx);
    }
};

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::qualifier& x) {
        return stream << taul::fmt_qualifier(x);
    }
}

