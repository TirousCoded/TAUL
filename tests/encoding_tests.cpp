

#include <gtest/gtest.h>

#include <taul/logger.h>
#include <taul/str.h>
#include <taul/encoding.h>


using namespace taul::string_literals;


// the static assertions in encoding.h will be taken as unit testing the
// semantics covered by them, which the tests below will thus presume

// below ConvertEncoding_*** tests presume impl uses taul::encode and taul::decode,
// and likewise may make some assumptions in this regard

// similar assumptions are made for testing taul::encoder and taul::decoder


// useful:
//  https://www.browserling.com/tools/utf8-encode
//  https://www.coderstool.com/utf16-encoding-decoding
//  https://convertcodes.com/utf32-encode-decode-convert-string/


// we'll use the following chars in our tests:
//      a b c 1 2 3 Δ 魂 💩

constexpr taul::unicode_t decoded_a() noexcept { return U'a'; }
constexpr taul::unicode_t decoded_b() noexcept { return U'b'; }
constexpr taul::unicode_t decoded_c() noexcept { return U'c'; }
constexpr taul::unicode_t decoded_1() noexcept { return U'1'; }
constexpr taul::unicode_t decoded_2() noexcept { return U'2'; }
constexpr taul::unicode_t decoded_3() noexcept { return U'3'; }
constexpr taul::unicode_t decoded_greek() noexcept { return U'Δ'; }
constexpr taul::unicode_t decoded_kanji() noexcept { return U'魂'; }
constexpr taul::unicode_t decoded_emoji() noexcept { return U'💩'; }

constexpr taul::unicode_t decoded_by_index(std::size_t index) noexcept {
    switch (index) {
    case 0: return decoded_a(); break;
    case 1: return decoded_b(); break;
    case 2: return decoded_c(); break;
    case 3: return decoded_1(); break;
    case 4: return decoded_2(); break;
    case 5: return decoded_3(); break;
    case 6: return decoded_greek(); break;
    case 7: return decoded_kanji(); break;
    case 8: return decoded_emoji(); break;
    default: return taul::unicode_t(-1); break;
    }
}

template<typename Char, typename Iterable>
inline std::basic_string<std::remove_const_t<Char>> _build_encoded(const Iterable& x) {
    std::basic_string<Char> result{};
    for (const auto& I : x) {
        result += I;
    }
    return result;
}
inline std::string encoded_chr_utf8(std::endian, std::initializer_list<char> x) {
    return _build_encoded<char>(x);
}
inline std::string encoded_chr_utf16(std::endian bo, std::initializer_list<char> forward, std::initializer_list<char> backward) {
    if (taul::is_big_endian)    return bo == std::endian::big ? _build_encoded<char>(forward) : _build_encoded<char>(backward);
    else                        return bo == std::endian::little ? _build_encoded<char>(forward) : _build_encoded<char>(backward);
}
inline std::u16string encoded_chr_utf16_alt(std::endian bo, std::initializer_list<char16_t> forward, std::initializer_list<char16_t> backward) {
    if (taul::is_big_endian)    return bo == std::endian::big ? _build_encoded<char16_t>(forward) : _build_encoded<char16_t>(backward);
    else                        return bo == std::endian::little ? _build_encoded<char16_t>(forward) : _build_encoded<char16_t>(backward);
}
inline std::string encoded_chr_utf32(std::endian bo, std::initializer_list<char> forward, std::initializer_list<char> backward) {
    if (taul::is_big_endian)    return bo == std::endian::big ? _build_encoded<char>(forward) : _build_encoded<char>(backward);
    else                        return bo == std::endian::little ? _build_encoded<char>(forward) : _build_encoded<char>(backward);
}
inline std::u32string encoded_chr_utf32_alt(std::endian bo, std::initializer_list<char32_t> forward, std::initializer_list<char32_t> backward) {
    if (taul::is_big_endian)    return bo == std::endian::big ? _build_encoded<char32_t>(forward) : _build_encoded<char32_t>(backward);
    else                        return bo == std::endian::little ? _build_encoded<char32_t>(forward) : _build_encoded<char32_t>(backward);
}

inline std::string encoded_a_utf8(std::endian bo) { return encoded_chr_utf8(bo, { 0x61i8 }); }
inline std::string encoded_a_utf16(std::endian bo) { return encoded_chr_utf16(bo, { 0x61i8, 0x00i8 }, { 0x00i8, 0x61i8 }); }
inline std::u16string encoded_a_utf16_alt(std::endian bo) { return encoded_chr_utf16_alt(bo, { 0x0061i16 }, { 0x6100i16 }); }
inline std::string encoded_a_utf32(std::endian bo) { return encoded_chr_utf32(bo, { 0x61i8, 0x00i8, 0x00i8, 0x00i8 }, { 0x00i8, 0x00i8, 0x00i8, 0x61i8 }); }
inline std::u32string encoded_a_utf32_alt(std::endian bo) { return encoded_chr_utf32_alt(bo, { 0x00000061i32 }, { 0x61000000i32 }); }

inline std::string encoded_b_utf8(std::endian bo) { return encoded_chr_utf8(bo, { 0x62i8 }); }
inline std::string encoded_b_utf16(std::endian bo) { return encoded_chr_utf16(bo, { 0x62i8, 0x00i8 }, { 0x00i8, 0x62i8 }); }
inline std::u16string encoded_b_utf16_alt(std::endian bo) { return encoded_chr_utf16_alt(bo, { 0x0062i16 }, { 0x6200i16 }); }
inline std::string encoded_b_utf32(std::endian bo) { return encoded_chr_utf32(bo, { 0x62i8, 0x00i8, 0x00i8, 0x00i8 }, { 0x00i8, 0x00i8, 0x00i8, 0x62i8 }); }
inline std::u32string encoded_b_utf32_alt(std::endian bo) { return encoded_chr_utf32_alt(bo, { 0x00000062i32 }, { 0x62000000i32 }); }

inline std::string encoded_c_utf8(std::endian bo) { return encoded_chr_utf8(bo, { 0x63i8 }); }
inline std::string encoded_c_utf16(std::endian bo) { return encoded_chr_utf16(bo, { 0x63i8, 0x00i8 }, { 0x00i8, 0x63i8 }); }
inline std::u16string encoded_c_utf16_alt(std::endian bo) { return encoded_chr_utf16_alt(bo, { 0x0063i16 }, { 0x6300i16 }); }
inline std::string encoded_c_utf32(std::endian bo) { return encoded_chr_utf32(bo, { 0x63i8, 0x00i8, 0x00i8, 0x00i8 }, { 0x00i8, 0x00i8, 0x00i8, 0x63i8 }); }
inline std::u32string encoded_c_utf32_alt(std::endian bo) { return encoded_chr_utf32_alt(bo, { 0x00000063i32 }, { 0x63000000i32 }); }

inline std::string encoded_1_utf8(std::endian bo) { return encoded_chr_utf8(bo, { 0x31i8 }); }
inline std::string encoded_1_utf16(std::endian bo) { return encoded_chr_utf16(bo, { 0x31i8, 0x00i8 }, { 0x00i8, 0x31i8 }); }
inline std::u16string encoded_1_utf16_alt(std::endian bo) { return encoded_chr_utf16_alt(bo, { 0x0031i16 }, { 0x3100i16 }); }
inline std::string encoded_1_utf32(std::endian bo) { return encoded_chr_utf32(bo, { 0x31i8, 0x00i8, 0x00i8, 0x00i8 }, { 0x00i8, 0x00i8, 0x00i8, 0x31i8 }); }
inline std::u32string encoded_1_utf32_alt(std::endian bo) { return encoded_chr_utf32_alt(bo, { 0x00000031i32 }, { 0x31000000i32 }); }

inline std::string encoded_2_utf8(std::endian bo) { return encoded_chr_utf8(bo, { 0x32i8 }); }
inline std::string encoded_2_utf16(std::endian bo) { return encoded_chr_utf16(bo, { 0x32i8, 0x00i8 }, { 0x00i8, 0x32i8 }); }
inline std::u16string encoded_2_utf16_alt(std::endian bo) { return encoded_chr_utf16_alt(bo, { 0x0032i16 }, { 0x3200i16 }); }
inline std::string encoded_2_utf32(std::endian bo) { return encoded_chr_utf32(bo, { 0x32i8, 0x00i8, 0x00i8, 0x00i8 }, { 0x00i8, 0x00i8, 0x00i8, 0x32i8 }); }
inline std::u32string encoded_2_utf32_alt(std::endian bo) { return encoded_chr_utf32_alt(bo, { 0x00000032i32 }, { 0x32000000i32 }); }

inline std::string encoded_3_utf8(std::endian bo) { return encoded_chr_utf8(bo, { 0x33i8 }); }
inline std::string encoded_3_utf16(std::endian bo) { return encoded_chr_utf16(bo, { 0x33i8, 0x00i8 }, { 0x00i8, 0x33i8 }); }
inline std::u16string encoded_3_utf16_alt(std::endian bo) { return encoded_chr_utf16_alt(bo, { 0x0033i16 }, { 0x3300i16 }); }
inline std::string encoded_3_utf32(std::endian bo) { return encoded_chr_utf32(bo, { 0x33i8, 0x00i8, 0x00i8, 0x00i8 }, { 0x00i8, 0x00i8, 0x00i8, 0x33i8 }); }
inline std::u32string encoded_3_utf32_alt(std::endian bo) { return encoded_chr_utf32_alt(bo, { 0x00000033i32 }, { 0x33000000i32 }); }

inline std::string encoded_greek_utf8(std::endian bo) { return encoded_chr_utf8(bo, { 0xcei8, 0x94i8 }); }
inline std::string encoded_greek_utf16(std::endian bo) { return encoded_chr_utf16(bo, { 0x94i8, 0x03i8 }, { 0x03i8, 0x94i8 }); }
inline std::u16string encoded_greek_utf16_alt(std::endian bo) { return encoded_chr_utf16_alt(bo, { 0x0394i16 }, { char16_t(0x9403i16) }); }
inline std::string encoded_greek_utf32(std::endian bo) { return encoded_chr_utf32(bo, { 0x94i8, 0x03i8, 0x00i8, 0x00i8 }, { 0x00i8, 0x00i8, 0x03i8, 0x94i8 }); }
inline std::u32string encoded_greek_utf32_alt(std::endian bo) { return encoded_chr_utf32_alt(bo, { 0x00000394i32 }, { char32_t(0x94030000i32) }); }

inline std::string encoded_kanji_utf8(std::endian bo) { return encoded_chr_utf8(bo, { 0xe9i8, 0xadi8, 0x82i8 }); }
inline std::string encoded_kanji_utf16(std::endian bo) { return encoded_chr_utf16(bo, { 0x42i8, 0x9bi8 }, { 0x9bi8, 0x42i8 }); }
inline std::u16string encoded_kanji_utf16_alt(std::endian bo) { return encoded_chr_utf16_alt(bo, { char16_t(0x9b42i16) }, { 0x429bi16 }); }
inline std::string encoded_kanji_utf32(std::endian bo) { return encoded_chr_utf32(bo, { 0x42i8, 0x9bi8, 0x00i8, 0x00i8 }, { 0x00i8, 0x00i8, 0x9bi8, 0x42i8 }); }
inline std::u32string encoded_kanji_utf32_alt(std::endian bo) { return encoded_chr_utf32_alt(bo, { 0x00009b42i32 }, { 0x429b0000i32 }); }

inline std::string encoded_emoji_utf8(std::endian bo) { return encoded_chr_utf8(bo, { 0xf0i8, 0x9fi8, 0x92i8, 0xa9i8 }); }
inline std::string encoded_emoji_utf16(std::endian bo) { return encoded_chr_utf16(bo, { 0x3di8, 0xd8i8, 0xa9i8, 0xdci8 }, { 0xd8i8, 0x3di8, 0xdci8, 0xa9i8 }); }
inline std::u16string encoded_emoji_utf16_alt(std::endian bo) { return encoded_chr_utf16_alt(bo, { char16_t(0xd83di16), char16_t(0xdca9i16) }, { char16_t(0x3dd8i16), char16_t(0xa9dci16) }); }
inline std::string encoded_emoji_utf32(std::endian bo) { return encoded_chr_utf32(bo, { 0xa9i8, 0xf4i8, 0x01i8, 0x00i8 }, { 0x00i8, 0x01i8, 0xf4i8, 0xa9i8 }); }
inline std::u32string encoded_emoji_utf32_alt(std::endian bo) { return encoded_chr_utf32_alt(bo, { char32_t(0x0001f4a9i32) }, { char32_t(0xa9f40100i32) }); }

inline std::string encoded_by_index_utf8(std::size_t index, std::endian bo) {
    switch (index) {
    case 0: return encoded_a_utf8(bo); break;
    case 1: return encoded_b_utf8(bo); break;
    case 2: return encoded_c_utf8(bo); break;
    case 3: return encoded_1_utf8(bo); break;
    case 4: return encoded_2_utf8(bo); break;
    case 5: return encoded_3_utf8(bo); break;
    case 6: return encoded_greek_utf8(bo); break;
    case 7: return encoded_kanji_utf8(bo); break;
    case 8: return encoded_emoji_utf8(bo); break;
    default: TAUL_DEADEND; return ""; break;
    }
}
inline std::string encoded_by_index_utf16(std::size_t index, std::endian bo) {
    switch (index) {
    case 0: return encoded_a_utf16(bo); break;
    case 1: return encoded_b_utf16(bo); break;
    case 2: return encoded_c_utf16(bo); break;
    case 3: return encoded_1_utf16(bo); break;
    case 4: return encoded_2_utf16(bo); break;
    case 5: return encoded_3_utf16(bo); break;
    case 6: return encoded_greek_utf16(bo); break;
    case 7: return encoded_kanji_utf16(bo); break;
    case 8: return encoded_emoji_utf16(bo); break;
    default: TAUL_DEADEND; return ""; break;
    }
}
inline std::u16string encoded_by_index_utf16_alt(std::size_t index, std::endian bo) {
    switch (index) {
    case 0: return encoded_a_utf16_alt(bo); break;
    case 1: return encoded_b_utf16_alt(bo); break;
    case 2: return encoded_c_utf16_alt(bo); break;
    case 3: return encoded_1_utf16_alt(bo); break;
    case 4: return encoded_2_utf16_alt(bo); break;
    case 5: return encoded_3_utf16_alt(bo); break;
    case 6: return encoded_greek_utf16_alt(bo); break;
    case 7: return encoded_kanji_utf16_alt(bo); break;
    case 8: return encoded_emoji_utf16_alt(bo); break;
    default: TAUL_DEADEND; return u""; break;
    }
}
inline std::string encoded_by_index_utf32(std::size_t index, std::endian bo) {
    switch (index) {
    case 0: return encoded_a_utf32(bo); break;
    case 1: return encoded_b_utf32(bo); break;
    case 2: return encoded_c_utf32(bo); break;
    case 3: return encoded_1_utf32(bo); break;
    case 4: return encoded_2_utf32(bo); break;
    case 5: return encoded_3_utf32(bo); break;
    case 6: return encoded_greek_utf32(bo); break;
    case 7: return encoded_kanji_utf32(bo); break;
    case 8: return encoded_emoji_utf32(bo); break;
    default: TAUL_DEADEND; return ""; break;
    }
}
inline std::u32string encoded_by_index_utf32_alt(std::size_t index, std::endian bo) {
    switch (index) {
    case 0: return encoded_a_utf32_alt(bo); break;
    case 1: return encoded_b_utf32_alt(bo); break;
    case 2: return encoded_c_utf32_alt(bo); break;
    case 3: return encoded_1_utf32_alt(bo); break;
    case 4: return encoded_2_utf32_alt(bo); break;
    case 5: return encoded_3_utf32_alt(bo); break;
    case 6: return encoded_greek_utf32_alt(bo); break;
    case 7: return encoded_kanji_utf32_alt(bo); break;
    case 8: return encoded_emoji_utf32_alt(bo); break;
    default: TAUL_DEADEND; return U""; break;
    }
}


// we'll also define the following 'illegal' sequences (w/ these unique per encoding:)
//      1) illegal UTF-8 empty string
//      2) illegal surrogate codepoint U+D800 in UTF-8
//      3) illegal surrogate codepoint U+DFFF in UTF-8
//      4) illegal codepoint U+110000 in UTF-8
//      5) illegal UTF-8 string ending before the end of the character
//      6) illegal UTF-8 non-continuation byte before the end of the character
//      7) illegal UTF-8 continuation byte
//      8) illegal UTF-8 overlong encoding
//      
//      1) illegal UTF-16 empty string
//      2) illegal UTF-16 leading surrogate followed by end of the character
//      3) illegal UTF-16 leading surrogate followed by unit which is not trailing surrogate
//      4) illegal UTF-16 leading surrogate followed by one byte
//      5) illegal UTF-16 trailing surrogate
//      6) illegal UTF-16 one byte string
//      
//      1) illegal UTF-32 empty string
//      2) illegal surrogate codepoint U+D800 in UTF-32
//      3) illegal surrogate codepoint U+DFFF in UTF-32
//      4) illegal codepoint U+110000 in UTF-32
//      5) illegal UTF-32 three byte string
//      6) illegal UTF-32 two byte string
//      7) illegal UTF-32 one byte string

//   0x110000
// = 0b0001'0001'0000'0000'0000'0000 (binary)
// = 0b1111'0100 0b1001'0000 0b1000'0000 0b1000'0000 (UTF-8)
// = 0b0000'0000 0b0001'0001 0b0000'0000 0b0000'0000 (UTF-32 BE)
// = 0b0000'0000 0b0000'0000 0b0001'0001 0b0000'0000 (UTF-32 LE)

//   0xd800
// = 0b1101'1000'0000'0000 (binary)
// = 0b1110'1101 0b1010'0000 0b1000'0000 (UTF-8)
// = 0b0000'0000 0b0000'0000 0b1101'1000 0b0000'0000 (UTF-32 BE)
// = 0b0000'0000 0b1101'1000 0b0000'0000 0b0000'0000 (UTF-32 LE)

//   0xdfff
// = 0b1101'1111'1111'1111 (binary)
// = 0b1110'1101 0b1011'1111 0b1011'1111 (UTF-8)
// = 0b0000'0000 0b0000'0000 0b1101'1111 0b1111'1111 (UTF-32 BE)
// = 0b1111'1111 0b1101'1111 0b0000'0000 0b0000'0000 (UTF-32 LE)

//   0x0063
// = 0b0110'0011 (binary)
// = 0b1111'0000 0b1000'0000 0b1000'0001 0b1010'0011 (overlong UTF-8)

inline std::string encoded_utf8_illegal_1(std::endian bo) { return encoded_chr_utf8(bo, std::initializer_list<char>{}); }
inline std::string encoded_utf8_illegal_2(std::endian bo) { return encoded_chr_utf8(bo, { 0b1110'1101i8, 0b1010'0000i8, 0b1000'0000i8 }); }
inline std::string encoded_utf8_illegal_3(std::endian bo) { return encoded_chr_utf8(bo, { 0b1110'1101i8, 0b1011'1111i8, 0b1011'1111i8 }); }
inline std::string encoded_utf8_illegal_4(std::endian bo) { return encoded_chr_utf8(bo, { 0b1111'0100i8, 0b1001'0000i8, 0b1000'0000i8, 0b1000'0000i8 }); }
inline std::string encoded_utf8_illegal_5(std::endian bo) { return encoded_chr_utf8(bo, { 0xf0i8, 0x9fi8 }); }
inline std::string encoded_utf8_illegal_6(std::endian bo) { return encoded_chr_utf8(bo, { 0xf0i8, 0x9fi8, 0xf0i8, 0x9fi8, 0x92i8, 0xa9i8 }); }
inline std::string encoded_utf8_illegal_7(std::endian bo) { return encoded_chr_utf8(bo, { 0xa9i8 }); }
inline std::string encoded_utf8_illegal_8(std::endian bo) { return encoded_chr_utf8(bo, { 0b1111'0000i8, 0b1000'0000i8, 0b1000'0001i8, 0b1010'0011i8 }); }

inline std::string encoded_utf16_illegal_1(std::endian bo) { return ""; }
inline std::u16string encoded_utf16_illegal_1_alt(std::endian bo) { return u""; }
inline std::string encoded_utf16_illegal_2(std::endian bo) { return encoded_chr_utf16(bo, { 0x3di8, 0xd8i8 }, { 0xd8i8, 0x3di8 }); }
inline std::u16string encoded_utf16_illegal_2_alt(std::endian bo) { return encoded_chr_utf16_alt(bo, { char16_t(0xd83di16) }, { char16_t(0x3dd8i16) }); }
inline std::string encoded_utf16_illegal_3(std::endian bo) { return encoded_chr_utf16(bo, { 0x3di8, 0xd8i8, 0x3di8, 0xd8i8 }, { 0xd8i8, 0x3di8, 0xd8i8, 0x3di8 }); }
inline std::u16string encoded_utf16_illegal_3_alt(std::endian bo) { return encoded_chr_utf16_alt(bo, { char16_t(0xd83di16), char16_t(0xd83di16) }, { char16_t(0x3dd8i16), char16_t(0x3dd8i16) }); }
inline std::string encoded_utf16_illegal_4(std::endian bo) { return encoded_chr_utf16(bo, { 0x3di8, 0xd8i8 }, { 0xd8i8, 0x3di8 }) + "\xac"; }
inline std::string encoded_utf16_illegal_5(std::endian bo) { return encoded_chr_utf16(bo, { 0xa9i8, 0xdci8 }, { 0xdci8, 0xa9i8 }); }
inline std::u16string encoded_utf16_illegal_5_alt(std::endian bo) { return encoded_chr_utf16_alt(bo, { char16_t(0xdca9i16) }, { char16_t(0xa9dci16) }); }
inline std::string encoded_utf16_illegal_6(std::endian bo) { return "\xac"; }

inline std::string encoded_utf32_illegal_1(std::endian bo) { return ""; }
inline std::u32string encoded_utf32_illegal_1_alt(std::endian bo) { return U""; }
inline std::string encoded_utf32_illegal_2(std::endian bo) { return encoded_chr_utf32(bo, { 0b0000'0000i8, 0b1101'1000i8, 0b0000'0000i8, 0b0000'0000i8 }, { 0b0000'0000i8, 0b0000'0000i8, 0b1101'1000i8, 0b0000'0000i8 }); }
inline std::u32string encoded_utf32_illegal_2_alt(std::endian bo) { return encoded_chr_utf32_alt(bo, { char32_t(0b0000'0000'0000'0000'1101'1000'0000'0000i32) }, { char32_t(0b0000'0000'1101'1000'0000'0000'0000'0000i32) }); }
inline std::string encoded_utf32_illegal_3(std::endian bo) { return encoded_chr_utf32(bo, { 0b1111'1111i8, 0b1101'1111i8, 0b0000'0000i8, 0b0000'0000i8 }, { 0b1111'1111i8, 0b1101'1111i8, 0b0000'0000i8, 0b0000'0000i8 }); }
inline std::u32string encoded_utf32_illegal_3_alt(std::endian bo) { return encoded_chr_utf32_alt(bo, { char32_t(0b0000'0000'0000'0000'1101'1111'1111'1111i32) }, { char32_t(0b1111'1111'1101'1111'0000'0000'0000'0000i32) }); }
inline std::string encoded_utf32_illegal_4(std::endian bo) { return encoded_chr_utf32(bo, { 0b0000'0000i8, 0b0000'0000i8, 0b0001'0001i8, 0b0000'0000i8 }, { 0b0000'0000i8, 0b0001'0001i8, 0b0000'0000i8, 0b0000'0000i8 }); }
inline std::u32string encoded_utf32_illegal_4_alt(std::endian bo) { return encoded_chr_utf32_alt(bo, { char32_t(0b0000'0000'0001'0001'0000'0000'0000'0000i32) }, { char32_t(0b0000'0000'0000'0000'0001'0001'0000'0000i32) }); }
inline std::string encoded_utf32_illegal_5(std::endian bo) { return "\xaf\xbe\xcd"; }
inline std::string encoded_utf32_illegal_6(std::endian bo) { return "\xaf\xbe"; }
inline std::string encoded_utf32_illegal_7(std::endian bo) { return "\xaf"; }


// these are the input strings used to test convert_encoding

// tests for taul::convert_encoding depends upon taul::encode and taul::decode working,
// so if they don't expect possible false positives/negatives for taul::convert_encoding

// the first ones are for successful usage, w/ all of these being the same string, but
// w/ different encodings + BOM usages

// the ensure_bom_in_output of convert_encoding_string below is used to ensure that we can
// test w/ an encoded string w/ an 'anti-BOM' which convert_encoding will need to handle

inline std::string convert_encoding_string_utf8(std::endian bo, bool output_bom) {
    std::string result{};
    if (output_bom) {
        result += std::string(taul::encoding_bom_chars<char>(taul::encoding::utf8));
    }
    result += encoded_by_index_utf8(0, bo);
    result += encoded_by_index_utf8(6, bo);
    result += encoded_by_index_utf8(3, bo);
    result += encoded_by_index_utf8(1, bo);
    result += encoded_by_index_utf8(7, bo);
    result += encoded_by_index_utf8(4, bo);
    result += encoded_by_index_utf8(2, bo);
    result += encoded_by_index_utf8(8, bo);
    result += encoded_by_index_utf8(5, bo);
    return result;
}

inline std::string convert_encoding_string_utf16(std::endian bo, bool output_bom) {
    std::string result{};
    if (output_bom) {
        result += std::string(taul::encoding_bom_chars<char>(taul::change_endian(bo, taul::encoding::utf16)));
    }
    result += encoded_by_index_utf16(0, bo);
    result += encoded_by_index_utf16(6, bo);
    result += encoded_by_index_utf16(3, bo);
    result += encoded_by_index_utf16(1, bo);
    result += encoded_by_index_utf16(7, bo);
    result += encoded_by_index_utf16(4, bo);
    result += encoded_by_index_utf16(2, bo);
    result += encoded_by_index_utf16(8, bo);
    result += encoded_by_index_utf16(5, bo);
    return result;
}

inline std::u16string convert_encoding_string_utf16_alt(std::endian bo, bool output_bom) {
    std::u16string result{};
    if (output_bom) {
        result += std::u16string(taul::encoding_bom_chars<char16_t>(taul::change_endian(bo, taul::encoding::utf16)));
    }
    result += encoded_by_index_utf16_alt(0, bo);
    result += encoded_by_index_utf16_alt(6, bo);
    result += encoded_by_index_utf16_alt(3, bo);
    result += encoded_by_index_utf16_alt(1, bo);
    result += encoded_by_index_utf16_alt(7, bo);
    result += encoded_by_index_utf16_alt(4, bo);
    result += encoded_by_index_utf16_alt(2, bo);
    result += encoded_by_index_utf16_alt(8, bo);
    result += encoded_by_index_utf16_alt(5, bo);
    return result;
}

inline std::string convert_encoding_string_utf32(std::endian bo, bool output_bom) {
    std::string result{};
    if (output_bom) {
        result += std::string(taul::encoding_bom_chars<char>(taul::change_endian(bo, taul::encoding::utf32)));
    }
    result += encoded_by_index_utf32(0, bo);
    result += encoded_by_index_utf32(6, bo);
    result += encoded_by_index_utf32(3, bo);
    result += encoded_by_index_utf32(1, bo);
    result += encoded_by_index_utf32(7, bo);
    result += encoded_by_index_utf32(4, bo);
    result += encoded_by_index_utf32(2, bo);
    result += encoded_by_index_utf32(8, bo);
    result += encoded_by_index_utf32(5, bo);
    return result;
}

inline std::u32string convert_encoding_string_utf32_alt(std::endian bo, bool output_bom) {
    std::u32string result{};
    if (output_bom) {
        result += std::u32string(taul::encoding_bom_chars<char32_t>(taul::change_endian(bo, taul::encoding::utf32)));
    }
    result += encoded_by_index_utf32_alt(0, bo);
    result += encoded_by_index_utf32_alt(6, bo);
    result += encoded_by_index_utf32_alt(3, bo);
    result += encoded_by_index_utf32_alt(1, bo);
    result += encoded_by_index_utf32_alt(7, bo);
    result += encoded_by_index_utf32_alt(4, bo);
    result += encoded_by_index_utf32_alt(2, bo);
    result += encoded_by_index_utf32_alt(8, bo);
    result += encoded_by_index_utf32_alt(5, bo);
    return result;
}

inline std::string convert_encoding_string(taul::encoding e, std::endian bo, bool output_bom) {
    switch (taul::base_encoding(e)) {
    case taul::encoding::utf8: return convert_encoding_string_utf8(bo, output_bom); break;
    case taul::encoding::utf16: return convert_encoding_string_utf16(bo, output_bom); break;
    case taul::encoding::utf32: return convert_encoding_string_utf32(bo, output_bom); break;
    default: TAUL_DEADEND; break;
    }
    return std::string{};
}


TEST(EncodingTests, Encode) {
    auto lgr = taul::make_stderr_logger();
    const std::vector<taul::encoding> utf8_encodings = {
        taul::encoding::utf8,
        taul::encoding::utf8_bom,
    };
    const std::vector<taul::encoding> utf16_encodings = {
        taul::encoding::utf16,
        taul::encoding::utf16_be,
        taul::encoding::utf16_le,
        taul::encoding::utf16_bom,
    };
    const std::vector<taul::encoding> utf32_encodings = {
        taul::encoding::utf32,
        taul::encoding::utf32_be,
        taul::encoding::utf32_le,
        taul::encoding::utf32_bom,
    };
    for (std::size_t i = 0; i < 9; i++) {
        TAUL_LOG(lgr, "testing taul::encode w/ input {}", (std::size_t)decoded_by_index(i));
        for (const auto& I : utf8_encodings) {
            TAUL_LOG(lgr, "testing taul::encode w/ encoding {}", I);
            const auto result = taul::encode<char>(I, decoded_by_index(i));
            EXPECT_TRUE(result);
            if (result) EXPECT_EQ(result.value(), encoded_by_index_utf8(i, taul::explicit_endian(I)));
        }
        for (const auto& I : utf16_encodings) {
            TAUL_LOG(lgr, "testing taul::encode w/ encoding {}", I);
            {
                const auto result = taul::encode<char>(I, decoded_by_index(i));
                EXPECT_TRUE(result);
                if (result) EXPECT_EQ(result.value(), encoded_by_index_utf16(i, taul::explicit_endian(I)));
            }
            {
                const auto result = taul::encode<char16_t>(I, decoded_by_index(i));
                EXPECT_TRUE(result);
                if (result) EXPECT_EQ(result.value(), encoded_by_index_utf16_alt(i, taul::explicit_endian(I)));
            }
        }
        for (const auto& I : utf32_encodings) {
            TAUL_LOG(lgr, "testing taul::encode w/ encoding {}", I);
            {
                const auto result = taul::encode<char>(I, decoded_by_index(i));
                EXPECT_TRUE(result);
                if (result) EXPECT_EQ(result.value(), encoded_by_index_utf32(i, taul::explicit_endian(I)));
            }
            {
                const auto result = taul::encode<char32_t>(I, decoded_by_index(i));
                EXPECT_TRUE(result);
                if (result) EXPECT_EQ(result.value(), encoded_by_index_utf32_alt(i, taul::explicit_endian(I)));
            }
        }
    }
}

TEST(EncodingTests, Encode_FailDueToInvalidInput) {
    EXPECT_FALSE(taul::is_unicode(0x110000));
    EXPECT_FALSE(taul::is_unicode(taul::first_surrogate));
    EXPECT_FALSE(taul::is_unicode(taul::last_surrogate));

    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf8, 0x110000));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf16, 0x110000));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf32, 0x110000));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf16_be, 0x110000));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf16_le, 0x110000));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf32_be, 0x110000));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf32_le, 0x110000));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf8_bom, 0x110000));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf16_bom, 0x110000));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf32_bom, 0x110000));

    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf8, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf16, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf32, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf16_be, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf16_le, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf32_be, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf32_le, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf8_bom, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf16_bom, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf32_bom, taul::first_surrogate));

    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf8, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf16, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf32, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf16_be, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf16_le, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf32_be, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf32_le, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf8_bom, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf16_bom, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char>(taul::encoding::utf32_bom, taul::last_surrogate));

    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf8, 0x110000));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf16, 0x110000));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf32, 0x110000));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf16_be, 0x110000));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf16_le, 0x110000));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf32_be, 0x110000));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf32_le, 0x110000));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf8_bom, 0x110000));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf16_bom, 0x110000));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf32_bom, 0x110000));

    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf8, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf16, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf32, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf16_be, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf16_le, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf32_be, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf32_le, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf8_bom, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf16_bom, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf32_bom, taul::first_surrogate));

    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf8, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf16, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf32, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf16_be, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf16_le, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf32_be, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf32_le, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf8_bom, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf16_bom, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char8_t>(taul::encoding::utf32_bom, taul::last_surrogate));

    EXPECT_FALSE(taul::encode<char16_t>(taul::encoding::utf16, 0x110000));
    EXPECT_FALSE(taul::encode<char16_t>(taul::encoding::utf16_be, 0x110000));
    EXPECT_FALSE(taul::encode<char16_t>(taul::encoding::utf16_le, 0x110000));
    EXPECT_FALSE(taul::encode<char16_t>(taul::encoding::utf16_bom, 0x110000));

    EXPECT_FALSE(taul::encode<char16_t>(taul::encoding::utf16, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char16_t>(taul::encoding::utf16_be, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char16_t>(taul::encoding::utf16_le, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char16_t>(taul::encoding::utf16_bom, taul::first_surrogate));
    
    EXPECT_FALSE(taul::encode<char16_t>(taul::encoding::utf16, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char16_t>(taul::encoding::utf16_be, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char16_t>(taul::encoding::utf16_le, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char16_t>(taul::encoding::utf16_bom, taul::last_surrogate));

    EXPECT_FALSE(taul::encode<char32_t>(taul::encoding::utf32, 0x110000));
    EXPECT_FALSE(taul::encode<char32_t>(taul::encoding::utf32_be, 0x110000));
    EXPECT_FALSE(taul::encode<char32_t>(taul::encoding::utf32_le, 0x110000));
    EXPECT_FALSE(taul::encode<char32_t>(taul::encoding::utf32_bom, 0x110000));
    
    EXPECT_FALSE(taul::encode<char32_t>(taul::encoding::utf32, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char32_t>(taul::encoding::utf32_be, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char32_t>(taul::encoding::utf32_le, taul::first_surrogate));
    EXPECT_FALSE(taul::encode<char32_t>(taul::encoding::utf32_bom, taul::first_surrogate));
    
    EXPECT_FALSE(taul::encode<char32_t>(taul::encoding::utf32, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char32_t>(taul::encoding::utf32_be, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char32_t>(taul::encoding::utf32_le, taul::last_surrogate));
    EXPECT_FALSE(taul::encode<char32_t>(taul::encoding::utf32_bom, taul::last_surrogate));

    EXPECT_FALSE(taul::encode<wchar_t>(taul::wchar_encoding(taul::encoding::utf16), 0x110000));
    EXPECT_FALSE(taul::encode<wchar_t>(taul::wchar_encoding(taul::encoding::utf16_be), 0x110000));
    EXPECT_FALSE(taul::encode<wchar_t>(taul::wchar_encoding(taul::encoding::utf16_le), 0x110000));
    EXPECT_FALSE(taul::encode<wchar_t>(taul::wchar_encoding(taul::encoding::utf16_bom), 0x110000));

    EXPECT_FALSE(taul::encode<wchar_t>(taul::wchar_encoding(taul::encoding::utf16), taul::first_surrogate));
    EXPECT_FALSE(taul::encode<wchar_t>(taul::wchar_encoding(taul::encoding::utf16_be), taul::first_surrogate));
    EXPECT_FALSE(taul::encode<wchar_t>(taul::wchar_encoding(taul::encoding::utf16_le), taul::first_surrogate));
    EXPECT_FALSE(taul::encode<wchar_t>(taul::wchar_encoding(taul::encoding::utf16_bom), taul::first_surrogate));
    
    EXPECT_FALSE(taul::encode<wchar_t>(taul::wchar_encoding(taul::encoding::utf16), taul::last_surrogate));
    EXPECT_FALSE(taul::encode<wchar_t>(taul::wchar_encoding(taul::encoding::utf16_be), taul::last_surrogate));
    EXPECT_FALSE(taul::encode<wchar_t>(taul::wchar_encoding(taul::encoding::utf16_le), taul::last_surrogate));
    EXPECT_FALSE(taul::encode<wchar_t>(taul::wchar_encoding(taul::encoding::utf16_bom), taul::last_surrogate));
}

TEST(EncodingTests, Encode_FailDueToNotAllowedCharType) {
    EXPECT_FALSE(taul::encode<char16_t>(taul::encoding::utf8, U'a'));
    EXPECT_FALSE(taul::encode<char16_t>(taul::encoding::utf32, U'a'));
    EXPECT_FALSE(taul::encode<char16_t>(taul::encoding::utf32_be, U'a'));
    EXPECT_FALSE(taul::encode<char16_t>(taul::encoding::utf32_le, U'a'));

    EXPECT_FALSE(taul::encode<char32_t>(taul::encoding::utf8, U'a'));
    EXPECT_FALSE(taul::encode<char32_t>(taul::encoding::utf16, U'a'));
    EXPECT_FALSE(taul::encode<char32_t>(taul::encoding::utf16_be, U'a'));
    EXPECT_FALSE(taul::encode<char32_t>(taul::encoding::utf16_le, U'a'));
}

TEST(EncodingTests, Decode) {
    auto lgr = taul::make_stderr_logger();
    const std::vector<taul::encoding> utf8_encodings = {
        taul::encoding::utf8,
        taul::encoding::utf8_bom,
    };
    const std::vector<taul::encoding> utf16_encodings = {
        taul::encoding::utf16,
        taul::encoding::utf16_be,
        taul::encoding::utf16_le,
        taul::encoding::utf16_bom,
    };
    const std::vector<taul::encoding> utf32_encodings = {
        taul::encoding::utf32,
        taul::encoding::utf32_be,
        taul::encoding::utf32_le,
        taul::encoding::utf32_bom,
    };
    for (std::size_t i = 0; i < 9; i++) {
        TAUL_LOG(lgr, "testing taul::decode w/ output {}", (std::size_t)decoded_by_index(i));
        for (const auto& I : utf8_encodings) {
            TAUL_LOG(lgr, "testing taul::decode w/ encoding {}", I);
            const auto result = taul::decode<char>(I, encoded_by_index_utf8(i, taul::explicit_endian(I)));
            EXPECT_TRUE(result);
            if (result) {
                EXPECT_EQ(result.value().cp, decoded_by_index(i));
                EXPECT_EQ(result.value().bytes, taul::units_required(I, decoded_by_index(i)) * taul::encoding_unit(I));
            }
        }
        for (const auto& I : utf16_encodings) {
            TAUL_LOG(lgr, "testing taul::decode w/ encoding {}", I);
            {
                const auto result = taul::decode<char>(I, encoded_by_index_utf16(i, taul::explicit_endian(I)));
                EXPECT_TRUE(result);
                if (result) {
                    EXPECT_EQ(result.value().cp, decoded_by_index(i));
                    EXPECT_EQ(result.value().bytes, taul::units_required(I, decoded_by_index(i)) * taul::encoding_unit(I));
                }
            }
            {
                const auto result = taul::decode<char16_t>(I, encoded_by_index_utf16_alt(i, taul::explicit_endian(I)));
                EXPECT_TRUE(result);
                if (result) {
                    EXPECT_EQ(result.value().cp, decoded_by_index(i));
                    EXPECT_EQ(result.value().bytes, taul::units_required(I, decoded_by_index(i)) * taul::encoding_unit(I));
                }
            }
        }
        for (const auto& I : utf32_encodings) {
            TAUL_LOG(lgr, "testing taul::decode w/ encoding {}", I);
            {
                const auto result = taul::decode<char>(I, encoded_by_index_utf32(i, taul::explicit_endian(I)));
                EXPECT_TRUE(result);
                if (result) {
                    EXPECT_EQ(result.value().cp, decoded_by_index(i));
                    EXPECT_EQ(result.value().bytes, taul::units_required(I, decoded_by_index(i)) * taul::encoding_unit(I));
                }
            }
            {
                const auto result = taul::decode<char32_t>(I, encoded_by_index_utf32_alt(i, taul::explicit_endian(I)));
                EXPECT_TRUE(result);
                if (result) {
                    EXPECT_EQ(result.value().cp, decoded_by_index(i));
                    EXPECT_EQ(result.value().bytes, taul::units_required(I, decoded_by_index(i)) * taul::encoding_unit(I));
                }
            }
        }
    }
}

TEST(EncodingTests, Decode_FailDueToInvalidInput) {
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf8, encoded_utf8_illegal_1(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf8, encoded_utf8_illegal_2(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf8, encoded_utf8_illegal_3(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf8, encoded_utf8_illegal_4(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf8, encoded_utf8_illegal_5(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf8, encoded_utf8_illegal_6(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf8, encoded_utf8_illegal_7(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf8, encoded_utf8_illegal_8(std::endian::native)));

    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf8_bom, encoded_utf8_illegal_1(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf8_bom, encoded_utf8_illegal_2(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf8_bom, encoded_utf8_illegal_3(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf8_bom, encoded_utf8_illegal_4(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf8_bom, encoded_utf8_illegal_5(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf8_bom, encoded_utf8_illegal_6(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf8_bom, encoded_utf8_illegal_7(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf8_bom, encoded_utf8_illegal_8(std::endian::native)));

    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16, encoded_utf16_illegal_1(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16, encoded_utf16_illegal_2(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16, encoded_utf16_illegal_3(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16, encoded_utf16_illegal_4(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16, encoded_utf16_illegal_5(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16, encoded_utf16_illegal_6(std::endian::native)));

    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_be, encoded_utf16_illegal_1(std::endian::big)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_be, encoded_utf16_illegal_2(std::endian::big)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_be, encoded_utf16_illegal_3(std::endian::big)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_be, encoded_utf16_illegal_4(std::endian::big)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_be, encoded_utf16_illegal_5(std::endian::big)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_be, encoded_utf16_illegal_6(std::endian::big)));

    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_le, encoded_utf16_illegal_1(std::endian::little)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_le, encoded_utf16_illegal_2(std::endian::little)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_le, encoded_utf16_illegal_3(std::endian::little)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_le, encoded_utf16_illegal_4(std::endian::little)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_le, encoded_utf16_illegal_5(std::endian::little)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_le, encoded_utf16_illegal_6(std::endian::little)));

    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_bom, encoded_utf16_illegal_1(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_bom, encoded_utf16_illegal_2(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_bom, encoded_utf16_illegal_3(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_bom, encoded_utf16_illegal_4(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_bom, encoded_utf16_illegal_5(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf16_bom, encoded_utf16_illegal_6(std::endian::native)));

    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf16, encoded_utf16_illegal_1_alt(std::endian::native)));
    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf16, encoded_utf16_illegal_2_alt(std::endian::native)));
    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf16, encoded_utf16_illegal_3_alt(std::endian::native)));
    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf16, encoded_utf16_illegal_5_alt(std::endian::native)));

    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf16_be, encoded_utf16_illegal_1_alt(std::endian::big)));
    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf16_be, encoded_utf16_illegal_2_alt(std::endian::big)));
    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf16_be, encoded_utf16_illegal_3_alt(std::endian::big)));
    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf16_be, encoded_utf16_illegal_5_alt(std::endian::big)));

    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf16_le, encoded_utf16_illegal_1_alt(std::endian::little)));
    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf16_le, encoded_utf16_illegal_2_alt(std::endian::little)));
    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf16_le, encoded_utf16_illegal_3_alt(std::endian::little)));
    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf16_le, encoded_utf16_illegal_5_alt(std::endian::little)));

    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf16_bom, encoded_utf16_illegal_1_alt(std::endian::native)));
    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf16_bom, encoded_utf16_illegal_2_alt(std::endian::native)));
    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf16_bom, encoded_utf16_illegal_3_alt(std::endian::native)));
    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf16_bom, encoded_utf16_illegal_5_alt(std::endian::native)));

    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32, encoded_utf32_illegal_1(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32, encoded_utf32_illegal_2(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32, encoded_utf32_illegal_3(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32, encoded_utf32_illegal_4(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32, encoded_utf32_illegal_5(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32, encoded_utf32_illegal_6(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32, encoded_utf32_illegal_7(std::endian::native)));

    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_be, encoded_utf32_illegal_1(std::endian::big)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_be, encoded_utf32_illegal_2(std::endian::big)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_be, encoded_utf32_illegal_3(std::endian::big)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_be, encoded_utf32_illegal_4(std::endian::big)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_be, encoded_utf32_illegal_5(std::endian::big)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_be, encoded_utf32_illegal_6(std::endian::big)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_be, encoded_utf32_illegal_7(std::endian::big)));

    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_le, encoded_utf32_illegal_1(std::endian::little)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_le, encoded_utf32_illegal_2(std::endian::little)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_le, encoded_utf32_illegal_3(std::endian::little)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_le, encoded_utf32_illegal_4(std::endian::little)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_le, encoded_utf32_illegal_5(std::endian::little)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_le, encoded_utf32_illegal_6(std::endian::little)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_le, encoded_utf32_illegal_7(std::endian::little)));

    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_bom, encoded_utf32_illegal_1(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_bom, encoded_utf32_illegal_2(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_bom, encoded_utf32_illegal_3(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_bom, encoded_utf32_illegal_4(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_bom, encoded_utf32_illegal_5(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_bom, encoded_utf32_illegal_6(std::endian::native)));
    EXPECT_FALSE(taul::decode<char>(taul::encoding::utf32_bom, encoded_utf32_illegal_7(std::endian::native)));

    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf32, encoded_utf32_illegal_1_alt(std::endian::native)));
    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf32, encoded_utf32_illegal_2_alt(std::endian::native)));
    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf32, encoded_utf32_illegal_3_alt(std::endian::native)));
    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf32, encoded_utf32_illegal_4_alt(std::endian::native)));

    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf32_be, encoded_utf32_illegal_1_alt(std::endian::big)));
    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf32_be, encoded_utf32_illegal_2_alt(std::endian::big)));
    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf32_be, encoded_utf32_illegal_3_alt(std::endian::big)));
    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf32_be, encoded_utf32_illegal_4_alt(std::endian::big)));

    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf32_le, encoded_utf32_illegal_1_alt(std::endian::little)));
    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf32_le, encoded_utf32_illegal_2_alt(std::endian::little)));
    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf32_le, encoded_utf32_illegal_3_alt(std::endian::little)));
    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf32_le, encoded_utf32_illegal_4_alt(std::endian::little)));

    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf32_bom, encoded_utf32_illegal_1_alt(std::endian::native)));
    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf32_bom, encoded_utf32_illegal_2_alt(std::endian::native)));
    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf32_bom, encoded_utf32_illegal_3_alt(std::endian::native)));
    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf32_bom, encoded_utf32_illegal_4_alt(std::endian::native)));
}

TEST(EncodingTests, Decode_FailDueToNotAllowedCharType) {
    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf8, u"a"));
    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf32, u"a"));
    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf32_be, u"a"));
    EXPECT_FALSE(taul::decode<char16_t>(taul::encoding::utf32_le, u"a"));

    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf8, U"a"));
    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf16, U"a"));
    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf16_be, U"a"));
    EXPECT_FALSE(taul::decode<char32_t>(taul::encoding::utf16_le, U"a"));
}

TEST(EncodingTests, ConvertEncoding_WithNoErrCodepoints) {
    auto lgr = taul::make_stderr_logger();
    const std::vector<taul::encoding> encodings = {
        taul::encoding::utf8,
        taul::encoding::utf16,
        taul::encoding::utf32,
        taul::encoding::utf16_be,
        taul::encoding::utf16_le,
        taul::encoding::utf32_be,
        taul::encoding::utf32_le,
        taul::encoding::utf8_bom,
        taul::encoding::utf16_bom,
        taul::encoding::utf32_bom,
    };
    for (const auto& I : encodings) {
        TAUL_LOG(lgr, "testing taul::convert_encoding w/ input encoding {}", I);
        for (const auto& J : encodings) {
            TAUL_LOG(lgr, "testing taul::convert_encoding w/ output encoding {}", J);
            {
                auto expected = convert_encoding_string(J, taul::explicit_endian(J), taul::explicit_bom(J));
                if (!taul::explicit_bom(I)) {
                    TAUL_LOG(lgr, "testing w/out BOM in input");
                    {
                        auto input = convert_encoding_string(I, taul::explicit_endian(I), false);
                        auto actual = taul::convert_encoding<char>(I, J, input);
                        EXPECT_TRUE(actual);
                        if (actual) EXPECT_EQ(*actual, expected);
                    }
                }
                TAUL_LOG(lgr, "testing w/ non-reversed BOM in input");
                {
                    auto input = convert_encoding_string(I, std::endian::native, true);
                    auto actual = taul::convert_encoding<char>(I, J, input);
                    EXPECT_TRUE(actual);
                    if (actual) EXPECT_EQ(*actual, expected);
                }
                TAUL_LOG(lgr, "testing w/ reversed BOM in input");
                {
                    auto input = convert_encoding_string(I, taul::opposite_endian(std::endian::native), true);
                    auto actual = taul::convert_encoding<char>(I, J, input);
                    EXPECT_TRUE(actual);
                    if (actual) EXPECT_EQ(*actual, expected);
                }
            }
            if (taul::base_encoding(I) == taul::encoding::utf16 && taul::base_encoding(J) == taul::encoding::utf16) {
                auto expected = convert_encoding_string_utf16_alt(taul::explicit_endian(J), taul::explicit_bom(J));
                if (!taul::explicit_bom(I)) {
                    TAUL_LOG(lgr, "testing w/out BOM in input");
                    {
                        auto input = convert_encoding_string_utf16_alt(taul::explicit_endian(I), false);
                        auto actual = taul::convert_encoding<char16_t>(I, J, input);
                        EXPECT_TRUE(actual);
                        if (actual) EXPECT_EQ(*actual, expected);
                    }
                }
                TAUL_LOG(lgr, "testing w/ non-reversed BOM in input");
                {
                    auto input = convert_encoding_string_utf16_alt(std::endian::native, true);
                    auto actual = taul::convert_encoding<char16_t>(I, J, input);
                    EXPECT_TRUE(actual);
                    if (actual) EXPECT_EQ(*actual, expected);
                }
                TAUL_LOG(lgr, "testing w/ reversed BOM in input");
                {
                    auto input = convert_encoding_string_utf16_alt(taul::opposite_endian(std::endian::native), true);
                    auto actual = taul::convert_encoding<char16_t>(I, J, input);
                    EXPECT_TRUE(actual);
                    if (actual) EXPECT_EQ(*actual, expected);
                }
            }
            if (taul::base_encoding(I) == taul::encoding::utf32 && taul::base_encoding(J) == taul::encoding::utf32) {
                auto expected = convert_encoding_string_utf32_alt(taul::explicit_endian(J), taul::explicit_bom(J));
                if (!taul::explicit_bom(I)) {
                    TAUL_LOG(lgr, "testing w/out BOM in input");
                    {
                        auto input = convert_encoding_string_utf32_alt(taul::explicit_endian(I), false);
                        auto actual = taul::convert_encoding<char32_t>(I, J, input);
                        EXPECT_TRUE(actual);
                        if (actual) EXPECT_EQ(*actual, expected);
                    }
                }
                TAUL_LOG(lgr, "testing w/ non-reversed BOM in input");
                {
                    auto input = convert_encoding_string_utf32_alt(std::endian::native, true);
                    auto actual = taul::convert_encoding<char32_t>(I, J, input);
                    EXPECT_TRUE(actual);
                    if (actual) EXPECT_EQ(*actual, expected);
                }
                TAUL_LOG(lgr, "testing w/ reversed BOM in input");
                {
                    auto input = convert_encoding_string_utf32_alt(taul::opposite_endian(std::endian::native), true);
                    auto actual = taul::convert_encoding<char32_t>(I, J, input);
                    EXPECT_TRUE(actual);
                    if (actual) EXPECT_EQ(*actual, expected);
                }
            }
        }
    }
}

TEST(EncodingTests, ConvertEncoding_WithErrCodepoints) {
    // using '^' to ensure impl works w/ arbitrary err argument
    {
        // UTF-8 -> UTF-32 (as char32_t)

        // the input has three invalid UTF-8 continuation bytes

        std::string input{};
        input += "abc";
        input += char(0b1011'0010);
        input += char(0b1011'0010);
        input += "12";
        input += char(0b1011'0010);
        input += "3";

        std::u32string expected{};
        expected += U"abc^^12^3";

        auto actual = taul::convert_encoding<char32_t>(taul::encoding::utf8, taul::encoding::utf32, input, U'^');
        EXPECT_TRUE(actual);
        if (actual) EXPECT_EQ(*actual, expected);
    }
    {
        // UTF-16 (as char16_t) -> UTF-32 (as char)

        // the input has stray leading and trailing surrogate units

        std::u16string input{};
        input += u"abc";
        input += char16_t(0b1101'1010'0100'0110); // stray leading surrogate
        input += u"12";
        input += char16_t(0b1101'1110'0100'0110); // stray trailing surrogate
        input += u"3";

        std::u32string expected_temp = U"abc^^12^^3";
        std::string expected(expected_temp.length() * sizeof(char32_t), U' ');
        std::memcpy((void*)expected.data(), (const void*)expected_temp.data(), expected.size());


        auto actual = taul::convert_encoding<char>(taul::encoding::utf16, taul::encoding::utf32, input, U'^');
        EXPECT_TRUE(actual);
        if (actual) EXPECT_EQ(*actual, expected);
    }
}

TEST(EncodingTests, ConvertEncoding_FailDueToNotAllowedCharTypeForInputEncoding) {
    EXPECT_FALSE(taul::convert_encoding<char32_t>(taul::encoding::utf8, taul::encoding::utf32, u"abc123"));
    EXPECT_FALSE(taul::convert_encoding<char32_t>(taul::encoding::utf8, taul::encoding::utf32, U"abc123"));
}

TEST(EncodingTests, ConvertEncoding_FailDueToNotAllowedCharTypeForOutputEncoding) {
    EXPECT_FALSE(taul::convert_encoding<char32_t>(taul::encoding::utf8, taul::encoding::utf8, "abc123"));
    EXPECT_FALSE(taul::convert_encoding<char32_t>(taul::encoding::utf8, taul::encoding::utf16, "abc123"));
    EXPECT_FALSE(taul::convert_encoding<char16_t>(taul::encoding::utf8, taul::encoding::utf8, "abc123"));
    EXPECT_FALSE(taul::convert_encoding<char16_t>(taul::encoding::utf8, taul::encoding::utf32, "abc123"));
}

TEST(EncodingTests, ConvertEncoding_FailDueToErrArgBeingInvalidCodepoint) {
    EXPECT_FALSE(taul::convert_encoding<char>(taul::encoding::utf8, taul::encoding::utf8, "abc123", taul::unicode_t(0x110000)));
    EXPECT_FALSE(taul::convert_encoding<char>(taul::encoding::utf8, taul::encoding::utf8, "abc123", taul::first_surrogate));
    EXPECT_FALSE(taul::convert_encoding<char>(taul::encoding::utf8, taul::encoding::utf8, "abc123", taul::last_surrogate));
    EXPECT_FALSE(taul::convert_encoding<char>(taul::encoding::utf8, taul::encoding::utf8, "abc123", taul::unicode_t(-1)));
}

TEST(EncodingTests, ConvertEncoding_FailDueToInputEncodingWithExplicitBOMButInputHasNoBOM) {
    EXPECT_FALSE(taul::convert_encoding<char>(taul::encoding::utf8_bom, taul::encoding::utf8, "abc123"));
    EXPECT_FALSE(taul::convert_encoding<char>(taul::encoding::utf16_bom, taul::encoding::utf8, u"abc123"));
    EXPECT_FALSE(taul::convert_encoding<char>(taul::encoding::utf32_bom, taul::encoding::utf8, U"abc123"));
}

TEST(EncodingTests, Encoder_NoUsage) {
    taul::encoder<char> a(taul::encoding::utf8);
    taul::encoder<char> b(taul::encoding::utf16_bom);
    taul::encoder<char32_t> c(taul::encoding::utf32_be);

    EXPECT_TRUE(a.result().empty());
    EXPECT_TRUE(b.result().empty());
    EXPECT_TRUE(c.result().empty());
}

TEST(EncodingTests, Encoder_Usage) {
    taul::encoder<char> a(taul::encoding::utf8);
    taul::encoder<char> b(taul::encoding::utf16_bom);
    taul::encoder<char32_t> c(taul::encoding::utf32_be);

    EXPECT_TRUE(a.next(U'a'));
    EXPECT_TRUE(a.next(U'b'));
    EXPECT_TRUE(a.next(U'c'));
    EXPECT_TRUE(a.next(U'1'));
    EXPECT_TRUE(a.next(U'2'));
    EXPECT_TRUE(a.next(U'3'));
    EXPECT_TRUE(a.next(U'Δ'));
    EXPECT_TRUE(a.next(U'魂'));
    EXPECT_TRUE(a.next(U'💩'));

    EXPECT_TRUE(b.next(U'a'));
    EXPECT_TRUE(b.next(U'b'));
    EXPECT_TRUE(b.next(U'c'));
    EXPECT_TRUE(b.next(U'1'));
    EXPECT_TRUE(b.next(U'2'));
    EXPECT_TRUE(b.next(U'3'));
    EXPECT_TRUE(b.next(U'Δ'));
    EXPECT_TRUE(b.next(U'魂'));
    EXPECT_TRUE(b.next(U'💩'));

    EXPECT_TRUE(c.next(U'a'));
    EXPECT_TRUE(c.next(U'b'));
    EXPECT_TRUE(c.next(U'c'));
    EXPECT_TRUE(c.next(U'1'));
    EXPECT_TRUE(c.next(U'2'));
    EXPECT_TRUE(c.next(U'3'));
    EXPECT_TRUE(c.next(U'Δ'));
    EXPECT_TRUE(c.next(U'魂'));
    EXPECT_TRUE(c.next(U'💩'));

    const auto actual_a = a.result();
    const auto actual_b = b.result();
    const auto actual_c = c.result();

    std::string expected_a =
        encoded_a_utf8(std::endian::native) +
        encoded_b_utf8(std::endian::native) +
        encoded_c_utf8(std::endian::native) +
        encoded_1_utf8(std::endian::native) +
        encoded_2_utf8(std::endian::native) +
        encoded_3_utf8(std::endian::native) +
        encoded_greek_utf8(std::endian::native) +
        encoded_kanji_utf8(std::endian::native) +
        encoded_emoji_utf8(std::endian::native);
    std::string expected_b =
        encoded_a_utf16(std::endian::native) +
        encoded_b_utf16(std::endian::native) +
        encoded_c_utf16(std::endian::native) +
        encoded_1_utf16(std::endian::native) +
        encoded_2_utf16(std::endian::native) +
        encoded_3_utf16(std::endian::native) +
        encoded_greek_utf16(std::endian::native) +
        encoded_kanji_utf16(std::endian::native) +
        encoded_emoji_utf16(std::endian::native);
    std::u32string expected_c =
        encoded_a_utf32_alt(std::endian::big) +
        encoded_b_utf32_alt(std::endian::big) +
        encoded_c_utf32_alt(std::endian::big) +
        encoded_1_utf32_alt(std::endian::big) +
        encoded_2_utf32_alt(std::endian::big) +
        encoded_3_utf32_alt(std::endian::big) +
        encoded_greek_utf32_alt(std::endian::big) +
        encoded_kanji_utf32_alt(std::endian::big) +
        encoded_emoji_utf32_alt(std::endian::big);

    EXPECT_EQ(actual_a, expected_a);
    EXPECT_EQ(actual_b, expected_b);
    EXPECT_EQ(actual_c, expected_c);
}

TEST(EncodingTests, Encoder_NextWithInvalidCodepoint) {
    taul::encoder<char> a(taul::encoding::utf8);
    taul::encoder<char> b(taul::encoding::utf16_bom);
    taul::encoder<char32_t> c(taul::encoding::utf32_be);

    EXPECT_TRUE(a.next(U'a'));
    EXPECT_FALSE(a.next(0x110000));
    EXPECT_TRUE(a.next(U'b'));
    EXPECT_FALSE(a.next(taul::first_surrogate));
    EXPECT_TRUE(a.next(U'c'));
    EXPECT_FALSE(a.next(taul::last_surrogate));

    EXPECT_TRUE(b.next(U'a'));
    EXPECT_FALSE(b.next(0x110000));
    EXPECT_TRUE(b.next(U'b'));
    EXPECT_FALSE(b.next(taul::first_surrogate));
    EXPECT_TRUE(b.next(U'c'));
    EXPECT_FALSE(b.next(taul::last_surrogate));

    EXPECT_TRUE(c.next(U'a'));
    EXPECT_FALSE(c.next(0x110000));
    EXPECT_TRUE(c.next(U'b'));
    EXPECT_FALSE(c.next(taul::first_surrogate));
    EXPECT_TRUE(c.next(U'c'));
    EXPECT_FALSE(c.next(taul::last_surrogate));

    const auto actual_a = a.result();
    const auto actual_b = b.result();
    const auto actual_c = c.result();

    std::string expected_a =
        encoded_a_utf8(std::endian::native) +
        encoded_b_utf8(std::endian::native) +
        encoded_c_utf8(std::endian::native);
    std::string expected_b =
        encoded_a_utf16(std::endian::native) +
        encoded_b_utf16(std::endian::native) +
        encoded_c_utf16(std::endian::native);
    std::u32string expected_c =
        encoded_a_utf32_alt(std::endian::big) +
        encoded_b_utf32_alt(std::endian::big) +
        encoded_c_utf32_alt(std::endian::big);

    EXPECT_EQ(actual_a, expected_a);
    EXPECT_EQ(actual_b, expected_b);
    EXPECT_EQ(actual_c, expected_c);
}

TEST(EncodingTests, Decoder_EmptyInput) {
    taul::decoder<char> a(taul::encoding::utf8, "");
    taul::decoder<char> b(taul::encoding::utf16_bom, "");
    taul::decoder<char32_t> c(taul::encoding::utf32_be, U"");

    {
        EXPECT_EQ(a.pos(), 0);
        EXPECT_TRUE(a.done());

        auto p0 = a.peek();

        EXPECT_EQ(a.pos(), 0);
        EXPECT_TRUE(a.done());

        auto r0 = a.peek();

        EXPECT_FALSE(p0);
        EXPECT_FALSE(r0);

        EXPECT_EQ(a.pos(), 0);
        EXPECT_TRUE(a.done());
    }
    {
        EXPECT_EQ(b.pos(), 0);
        EXPECT_TRUE(b.done());

        auto p0 = b.peek();

        EXPECT_EQ(b.pos(), 0);
        EXPECT_TRUE(b.done());

        auto r0 = b.peek();

        EXPECT_FALSE(p0);
        EXPECT_FALSE(r0);

        EXPECT_EQ(b.pos(), 0);
        EXPECT_TRUE(b.done());
    }
    {
        EXPECT_EQ(c.pos(), 0);
        EXPECT_TRUE(c.done());

        auto p0 = c.peek();

        EXPECT_EQ(c.pos(), 0);
        EXPECT_TRUE(c.done());

        auto r0 = c.peek();

        EXPECT_FALSE(p0);
        EXPECT_FALSE(r0);

        EXPECT_EQ(c.pos(), 0);
        EXPECT_TRUE(c.done());
    }
}

TEST(EncodingTests, Decoder_NonEmptyInput) {
    std::string input_a =
        encoded_a_utf8(std::endian::native) +
        encoded_1_utf8(std::endian::native) +
        encoded_greek_utf8(std::endian::native) +
        encoded_kanji_utf8(std::endian::native) +
        encoded_emoji_utf8(std::endian::native);
    std::string input_b =
        encoded_a_utf16(std::endian::native) +
        encoded_1_utf16(std::endian::native) +
        encoded_greek_utf16(std::endian::native) +
        encoded_kanji_utf16(std::endian::native) +
        encoded_emoji_utf16(std::endian::native);
    std::u32string input_c =
        encoded_a_utf32_alt(std::endian::big) +
        encoded_1_utf32_alt(std::endian::big) +
        encoded_greek_utf32_alt(std::endian::big) +
        encoded_kanji_utf32_alt(std::endian::big) +
        encoded_emoji_utf32_alt(std::endian::big);

    taul::decoder<char> a(taul::encoding::utf8, input_a);
    taul::decoder<char> b(taul::encoding::utf16_bom, input_b);
    taul::decoder<char32_t> c(taul::encoding::utf32_be, input_c);
    
    {
        EXPECT_EQ(a.pos(), 0);
        EXPECT_FALSE(a.done());

        auto p0 = a.peek();

        EXPECT_EQ(a.pos(), 0);
        EXPECT_FALSE(a.done());

        auto r0 = a.next();

        EXPECT_TRUE(r0);
        if (r0) EXPECT_EQ(r0->cp, U'a');
        if (r0) EXPECT_EQ(r0->bytes, 1);
        EXPECT_TRUE(p0);
        if (p0) EXPECT_EQ(p0->cp, U'a');
        if (p0) EXPECT_EQ(p0->bytes, 1);

        EXPECT_EQ(a.pos(), 1);
        EXPECT_FALSE(a.done());

        auto p1 = a.peek();

        EXPECT_EQ(a.pos(), 1);
        EXPECT_FALSE(a.done());

        auto r1 = a.next();

        EXPECT_TRUE(r1);
        if (r1) EXPECT_EQ(r1->cp, U'1');
        if (r1) EXPECT_EQ(r1->bytes, 1);
        EXPECT_TRUE(p1);
        if (p1) EXPECT_EQ(p1->cp, U'1');
        if (p1) EXPECT_EQ(p1->bytes, 1);

        EXPECT_EQ(a.pos(), 2);
        EXPECT_FALSE(a.done());

        auto p2 = a.peek();

        EXPECT_EQ(a.pos(), 2);
        EXPECT_FALSE(a.done());

        auto r2 = a.next();

        EXPECT_TRUE(r2);
        if (r2) EXPECT_EQ(r2->cp, U'Δ');
        if (r2) EXPECT_EQ(r2->bytes, 2);
        EXPECT_TRUE(p2);
        if (p2) EXPECT_EQ(p2->cp, U'Δ');
        if (p2) EXPECT_EQ(p2->bytes, 2);

        EXPECT_EQ(a.pos(), 4);
        EXPECT_FALSE(a.done());

        auto p3 = a.peek();

        EXPECT_EQ(a.pos(), 4);
        EXPECT_FALSE(a.done());

        auto r3 = a.next();

        EXPECT_TRUE(r3);
        if (r3) EXPECT_EQ(r3->cp, U'魂');
        if (r3) EXPECT_EQ(r3->bytes, 3);
        EXPECT_TRUE(p3);
        if (p3) EXPECT_EQ(p3->cp, U'魂');
        if (p3) EXPECT_EQ(p3->bytes, 3);

        EXPECT_EQ(a.pos(), 7);
        EXPECT_FALSE(a.done());

        auto p4 = a.peek();

        EXPECT_EQ(a.pos(), 7);
        EXPECT_FALSE(a.done());

        auto r4 = a.next();

        EXPECT_TRUE(r4);
        if (r4) EXPECT_EQ(r4->cp, U'💩');
        if (r4) EXPECT_EQ(r4->bytes, 4);
        EXPECT_TRUE(p4);
        if (p4) EXPECT_EQ(p4->cp, U'💩');
        if (p4) EXPECT_EQ(p4->bytes, 4);

        EXPECT_EQ(a.pos(), 11);
        EXPECT_TRUE(a.done());

        auto p5 = a.peek();

        EXPECT_EQ(a.pos(), 11);
        EXPECT_TRUE(a.done());

        auto r5 = a.next();

        EXPECT_FALSE(r5);
        EXPECT_FALSE(p5);

        EXPECT_EQ(a.pos(), 11);
        EXPECT_TRUE(a.done());
    }
    {
        EXPECT_EQ(b.pos(), 0);
        EXPECT_FALSE(b.done());

        auto p0 = b.peek();

        EXPECT_EQ(b.pos(), 0);
        EXPECT_FALSE(b.done());

        auto r0 = b.next();

        EXPECT_TRUE(r0);
        if (r0) EXPECT_EQ(r0->cp, U'a');
        if (r0) EXPECT_EQ(r0->bytes, 2);
        EXPECT_TRUE(p0);
        if (p0) EXPECT_EQ(p0->cp, U'a');
        if (p0) EXPECT_EQ(p0->bytes, 2);

        EXPECT_EQ(b.pos(), 2);
        EXPECT_FALSE(b.done());

        auto p1 = b.peek();

        EXPECT_EQ(b.pos(), 2);
        EXPECT_FALSE(b.done());

        auto r1 = b.next();

        EXPECT_TRUE(r1);
        if (r1) EXPECT_EQ(r1->cp, U'1');
        if (r1) EXPECT_EQ(r1->bytes, 2);
        EXPECT_TRUE(p1);
        if (p1) EXPECT_EQ(p1->cp, U'1');
        if (p1) EXPECT_EQ(p1->bytes, 2);

        EXPECT_EQ(b.pos(), 4);
        EXPECT_FALSE(b.done());

        auto p2 = b.peek();

        EXPECT_EQ(b.pos(), 4);
        EXPECT_FALSE(b.done());

        auto r2 = b.next();

        EXPECT_TRUE(r2);
        if (r2) EXPECT_EQ(r2->cp, U'Δ');
        if (r2) EXPECT_EQ(r2->bytes, 2);
        EXPECT_TRUE(p2);
        if (p2) EXPECT_EQ(p2->cp, U'Δ');
        if (p2) EXPECT_EQ(p2->bytes, 2);

        EXPECT_EQ(b.pos(), 6);
        EXPECT_FALSE(b.done());

        auto p3 = b.peek();

        EXPECT_EQ(b.pos(), 6);
        EXPECT_FALSE(b.done());

        auto r3 = b.next();

        EXPECT_TRUE(r3);
        if (r3) EXPECT_EQ(r3->cp, U'魂');
        if (r3) EXPECT_EQ(r3->bytes, 2);
        EXPECT_TRUE(p3);
        if (p3) EXPECT_EQ(p3->cp, U'魂');
        if (p3) EXPECT_EQ(p3->bytes, 2);

        EXPECT_EQ(b.pos(), 8);
        EXPECT_FALSE(b.done());

        auto p4 = b.peek();

        EXPECT_EQ(b.pos(), 8);
        EXPECT_FALSE(b.done());

        auto r4 = b.next();

        EXPECT_TRUE(r4);
        if (r4) EXPECT_EQ(r4->cp, U'💩');
        if (r4) EXPECT_EQ(r4->bytes, 4);
        EXPECT_TRUE(p4);
        if (p4) EXPECT_EQ(p4->cp, U'💩');
        if (p4) EXPECT_EQ(p4->bytes, 4);

        EXPECT_EQ(b.pos(), 12);
        EXPECT_TRUE(b.done());

        auto p5 = b.peek();

        EXPECT_EQ(b.pos(), 12);
        EXPECT_TRUE(b.done());

        auto r5 = b.next();

        EXPECT_FALSE(r5);
        EXPECT_FALSE(p5);

        EXPECT_EQ(b.pos(), 12);
        EXPECT_TRUE(b.done());
    }
    {
        EXPECT_EQ(c.pos(), 0);
        EXPECT_FALSE(c.done());

        auto p0 = c.peek();
        
        EXPECT_EQ(c.pos(), 0);
        EXPECT_FALSE(c.done());

        auto r0 = c.next();

        EXPECT_TRUE(r0);
        if (r0) EXPECT_EQ(r0->cp, U'a');
        if (r0) EXPECT_EQ(r0->bytes, 4);
        EXPECT_TRUE(p0);
        if (p0) EXPECT_EQ(p0->cp, U'a');
        if (p0) EXPECT_EQ(p0->bytes, 4);

        EXPECT_EQ(c.pos(), 1);
        EXPECT_FALSE(c.done());

        auto p1 = c.peek();

        EXPECT_EQ(c.pos(), 1);
        EXPECT_FALSE(c.done());

        auto r1 = c.next();

        EXPECT_TRUE(r1);
        if (r1) EXPECT_EQ(r1->cp, U'1');
        if (r1) EXPECT_EQ(r1->bytes, 4);
        EXPECT_TRUE(p1);
        if (p1) EXPECT_EQ(p1->cp, U'1');
        if (p1) EXPECT_EQ(p1->bytes, 4);

        EXPECT_EQ(c.pos(), 2);
        EXPECT_FALSE(c.done());

        auto p2 = c.peek();

        EXPECT_EQ(c.pos(), 2);
        EXPECT_FALSE(c.done());

        auto r2 = c.next();

        EXPECT_TRUE(r2);
        if (r2) EXPECT_EQ(r2->cp, U'Δ');
        if (r2) EXPECT_EQ(r2->bytes, 4);
        EXPECT_TRUE(p2);
        if (p2) EXPECT_EQ(p2->cp, U'Δ');
        if (p2) EXPECT_EQ(p2->bytes, 4);

        EXPECT_EQ(c.pos(), 3);
        EXPECT_FALSE(c.done());

        auto p3 = c.peek();

        EXPECT_EQ(c.pos(), 3);
        EXPECT_FALSE(c.done());

        auto r3 = c.next();

        EXPECT_TRUE(r3);
        if (r3) EXPECT_EQ(r3->cp, U'魂');
        if (r3) EXPECT_EQ(r3->bytes, 4);
        EXPECT_TRUE(p3);
        if (p3) EXPECT_EQ(p3->cp, U'魂');
        if (p3) EXPECT_EQ(p3->bytes, 4);

        EXPECT_EQ(c.pos(), 4);
        EXPECT_FALSE(c.done());

        auto p4 = c.peek();

        EXPECT_EQ(c.pos(), 4);
        EXPECT_FALSE(c.done());

        auto r4 = c.next();

        EXPECT_TRUE(r4);
        if (r4) EXPECT_EQ(r4->cp, U'💩');
        if (r4) EXPECT_EQ(r4->bytes, 4);
        EXPECT_TRUE(p4);
        if (p4) EXPECT_EQ(p4->cp, U'💩');
        if (p4) EXPECT_EQ(p4->bytes, 4);

        EXPECT_EQ(c.pos(), 5);
        EXPECT_TRUE(c.done());

        auto p5 = c.peek();

        EXPECT_EQ(c.pos(), 5);
        EXPECT_TRUE(c.done());

        auto r5 = c.next();

        EXPECT_FALSE(r5);
        EXPECT_FALSE(p5);

        EXPECT_EQ(c.pos(), 5);
        EXPECT_TRUE(c.done());
    }
}

TEST(EncodingTests, Decoder_NextWithInputErrors) {
    // we'll just extrapolate from one UTF-8 example...

    std::string input_a =
        encoded_a_utf8(std::endian::native) +
        char(0b1010'0100ui8) + // illegal continuation byte
        char(0b1010'0100ui8) + // illegal continuation byte
        encoded_1_utf8(std::endian::native);

    taul::decoder<char> a(taul::encoding::utf8, input_a);

    {
        EXPECT_EQ(a.pos(), 0);
        EXPECT_FALSE(a.done());

        auto p0 = a.peek();
        
        EXPECT_EQ(a.pos(), 0);
        EXPECT_FALSE(a.done());

        auto r0 = a.next();

        EXPECT_TRUE(r0);
        if (r0) EXPECT_EQ(r0->cp, U'a');
        if (r0) EXPECT_EQ(r0->bytes, 1);
        EXPECT_TRUE(p0);
        if (p0) EXPECT_EQ(p0->cp, U'a');
        if (p0) EXPECT_EQ(p0->bytes, 1);

        EXPECT_EQ(a.pos(), 1);
        EXPECT_FALSE(a.done());

        auto p1 = a.peek();

        EXPECT_EQ(a.pos(), 1);
        EXPECT_FALSE(a.done());

        auto r1 = a.next();

        EXPECT_FALSE(r1);
        EXPECT_FALSE(p1);

        EXPECT_EQ(a.pos(), 1);
        EXPECT_FALSE(a.done());

        a.skip();

        EXPECT_EQ(a.pos(), 2);
        EXPECT_FALSE(a.done());

        auto p2 = a.peek();

        EXPECT_EQ(a.pos(), 2);
        EXPECT_FALSE(a.done());

        auto r2 = a.next();

        EXPECT_FALSE(r2);
        EXPECT_FALSE(p2);

        EXPECT_EQ(a.pos(), 2);
        EXPECT_FALSE(a.done());

        a.skip();

        EXPECT_EQ(a.pos(), 3);
        EXPECT_FALSE(a.done());

        auto p3 = a.peek();

        EXPECT_EQ(a.pos(), 3);
        EXPECT_FALSE(a.done());

        auto r3 = a.next();

        EXPECT_TRUE(r3);
        if (r3) EXPECT_EQ(r3->cp, U'1');
        if (r3) EXPECT_EQ(r3->bytes, 1);
        EXPECT_TRUE(p3);
        if (p3) EXPECT_EQ(p3->cp, U'1');
        if (p3) EXPECT_EQ(p3->bytes, 1);

        EXPECT_EQ(a.pos(), 4);
        EXPECT_TRUE(a.done());
    }
}

TEST(EncodingTests, Decoder_SkipUsage) {
    std::string input_a =
        encoded_a_utf8(std::endian::native) +
        encoded_b_utf8(std::endian::native) +
        encoded_c_utf8(std::endian::native) +
        encoded_1_utf8(std::endian::native) +
        encoded_2_utf8(std::endian::native) +
        encoded_3_utf8(std::endian::native);

    taul::decoder<char> a(taul::encoding::utf8, input_a);

    {
        EXPECT_EQ(a.pos(), 0);
        EXPECT_FALSE(a.done());

        auto r0 = a.next();

        EXPECT_TRUE(r0);
        if (r0) EXPECT_EQ(r0->cp, U'a');
        if (r0) EXPECT_EQ(r0->bytes, 1);

        EXPECT_EQ(a.pos(), 1);
        EXPECT_FALSE(a.done());

        a.skip(2);

        EXPECT_EQ(a.pos(), 3);
        EXPECT_FALSE(a.done());

        auto r1 = a.next();

        EXPECT_TRUE(r1);
        if (r1) EXPECT_EQ(r1->cp, U'1');
        if (r1) EXPECT_EQ(r1->bytes, 1);

        EXPECT_EQ(a.pos(), 4);
        EXPECT_FALSE(a.done());

        // skip so it stops prematurely

        a.skip(7);

        EXPECT_EQ(a.pos(), 6);
        EXPECT_TRUE(a.done());
    }
}

TEST(EncodingTests, CheckBOM) {
    auto lgr = taul::make_stderr_logger();
    std::vector<taul::encoding> encodings = {
        taul::encoding::utf8,
        taul::encoding::utf16,
        taul::encoding::utf32,
        taul::encoding::utf16_be,
        taul::encoding::utf16_le,
        taul::encoding::utf32_be,
        taul::encoding::utf32_le,
        taul::encoding::utf8_bom,
        taul::encoding::utf16_bom,
        taul::encoding::utf32_bom,
    };
    for (const auto& I : encodings) {
        TAUL_LOG(lgr, "testing taul::check_bom w/ encoding {}", I);
        {
            EXPECT_EQ(taul::check_bom<char>(I, ""), taul::bom_status::no_bom);
        }
        {
            taul::encoder<char> enc(I);
            enc.next(U'a');
            enc.next(U'b');
            enc.next(U'c');
            enc.next(U'1');
            enc.next(U'2');
            enc.next(U'3');
            const auto s = enc.result();

            EXPECT_EQ(taul::check_bom<char>(I, s), taul::bom_status::no_bom);
        }
        if (taul::base_encoding(I) == taul::encoding::utf16) {
            taul::encoder<char16_t> enc(I);
            enc.next(U'a');
            enc.next(U'b');
            enc.next(U'c');
            enc.next(U'1');
            enc.next(U'2');
            enc.next(U'3');
            const auto s = enc.result();

            EXPECT_EQ(taul::check_bom<char16_t>(I, s), taul::bom_status::no_bom);
        }
        if (taul::base_encoding(I) == taul::encoding::utf32) {
            taul::encoder<char32_t> enc(I);
            enc.next(U'a');
            enc.next(U'b');
            enc.next(U'c');
            enc.next(U'1');
            enc.next(U'2');
            enc.next(U'3');
            const auto s = enc.result();

            EXPECT_EQ(taul::check_bom<char32_t>(I, s), taul::bom_status::no_bom);
        }
        {
            taul::encoder<char> enc(I);
            enc.next(taul::bom_codepoint);
            enc.next(U'a');
            enc.next(U'b');
            enc.next(U'c');
            enc.next(U'1');
            enc.next(U'2');
            enc.next(U'3');
            const auto s = enc.result();

            EXPECT_EQ(taul::check_bom<char>(I, s), taul::bom_status::bom);
        }
        if (taul::base_encoding(I) == taul::encoding::utf16) {
            taul::encoder<char16_t> enc(I);
            enc.next(taul::bom_codepoint);
            enc.next(U'a');
            enc.next(U'b');
            enc.next(U'c');
            enc.next(U'1');
            enc.next(U'2');
            enc.next(U'3');
            const auto s = enc.result();

            EXPECT_EQ(taul::check_bom<char16_t>(I, s), taul::bom_status::bom);
        }
        if (taul::base_encoding(I) == taul::encoding::utf32) {
            taul::encoder<char32_t> enc(I);
            enc.next(taul::bom_codepoint);
            enc.next(U'a');
            enc.next(U'b');
            enc.next(U'c');
            enc.next(U'1');
            enc.next(U'2');
            enc.next(U'3');
            const auto s = enc.result();

            EXPECT_EQ(taul::check_bom<char32_t>(I, s), taul::bom_status::bom);
        }
        if (taul::base_encoding(I) != taul::encoding::utf8) {
            taul::encoder<char> enc(taul::change_endian(taul::opposite_endian(taul::explicit_endian(I)), I));
            enc.next(taul::bom_codepoint);
            enc.next(U'a');
            enc.next(U'b');
            enc.next(U'c');
            enc.next(U'1');
            enc.next(U'2');
            enc.next(U'3');
            const auto s = enc.result();

            EXPECT_EQ(taul::check_bom<char>(I, s), taul::bom_status::antibom);
        }
        if (taul::base_encoding(I) == taul::encoding::utf16) {
            taul::encoder<char16_t> enc(taul::change_endian(taul::opposite_endian(taul::explicit_endian(I)), I));
            enc.next(taul::bom_codepoint);
            enc.next(U'a');
            enc.next(U'b');
            enc.next(U'c');
            enc.next(U'1');
            enc.next(U'2');
            enc.next(U'3');
            const auto s = enc.result();

            EXPECT_EQ(taul::check_bom<char16_t>(I, s), taul::bom_status::antibom);
        }
        if (taul::base_encoding(I) == taul::encoding::utf32) {
            taul::encoder<char32_t> enc(taul::change_endian(taul::opposite_endian(taul::explicit_endian(I)), I));
            enc.next(taul::bom_codepoint);
            enc.next(U'a');
            enc.next(U'b');
            enc.next(U'c');
            enc.next(U'1');
            enc.next(U'2');
            enc.next(U'3');
            const auto s = enc.result();

            EXPECT_EQ(taul::check_bom<char32_t>(I, s), taul::bom_status::antibom);
        }
    }
}

