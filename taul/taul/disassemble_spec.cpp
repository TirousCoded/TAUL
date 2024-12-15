

#include "disassemble_spec.h"

#include "string_and_charset.h"


// this'll let us pretty-print things like \n, alongside whatever
// escape sequences may have already been present

static std::string _clean_string(std::string_view x) {
    return taul::fmt_taul_string(taul::parse_taul_string(x));
}
static std::string _clean_charset(std::string_view x) {
    return taul::fmt_taul_charset(taul::parse_taul_charset(x));
}


std::string taul::disassemble_spec(const spec& spec) {
    internal::disassemble_spec_interpreter interp{};
    interp.interpret(spec);
    return interp.result;
}

std::string taul::disassemble_spec(const spec& spec, const source_code& src) {
    internal::disassemble_spec_interpreter interp{};
    interp.src = &src;
    interp.interpret(spec);
    return interp.result;
}

std::string taul::internal::disassemble_spec_interpreter::fmt_here() const {
    return
        src
        ? deref_assert(src).location_at(pos()).fmt(false)
        : fmt_pos(pos());
}

void taul::internal::disassemble_spec_interpreter::on_startup() {
    // do nothing
}

void taul::internal::disassemble_spec_interpreter::on_shutdown() {
    result = std::format("spec disassembly ({} insts)", instruction) + result;
}

void taul::internal::disassemble_spec_interpreter::on_close() {
    result += std::format("\n{} {}", fmt_here(), spec_opcode::close);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_alternative() {
    result += std::format("\n{} {}", fmt_here(), spec_opcode::alternative);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_lpr_decl(std::string_view name) {
    result += std::format("\n{} {} ('{}')", fmt_here(), spec_opcode::lpr_decl, _clean_string(name));
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_ppr_decl(std::string_view name) {
    result += std::format("\n{} {} ('{}')", fmt_here(), spec_opcode::ppr_decl, _clean_string(name));
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_lpr(std::string_view name, qualifier qualifier) {
    result += std::format("\n{} {} ('{}', {})", fmt_here(), spec_opcode::lpr, _clean_string(name), qualifier);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_ppr(std::string_view name, qualifier qualifier) {
    result += std::format("\n{} {} ('{}', {})", fmt_here(), spec_opcode::ppr, _clean_string(name), qualifier);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_end() {
    result += std::format("\n{} {}", fmt_here(), spec_opcode::end);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_any() {
    result += std::format("\n{} {}", fmt_here(), spec_opcode::any);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_string(std::string_view s) {
    result += std::format("\n{} {} ('{}')", fmt_here(), spec_opcode::string, _clean_string(s));
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_charset(std::string_view s) {
    result += std::format("\n{} {} ('{}')", fmt_here(), spec_opcode::charset, _clean_charset(s));
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_token() {
    result += std::format("\n{} {}", fmt_here(), spec_opcode::token);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_failure() {
    result += std::format("\n{} {}", fmt_here(), spec_opcode::failure);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_name(std::string_view name) {
    result += std::format("\n{} {} ('{}')", fmt_here(), spec_opcode::name, _clean_string(name));
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_sequence() {
    result += std::format("\n{} {}", fmt_here(), spec_opcode::sequence);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_lookahead() {
    result += std::format("\n{} {}", fmt_here(), spec_opcode::lookahead);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_lookahead_not() {
    result += std::format("\n{} {}", fmt_here(), spec_opcode::lookahead_not);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_not() {
    result += std::format("\n{} {}", fmt_here(), spec_opcode::not0);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_optional() {
    result += std::format("\n{} {}", fmt_here(), spec_opcode::optional);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_kleene_star() {
    result += std::format("\n{} {}", fmt_here(), spec_opcode::kleene_star);
    instruction++;
}

void taul::internal::disassemble_spec_interpreter::on_kleene_plus() {
    result += std::format("\n{} {}", fmt_here(), spec_opcode::kleene_plus);
    instruction++;
}

std::string taul::internal::disassemble_llspec(const llspec& spec) {
    internal::disassemble_llspec_interpreter interp{};
    interp.interpret(spec);
    return interp.result;
}

std::string taul::internal::disassemble_llspec(const llspec& spec, const source_code& src) {
    internal::disassemble_llspec_interpreter interp{};
    interp.src = &src;
    interp.interpret(spec);
    return interp.result;
}

std::string taul::internal::disassemble_llspec_interpreter::fmt_here() const {
    return
        src
        ? deref_assert(src).location_at(pos()).fmt(false)
        : fmt_pos(pos());
}

void taul::internal::disassemble_llspec_interpreter::on_startup() {
    // do nothing
}

void taul::internal::disassemble_llspec_interpreter::on_shutdown() {
    result = std::format("llspec disassembly ({} insts)", instruction) + result;
}

void taul::internal::disassemble_llspec_interpreter::on_close() {
    result += std::format("\n{} {}", fmt_here(), llspec_opcode::close);
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_alternative() {
    result += std::format("\n{} {}", fmt_here(), llspec_opcode::alternative);
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_lpr_decl(std::string_view name) {
    result += std::format("\n{} {} ('{}')", fmt_here(), llspec_opcode::lpr_decl, _clean_string(name));
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_ppr_decl(std::string_view name) {
    result += std::format("\n{} {} ('{}')", fmt_here(), llspec_opcode::ppr_decl, _clean_string(name));
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_lpr(std::string_view name, qualifier qualifier) {
    result += std::format("\n{} {} ('{}', {})", fmt_here(), llspec_opcode::lpr, _clean_string(name), qualifier);
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_ppr(std::string_view name, qualifier qualifier) {
    result += std::format("\n{} {} ('{}', {})", fmt_here(), llspec_opcode::ppr, _clean_string(name), qualifier);
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_end() {
    result += std::format("\n{} {}", fmt_here(), llspec_opcode::end);
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_any() {
    result += std::format("\n{} {}", fmt_here(), llspec_opcode::any);
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_string(std::string_view s) {
    result += std::format("\n{} {} ('{}')", fmt_here(), llspec_opcode::string, _clean_string(s));
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_charset(std::string_view s) {
    result += std::format("\n{} {} ('{}')", fmt_here(), llspec_opcode::charset, _clean_charset(s));
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_token() {
    result += std::format("\n{} {}", fmt_here(), llspec_opcode::token);
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_failure() {
    result += std::format("\n{} {}", fmt_here(), llspec_opcode::failure);
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_name(std::string_view name, preced_t preced_val) {
    result += std::format("\n{} {} ('{}', preced_val: {})", fmt_here(), llspec_opcode::name, _clean_string(name), size_t(preced_val));
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_sequence() {
    result += std::format("\n{} {}", fmt_here(), llspec_opcode::sequence);
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_lookahead() {
    result += std::format("\n{} {}", fmt_here(), llspec_opcode::lookahead);
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_lookahead_not() {
    result += std::format("\n{} {}", fmt_here(), llspec_opcode::lookahead_not);
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_not() {
    result += std::format("\n{} {}", fmt_here(), llspec_opcode::not0);
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_optional() {
    result += std::format("\n{} {}", fmt_here(), llspec_opcode::optional);
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_kleene_star() {
    result += std::format("\n{} {}", fmt_here(), llspec_opcode::kleene_star);
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_kleene_plus() {
    result += std::format("\n{} {}", fmt_here(), llspec_opcode::kleene_plus);
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_preced_pred(preced_t preced_max, preced_t preced_val) {
    result += std::format("\n{} {} (preced_max: {}, preced_val: {})", fmt_here(), llspec_opcode::preced_pred, preced_max, preced_val);
    instruction++;
}

void taul::internal::disassemble_llspec_interpreter::on_pylon() {
    result += std::format("\n{} {}", fmt_here(), llspec_opcode::pylon);
    instruction++;
}

