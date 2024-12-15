

#include "spec.h"

#include "asserts.h"

#include "internal/buff.h"


taul::spec taul::spec::concat(const spec& a, const spec& b) {
    spec result{};
    result.bin = internal::buff_concat(a.bin, b.bin);
    result.associate(a.src);
    return result;
}

taul::spec_writer& taul::spec_writer::pos(source_pos new_pos) {
    _pos = new_pos;
    return *this;
}

taul::spec_writer& taul::spec_writer::close() {
    internal::buff_write(_temp.bin, spec_opcode::close, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::alternative() {
    internal::buff_write(_temp.bin, spec_opcode::alternative, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::lpr_decl(std::string_view name) {
    internal::buff_write(_temp.bin, spec_opcode::lpr_decl, _pos, name);
    return *this;
}

taul::spec_writer& taul::spec_writer::ppr_decl(std::string_view name) {
    internal::buff_write(_temp.bin, spec_opcode::ppr_decl, _pos, name);
    return *this;
}

taul::spec_writer& taul::spec_writer::lpr(std::string_view name, qualifier qualifier) {
    internal::buff_write(_temp.bin, spec_opcode::lpr, _pos, name, qualifier);
    return *this;
}

taul::spec_writer& taul::spec_writer::ppr(std::string_view name, qualifier qualifier) {
    internal::buff_write(_temp.bin, spec_opcode::ppr, _pos, name, qualifier);
    return *this;
}

taul::spec_writer& taul::spec_writer::end() {
    internal::buff_write(_temp.bin, spec_opcode::end, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::any() {
    internal::buff_write(_temp.bin, spec_opcode::any, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::string(std::string_view s) {
    internal::buff_write(_temp.bin, spec_opcode::string, _pos, s);
    return *this;
}

taul::spec_writer& taul::spec_writer::charset(std::string_view s) {
    internal::buff_write(_temp.bin, spec_opcode::charset, _pos, s);
    return *this;
}

taul::spec_writer& taul::spec_writer::token() {
    internal::buff_write(_temp.bin, spec_opcode::token, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::failure() {
    internal::buff_write(_temp.bin, spec_opcode::failure, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::name(std::string_view name) {
    internal::buff_write(_temp.bin, spec_opcode::name, _pos, name);
    return *this;
}

taul::spec_writer& taul::spec_writer::sequence() {
    internal::buff_write(_temp.bin, spec_opcode::sequence, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::lookahead() {
    internal::buff_write(_temp.bin, spec_opcode::lookahead, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::lookahead_not() {
    internal::buff_write(_temp.bin, spec_opcode::lookahead_not, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::not0() {
    internal::buff_write(_temp.bin, spec_opcode::not0, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::optional() {
    internal::buff_write(_temp.bin, spec_opcode::optional, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::kleene_star() {
    internal::buff_write(_temp.bin, spec_opcode::kleene_star, _pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::kleene_plus() {
    internal::buff_write(_temp.bin, spec_opcode::kleene_plus, _pos);
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
    size_t offset = 0;
    on_startup();
    _peek.reset();
    // keep iterating while we either have more things to decode, or while _peek
    // isn't std::nullopt (to account for edge case where at end of spec offset will
    // equal x.bin.size(), but we still have one more cycle to do)
    while (offset < x.bin.size() || (bool)_peek) {
        offset += _step(x, offset);
    }
    on_shutdown();
}

size_t taul::spec_interpreter::_step(const spec& s, size_t offset) {
    static_assert(spec_opcodes == 20);
    size_t len = 0;
    auto [opcode, new_pos] = internal::buff_read<spec_opcode, source_pos>(s.bin, offset, len);
    _pos = new_pos;
    // this needs to be called below between usage of internal::buff_read and the
    // on_### method call itself to setup _peek properly
    auto setup_peek =
        [&] {
        _peek =
            offset + len < s.bin.size()
            ? std::make_optional(internal::buff_peek_one<spec_opcode>(s.bin, offset + len))
            : std::nullopt;
        };
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
    case spec_opcode::lpr_decl:
    {
        auto [name] = internal::buff_read<std::string_view>(s.bin, offset + len, len);
        setup_peek();
        on_lpr_decl(name);
    }
    break;
    case spec_opcode::ppr_decl:
    {
        auto [name] = internal::buff_read<std::string_view>(s.bin, offset + len, len);
        setup_peek();
        on_ppr_decl(name);
    }
    break;
    case spec_opcode::lpr:
    {
        auto [name, qualifier] = internal::buff_read<std::string_view, taul::qualifier>(s.bin, offset + len, len);
        setup_peek();
        on_lpr(name, qualifier);
    }
    break;
    case spec_opcode::ppr:
    {
        auto [name, qualifier] = internal::buff_read<std::string_view, taul::qualifier>(s.bin, offset + len, len);
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
        auto [s0] = internal::buff_read<std::string_view>(s.bin, offset + len, len);
        setup_peek();
        on_string(s0);
    }
    break;
    case spec_opcode::charset:
    {
        auto [s0] = internal::buff_read<std::string_view>(s.bin, offset + len, len);
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
        auto [name] = internal::buff_read<std::string_view>(s.bin, offset + len, len);
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
    return len;
}

