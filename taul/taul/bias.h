

#pragma once


#include <cstdint>
#include <ostream>
#include <string>
#include <format>


namespace taul {


    enum class bias : std::uint8_t {
        first_longest,
        first_shortest,
        last_longest,
        last_shortest,
        first,
        last,

        longest = first_longest,
        shortest = first_shortest,
    };

    std::string fmt_bias(bias x);
}


template<>
struct std::formatter<taul::bias> final : std::formatter<std::string> {
    auto format(taul::bias x, format_context& ctx) const {
        return formatter<string>::format(taul::fmt_bias(x), ctx);
    }
};

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::bias& x) {
        return stream << taul::fmt_bias(x);
    }
}

