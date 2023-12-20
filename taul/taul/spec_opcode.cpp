

#include "spec_opcode.h"

#include "asserts.h"


std::string taul::fmt_spec_opcode(spec_opcode x) {
    static_assert(spec_opcodes == 21);
    std::string result{};
    switch (x) {
    case spec_opcode::grammar_bias:     result = "grammar-bias";    break;
    case spec_opcode::close:            result = "close";           break;
    case spec_opcode::lpr_decl:         result = "lpr-decl";        break;
    case spec_opcode::ppr_decl:         result = "ppr-decl";        break;
    case spec_opcode::lpr:              result = "lpr";             break;
    case spec_opcode::ppr:              result = "ppr";             break;
    case spec_opcode::begin:            result = "begin";           break;
    case spec_opcode::end:              result = "end";             break;
    case spec_opcode::any:              result = "any";             break;
    case spec_opcode::string:           result = "string";          break;
    case spec_opcode::charset:          result = "charset";         break;
    case spec_opcode::token:            result = "token";           break;
    case spec_opcode::failure:          result = "failure";         break;
    case spec_opcode::name:             result = "name";            break;
    case spec_opcode::sequence:         result = "sequence";        break;
    case spec_opcode::set:              result = "set";             break;
    case spec_opcode::modifier:         result = "modifier";        break;
    case spec_opcode::assertion:        result = "assertion";       break;
    case spec_opcode::constraint:       result = "constraint";      break;
    case spec_opcode::junction:         result = "junction";        break;
    case spec_opcode::localend:         result = "localend";        break;
    //case spec_opcode::lsubexpr:         result = "lsubexpr";        break;
    default:                            TAUL_DEADEND;               break;
    }
    return result;
}

