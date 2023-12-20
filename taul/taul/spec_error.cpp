

#include "spec_error.h"

#include "asserts.h"


std::string taul::fmt_spec_error(spec_error x) {
    static_assert(spec_errors == 18);
    std::string result{};
    switch (x) {
    case spec_error::syntax_error:                  result = "syntax-error";                    break;
    case spec_error::scope_not_closed:              result = "scope-not-closed";                break;
    case spec_error::stray_close:                   result = "stray-close";                     break;
    case spec_error::rule_name_conflict:            result = "rule-name-conflict";              break;
    case spec_error::rule_never_defined:            result = "rule-never-defined";              break;
    case spec_error::rule_never_declared:           result = "rule-never-declared";             break;
    case spec_error::rule_already_defined:          result = "rule-already-defined";            break;
    case spec_error::rule_not_found:                result = "rule-not-found";                  break;
    case spec_error::rule_may_not_be_ppr:           result = "rule-may-not-be-ppr";             break;
    case spec_error::illegal_in_lpr_scope:          result = "illegal-in-lpr-scope";            break;
    case spec_error::illegal_in_ppr_scope:          result = "illegal-in-ppr-scope";            break;
    case spec_error::illegal_in_no_scope:           result = "illegal-in-no-scope";             break;
    case spec_error::illegal_subexpr_count:         result = "illegal-subexpr-count";           break;
    case spec_error::junction_missing:              result = "junction-missing";                break;
    case spec_error::junction_not_in_constraint:    result = "junction-not-in-constraint";      break;
    case spec_error::junction_misplaced:            result = "junction-misplaced";              break;
    case spec_error::junction_already_established:  result = "junction-already-established";    break;
    case spec_error::illegal_localend:              result = "illegal-localend";                break;
    default:                                        TAUL_DEADEND;                               break;
    }
    return result;
}

