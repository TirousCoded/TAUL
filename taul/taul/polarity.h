

#pragma once


#include <cstdint>
#include <ostream>
#include <string>
#include <format>


namespace taul {


    enum class polarity : std::uint8_t {
        positive,
        negative,
    };

    std::string fmt_polarity(polarity x);
}


template<>
struct std::formatter<taul::polarity> final : std::formatter<std::string> {
    auto format(taul::polarity x, format_context& ctx) const {
        return formatter<string>::format(taul::fmt_polarity(x), ctx);
    }
};

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::polarity& x) {
        return stream << taul::fmt_polarity(x);
    }
}

