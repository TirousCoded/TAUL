

#include "disassemble_spec.h"

#include "string_and_charset.h"


// this'll let us pretty-print things like \n, alongside whatever
// escape sequences may have already been present

std::string _clean_string(std::string_view x) {
    return taul::fmt_taul_string(taul::parse_taul_string(x));
}
std::string _clean_charset(std::string_view x) {
    return taul::fmt_taul_charset(taul::parse_taul_charset(x));
}


std::string taul::disassemble_spec(const spec& spec) {
    internal::disassemble_spec_interpreter interp{};
    interp.interpret(spec);
    return interp.result;
}

void taul::internal::disassemble_spec_interpreter::on_startup() {
    // do nothing
}

void taul::internal::disassemble_spec_interpreter::on_shutdown() {
    result = std::format("spec disassembly ({} insts)", instruction) + result;
}

void taul::internal::disassemble_spec_interpreter::on_pos(source_pos new_pos) {
    result += std::format("\n[{}] {} ({})", instruction, spec_opcode::pos, new_pos);
}

void taul::internal::disassemble_spec_interpreter::on_close() {
    result += std::format("\n[{}] {}", instruction, spec_opcode::close);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_alternative() {
    result += std::format("\n[{}] {}", instruction, spec_opcode::alternative);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_lpr_decl(std::string_view name) {
    result += std::format("\n[{}] {} ('{}')", instruction, spec_opcode::lpr_decl, _clean_string(name));
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_ppr_decl(std::string_view name) {
    result += std::format("\n[{}] {} ('{}')", instruction, spec_opcode::ppr_decl, _clean_string(name));
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_lpr(std::string_view name, qualifier qualifier) {
    result += std::format("\n[{}] {} ('{}', {})", instruction, spec_opcode::lpr, _clean_string(name), qualifier);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_ppr(std::string_view name, qualifier qualifier) {
    result += std::format("\n[{}] {} ('{}', {})", instruction, spec_opcode::ppr, _clean_string(name), qualifier);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_end() {
    result += std::format("\n[{}] {}", instruction, spec_opcode::end);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_any() {
    result += std::format("\n[{}] {}", instruction, spec_opcode::any);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_string(std::string_view s) {
    result += std::format("\n[{}] {} ('{}')", instruction, spec_opcode::string, _clean_string(s));
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_charset(std::string_view s) {
    result += std::format("\n[{}] {} ('{}')", instruction, spec_opcode::charset, _clean_charset(s));
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_token() {
    result += std::format("\n[{}] {}", instruction, spec_opcode::token);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_failure() {
    result += std::format("\n[{}] {}", instruction, spec_opcode::failure);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_name(std::string_view name) {
    result += std::format("\n[{}] {} ('{}')", instruction, spec_opcode::name, _clean_string(name));
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_sequence() {
    result += std::format("\n[{}] {}", instruction, spec_opcode::sequence);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_lookahead() {
    result += std::format("\n[{}] {}", instruction, spec_opcode::lookahead);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_lookahead_not() {
    result += std::format("\n[{}] {}", instruction, spec_opcode::lookahead_not);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_not() {
    result += std::format("\n[{}] {}", instruction, spec_opcode::not0);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_optional() {
    result += std::format("\n[{}] {}", instruction, spec_opcode::optional);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_kleene_star() {
    result += std::format("\n[{}] {}", instruction, spec_opcode::kleene_star);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_kleene_plus() {
    result += std::format("\n[{}] {}", instruction, spec_opcode::kleene_plus);
    instruction++;
}

