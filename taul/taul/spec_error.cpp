

#include "spec_error.h"

#include "asserts.h"


std::string taul::fmt_spec_error(spec_error x) {
    static_assert(spec_errors == 16);
    std::string result{};
    switch (x) {
    case spec_error::source_code_not_found:             result = "source-code-not-found";               break;
    case spec_error::syntax_error:                      result = "syntax-error";                        break;
    case spec_error::scope_not_closed:                  result = "scope-not-closed";                    break;
    case spec_error::stray_close:                       result = "stray-close";                         break;
    case spec_error::rule_name_conflict:                result = "rule-name-conflict";                  break;
    case spec_error::rule_never_defined:                result = "rule-never-defined";                  break;
    case spec_error::rule_never_declared:               result = "rule-never-declared";                 break;
    case spec_error::rule_already_defined:              result = "rule-already-defined";                break;
    case spec_error::rule_not_found:                    result = "rule-not-found";                      break;
    case spec_error::rule_may_not_be_ppr:               result = "rule-may-not-be-ppr";                 break;
    case spec_error::illegal_in_lpr_scope:              result = "illegal-in-lpr-scope";                break;
    case spec_error::illegal_in_ppr_scope:              result = "illegal-in-ppr-scope";                break;
    case spec_error::illegal_in_no_scope:               result = "illegal-in-no-scope";                 break;
    case spec_error::illegal_subexpr_count:             result = "illegal-subexpr-count";               break;
    case spec_error::illegal_qualifier:                 result = "illegal-qualifier";                   break;
    case spec_error::illegal_in_single_terminal_scope:  result = "illegal-in-single-terminal-scope";    break;
    default:                                            TAUL_DEADEND;                                   break;
    }
    return result;
}

