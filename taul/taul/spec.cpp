

#include "spec.h"

#include "asserts.h"

#include "internal/buff.h"


taul::spec taul::spec::concat(const spec& a, const spec& b) {
    spec result{};
    result._bin = internal::buff::concat(a._bin, b._bin);
    result.associate(a._src);
    return result;
}

taul::spec_writer& taul::spec_writer::pos(source_pos new_pos) {
    _pos = new_pos;
    return *this;
}

taul::spec_writer& taul::spec_writer::close() {
    _temp._bin.write(spec_opcode::close, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::alternative() {
    _temp._bin.write(spec_opcode::alternative, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::right_assoc() {
    _temp._bin.write(spec_opcode::right_assoc, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::lpr_decl(std::string_view name) {
    _temp._bin.write(spec_opcode::lpr_decl, _pos, name);
    return *this;
}

taul::spec_writer& taul::spec_writer::ppr_decl(std::string_view name) {
    _temp._bin.write(spec_opcode::ppr_decl, _pos, name);
    return *this;
}

taul::spec_writer& taul::spec_writer::lpr(std::string_view name, qualifier qualifier) {
    _temp._bin.write(spec_opcode::lpr, _pos, name, qualifier);
    return *this;
}

taul::spec_writer& taul::spec_writer::ppr(std::string_view name, qualifier qualifier) {
    _temp._bin.write(spec_opcode::ppr, _pos, name, qualifier);
    return *this;
}

taul::spec_writer& taul::spec_writer::end() {
    _temp._bin.write(spec_opcode::end, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::any() {
    _temp._bin.write(spec_opcode::any, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::string(std::string_view s) {
    _temp._bin.write(spec_opcode::string, _pos, s);
    return *this;
}

taul::spec_writer& taul::spec_writer::charset(std::string_view s) {
    _temp._bin.write(spec_opcode::charset, _pos, s);
    return *this;
}

taul::spec_writer& taul::spec_writer::token() {
    _temp._bin.write(spec_opcode::token, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::failure() {
    _temp._bin.write(spec_opcode::failure, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::name(std::string_view name) {
    _temp._bin.write(spec_opcode::name, _pos, name);
    return *this;
}

taul::spec_writer& taul::spec_writer::sequence() {
    _temp._bin.write(spec_opcode::sequence, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::lookahead() {
    _temp._bin.write(spec_opcode::lookahead, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::lookahead_not() {
    _temp._bin.write(spec_opcode::lookahead_not, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::not0() {
    _temp._bin.write(spec_opcode::not0, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::optional() {
    _temp._bin.write(spec_opcode::optional, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::kleene_star() {
    _temp._bin.write(spec_opcode::kleene_star, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::kleene_plus() {
    _temp._bin.write(spec_opcode::kleene_plus, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::write_spec(const spec& x) {
    write_spec_method_spec_interpreter interp(*this);
    interp.interpret(x);
    return *this;
}

taul::spec_writer& taul::spec_writer::write_spec(const spec_writer& x) {
    TAUL_ASSERT(&x != this); // just easier to forbid this for now
    return write_spec(x._temp);
}

taul::spec taul::spec_writer::done() {
    _pos = 0; // can't forget
    spec result{};
    std::swap(result, _temp);
    return result;
}

taul::source_pos taul::spec_interpreter::pos() const noexcept {
    return _pos;
}

std::optional<taul::spec_opcode> taul::spec_interpreter::peek() const noexcept {
    return _peek;
}

void taul::spec_interpreter::interpret(const spec& x) {
    auto rdr = internal::buff_reader(x._bin);
    _peek.reset();
    on_startup();
    while (_step(rdr)) {}
    on_shutdown();
}

bool taul::spec_interpreter::_step(internal::buff_reader& rdr) {
    static_assert(spec_opcodes == 21);
    auto header = rdr.read<spec_opcode, source_pos>();
    if (!header) return false;
    auto [opcode, new_pos] = deref_assert(header);
    _pos = new_pos;
    // this needs to be called below between usage of rdr.read and the
    // on_### method call itself to setup _peek properly
    auto setup_peek = [&] { _peek = rdr.peek_one<spec_opcode>(); };
    switch (opcode) {
    case spec_opcode::close:
    {
        setup_peek();
        on_close();
    }
    break;
    case spec_opcode::alternative:
    {
        setup_peek();
        on_alternative();
    }
    break;
    case spec_opcode::right_assoc:
    {
        setup_peek();
        on_right_assoc();
    }
    break;
    case spec_opcode::lpr_decl:
    {
        auto [name] = rdr.read<std::string_view>().value();
        setup_peek();
        on_lpr_decl(name);
    }
    break;
    case spec_opcode::ppr_decl:
    {
        auto [name] = rdr.read<std::string_view>().value();
        setup_peek();
        on_ppr_decl(name);
    }
    break;
    case spec_opcode::lpr:
    {
        auto [name, qualifier] = rdr.read<std::string_view, taul::qualifier>().value();
        setup_peek();
        on_lpr(name, qualifier);
    }
    break;
    case spec_opcode::ppr:
    {
        auto [name, qualifier] = rdr.read<std::string_view, taul::qualifier>().value();
        setup_peek();
        on_ppr(name, qualifier);
    }
    break;
    case spec_opcode::end:
    {
        setup_peek();
        on_end();
    }
    break;
    case spec_opcode::any:
    {
        setup_peek();
        on_any();
    }
    break;
    case spec_opcode::string:
    {
        auto [s0] = rdr.read<std::string_view>().value();
        setup_peek();
        on_string(s0);
    }
    break;
    case spec_opcode::charset:
    {
        auto [s0] = rdr.read<std::string_view>().value();
        setup_peek();
        on_charset(s0);
    }
    break;
    case spec_opcode::token:
    {
        setup_peek();
        on_token();
    }
    break;
    case spec_opcode::failure:
    {
        setup_peek();
        on_failure();
    }
    break;
    case spec_opcode::name:
    {
        auto [name] = rdr.read<std::string_view>().value();
        setup_peek();
        on_name(name);
    }
    break;
    case spec_opcode::sequence:
    {
        setup_peek();
        on_sequence();
    }
    break;
    case spec_opcode::lookahead:
    {
        setup_peek();
        on_lookahead();
    }
    break;
    case spec_opcode::lookahead_not:
    {
        setup_peek();
        on_lookahead_not();
    }
    break;
    case spec_opcode::not0:
    {
        setup_peek();
        on_not();
    }
    break;
    case spec_opcode::optional:
    {
        setup_peek();
        on_optional();
    }
    break;
    case spec_opcode::kleene_star:
    {
        setup_peek();
        on_kleene_star();
    }
    break;
    case spec_opcode::kleene_plus:
    {
        setup_peek();
        on_kleene_plus();
    }
    break;
    default: TAUL_DEADEND; break;
    }
    return true;
}

