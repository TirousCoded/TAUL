

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
        else if (const auto cp = unicode_by_id(x)) {
            return
                is_ascii(*cp)
                ? std::format("cp:'{}'", fmt_taul_char(*cp))
                // we could use fmt_taul_char for ALL Unicode, but I think I prefer fmt_unicode for non-ASCII instead
                : std::format("cp:{}", fmt_unicode(*cp));
        }
    }
    else if (is_lpr_id(x)) {
        if (is_end_id(x)) return "lpr:<end-of-input>";
        if (is_failure_id(x)) return "lpr:<failure>";
        else return std::format("lpr:{}", lpr_index_by_id(x).value());
    }
    else if (is_ppr_id(x)) {
        return std::format("ppr:{}", ppr_index_by_id(x).value());
    }
    return std::format("<illegal ({})>", size_t(x));
}

