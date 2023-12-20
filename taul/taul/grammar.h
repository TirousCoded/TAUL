

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
#include "parser.h"

#include "internal/util.h"


namespace taul {


    namespace internal {


        struct grammar_data;
        class grammar_wide_lexer_state;
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


        // this returns a "grammar-wide lexer" which evaluates w/ all LPRs
        // in the grammar

        taul::lexer full_lexer(bool cut_skip_tokens = true) const;

        operator taul::lexer() const;


        // these are used to get lexers/parsers associated w/ LPRs/PPRs

        // the below throw std::out_of_range if there is no LPR/PPR w/ name

        taul::lexer lexer(const std::string& name) const;
        taul::lexer lexer(std::string_view name) const;
        taul::lexer lexer(const char* name) const;

        taul::parser parser(const std::string& name) const;
        taul::parser parser(std::string_view name) const;
        taul::parser parser(const char* name) const;


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


namespace taul::internal {


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


        // this is stored here so we can access it quickly when full_lexer
        // is called, to help avoid heap alloc

        std::shared_ptr<grammar_wide_lexer_state> _gwls;


        void build_lookup();
        void build_gwls();
    };


    // IMPORTANT: do NOT give grammar_wide_lexer_state a strong reference to 
    //            taul::internal::grammar_data, as doing so will result in
    //            a strong reference cycle!!!

    class grammar_wide_lexer_state final : public taul::lexer_state {
    public:

        taul::bias _bias;
        std::span<const lexer_rule> _lprs;


        grammar_wide_lexer_state(const grammar_data& gd);
    };

    token base_grammar_wide_lexer_function(
        const std::shared_ptr<lexer_state>& state,
        std::string_view txt,
        source_pos offset,
        const std::shared_ptr<logger>& lgr,
        bool cut_skip_tokens);

    inline token grammar_wide_lexer_function_cut_skip_tokens(
        const std::shared_ptr<lexer_state>& state,
        std::string_view txt,
        source_pos offset,
        const std::shared_ptr<logger>& lgr) {
        return base_grammar_wide_lexer_function(state, txt, offset, lgr, true);
    }

    inline token grammar_wide_lexer_function_dont_cut_skip_tokens(
        const std::shared_ptr<lexer_state>& state,
        std::string_view txt,
        source_pos offset,
        const std::shared_ptr<logger>& lgr) {
        return base_grammar_wide_lexer_function(state, txt, offset, lgr, false);
    }
}

