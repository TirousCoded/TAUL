

#pragma once


#include <cstdint>
#include <ostream>
#include <string>
#include <format>


namespace taul {


    enum class bias : std::uint8_t {
        fl,     // first longest
        fs,     // first shortest
        ll,     // last longest
        ls,     // last shortest
        f,      // first
        l,      // last
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

