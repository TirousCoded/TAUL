

#include "symbol_id.h"

#include "asserts.h"
#include "unicode.h"
#include "string_and_charset.h"


// TODO:
//      in loader.h, currently we have a *layer over* fmt_symbol_id used to add LPR/PPR names to the fmt string
//
//      in future refactors, try and find a more *official* way to do this, w/out us having some extra
//      fmt code for this at some random other point in our system
//
//      also, extend this to things like fmt for symbol_range too

std::string taul::fmt_symbol_id(symbol_id x) {
    if (is_cp_id(x)) {
        if (is_end_id(x)) return "cp:<end-of-input>";
        else if (is_ascii(unicode_t(x))) return std::format("cp:'{}'", fmt_taul_char(char(x)));
        // we could use fmt_taul_char for ALL Unicode, but I think I prefer fmt_unicode for non-ASCII instead
        else return std::format("cp:{}", fmt_unicode(unicode_t(x)));
    }
    else if (is_lpr_id(x)) {
        if (is_end_id(x)) return "lpr:<end-of-input>";
        if (is_failure_id(x)) return "lpr:<failure>";
        else return std::format("lpr:{}", std::size_t(x - TAUL_FIRST_ID(lpr)));
    }
    else if (is_ppr_id(x)) {
        return std::format("ppr:{}", std::size_t(x - TAUL_FIRST_ID(ppr)));
    }
    return std::format("<illegal ({})>", size_t(x));
}

