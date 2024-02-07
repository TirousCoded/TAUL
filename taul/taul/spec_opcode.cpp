

#include "spec_opcode.h"

#include "asserts.h"


std::string taul::fmt_spec_opcode(spec_opcode x) {
    static_assert(spec_opcodes == 20);
    std::string result{};
    switch (x) {
    case spec_opcode::close:            result = "close";           break;
    case spec_opcode::lpr_decl:         result = "lpr-decl";        break;
    case spec_opcode::ppr_decl:         result = "ppr-decl";        break;
    case spec_opcode::lpr:              result = "lpr";             break;
    case spec_opcode::ppr:              result = "ppr";             break;
    case spec_opcode::end:              result = "end";             break;
    case spec_opcode::any:              result = "any";             break;
    case spec_opcode::string:           result = "string";          break;
    case spec_opcode::charset:          result = "charset";         break;
    case spec_opcode::token:            result = "token";           break;
    case spec_opcode::failure:          result = "failure";         break;
    case spec_opcode::name:             result = "name";            break;
    case spec_opcode::sequence:         result = "sequence";        break;
    case spec_opcode::set:              result = "set";             break;
    case spec_opcode::lookahead:        result = "lookahead";       break;
    case spec_opcode::lookahead_not:    result = "lookahead-not";   break;
    case spec_opcode::not0:             result = "not";             break;
    case spec_opcode::optional:         result = "optional";        break;
    case spec_opcode::kleene_star:      result = "kleene-star";     break;
    case spec_opcode::kleene_plus:      result = "kleene-plus";     break;
    default:                            TAUL_DEADEND;               break;
    }
    return result;
}

