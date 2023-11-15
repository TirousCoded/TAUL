

#include "spec.h"

#include "asserts.h"


taul::spec_writer& taul::spec_writer::grammar_bias(bias b) {
    internal::spec_write_opcode(_temp, spec_opcode::grammar_bias);
    internal::spec_write_bias(_temp, b);
    return *this;
}

taul::spec_writer& taul::spec_writer::close() {
    internal::spec_write_opcode(_temp, spec_opcode::close);
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

taul::spec_writer& taul::spec_writer::lpr(std::string_view name) {
    internal::spec_write_opcode(_temp, spec_opcode::lpr);
    internal::spec_write_str(_temp, name);
    return *this;
}

taul::spec_writer& taul::spec_writer::ppr(std::string_view name) {
    internal::spec_write_opcode(_temp, spec_opcode::ppr);
    internal::spec_write_str(_temp, name);
    return *this;
}

taul::spec_writer& taul::spec_writer::char0() {
    internal::spec_write_opcode(_temp, spec_opcode::char0);
    return *this;
}

taul::spec taul::spec_writer::done() {
    spec result{};
    std::swap(result, _temp);
    return result;
}

void taul::spec_interpreter::interpret(const spec& x) {
    std::size_t offset = 0;
    on_startup();
    while (offset < x.size()) {
        offset += _step(x, offset);
    }
    on_shutdown();
}

std::size_t taul::spec_interpreter::_step(const spec& s, std::size_t offset) {
    static_assert(spec_opcodes == 7);
    std::size_t len = 0;
    const auto opcode = internal::spec_read_opcode(s, offset, len);
    switch (opcode) {
    case spec_opcode::grammar_bias:
    {
        on_grammar_bias(internal::spec_read_bias(s, offset + len, len));
    }
    break;
    case spec_opcode::close:
    {
        on_close();
    }
    break;
    case spec_opcode::lpr_decl:
    {
        on_lpr_decl(internal::spec_read_str(s, offset + len, len));
    }
    break;
    case spec_opcode::ppr_decl:
    {
        on_ppr_decl(internal::spec_read_str(s, offset + len, len));
    }
    break;
    case spec_opcode::lpr:
    {
        on_lpr(internal::spec_read_str(s, offset + len, len));
    }
    break;
    case spec_opcode::ppr:
    {
        on_ppr(internal::spec_read_str(s, offset + len, len));
    }
    break;
    case spec_opcode::char0:
    {
        on_char();
    }
    break;
    default: TAUL_DEADEND; break;
    }
    return len;
}

void taul::internal::spec_write_u8(spec& s, std::uint8_t x) noexcept {
    s.push_back(x);
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

void taul::internal::spec_write_opcode(spec& s, spec_opcode opcode) noexcept {
    spec_write_u8(s, (std::uint8_t)opcode);
}

void taul::internal::spec_write_bias(spec& s, bias b) noexcept {
    spec_write_u8(s, (std::uint8_t)b);
}

void taul::internal::spec_write_str(spec& s, std::string_view x) noexcept {
    spec_write_u32(s, (std::uint32_t)x.length());
    for (const auto& I : x) {
        spec_write_u8(s, I);
    }
}

std::uint8_t taul::internal::spec_read_u8(const spec& s, std::size_t offset, std::size_t& len) noexcept {
    TAUL_IN_BOUNDS(offset, 0, s.size());
    std::uint8_t result = s[offset];
    len++;
    return result;
}

std::uint16_t taul::internal::spec_read_u16(const spec& s, std::size_t offset, std::size_t& len) noexcept {
    TAUL_IN_BOUNDS(offset + 0, 0, s.size());
    TAUL_IN_BOUNDS(offset + 1, 0, s.size());
    std::uint16_t result{};
    std::memcpy((void*)&result, (const void*)(s.data() + offset), sizeof(result));
    len += 2;
    return result;
}

std::uint32_t taul::internal::spec_read_u32(const spec& s, std::size_t offset, std::size_t& len) noexcept {
    TAUL_IN_BOUNDS(offset + 0, 0, s.size());
    TAUL_IN_BOUNDS(offset + 3, 0, s.size());
    std::uint32_t result{};
    std::memcpy((void*)&result, (const void*)(s.data() + offset), sizeof(result));
    len += 4;
    return result;
}

taul::spec_opcode taul::internal::spec_read_opcode(const spec& s, std::size_t offset, std::size_t& len) noexcept {
    return (taul::spec_opcode)spec_read_u8(s, offset, len);
}

taul::bias taul::internal::spec_read_bias(const spec& s, std::size_t offset, std::size_t& len) noexcept {
    return (taul::bias)spec_read_u8(s, offset, len);
}

std::string_view taul::internal::spec_read_str(const spec& s, std::size_t offset, std::size_t& len) noexcept {
    const auto length = spec_read_u32(s, offset, len);
    const auto ptr = (const char*)(s.data() + offset + sizeof(std::uint32_t));
    len += length;
    return std::string_view(ptr, length);
}
