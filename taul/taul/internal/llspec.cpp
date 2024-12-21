

#include "llspec.h"

#include "buff.h"


std::string taul::internal::fmt_llspec_opcode(llspec_opcode x) {
    std::string result{};
    if (x <= llspec_opcode::_end_of_spec_mirror) {
        return fmt_spec_opcode(spec_opcode(x));
    }
    else {
        static_assert(llspec_opcodes == 22);
        switch (x) {
        case llspec_opcode::preced_pred: result = "preced_pred"; break;
        case llspec_opcode::pylon: result = "pylon"; break;
        default: TAUL_DEADEND; break;
        }
    }
    return result;
}

taul::internal::llspec taul::internal::llspec::concat(const llspec& a, const llspec& b) {
    llspec result{};
    result._bin = buff::concat(a._bin, b._bin);
    result.associate(a._src);
    return result;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::pos(source_pos new_pos) {
    _pos = new_pos;
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::close() {
    _temp._bin.write(llspec_opcode::close, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::alternative() {
    _temp._bin.write(llspec_opcode::alternative, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::lpr_decl(std::string_view name) {
    _temp._bin.write(llspec_opcode::lpr_decl, _pos, name);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::ppr_decl(std::string_view name) {
    _temp._bin.write(llspec_opcode::ppr_decl, _pos, name);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::lpr(std::string_view name, qualifier qualifier) {
    _temp._bin.write(llspec_opcode::lpr, _pos, name, qualifier);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::ppr(std::string_view name, qualifier qualifier) {
    _temp._bin.write(llspec_opcode::ppr, _pos, name, qualifier);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::end() {
    _temp._bin.write(llspec_opcode::end, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::any() {
    _temp._bin.write(llspec_opcode::any, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::string(std::string_view s) {
    _temp._bin.write(llspec_opcode::string, _pos, s);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::charset(std::string_view s) {
    _temp._bin.write(llspec_opcode::charset, _pos, s);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::token() {
    _temp._bin.write(llspec_opcode::token, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::failure() {
    _temp._bin.write(llspec_opcode::failure, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::name(std::string_view name, preced_t preced_val) {
    _temp._bin.write(llspec_opcode::name, _pos, name, preced_val);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::sequence() {
    _temp._bin.write(llspec_opcode::sequence, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::lookahead() {
    _temp._bin.write(llspec_opcode::lookahead, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::lookahead_not() {
    _temp._bin.write(llspec_opcode::lookahead_not, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::not0() {
    _temp._bin.write(llspec_opcode::not0, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::optional() {
    _temp._bin.write(llspec_opcode::optional, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::kleene_star() {
    _temp._bin.write(llspec_opcode::kleene_star, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::kleene_plus() {
    _temp._bin.write(llspec_opcode::kleene_plus, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::preced_pred(preced_t preced_max, preced_t preced_val) {
    TAUL_ASSERT(preced_val == signal_preced_val); // <- at present, anything else is probably wrong
    _temp._bin.write(llspec_opcode::preced_pred, _pos, preced_max, preced_val);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::pylon() {
    _temp._bin.write(llspec_opcode::pylon, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::write_spec(const llspec& x) {
    write_spec_method_llspec_interpreter interp(*this);
    interp.interpret(x);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::write_spec(const llspec_writer& x) {
    TAUL_ASSERT(&x != this); // just easier to forbid this for now
    return write_spec(x._temp);
}

taul::internal::llspec taul::internal::llspec_writer::done() {
    _pos = 0; // can't forget
    llspec result{};
    std::swap(result, _temp);
    return result;
}

taul::source_pos taul::internal::llspec_interpreter::pos() const noexcept {
    return _pos;
}

std::optional<taul::internal::llspec_opcode> taul::internal::llspec_interpreter::peek() const noexcept {
    return _peek;
}

void taul::internal::llspec_interpreter::interpret(const llspec& x) {
    auto rdr = buff_reader(x._bin);
    _peek.reset();
    on_startup();
    while (_step(rdr)) {}
    on_shutdown();
}

bool taul::internal::llspec_interpreter::_step(buff_reader& rdr) {
    static_assert(llspec_opcodes == 22);
    auto offset = rdr.offset;
    auto header = rdr.read<llspec_opcode, source_pos>();
    if (!header) return false;
    auto [opcode, new_pos] = deref_assert(header);
    _pos = new_pos;
    // this needs to be called below between usage of rdr.read and the
    // on_### method call itself to setup _peek properly
    auto setup_peek = [&] { _peek = rdr.peek_one<llspec_opcode>(); };
    switch (opcode) {
    case llspec_opcode::close:
    {
        setup_peek();
        on_close();
    }
    break;
    case llspec_opcode::alternative:
    {
        setup_peek();
        on_alternative();
    }
    break;
    case llspec_opcode::lpr_decl:
    {
        auto [name] = rdr.read<std::string_view>().value();
        setup_peek();
        on_lpr_decl(name);
    }
    break;
    case llspec_opcode::ppr_decl:
    {
        auto [name] = rdr.read<std::string_view>().value();
        setup_peek();
        on_ppr_decl(name);
    }
    break;
    case llspec_opcode::lpr:
    {
        auto [name, qualifier] = rdr.read<std::string_view, taul::qualifier>().value();
        setup_peek();
        on_lpr(name, qualifier);
    }
    break;
    case llspec_opcode::ppr:
    {
        auto [name, qualifier] = rdr.read<std::string_view, taul::qualifier>().value();
        setup_peek();
        on_ppr(name, qualifier);
    }
    break;
    case llspec_opcode::end:
    {
        setup_peek();
        on_end();
    }
    break;
    case llspec_opcode::any:
    {
        setup_peek();
        on_any();
    }
    break;
    case llspec_opcode::string:
    {
        auto [s0] = rdr.read<std::string_view>().value();
        setup_peek();
        on_string(s0);
    }
    break;
    case llspec_opcode::charset:
    {
        auto [s0] = rdr.read<std::string_view>().value();
        setup_peek();
        on_charset(s0);
    }
    break;
    case llspec_opcode::token:
    {
        setup_peek();
        on_token();
    }
    break;
    case llspec_opcode::failure:
    {
        setup_peek();
        on_failure();
    }
    break;
    case llspec_opcode::name:
    {
        auto [name, preced_val] = rdr.read<std::string_view, preced_t>().value();
        setup_peek();
        on_name(name, preced_val);
    }
    break;
    case llspec_opcode::sequence:
    {
        setup_peek();
        on_sequence();
    }
    break;
    case llspec_opcode::lookahead:
    {
        setup_peek();
        on_lookahead();
    }
    break;
    case llspec_opcode::lookahead_not:
    {
        setup_peek();
        on_lookahead_not();
    }
    break;
    case llspec_opcode::not0:
    {
        setup_peek();
        on_not();
    }
    break;
    case llspec_opcode::optional:
    {
        setup_peek();
        on_optional();
    }
    break;
    case llspec_opcode::kleene_star:
    {
        setup_peek();
        on_kleene_star();
    }
    break;
    case llspec_opcode::kleene_plus:
    {
        setup_peek();
        on_kleene_plus();
    }
    break;
    case llspec_opcode::preced_pred:
    {
        auto [preced_max, preced_val] = rdr.read<preced_t, preced_t>().value();
        setup_peek();
        on_preced_pred(preced_max, preced_val);
    }
    break;
    case llspec_opcode::pylon:
    {
        setup_peek();
        on_pylon();
    }
    break;
    default: TAUL_DEADEND; break;
    }
    return true;
}

