

#pragma once


#include "symbol_id.h"
//#include "symbols.h"
//#include "symbol_traits.h"


namespace taul {


    // just need this to deal w/ some cyclical include problems...


    struct glyph;
    struct token;

    template<typename Symbol>
    struct symbol_traits;


    template<typename Symbol, typename Traits = symbol_traits<Symbol>>
    class symbol_set;

    template<typename Symbol, typename Traits = symbol_traits<Symbol>>
    struct symbol_set_iterator;


    using glyph_set = symbol_set<glyph>;
    using token_set = symbol_set<token>;
}

