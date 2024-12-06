

#include "spec_lowerer.h"


void taul::internal::spec_lowerer::on_startup() {
    // do nothing
}

void taul::internal::spec_lowerer::on_shutdown() {
    // do nothing
}

void taul::internal::spec_lowerer::on_pos(source_pos new_pos) {
    output.pos(new_pos);
}

void taul::internal::spec_lowerer::on_close() {
    output.close();
}

void taul::internal::spec_lowerer::on_alternative() {
    output.alternative();
}

void taul::internal::spec_lowerer::on_lpr_decl(std::string_view name) {
    output.lpr_decl(name);
}

void taul::internal::spec_lowerer::on_ppr_decl(std::string_view name) {
    output.ppr_decl(name);
}

void taul::internal::spec_lowerer::on_lpr(std::string_view name, qualifier qualifier) {
    output.lpr(name, qualifier);
}

void taul::internal::spec_lowerer::on_ppr(std::string_view name, qualifier qualifier) {
    output.ppr(name, qualifier);
}

void taul::internal::spec_lowerer::on_end() {
    output.end();
}

void taul::internal::spec_lowerer::on_any() {
    output.any();
}

void taul::internal::spec_lowerer::on_string(std::string_view s) {
    output.string(s);
}

void taul::internal::spec_lowerer::on_charset(std::string_view s) {
    output.charset(s);
}

void taul::internal::spec_lowerer::on_token() {
    output.token();
}

void taul::internal::spec_lowerer::on_failure() {
    output.failure();
}

void taul::internal::spec_lowerer::on_name(std::string_view name) {
    output.name(name);
}

void taul::internal::spec_lowerer::on_sequence() {
    output.sequence();
}

void taul::internal::spec_lowerer::on_lookahead() {
    output.lookahead();
}

void taul::internal::spec_lowerer::on_lookahead_not() {
    output.lookahead_not();
}

void taul::internal::spec_lowerer::on_not() {
    output.not0();
}

void taul::internal::spec_lowerer::on_optional() {
    output.optional();
}

void taul::internal::spec_lowerer::on_kleene_star() {
    output.kleene_star();
}

void taul::internal::spec_lowerer::on_kleene_plus() {
    output.kleene_plus();
}

