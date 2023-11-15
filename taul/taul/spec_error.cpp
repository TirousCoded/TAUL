

#include "spec_error.h"

#include "asserts.h"


std::string taul::fmt_spec_error(spec_error x) {
    static_assert(spec_errors == 10);
    std::string result{};
    switch (x) {
    case spec_error::syntax_error:          result = "syntax-error";            break;
    case spec_error::scope_not_closed:      result = "scope-not-closed";        break;
    case spec_error::stray_close:           result = "stray-close";             break;
    case spec_error::rule_name_conflict:    result = "rule-name-conflict";      break;
    case spec_error::rule_never_defined:    result = "rule-never-defined";      break;
    case spec_error::rule_never_declared:   result = "rule-never-declared";     break;
    case spec_error::rule_already_defined:  result = "rule-already-defined";    break;
    case spec_error::illegal_in_lpr_scope:  result = "illegal-in-lpr-scope";    break;
    case spec_error::illegal_in_ppr_scope:  result = "illegal-in-ppr-scope";    break;
    case spec_error::illegal_in_no_scope:   result = "illegal-in-no-scope";     break;
    default:                                TAUL_DEADEND;                       break;
    }
    return result;
}

