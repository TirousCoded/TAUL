

#include "llspec.h"

#include "buff.h"


std::string taul::internal::fmt_llspec_opcode(llspec_opcode x) {
    std::string result{};
    if (x <= llspec_opcode::_end_of_spec_mirror) {
        return fmt_spec_opcode(spec_opcode(x));
    }
    else {
        static_assert(llspec_opcodes == 21);
        TAUL_DEADEND;
    }
    return result;
}

taul::internal::llspec taul::internal::llspec::concat(const llspec& a, const llspec& b) {
    llspec result{};
    result.bin = internal::buff_concat(a.bin, b.bin);
    result.associate(a.src);
    return result;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::pos(source_pos new_pos) {
    internal::buff_write(_temp.bin, llspec_opcode::pos);
    internal::buff_write(_temp.bin, new_pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::close() {
    internal::buff_write(_temp.bin, llspec_opcode::close);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::alternative() {
    internal::buff_write(_temp.bin, llspec_opcode::alternative);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::lpr_decl(std::string_view name) {
    internal::buff_write(_temp.bin, llspec_opcode::lpr_decl);
    internal::buff_write_s(_temp.bin, name);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::ppr_decl(std::string_view name) {
    internal::buff_write(_temp.bin, llspec_opcode::ppr_decl);
    internal::buff_write_s(_temp.bin, name);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::lpr(std::string_view name, qualifier qualifier) {
    internal::buff_write(_temp.bin, llspec_opcode::lpr);
    internal::buff_write_s(_temp.bin, name);
    internal::buff_write(_temp.bin, qualifier);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::ppr(std::string_view name, qualifier qualifier) {
    internal::buff_write(_temp.bin, llspec_opcode::ppr);
    internal::buff_write_s(_temp.bin, name);
    internal::buff_write(_temp.bin, qualifier);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::end() {
    internal::buff_write(_temp.bin, llspec_opcode::end);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::any() {
    internal::buff_write(_temp.bin, llspec_opcode::any);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::string(std::string_view s) {
    internal::buff_write(_temp.bin, llspec_opcode::string);
    internal::buff_write_s(_temp.bin, s);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::charset(std::string_view s) {
    internal::buff_write(_temp.bin, llspec_opcode::charset);
    internal::buff_write_s(_temp.bin, s);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::token() {
    internal::buff_write(_temp.bin, llspec_opcode::token);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::failure() {
    internal::buff_write(_temp.bin, llspec_opcode::failure);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::name(std::string_view name) {
    internal::buff_write(_temp.bin, llspec_opcode::name);
    internal::buff_write_s(_temp.bin, name);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::sequence() {
    internal::buff_write(_temp.bin, llspec_opcode::sequence);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::lookahead() {
    internal::buff_write(_temp.bin, llspec_opcode::lookahead);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::lookahead_not() {
    internal::buff_write(_temp.bin, llspec_opcode::lookahead_not);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::not0() {
    internal::buff_write(_temp.bin, llspec_opcode::not0);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::optional() {
    internal::buff_write(_temp.bin, llspec_opcode::optional);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::kleene_star() {
    internal::buff_write(_temp.bin, llspec_opcode::kleene_star);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::kleene_plus() {
    internal::buff_write(_temp.bin, llspec_opcode::kleene_plus);
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
    llspec result{};
    std::swap(result, _temp);
    return result;
}

std::optional<taul::internal::llspec_opcode> taul::internal::llspec_interpreter::lookahead() const noexcept {
    return _lookahead;
}

void taul::internal::llspec_interpreter::interpret(const llspec& x) {
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

size_t taul::internal::llspec_interpreter::_step(const llspec& s, size_t offset) {
    static_assert(llspec_opcodes == 21);
    size_t len = 0;
    // if offset == 0 (ie. we're at start of interpretation process) then decode first
    // opcode (ie. seperately from lookahead), otherwise make lookahead the opcode
    const auto opcode =
        offset == 0
        ? internal::buff_read<llspec_opcode>(s.bin, offset, len)
        : _lookahead.value();
    // this needs to be called below between usage of internal::buff_read and the
    // on_### method call itself to setup _lookahead properly
    auto setup_lookahead =
        [&] {
        _lookahead =
            offset + len < s.bin.size()
            ? std::make_optional(internal::buff_read<llspec_opcode>(s.bin, offset + len, len))
            : std::nullopt;
        };
    switch (opcode) {
    case llspec_opcode::pos:
    {
        const auto new_pos = internal::buff_read<uint32_t>(s.bin, offset + len, len);
        setup_lookahead();
        on_pos(new_pos);
    }
    break;
    case llspec_opcode::close:
    {
        setup_lookahead();
        on_close();
    }
    break;
    case llspec_opcode::alternative:
    {
        setup_lookahead();
        on_alternative();
    }
    break;
    case llspec_opcode::lpr_decl:
    {
        const auto name = internal::buff_read_s(s.bin, offset + len, len);
        setup_lookahead();
        on_lpr_decl(name);
    }
    break;
    case llspec_opcode::ppr_decl:
    {
        const auto name = internal::buff_read_s(s.bin, offset + len, len);
        setup_lookahead();
        on_ppr_decl(name);
    }
    break;
    case llspec_opcode::lpr:
    {
        const auto name = internal::buff_read_s(s.bin, offset + len, len);
        const auto qualifier = internal::buff_read<taul::qualifier>(s.bin, offset + len, len);
        setup_lookahead();
        on_lpr(name, qualifier);
    }
    break;
    case llspec_opcode::ppr:
    {
        const auto name = internal::buff_read_s(s.bin, offset + len, len);
        const auto qualifier = internal::buff_read<taul::qualifier>(s.bin, offset + len, len);
        setup_lookahead();
        on_ppr(name, qualifier);
    }
    break;
    case llspec_opcode::end:
    {
        setup_lookahead();
        on_end();
    }
    break;
    case llspec_opcode::any:
    {
        setup_lookahead();
        on_any();
    }
    break;
    case llspec_opcode::string:
    {
        const auto s0 = internal::buff_read_s(s.bin, offset + len, len);
        setup_lookahead();
        on_string(s0);
    }
    break;
    case llspec_opcode::charset:
    {
        const auto s0 = internal::buff_read_s(s.bin, offset + len, len);
        setup_lookahead();
        on_charset(s0);
    }
    break;
    case llspec_opcode::token:
    {
        setup_lookahead();
        on_token();
    }
    break;
    case llspec_opcode::failure:
    {
        setup_lookahead();
        on_failure();
    }
    break;
    case llspec_opcode::name:
    {
        const auto name = internal::buff_read_s(s.bin, offset + len, len);
        setup_lookahead();
        on_name(name);
    }
    break;
    case llspec_opcode::sequence:
    {
        setup_lookahead();
        on_sequence();
    }
    break;
    case llspec_opcode::lookahead:
    {
        setup_lookahead();
        on_lookahead();
    }
    break;
    case llspec_opcode::lookahead_not:
    {
        setup_lookahead();
        on_lookahead_not();
    }
    break;
    case llspec_opcode::not0:
    {
        setup_lookahead();
        on_not();
    }
    break;
    case llspec_opcode::optional:
    {
        setup_lookahead();
        on_optional();
    }
    break;
    case llspec_opcode::kleene_star:
    {
        setup_lookahead();
        on_kleene_star();
    }
    break;
    case llspec_opcode::kleene_plus:
    {
        setup_lookahead();
        on_kleene_plus();
    }
    break;
    default: TAUL_DEADEND; break;
    }
    return len;
}

