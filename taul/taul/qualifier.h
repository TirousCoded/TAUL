

#pragma once


#include <cstdint>
#include <ostream>
#include <string>
#include <format>


namespace taul {


    enum class qualifier : std::uint8_t {
        none,
        skip,       // skip qualified LPRs participates in grammar-wide usage, but is discarded during tokenization
        support,    // support qualified LPRs does not participate in grammar-wide usage
        precedence, // precedence qualified PPRs allow for direct left-recursion in top-level alternatives

        num,        // not a valid qualifier
    };

    constexpr size_t qualifiers = size_t(qualifier::num);

    std::string fmt_qualifier(qualifier x);


    // these are here to summarize end-user usage

    constexpr qualifier skip        = qualifier::skip;
    constexpr qualifier support     = qualifier::support;
    constexpr qualifier precedence  = qualifier::precedence;
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

