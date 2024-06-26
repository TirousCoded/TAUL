

#include "spec.h"

#include "asserts.h"


taul::spec_writer& taul::spec_writer::pos(source_pos new_pos) {
    internal::spec_write_opcode(_temp, spec_opcode::pos);
    internal::spec_write_u32(_temp, new_pos);
    return *this;
}

taul::spec_writer& taul::spec_writer::close() {
    internal::spec_write_opcode(_temp, spec_opcode::close);
    return *this;
}

taul::spec_writer& taul::spec_writer::alternative() {
    internal::spec_write_opcode(_temp, spec_opcode::alternative);
    return *this;
}

taul::spec_writer& taul::spec_writer::lpr_decl(std::string_view name) {
    internal::spec_write_opcode(_temp, spec_opcode::lpr_decl);
    internal::spec_write_str(_temp, name);
    return *this;
}

taul::spec_writer& taul::spec_writer::ppr_decl(std::string_view name) {
    internal::spec_write_opcode(_temp, spec_opcode::ppr_decl);
    internal::spec_write_str(_temp, name);
    return *this;
}

taul::spec_writer& taul::spec_writer::lpr(std::string_view name, qualifier qualifier) {
    internal::spec_write_opcode(_temp, spec_opcode::lpr);
    internal::spec_write_str(_temp, name);
    internal::spec_write_qualifier(_temp, qualifier);
    return *this;
}

taul::spec_writer& taul::spec_writer::ppr(std::string_view name, qualifier qualifier) {
    internal::spec_write_opcode(_temp, spec_opcode::ppr);
    internal::spec_write_str(_temp, name);
    internal::spec_write_qualifier(_temp, qualifier);
    return *this;
}

taul::spec_writer& taul::spec_writer::end() {
    internal::spec_write_opcode(_temp, spec_opcode::end);
    return *this;
}

taul::spec_writer& taul::spec_writer::any() {
    internal::spec_write_opcode(_temp, spec_opcode::any);
    return *this;
}

taul::spec_writer& taul::spec_writer::string(std::string_view s) {
    internal::spec_write_opcode(_temp, spec_opcode::string);
    internal::spec_write_str(_temp, s);
    return *this;
}

taul::spec_writer& taul::spec_writer::charset(std::string_view s) {
    internal::spec_write_opcode(_temp, spec_opcode::charset);
    internal::spec_write_str(_temp, s);
    return *this;
}

taul::spec_writer& taul::spec_writer::token() {
    internal::spec_write_opcode(_temp, spec_opcode::token);
    return *this;
}

taul::spec_writer& taul::spec_writer::failure() {
    internal::spec_write_opcode(_temp, spec_opcode::failure);
    return *this;
}

taul::spec_writer& taul::spec_writer::name(std::string_view name) {
    internal::spec_write_opcode(_temp, spec_opcode::name);
    internal::spec_write_str(_temp, name);
    return *this;
}

taul::spec_writer& taul::spec_writer::sequence() {
    internal::spec_write_opcode(_temp, spec_opcode::sequence);
    return *this;
}

taul::spec_writer& taul::spec_writer::lookahead() {
    internal::spec_write_opcode(_temp, spec_opcode::lookahead);
    return *this;
}

taul::spec_writer& taul::spec_writer::lookahead_not() {
    internal::spec_write_opcode(_temp, spec_opcode::lookahead_not);
    return *this;
}

taul::spec_writer& taul::spec_writer::not0() {
    internal::spec_write_opcode(_temp, spec_opcode::not0);
    return *this;
}

taul::spec_writer& taul::spec_writer::optional() {
    internal::spec_write_opcode(_temp, spec_opcode::optional);
    return *this;
}

taul::spec_writer& taul::spec_writer::kleene_star() {
    internal::spec_write_opcode(_temp, spec_opcode::kleene_star);
    return *this;
}

taul::spec_writer& taul::spec_writer::kleene_plus() {
    internal::spec_write_opcode(_temp, spec_opcode::kleene_plus);
    return *this;
}

taul::spec_writer& taul::spec_writer::write_spec(const spec& x) {
    internal::write_spec_method_spec_interpreter interp(*this);
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

void taul::spec_interpreter::interpret(const spec& x) {
    std::size_t offset = 0;
    on_startup();
    while (offset < x.bin.size()) {
        offset += _step(x, offset);
    }
    on_shutdown();
}

std::size_t taul::spec_interpreter::_step(const spec& s, std::size_t offset) {
    static_assert(spec_opcodes == 21);
    // len gets passed into internal::spec_read_*** functions and gets added-to
    // each time to record the total number of bytes traversed
    std::size_t len = 0;
    const auto opcode = internal::spec_read_opcode(s, offset, len);
    switch (opcode) {
    case spec_opcode::pos:
    {
        const auto new_pos = internal::spec_read_u32(s, offset + len, len);
        on_pos(new_pos);
    }
    break;
    case spec_opcode::close:
    {
        on_close();
    }
    break;
    case spec_opcode::alternative:
    {
        on_alternative();
    }
    break;
    case spec_opcode::lpr_decl:
    {
        const auto name = internal::spec_read_str(s, offset + len, len);
        on_lpr_decl(name);
    }
    break;
    case spec_opcode::ppr_decl:
    {
        const auto name = internal::spec_read_str(s, offset + len, len);
        on_ppr_decl(name);
    }
    break;
    case spec_opcode::lpr:
    {
        const auto name = internal::spec_read_str(s, offset + len, len);
        const auto qualifier = internal::spec_read_qualifier(s, offset + len, len);
        on_lpr(name, qualifier);
    }
    break;
    case spec_opcode::ppr:
    {
        const auto name = internal::spec_read_str(s, offset + len, len);
        const auto qualifier = internal::spec_read_qualifier(s, offset + len, len);
        on_ppr(name, qualifier);
    }
    break;
    case spec_opcode::end:
    {
        on_end();
    }
    break;
    case spec_opcode::any:
    {
        on_any();
    }
    break;
    case spec_opcode::string:
    {
        const auto s0 = internal::spec_read_str(s, offset + len, len);
        on_string(s0);
    }
    break;
    case spec_opcode::charset:
    {
        const auto s0 = internal::spec_read_str(s, offset + len, len);
        on_charset(s0);
    }
    break;
    case spec_opcode::token:
    {
        on_token();
    }
    break;
    case spec_opcode::failure:
    {
        on_failure();
    }
    break;
    case spec_opcode::name:
    {
        const auto name = internal::spec_read_str(s, offset + len, len);
        on_name(name);
    }
    break;
    case spec_opcode::sequence:
    {
        on_sequence();
    }
    break;
    case spec_opcode::lookahead:
    {
        on_lookahead();
    }
    break;
    case spec_opcode::lookahead_not:
    {
        on_lookahead_not();
    }
    break;
    case spec_opcode::not0:
    {
        on_not();
    }
    break;
    case spec_opcode::optional:
    {
        on_optional();
    }
    break;
    case spec_opcode::kleene_star:
    {
        on_kleene_star();
    }
    break;
    case spec_opcode::kleene_plus:
    {
        on_kleene_plus();
    }
    break;
    default: TAUL_DEADEND; break;
    }
    return len;
}

void taul::internal::spec_write_u8(spec& s, std::uint8_t x) noexcept {
    s.bin.push_back(x);
}

void taul::internal::spec_write_u16(spec& s, std::uint16_t x) noexcept {
    std::uint8_t* ptr = (std::uint8_t*)&x;
    spec_write_u8(s, ptr[0]);
    spec_write_u8(s, ptr[1]);
}

void taul::internal::spec_write_u32(spec& s, std::uint32_t x) noexcept {
    std::uint8_t* ptr = (std::uint8_t*)&x;
    spec_write_u8(s, ptr[0]);
    spec_write_u8(s, ptr[1]);
    spec_write_u8(s, ptr[2]);
    spec_write_u8(s, ptr[3]);
}

void taul::internal::spec_write_char(spec& s, char x) noexcept {
    spec_write_u8(s, *(std::uint8_t*)&x);
}

void taul::internal::spec_write_opcode(spec& s, spec_opcode opcode) noexcept {
    spec_write_u8(s, (std::uint8_t)opcode);
}

void taul::internal::spec_write_qualifier(spec& s, qualifier p) noexcept {
    spec_write_u8(s, (std::uint8_t)p);
}

void taul::internal::spec_write_str(spec& s, std::string_view x) noexcept {
    spec_write_u32(s, (std::uint32_t)x.length());
    for (const auto& I : x) {
        spec_write_u8(s, I);
    }
}

std::uint8_t taul::internal::spec_read_u8(const spec& s, std::size_t offset, std::size_t& len) noexcept {
    TAUL_IN_BOUNDS(offset, 0, s.bin.size());
    std::uint8_t result = s.bin[offset];
    len++;
    return result;
}

std::uint16_t taul::internal::spec_read_u16(const spec& s, std::size_t offset, std::size_t& len) noexcept {
    TAUL_IN_BOUNDS(offset + 0, 0, s.bin.size());
    TAUL_IN_BOUNDS(offset + 1, 0, s.bin.size());
    std::uint16_t result{};
    std::memcpy((void*)&result, (const void*)(s.bin.data() + offset), sizeof(result));
    len += 2;
    return result;
}

std::uint32_t taul::internal::spec_read_u32(const spec& s, std::size_t offset, std::size_t& len) noexcept {
    TAUL_IN_BOUNDS(offset + 0, 0, s.bin.size());
    TAUL_IN_BOUNDS(offset + 3, 0, s.bin.size());
    std::uint32_t result{};
    std::memcpy((void*)&result, (const void*)(s.bin.data() + offset), sizeof(result));
    len += 4;
    return result;
}

char taul::internal::spec_read_char(const spec& s, std::size_t offset, std::size_t& len) noexcept {
    std::uint8_t v = spec_read_u8(s, offset, len);
    return *(char*)&v;
}

taul::spec_opcode taul::internal::spec_read_opcode(const spec& s, std::size_t offset, std::size_t& len) noexcept {
    return (taul::spec_opcode)spec_read_u8(s, offset, len);
}

taul::qualifier taul::internal::spec_read_qualifier(const spec& s, std::size_t offset, std::size_t& len) noexcept {
    return (taul::qualifier)spec_read_u8(s, offset, len);
}

std::string_view taul::internal::spec_read_str(const spec& s, std::size_t offset, std::size_t& len) noexcept {
    const auto length = spec_read_u32(s, offset, len);
    const auto ptr = (const char*)(s.bin.data() + offset + sizeof(std::uint32_t));
    len += length;
    return std::string_view(ptr, length);
}

taul::spec taul::spec::concat(const spec& a, const spec& b) {
    spec result{};
    result.bin.resize(a.bin.size() + b.bin.size());
    if (!result.bin.empty()) {
        std::memcpy((void*)result.bin.data(), (void*)a.bin.data(), a.bin.size());
        std::memcpy((void*)(result.bin.data() + a.bin.size()), (void*)b.bin.data(), b.bin.size());
    }
    result.associate(a.src);
    return result;
}
