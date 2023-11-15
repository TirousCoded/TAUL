

#include "spec_opcode.h"

#include "asserts.h"


std::string taul::fmt_spec_opcode(spec_opcode x) {
    static_assert(spec_opcodes == 7);
    std::string result{};
    switch (x) {
    case spec_opcode::grammar_bias:     result = "grammar-bias";    break;
    case spec_opcode::close:            result = "close";           break;
    case spec_opcode::lpr_decl:         result = "lpr-decl";        break;
    case spec_opcode::ppr_decl:         result = "ppr-decl";        break;
    case spec_opcode::lpr:              result = "lpr";             break;
    case spec_opcode::ppr:              result = "ppr";             break;
    case spec_opcode::char0:            result = "char";            break;
    default:                            TAUL_DEADEND;               break;
    }
    return result;
}

