

#pragma once


#include <stddef.h>
#include <ostream>
#include <format>

#include "unicode.h"
#include "str.h"
#include "source_code.h"
#include "grammar.h"
#include "symbol_id.h"


namespace taul {


    // due to how simple our semantics are, we don't need a full-on traits type,
    // so instead we'll use the below function to *assert* statically that a given
    // symbol type provides the expected functionality

    template<typename Symbol>
    inline symbol_id get_symbol_id(const Symbol& x) noexcept { return x.id; }


    struct glyph final {
        symbol_id id = end_cp_id; // behaviour undefined if symbol ID is not codepoint ID
        source_pos pos = 0;
        source_len len = 0; // glyph len may exceed 1 for things like CRLF, or multibyte encoded characters


        // TODO: these have not been unit tested

        // low_pos returns pos
        // high_pos returns pos + len (as source_pos)

        inline source_pos low_pos() const noexcept { return pos; }
        inline source_pos high_pos() const noexcept { return pos + len; }


        // cp returns the codepoint associated w/ the glyph, if any

        // end-of-input glyphs will have no associated codepoint

        std::optional<unicode_t> cp() const noexcept;


        inline bool is_normal() const noexcept { TAUL_ASSERT(taul::is_cp_id(id)); return taul::is_normal_id(id); }
        inline bool is_end() const noexcept { TAUL_ASSERT(taul::is_cp_id(id)); return taul::is_end_id(id); }

        // explicit convert to bool checks is_normal

        inline explicit operator bool() const noexcept { return is_normal(); }


        // behaviour is undefined if semantically indeterminate fields are not in their default init state
        // behaviour is undefined if symbol ID is not codepoint ID

        bool equal(const glyph& other) const noexcept;

        inline bool operator==(const glyph& rhs) const noexcept { return equal(rhs); }
        inline bool operator!=(const glyph& rhs) const noexcept { return !equal(rhs); }


        std::string fmt() const;


        static glyph normal(unicode_t cp, source_pos pos = 0, source_len len = 1) noexcept;
        static glyph end(source_pos pos = 0) noexcept;
    };

    struct token final {
        symbol_id id = end_lpr_id; // behaviour undefined if symbol ID is not LPR ID
        source_pos pos = 0;
        source_len len = 0;
        std::optional<lpr_ref> lpr = std::nullopt;


        // TODO: these have not been unit tested

        // low_pos returns pos
        // high_pos returns pos + len (as source_pos)

        inline source_pos low_pos() const noexcept { return pos; }
        inline source_pos high_pos() const noexcept { return pos + len; }


        // str returns the portion of source string src associated w/ the token
        
        // end-of-input and failure tokens correspond to empty strings

        // behaviour is undefined if src is not the correct source string to use

        taul::str str(str src) const noexcept;


        inline bool is_normal() const noexcept { TAUL_ASSERT(taul::is_lpr_id(id)); return taul::is_normal_id(id); }
        inline bool is_end() const noexcept { TAUL_ASSERT(taul::is_lpr_id(id)); return taul::is_end_id(id); }
        inline bool is_failure() const noexcept { TAUL_ASSERT(taul::is_lpr_id(id)); return taul::is_failure_id(id); }

        // explicit convert to bool checks is_normal

        inline explicit operator bool() const noexcept { return is_normal(); }


        // behaviour is undefined if semantically indeterminate fields are not in their default init state
        // behaviour is undefined if symbol ID is not LPR ID

        bool equal(const token& other) const noexcept;

        inline bool operator==(const token& rhs) const noexcept { return equal(rhs); }
        inline bool operator!=(const token& rhs) const noexcept { return !equal(rhs); }


        std::string fmt() const;


        static token normal(lpr_ref lpr, source_pos pos = 0, source_len len = 0) noexcept;

        // behaviour is undefined if gram does not have an LPR named name

        static token normal(const grammar& gram, taul::str name, source_pos pos = 0, source_len len = 0);

        // end-of-input tokens are always zero length

        static token end(source_pos pos = 0) noexcept;
        static token failure(source_pos pos = 0, source_len len = 0) noexcept;
    };


    template<>
    struct symbol_traits<glyph> final {
        using preferred_type = unicode_t;
        static constexpr symbol_id first_id = TAUL_FIRST_ID(cp);
        static constexpr symbol_id last_id = TAUL_LAST_ID(cp);
        static constexpr symbol_id end_of_input_id = end_cp_id;
        static constexpr symbol_id first_nonterminal_id = TAUL_FIRST_ID(lpr);
        static constexpr symbol_id last_nonterminal_id = TAUL_LAST_ID(lpr);
        inline static bool legal_id(symbol_id x) noexcept { return is_cp_id(x); }
        inline static bool legal_nonterminal_id(symbol_id x) noexcept { return is_lpr_id(x); }
        inline static symbol_id id(const preferred_type& x) noexcept { return cp_id(x); }
        inline static symbol_id id(const glyph& x) noexcept { return x.id; }
        static std::optional<preferred_type> preferred(symbol_id x) noexcept {
            if (!legal_id(x)) return std::nullopt;
            if (!is_normal_id(x)) return std::nullopt;
            return std::make_optional<preferred_type>(preferred_type(x) - preferred_type(first_id));
        }
    };

    template<>
    struct symbol_traits<token> final {
        using preferred_type = std::size_t; // LPR index in grammar
        static constexpr symbol_id first_id = TAUL_FIRST_ID(lpr);
        static constexpr symbol_id last_id = TAUL_LAST_ID(lpr);
        static constexpr symbol_id end_of_input_id = end_lpr_id;
        static constexpr symbol_id first_nonterminal_id = TAUL_FIRST_ID(ppr);
        static constexpr symbol_id last_nonterminal_id = TAUL_LAST_ID(ppr);
        inline static bool legal_id(symbol_id x) noexcept { return is_lpr_id(x); }
        inline static bool legal_nonterminal_id(symbol_id x) noexcept { return is_ppr_id(x); }
        inline static symbol_id id(const preferred_type& x) noexcept { return lpr_id(x); }
        inline static symbol_id id(const token& x) noexcept { return x.id; }
        static std::optional<preferred_type> preferred(symbol_id x) noexcept {
            if (!legal_id(x)) return std::nullopt;
            if (!is_normal_id(x)) return std::nullopt;
            return std::make_optional<preferred_type>(preferred_type(x) - preferred_type(first_id));
        }
    };
}


template<>
struct std::formatter<taul::glyph> final : std::formatter<std::string> {
    auto format(const taul::glyph& x, format_context& ctx) const {
        return formatter<string>::format(x.fmt(), ctx);
    }
};

template<>
struct std::formatter<taul::token> final : std::formatter<std::string> {
    auto format(const taul::token& x, format_context& ctx) const {
        return formatter<string>::format(x.fmt(), ctx);
    }
};

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::glyph& x) {
        return stream << x.fmt();
    }
}

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::token& x) {
        return stream << x.fmt();
    }
}

