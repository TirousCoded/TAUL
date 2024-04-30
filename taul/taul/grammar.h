

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


    struct spec;


    //class lpr_association_error;
    //class ppr_association_error;

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


        lpr_ref();
        lpr_ref(const lpr_ref& other);
        lpr_ref(lpr_ref&& other) noexcept;

        ~lpr_ref() noexcept = default;

        lpr_ref& operator=(const lpr_ref& rhs);
        lpr_ref& operator=(lpr_ref&& rhs) noexcept;


        str name() const;
        std::size_t index() const;
        qualifier qualifier() const;

        // TODO: id has not been unit tested

        symbol_id id() const noexcept;

        
        // first_set and follow_set return the FIRST/FOLLOW sets of the LPR

        // prefix_set returns the actual set used to generate the parse table,
        // which is a function of the FIRST and FOLLOW set, according to the
        // rules of LL(1) parse table generation

        const glyph_set& first_set() const noexcept;
        const glyph_set& follow_set() const noexcept;
        const glyph_set& prefix_set() const noexcept;


        static bool equal(const lpr_ref& lhs, const lpr_ref& rhs) noexcept;


        std::string fmt() const;
        std::size_t hash() const noexcept;


    private:

        const internal::lexer_rule* _rule;
        const internal::grammar_data* _gram;


        lpr_ref(const internal::lexer_rule* a, const internal::grammar_data* b) noexcept;
    };

    class ppr_ref final {
    public:

        friend class grammar;


        ppr_ref();
        ppr_ref(const ppr_ref& other);
        ppr_ref(ppr_ref&& other) noexcept;

        ~ppr_ref() noexcept = default;

        ppr_ref& operator=(const ppr_ref& rhs);
        ppr_ref& operator=(ppr_ref&& rhs) noexcept;


        str name() const;
        std::size_t index() const;

        // TODO: id has not been unit tested

        symbol_id id() const noexcept;


        // first_set and follow_set return the FIRST/FOLLOW sets of the LPR

        // prefix_set returns the actual set used to generate the parse table,
        // which is a function of the FIRST and FOLLOW set, according to the
        // rules of LL(1) parse table generation

        const token_set& first_set() const noexcept;
        const token_set& follow_set() const noexcept;
        const token_set& prefix_set() const noexcept;


        static bool equal(const ppr_ref& lhs, const ppr_ref& rhs) noexcept;


        std::string fmt() const;
        std::size_t hash() const noexcept;


    private:

        const internal::parser_rule* _rule;
        const internal::grammar_data* _gram;


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
        inline std::size_t operator()(const taul::lpr_ref& s) const noexcept {
            return s.hash();
        }
    };
}

namespace std {
    template<>
    struct std::hash<taul::ppr_ref> {
        inline std::size_t operator()(const taul::ppr_ref& s) const noexcept {
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


        grammar();
        grammar(const grammar& x);
        grammar(grammar&& x) noexcept;

        ~grammar() noexcept = default;

        grammar& operator=(const grammar& rhs);
        grammar& operator=(grammar&& rhs) noexcept;


        // lprs/pprs return the number of LPRs/PPRs

        std::size_t lprs() const noexcept;
        std::size_t pprs() const noexcept;

        // nonsupport_lprs is useful for writing regression tests for grammars

        std::size_t nonsupport_lprs() const noexcept;


        // lpr/ppr return LPR/PPR refs by index

        // lpr/ppr throw std::out_of_range if index is out-of-bounds

        lpr_ref lpr_at(std::size_t index) const;
        ppr_ref ppr_at(std::size_t index) const;

        // lpr/ppr return LPR/PPR refs by name

        std::optional<lpr_ref> lpr(const str& name) const;
        std::optional<ppr_ref> ppr(const str& name) const;


        // these are used to check for the existence of LPRs/PPRs

        bool has_rule(const str& name) const noexcept;
        bool has_lpr(const str& name) const noexcept;
        bool has_ppr(const str& name) const noexcept;


        // is_associated checks if rule is associated w/ this grammar

        // TODO: these have not been unit tested

        bool is_associated(lpr_ref rule) const noexcept;
        bool is_associated(ppr_ref rule) const noexcept;


        std::string fmt(const char* tab = "    ") const;
        std::string fmt_internals(const char* tab = "    ") const;


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

