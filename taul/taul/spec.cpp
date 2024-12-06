

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
    internal::buff_write(_temp.bin, spec_opcode::pos);
    internal::buff_write(_temp.bin, new_pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::close() {
    internal::buff_write(_temp.bin, spec_opcode::close);
    return *this;
}

taul::spec_writer& taul::spec_writer::alternative() {
    internal::buff_write(_temp.bin, spec_opcode::alternative);
    return *this;
}

taul::spec_writer& taul::spec_writer::lpr_decl(std::string_view name) {
    internal::buff_write(_temp.bin, spec_opcode::lpr_decl);
    internal::buff_write_s(_temp.bin, name);
    return *this;
}

taul::spec_writer& taul::spec_writer::ppr_decl(std::string_view name) {
    internal::buff_write(_temp.bin, spec_opcode::ppr_decl);
    internal::buff_write_s(_temp.bin, name);
    return *this;
}

taul::spec_writer& taul::spec_writer::lpr(std::string_view name, qualifier qualifier) {
    internal::buff_write(_temp.bin, spec_opcode::lpr);
    internal::buff_write_s(_temp.bin, name);
    internal::buff_write(_temp.bin, qualifier);
    return *this;
}

taul::spec_writer& taul::spec_writer::ppr(std::string_view name, qualifier qualifier) {
    internal::buff_write(_temp.bin, spec_opcode::ppr);
    internal::buff_write_s(_temp.bin, name);
    internal::buff_write(_temp.bin, qualifier);
    return *this;
}

taul::spec_writer& taul::spec_writer::end() {
    internal::buff_write(_temp.bin, spec_opcode::end);
    return *this;
}

taul::spec_writer& taul::spec_writer::any() {
    internal::buff_write(_temp.bin, spec_opcode::any);
    return *this;
}

taul::spec_writer& taul::spec_writer::string(std::string_view s) {
    internal::buff_write(_temp.bin, spec_opcode::string);
    internal::buff_write_s(_temp.bin, s);
    return *this;
}

taul::spec_writer& taul::spec_writer::charset(std::string_view s) {
    internal::buff_write(_temp.bin, spec_opcode::charset);
    internal::buff_write_s(_temp.bin, s);
    return *this;
}

taul::spec_writer& taul::spec_writer::token() {
    internal::buff_write(_temp.bin, spec_opcode::token);
    return *this;
}

taul::spec_writer& taul::spec_writer::failure() {
    internal::buff_write(_temp.bin, spec_opcode::failure);
    return *this;
}

taul::spec_writer& taul::spec_writer::name(std::string_view name) {
    internal::buff_write(_temp.bin, spec_opcode::name);
    internal::buff_write_s(_temp.bin, name);
    return *this;
}

taul::spec_writer& taul::spec_writer::sequence() {
    internal::buff_write(_temp.bin, spec_opcode::sequence);
    return *this;
}

taul::spec_writer& taul::spec_writer::lookahead() {
    internal::buff_write(_temp.bin, spec_opcode::lookahead);
    return *this;
}

taul::spec_writer& taul::spec_writer::lookahead_not() {
    internal::buff_write(_temp.bin, spec_opcode::lookahead_not);
    return *this;
}

taul::spec_writer& taul::spec_writer::not0() {
    internal::buff_write(_temp.bin, spec_opcode::not0);
    return *this;
}

taul::spec_writer& taul::spec_writer::optional() {
    internal::buff_write(_temp.bin, spec_opcode::optional);
    return *this;
}

taul::spec_writer& taul::spec_writer::kleene_star() {
    internal::buff_write(_temp.bin, spec_opcode::kleene_star);
    return *this;
}

taul::spec_writer& taul::spec_writer::kleene_plus() {
    internal::buff_write(_temp.bin, spec_opcode::kleene_plus);
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
    spec result{};
    std::swap(result, _temp);
    return result;
}

std::optional<taul::spec_opcode> taul::spec_interpreter::lookahead() const noexcept {
    return _lookahead;
}

void taul::spec_interpreter::interpret(const spec& x) {
    size_t offset = 0;
    on_startup();
    _lookahead.reset();
    // keep iterating while we either have more things to decode, or while _lookahead
    // isn't std::nullopt (to account for edge case where at end of spec offset will
    // equal x.bin.size(), but we still have one more cycle to do)
    while (offset < x.bin.size() || (bool)_lookahead) {
        offset += _step(x, offset);
    }
    on_shutdown();
}

size_t taul::spec_interpreter::_step(const spec& s, size_t offset) {
    static_assert(spec_opcodes == 21);
    size_t len = 0;
    // if offset == 0 (ie. we're at start of interpretation process) then decode first
    // opcode (ie. seperately from lookahead), otherwise make lookahead the opcode
    const auto opcode =
        offset == 0
        ? internal::buff_read<spec_opcode>(s.bin, offset, len)
        : _lookahead.value();
    // this needs to be called below between usage of internal::buff_read and the
    // on_### method call itself to setup _lookahead properly
    auto setup_lookahead =
        [&] {
        _lookahead =
            offset + len < s.bin.size()
            ? std::make_optional(internal::buff_read<spec_opcode>(s.bin, offset + len, len))
            : std::nullopt;
        };
    switch (opcode) {
    case spec_opcode::pos:
    {
        const auto new_pos = internal::buff_read<uint32_t>(s.bin, offset + len, len);
        setup_lookahead();
        on_pos(new_pos);
    }
    break;
    case spec_opcode::close:
    {
        setup_lookahead();
        on_close();
    }
    break;
    case spec_opcode::alternative:
    {
        setup_lookahead();
        on_alternative();
    }
    break;
    case spec_opcode::lpr_decl:
    {
        const auto name = internal::buff_read_s(s.bin, offset + len, len);
        setup_lookahead();
        on_lpr_decl(name);
    }
    break;
    case spec_opcode::ppr_decl:
    {
        const auto name = internal::buff_read_s(s.bin, offset + len, len);
        setup_lookahead();
        on_ppr_decl(name);
    }
    break;
    case spec_opcode::lpr:
    {
        const auto name = internal::buff_read_s(s.bin, offset + len, len);
        const auto qualifier = internal::buff_read<taul::qualifier>(s.bin, offset + len, len);
        setup_lookahead();
        on_lpr(name, qualifier);
    }
    break;
    case spec_opcode::ppr:
    {
        const auto name = internal::buff_read_s(s.bin, offset + len, len);
        const auto qualifier = internal::buff_read<taul::qualifier>(s.bin, offset + len, len);
        setup_lookahead();
        on_ppr(name, qualifier);
    }
    break;
    case spec_opcode::end:
    {
        setup_lookahead();
        on_end();
    }
    break;
    case spec_opcode::any:
    {
        setup_lookahead();
        on_any();
    }
    break;
    case spec_opcode::string:
    {
        const auto s0 = internal::buff_read_s(s.bin, offset + len, len);
        setup_lookahead();
        on_string(s0);
    }
    break;
    case spec_opcode::charset:
    {
        const auto s0 = internal::buff_read_s(s.bin, offset + len, len);
        setup_lookahead();
        on_charset(s0);
    }
    break;
    case spec_opcode::token:
    {
        setup_lookahead();
        on_token();
    }
    break;
    case spec_opcode::failure:
    {
        setup_lookahead();
        on_failure();
    }
    break;
    case spec_opcode::name:
    {
        const auto name = internal::buff_read_s(s.bin, offset + len, len);
        setup_lookahead();
        on_name(name);
    }
    break;
    case spec_opcode::sequence:
    {
        setup_lookahead();
        on_sequence();
    }
    break;
    case spec_opcode::lookahead:
    {
        setup_lookahead();
        on_lookahead();
    }
    break;
    case spec_opcode::lookahead_not:
    {
        setup_lookahead();
        on_lookahead_not();
    }
    break;
    case spec_opcode::not0:
    {
        setup_lookahead();
        on_not();
    }
    break;
    case spec_opcode::optional:
    {
        setup_lookahead();
        on_optional();
    }
    break;
    case spec_opcode::kleene_star:
    {
        setup_lookahead();
        on_kleene_star();
    }
    break;
    case spec_opcode::kleene_plus:
    {
        setup_lookahead();
        on_kleene_plus();
    }
    break;
    default: TAUL_DEADEND; break;
    }
    return len;
}

