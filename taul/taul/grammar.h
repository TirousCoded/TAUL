

#pragma once


#include <ostream>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <span>

#include "logger.h"
#include "bias.h"
#include "rules.h"
#include "lexer.h"


namespace taul {


    namespace internal {


        struct for_internal_use_tag final {};

        struct grammar_data;
    }


    class grammar final {
    public:

        grammar();
        grammar(const grammar& x);
        grammar(grammar&& x) noexcept;

        ~grammar() noexcept = default;

        grammar& operator=(const grammar& rhs);
        grammar& operator=(grammar&& rhs) noexcept;


        // internal, do not use
        grammar(
            internal::for_internal_use_tag, 
            std::shared_ptr<internal::grammar_data> data);


        bias bias() const noexcept;


        std::span<const lexer_rule> lprs() const noexcept;
        std::span<const parser_rule> pprs() const noexcept;


        // for code below w/ 'const char* name', behaviour is undefined if name == nullptr


        // the below throw std::out_of_range if there is no LPR/PPR w/ name

        const lexer_rule& lpr(const std::string& name) const;
        const lexer_rule& lpr(std::string_view name) const;
        const lexer_rule& lpr(const char* name) const;

        const parser_rule& ppr(const std::string& name) const;
        const parser_rule& ppr(std::string_view name) const;
        const parser_rule& ppr(const char* name) const;


        // these are used to get lexers/parsers associated w/ LPRs/PPRs

        // the below throw std::out_of_range if there is no LPR/PPR w/ name

        taul::lexer lexer(const std::string& name) const;
        taul::lexer lexer(std::string_view name) const;
        taul::lexer lexer(const char* name) const;


        // contains returns if *either* and LPR or a PPR exists w/ name, w/
        // overloads for checking only for LPRs or PPRs

        bool contains(const std::string& name) const noexcept;
        bool contains(std::string_view name) const noexcept;
        bool contains(const char* name) const noexcept;

        bool contains_lpr(const std::string& name) const noexcept;
        bool contains_lpr(std::string_view name) const noexcept;
        bool contains_lpr(const char* name) const noexcept;

        bool contains_ppr(const std::string& name) const noexcept;
        bool contains_ppr(std::string_view name) const noexcept;
        bool contains_ppr(const char* name) const noexcept;


        std::string fmt(std::string_view tab = "    ") const;


    private:

        std::shared_ptr<internal::grammar_data> _data;
    };


    namespace internal {


        struct grammar_data final {
            // please notice that the use of std::string_view below for maps
            // means that we need to be careful to consider how SSO may cause
            // string address changes during _lprs/_pprs reallocs
            //
            // this isn't something to worry about really, due to how we're 
            // building grammar objects, but in case we ever change it, do 
            // keep this in mind

            taul::bias _bias;

            std::vector<lexer_rule> _lprs;
            std::vector<parser_rule> _pprs;

            struct entry final {
                bool        lpr;    // if index is for lpr (and if not, for ppr)
                std::size_t index;
            };

            // using a clever trick here where using an std::string_view to
            // the _lprs/_pprs entry 'name' field memory lets us use string
            // views to perform lookups w/out alloc

            std::unordered_map<std::string_view, entry> _lookup;


            void build_lookup();
        };
    }
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

