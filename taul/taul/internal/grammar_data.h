

#pragma once


#include <string_view>
#include <unordered_map>

#include "../str.h"
#include "../qualifier.h"

#include "nonterminal_id_alloc.h"
#include "parse_table.h"


namespace taul::internal {


    struct lexer_rule final {
        str             name;
        std::size_t     index       = -1;
        taul::qualifier qualifier   = taul::qualifier::none;


        std::string fmt() const;
    };

    struct parser_rule final {
        str             name;
        std::size_t     index       = -1;


        std::string fmt() const;
    };


    struct grammar_data final {
        struct entry final {
            bool        lpr;    // if index is for LPR (and if not, for PPR)
            std::size_t index;
        };


        nonterminal_id_allocs<glyph> _lpr_id_allocs;
        nonterminal_id_allocs<token> _ppr_id_allocs;
        parse_table<glyph> _lpr_pt;
        parse_table<token> _ppr_pt;

        std::vector<lexer_rule> _lprs;
        std::vector<parser_rule> _pprs;
        std::unordered_map<str, entry> _lookup;


        void build_lookup();
    };
}

