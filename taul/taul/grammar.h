

#pragma once


#include <stddef.h>
#include <stdexcept>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <span>
#include <optional>
#include <format>
#include <functional>

#include "logger.h"
#include "str.h"
#include "qualifier.h"
#include "symbol_set_decls.h"


namespace taul {


    namespace internal {
        struct lexer_rule;
        struct parser_rule;
        struct grammar_data;
    }


    class spec;


    class lpr_ref;
    class ppr_ref;

    class grammar;


    // in taul::grammar, lexer/parser production rules will be referred
    // to as 'LPRs', and 'PPRs', respectively


    // lpr_ref/ppr_ref encapsulate immutable references to LPRs/PPRs

    // these handles become dangling if they outlive their grammar

    // these handles are non-nullable, and so must be wrapped in std::optional
    // in order to have a null state

    // these handles do have default init states, but they should not be used,
    // and lpr_ref/ppr_ref behaviour is undefined in these cases


    class lpr_ref final {
    public:

        friend class grammar;


        lpr_ref() = default;
        lpr_ref(const lpr_ref&) = default;
        lpr_ref(lpr_ref&&) noexcept = default;

        ~lpr_ref() noexcept = default;

        lpr_ref& operator=(const lpr_ref&) = default;
        lpr_ref& operator=(lpr_ref&&) noexcept = default;


        str name() const noexcept;
        size_t index() const noexcept;
        symbol_id id() const noexcept;
        qualifier qualifier() const noexcept;

        
        // first_set and follow_set return the FIRST/FOLLOW sets of the LPR

        // prefix_set returns the actual set used to generate the parse table,
        // which is a function of the FIRST and FOLLOW set, according to the
        // rules of LL(1) parse table generation

        const glyph_set& first_set() const noexcept;
        const glyph_set& follow_set() const noexcept;
        const glyph_set& prefix_set() const noexcept;


        bool equal(const lpr_ref& other) const noexcept;


        std::string fmt() const;
        size_t hash() const noexcept;


    private:

        const internal::lexer_rule* _rule = nullptr;
        const internal::grammar_data* _gram = nullptr;


        lpr_ref(const internal::lexer_rule* a, const internal::grammar_data* b) noexcept;
    };

    class ppr_ref final {
    public:

        friend class grammar;


        ppr_ref() = default;
        ppr_ref(const ppr_ref&) = default;
        ppr_ref(ppr_ref&&) noexcept = default;

        ~ppr_ref() noexcept = default;

        ppr_ref& operator=(const ppr_ref&) = default;
        ppr_ref& operator=(ppr_ref&&) noexcept = default;


        // TODO: for 'qualifier', our unit tests only really cover cases where it
        //       returns taul::qualifier::none, w/ us not testing situations where
        //       something else is returned, so improve these tests at some point

        str name() const noexcept;
        size_t index() const noexcept;
        symbol_id id() const noexcept;
        qualifier qualifier() const noexcept;


        // first_set and follow_set return the FIRST/FOLLOW sets of the LPR

        // prefix_set returns the actual set used to generate the parse table,
        // which is a function of the FIRST and FOLLOW set, according to the
        // rules of LL(1) parse table generation

        const token_set& first_set() const noexcept;
        const token_set& follow_set() const noexcept;
        const token_set& prefix_set() const noexcept;


        bool equal(const ppr_ref& other) const noexcept;


        std::string fmt() const;
        size_t hash() const noexcept;


    private:

        const internal::parser_rule* _rule = nullptr;
        const internal::grammar_data* _gram = nullptr;


        ppr_ref(const internal::parser_rule* a, const internal::grammar_data* b) noexcept;
    };


    bool operator==(const lpr_ref& lhs, const lpr_ref& rhs) noexcept;
    bool operator==(const ppr_ref& lhs, const ppr_ref& rhs) noexcept;

    bool operator!=(const lpr_ref& lhs, const lpr_ref& rhs) noexcept;
    bool operator!=(const ppr_ref& lhs, const ppr_ref& rhs) noexcept;
}


template<>
struct std::formatter<taul::lpr_ref> final : std::formatter<std::string> {
    auto format(const taul::lpr_ref& x, format_context& ctx) const {
        return formatter<string>::format(x.fmt(), ctx);
    }
};

template<>
struct std::formatter<taul::ppr_ref> final : std::formatter<std::string> {
    auto format(const taul::ppr_ref& x, format_context& ctx) const {
        return formatter<string>::format(x.fmt(), ctx);
    }
};

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::lpr_ref& x) {
        return stream << x.fmt();
    }
}

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::ppr_ref& x) {
        return stream << x.fmt();
    }
}

namespace std {
    template<>
    struct std::hash<taul::lpr_ref> {
        inline size_t operator()(const taul::lpr_ref& s) const noexcept {
            return s.hash();
        }
    };
}

namespace std {
    template<>
    struct std::hash<taul::ppr_ref> {
        inline size_t operator()(const taul::ppr_ref& s) const noexcept {
            return s.hash();
        }
    };
}


namespace taul {


    namespace internal {
        const grammar_data& launder_grammar_data(const grammar& x) noexcept;
    }


    // grammars are handles to shared underlying state

    class grammar final {
    public:

        friend const internal::grammar_data& internal::launder_grammar_data(const grammar& x) noexcept;


        // internal, do not use
        grammar(internal::grammar_data&& gramdat);


        grammar() = default;
        grammar(const grammar&) = default;
        grammar(grammar&&) noexcept = default;

        ~grammar() noexcept = default;

        grammar& operator=(const grammar&) = default;
        grammar& operator=(grammar&&) noexcept = default;


        // lprs/pprs return the number of LPRs/PPRs

        size_t lprs() const noexcept;
        size_t pprs() const noexcept;

        // nonsupport_lprs is useful for writing regression tests for grammars

        size_t nonsupport_lprs() const noexcept;


        // lpr/ppr return LPR/PPR refs by index

        // lpr/ppr throw std::out_of_range if index is out-of-bounds

        lpr_ref lpr_at(size_t index) const;
        ppr_ref ppr_at(size_t index) const;

        // TODO: these *by index* overloads have not been unit tested

        std::optional<lpr_ref> lpr(size_t index) const noexcept;
        std::optional<ppr_ref> ppr(size_t index) const noexcept;

        // lpr/ppr return LPR/PPR refs by name

        std::optional<lpr_ref> lpr(const str& name) const noexcept;
        std::optional<ppr_ref> ppr(const str& name) const noexcept;


        // these are used to check for the existence of LPRs/PPRs

        bool has_rule(const str& name) const noexcept;
        bool has_lpr(const str& name) const noexcept;
        bool has_ppr(const str& name) const noexcept;


        // is_associated checks if rule is associated w/ this grammar

        bool is_associated(lpr_ref rule) const noexcept;
        bool is_associated(ppr_ref rule) const noexcept;


        std::string fmt(const char* tab = "    ") const;
        std::string fmt_internals(const char* tab = "    ") const;


        // TODO: while unit tested, the tests for serialize/deserialize ARE NOT
        //       comprehensive, and thus DO NOT cover ALL aspects of what info
        //       an impl must be able to keep track of during serialization

        // serialize serializes the grammar, returning a string encoding
        // its data, w/ this string being portable across processes/files

        std::string serialize() const;

        // deserialize attempts to generate a grammar from string x

        static std::optional<grammar> deserialize(std::string_view x);

        template<std::convertible_to<std::string_view> T>
        static inline std::optional<grammar> deserialize(const T& x) {
            return deserialize(std::string_view(x));
        }


    private:

        std::shared_ptr<internal::grammar_data> _data;
    };
}


template<>
struct std::formatter<taul::grammar> final : std::formatter<std::string> {
    auto format(const taul::grammar& x, format_context& ctx) const {
        return formatter<string>::format(x.fmt(), ctx);
    }
};

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::grammar& x) {
        return stream << x.fmt();
    }
}

