

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
    result.bin = internal::buff_concat(a.bin, b.bin);
    result.associate(a.src);
    return result;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::pos(source_pos new_pos) {
    _pos = new_pos;
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::close() {
    internal::buff_write(_temp.bin, llspec_opcode::close, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::alternative() {
    internal::buff_write(_temp.bin, llspec_opcode::alternative, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::lpr_decl(std::string_view name) {
    internal::buff_write(_temp.bin, llspec_opcode::lpr_decl, _pos, name);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::ppr_decl(std::string_view name) {
    internal::buff_write(_temp.bin, llspec_opcode::ppr_decl, _pos, name);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::lpr(std::string_view name, qualifier qualifier) {
    internal::buff_write(_temp.bin, llspec_opcode::lpr, _pos, name, qualifier);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::ppr(std::string_view name, qualifier qualifier) {
    internal::buff_write(_temp.bin, llspec_opcode::ppr, _pos, name, qualifier);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::end() {
    internal::buff_write(_temp.bin, llspec_opcode::end, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::any() {
    internal::buff_write(_temp.bin, llspec_opcode::any, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::string(std::string_view s) {
    internal::buff_write(_temp.bin, llspec_opcode::string, _pos, s);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::charset(std::string_view s) {
    internal::buff_write(_temp.bin, llspec_opcode::charset, _pos, s);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::token() {
    internal::buff_write(_temp.bin, llspec_opcode::token, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::failure() {
    internal::buff_write(_temp.bin, llspec_opcode::failure, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::name(std::string_view name, preced_t preced_val) {
    internal::buff_write(_temp.bin, llspec_opcode::name, _pos, name, preced_val);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::sequence() {
    internal::buff_write(_temp.bin, llspec_opcode::sequence, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::lookahead() {
    internal::buff_write(_temp.bin, llspec_opcode::lookahead, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::lookahead_not() {
    internal::buff_write(_temp.bin, llspec_opcode::lookahead_not, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::not0() {
    internal::buff_write(_temp.bin, llspec_opcode::not0, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::optional() {
    internal::buff_write(_temp.bin, llspec_opcode::optional, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::kleene_star() {
    internal::buff_write(_temp.bin, llspec_opcode::kleene_star, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::kleene_plus() {
    internal::buff_write(_temp.bin, llspec_opcode::kleene_plus, _pos);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::preced_pred(preced_t preced_max, preced_t preced_val) {
    TAUL_ASSERT(preced_val == signal_preced_val); // <- at present, anything else is probably wrong
    internal::buff_write(_temp.bin, llspec_opcode::preced_pred, _pos, preced_max, preced_val);
    return *this;
}

taul::internal::llspec_writer& taul::internal::llspec_writer::pylon() {
    internal::buff_write(_temp.bin, llspec_opcode::pylon, _pos);
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

size_t taul::internal::llspec_interpreter::_step(const llspec& s, size_t offset) {
    static_assert(llspec_opcodes == 22);
    size_t len = 0;
    auto [opcode, new_pos] = internal::buff_read<llspec_opcode, source_pos>(s.bin, offset, len);
    _pos = new_pos;
    // below, this needs to be called after further oprands reads, but before the
    // on_### method call itself to setup _peek properly
    auto setup_peek =
        [&] {
        _peek =
            offset + len < s.bin.size()
            ? std::make_optional(internal::buff_peek_one<llspec_opcode>(s.bin, offset + len))
            : std::nullopt;
        };
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
        auto [name] = internal::buff_read<std::string_view>(s.bin, offset + len, len);
        setup_peek();
        on_lpr_decl(name);
    }
    break;
    case llspec_opcode::ppr_decl:
    {
        auto [name] = internal::buff_read<std::string_view>(s.bin, offset + len, len);
        setup_peek();
        on_ppr_decl(name);
    }
    break;
    case llspec_opcode::lpr:
    {
        auto [name, qualifier] = internal::buff_read<std::string_view, taul::qualifier>(s.bin, offset + len, len);
        setup_peek();
        on_lpr(name, qualifier);
    }
    break;
    case llspec_opcode::ppr:
    {
        auto [name, qualifier] = internal::buff_read<std::string_view, taul::qualifier>(s.bin, offset + len, len);
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
        auto [s0] = internal::buff_read<std::string_view>(s.bin, offset + len, len);
        setup_peek();
        on_string(s0);
    }
    break;
    case llspec_opcode::charset:
    {
        auto [s0] = internal::buff_read<std::string_view>(s.bin, offset + len, len);
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
        auto [name, preced_val] = internal::buff_read<std::string_view, preced_t>(s.bin, offset + len, len);
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
        auto [preced_max, preced_val] = internal::buff_read<preced_t, preced_t>(s.bin, offset + len, len);
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
    return len;
}

