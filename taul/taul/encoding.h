

#pragma once


#include <iostream>
#include <string>
#include <string_view>
#include <span>
#include <optional>
#include <format>

#include "asserts.h"
#include "endian.h"
#include "unicode.h"


namespace taul {


    // NOTE: take note that within this portion of the TAUL library, an 'encoded unit' 
    //       refers to a byte sequence which encodes a single codepoint value using
    //       some specific encoding


    enum class encoding : std::uint8_t;


    // encode returns a string encoding codepoint x w/ encoding out_e

    // if explicit_bom(out_e) == true, then the encoding used will be drop_bom(out_e)
    
    // encode returns std::nullopt if is_unicode(x) == false
    // encode returns std::nullopt if allowed_char_type<OutChar>(out_e) == false

    template<typename OutChar>
    inline std::optional<std::basic_string<OutChar>> encode(
        encoding out_e, 
        unicode_t x);

    // decode returns the first codepoint decodable from x w/ encoding in_e

    // if explicit_bom(in_e) == true, then the encoding used will be drop_bom(in_e)

    // decode returns std::nullopt if decoding fails
    // decode returns std::nullopt if allowed_char_type<InChar>(in_e) == false

    struct decode_result final {
        unicode_t   cp;     // the codepoint decoded
        std::size_t bytes;  // the number of bytes which were decoded


        template<typename Char>
        inline std::size_t chars() const noexcept {
            return bytes / sizeof(Char);
        }
    };

    template<typename InChar>
    inline std::optional<decode_result> decode(
        encoding in_e, 
        std::basic_string_view<InChar> x);

    // convert_encoding returns x converted from encoding in_e, to encoding
    // out_e, possibly alongside a change in the char type of the string

    // convert_encoding can handle BOM information, and will auto-detect BOMs

    // convert_encoding returns std::nullopt if allowed_char_type<InChar>(in_e) == false
    // convert_encoding returns std::nullopt if allowed_char_type<OutChar>(out_e) == false
    // convert_encoding returns std::nullopt if is_unicode(err) == false
    // convert_encoding returns std::nullopt if explicit_bom(in_e) == true,
    // but x does not provide a BOM

    // if a char in x fails to decode, codepoint err will be encoded and
    // output to signal this failure, and the failed char will otherwise
    // be skipped

    constexpr unicode_t default_err = U'?';

    template<typename OutChar, typename InChar>
    inline std::optional<std::basic_string<OutChar>> convert_encoding(
        encoding in_e, 
        encoding out_e, 
        std::basic_string_view<InChar> x,
        unicode_t err = default_err);

    // these overloads just wrap the above, and have not been unit tested

    template<typename OutChar, typename InChar>
    inline std::optional<std::basic_string<OutChar>> convert_encoding(
        encoding in_e, 
        encoding out_e, 
        const std::basic_string<InChar>& x,
        unicode_t err = default_err);
    
    // convert_encoding behaviour is undefined if x is invalid
    
    template<typename OutChar, typename InChar>
    inline std::optional<std::basic_string<OutChar>> convert_encoding(
        encoding in_e, 
        encoding out_e, 
        const InChar* x,
        unicode_t err = default_err);


    template<typename OutChar>
    class encoder final {
    public:

        using string_t = std::basic_string<OutChar>;


        // behaviour is undefined if allowed_char_type<OutChar>(out_e) == false

        inline encoder(encoding out_e);


        // next attempts to write x to the output, returning if successful

        inline bool next(unicode_t x);

        // result returns the output string of the encoder created up to now

        // the encoder should not be used after result is called

        inline string_t result() noexcept;


    private:

        encoding _out_e;
        string_t _result;
    };

    template<typename InChar>
    class decoder final {
    public:

        using string_view_t = std::basic_string_view<InChar>;


        // behaviour is undefined if allowed_char_type<InChar>(in_e) == false

        inline decoder(encoding in_e, string_view_t input);


        // pos returns the current read position of the decoder

        inline std::size_t pos() const noexcept;

        // next attempts to decode a codepoint from the current read
        // position of the decoder in the input, advancing the read
        // position if successful

        inline std::optional<decode_result> next();

        // peek attempts to decode a codepoint from the current read
        // position of the decoder in the input, but w/out advancing
        // the read position

        inline std::optional<decode_result> peek();

        // skip advances the read position by n chars, stopping
        // prematurely if the end of the input is reached

        inline void skip(std::size_t n = 1) noexcept;

        // done returns if the read position has reached the end of
        // the decoder's input

        inline bool done() const noexcept;


    private:

        encoding _in_e;
        string_view_t _input;
        std::size_t _pos;
    };


    // TODO: mixed endian not supported yet (if ever)

    static_assert(!is_mixed_endian);


    enum class encoding : std::uint8_t {

        // UTF-8/16/32

        // these are the 'base encodings' w/out any explicit
        // details about endianness or BOMs

        // as the input encoding, these use the native endianness
        // if no BOM is found, using the BOM instead if one is found

        // as the output encoding, these output w/ the native
        // endianness, and output no BOM

        utf8,
        utf16,
        utf32,

        // UTF-16/32 BE/LE

        // these are the UTF-16/32 encodings w/ explicit endianness

        // as the input encoding, these use their explicitly stated
        // endianness, but this is overrided by BOM if one is found

        // as the output encoding, these output w/ their stated
        // endianness, and output no BOM
        
        utf16_be,
        utf16_le,

        utf32_be,
        utf32_le,

        // UTF-8/16/32 BOM

        // these are the UTF-8/16/32 encodings w/ explicit BOM

        // as the input encoding, these require a BOM to be present
        // for encoding to succeed, and use its endianness information

        // as the output encoding, these output w/ native endianness,
        // and output a BOM

        utf8_bom,
        utf16_bom,
        utf32_bom,
    };

    std::string fmt_encoding(encoding x);


    // some quality-of-life constants to let end-user avoid writing 'encoding::' part

    constexpr encoding utf8 = encoding::utf8;
    constexpr encoding utf16 = encoding::utf16;
    constexpr encoding utf32 = encoding::utf32;
    constexpr encoding utf16_be = encoding::utf16_be;
    constexpr encoding utf16_le = encoding::utf16_le;
    constexpr encoding utf32_be = encoding::utf32_be;
    constexpr encoding utf32_le = encoding::utf32_le;
    constexpr encoding utf8_bom = encoding::utf8_bom;
    constexpr encoding utf16_bom = encoding::utf16_bom;
    constexpr encoding utf32_bom = encoding::utf32_bom;


    // base_encoding returns the base encoding of x, w/out explicit
    // endianness or BOM

    constexpr encoding base_encoding(encoding x) noexcept {
        switch (x) {
        case utf8:          return utf8;        break;
        case utf16:         return utf16;       break;
        case utf32:         return utf32;       break;
        case utf16_be:      return utf16;       break;
        case utf16_le:      return utf16;       break;
        case utf32_be:      return utf32;       break;
        case utf32_le:      return utf32;       break;
        case utf8_bom:      return utf8;        break;
        case utf16_bom:     return utf16;       break;
        case utf32_bom:     return utf32;       break;
        default:            return encoding{};  break;
        }
    }

    static_assert(base_encoding(utf8) == utf8);
    static_assert(base_encoding(utf16) == utf16);
    static_assert(base_encoding(utf32) == utf32);
    static_assert(base_encoding(utf16_be) == utf16);
    static_assert(base_encoding(utf16_le) == utf16);
    static_assert(base_encoding(utf32_be) == utf32);
    static_assert(base_encoding(utf32_le) == utf32);
    static_assert(base_encoding(utf8_bom) == utf8);
    static_assert(base_encoding(utf16_bom) == utf16);
    static_assert(base_encoding(utf32_bom) == utf32);


    // explicit_endian returns the endianness explicitly associated w/
    // encoding x, or std::endian::native if it has no explicit endianness

    constexpr std::endian explicit_endian(encoding x) noexcept {
        switch (x) {
        case utf16_be:      return std::endian::big;    break;
        case utf16_le:      return std::endian::little; break;
        case utf32_be:      return std::endian::big;    break;
        case utf32_le:      return std::endian::little; break;
        default:            return std::endian::native; break;
        }
    }

    static_assert(explicit_endian(utf8) == std::endian::native);
    static_assert(explicit_endian(utf16) == std::endian::native);
    static_assert(explicit_endian(utf32) == std::endian::native);
    static_assert(explicit_endian(utf16_be) == std::endian::big);
    static_assert(explicit_endian(utf16_le) == std::endian::little);
    static_assert(explicit_endian(utf32_be) == std::endian::big);
    static_assert(explicit_endian(utf32_le) == std::endian::little);
    static_assert(explicit_endian(utf8_bom) == std::endian::native);
    static_assert(explicit_endian(utf16_bom) == std::endian::native);
    static_assert(explicit_endian(utf32_bom) == std::endian::native);


    // explicit_bom returns if the encoding x has an explicit BOM

    // as the input encoding, encodings w/ an explicit BOMs require a BOM
    // to be present for decoding to succeed

    constexpr bool explicit_bom(encoding x) noexcept {
        switch (x) {
        case utf8_bom:      return true;    break;
        case utf16_bom:     return true;    break;
        case utf32_bom:     return true;    break;
        default:            return false;   break;
        }
    }

    static_assert(explicit_bom(utf8) == false);
    static_assert(explicit_bom(utf16) == false);
    static_assert(explicit_bom(utf32) == false);
    static_assert(explicit_bom(utf16_be) == false);
    static_assert(explicit_bom(utf16_le) == false);
    static_assert(explicit_bom(utf32_be) == false);
    static_assert(explicit_bom(utf32_le) == false);
    static_assert(explicit_bom(utf8_bom) == true);
    static_assert(explicit_bom(utf16_bom) == true);
    static_assert(explicit_bom(utf32_bom) == true);


    // drop_endian returns the encoding x w/out explicit endianness, if any

    constexpr encoding drop_endian(encoding x) noexcept {
        switch (x) {
        case utf8:          return utf8;        break;
        case utf16:         return utf16;       break;
        case utf32:         return utf32;       break;
        case utf16_be:      return utf16;       break;
        case utf16_le:      return utf16;       break;
        case utf32_be:      return utf32;       break;
        case utf32_le:      return utf32;       break;
        case utf8_bom:      return utf8_bom;    break;
        case utf16_bom:     return utf16_bom;   break;
        case utf32_bom:     return utf32_bom;   break;
        default:            return encoding{};  break;
        }
    }

    static_assert(drop_endian(utf8) == utf8);
    static_assert(drop_endian(utf16) == utf16);
    static_assert(drop_endian(utf32) == utf32);
    static_assert(drop_endian(utf16_be) == utf16);
    static_assert(drop_endian(utf16_le) == utf16);
    static_assert(drop_endian(utf32_be) == utf32);
    static_assert(drop_endian(utf32_le) == utf32);
    static_assert(drop_endian(utf8_bom) == utf8_bom);
    static_assert(drop_endian(utf16_bom) == utf16_bom);
    static_assert(drop_endian(utf32_bom) == utf32_bom);


    // drop_bom returns the encoding x w/out explicit BOM, if any

    constexpr encoding drop_bom(encoding x) noexcept {
        switch (x) {
        case utf8:      return utf8;        break;
        case utf16:     return utf16;       break;
        case utf32:     return utf32;       break;
        case utf16_be:  return utf16_be;    break;
        case utf16_le:  return utf16_le;    break;
        case utf32_be:  return utf32_be;    break;
        case utf32_le:  return utf32_le;    break;
        case utf8_bom:  return utf8;        break;
        case utf16_bom: return utf16;       break;
        case utf32_bom: return utf32;       break;
        default:        return encoding{};  break;
        }
    }

    static_assert(drop_bom(utf8) == utf8);
    static_assert(drop_bom(utf16) == utf16);
    static_assert(drop_bom(utf32) == utf32);
    static_assert(drop_bom(utf16_be) == utf16_be);
    static_assert(drop_bom(utf16_le) == utf16_le);
    static_assert(drop_bom(utf32_be) == utf32_be);
    static_assert(drop_bom(utf32_le) == utf32_le);
    static_assert(drop_bom(utf8_bom) == utf8);
    static_assert(drop_bom(utf16_bom) == utf16);
    static_assert(drop_bom(utf32_bom) == utf32);


    // change_endian returns the encoding x w/ its current endianness 
    // changed to a different, more explicit, one

    // for UTF-8, this always returns UTF-8 w/out BOM
    // for UTF-16 and UTF-32, this means returning explicit BE/LE versions
    // for BOM encodings, this means first dropping the BOM

    constexpr encoding change_endian(std::endian bo, encoding x) noexcept {
        if (bo == std::endian::big) {
            switch (x) {
            case utf8:      return utf8;        break;
            case utf16:     return utf16_be;    break;
            case utf32:     return utf32_be;    break;
            case utf16_be:  return utf16_be;    break;
            case utf16_le:  return utf16_be;    break;
            case utf32_be:  return utf32_be;    break;
            case utf32_le:  return utf32_be;    break;
            case utf8_bom:  return utf8;        break;
            case utf16_bom: return utf16_be;    break;
            case utf32_bom: return utf32_be;    break;
            default:        return encoding{};  break;
            }
        }
        else {
            switch (x) {
            case utf8:      return utf8;        break;
            case utf16:     return utf16_le;    break;
            case utf32:     return utf32_le;    break;
            case utf16_be:  return utf16_le;    break;
            case utf16_le:  return utf16_le;    break;
            case utf32_be:  return utf32_le;    break;
            case utf32_le:  return utf32_le;    break;
            case utf8_bom:  return utf8;        break;
            case utf16_bom: return utf16_le;    break;
            case utf32_bom: return utf32_le;    break;
            default:        return encoding{};  break;
            }
        }
    }

    static_assert(change_endian(std::endian::big, utf8) == utf8);
    static_assert(change_endian(std::endian::big, utf16) == utf16_be);
    static_assert(change_endian(std::endian::big, utf32) == utf32_be);
    static_assert(change_endian(std::endian::big, utf16_be) == utf16_be);
    static_assert(change_endian(std::endian::big, utf16_le) == utf16_be);
    static_assert(change_endian(std::endian::big, utf32_be) == utf32_be);
    static_assert(change_endian(std::endian::big, utf32_le) == utf32_be);
    static_assert(change_endian(std::endian::big, utf8_bom) == utf8);
    static_assert(change_endian(std::endian::big, utf16_bom) == utf16_be);
    static_assert(change_endian(std::endian::big, utf32_bom) == utf32_be);

    static_assert(change_endian(std::endian::little, utf8) == utf8);
    static_assert(change_endian(std::endian::little, utf16) == utf16_le);
    static_assert(change_endian(std::endian::little, utf32) == utf32_le);
    static_assert(change_endian(std::endian::little, utf16_be) == utf16_le);
    static_assert(change_endian(std::endian::little, utf16_le) == utf16_le);
    static_assert(change_endian(std::endian::little, utf32_be) == utf32_le);
    static_assert(change_endian(std::endian::little, utf32_le) == utf32_le);
    static_assert(change_endian(std::endian::little, utf8_bom) == utf8);
    static_assert(change_endian(std::endian::little, utf16_bom) == utf16_le);
    static_assert(change_endian(std::endian::little, utf32_bom) == utf32_le);


    // encoding_unit returns the number of bytes taken up by a *unit* of the
    // encoding x

    // for UTF-8, UTF-16, and UTF-32, these sizes are 1, 2, and 4, respectively

    // these can be a measure of the size of the char type needed for a given
    // encoding, or in case of byte strings, the number of byte chars needed
    // to be equivalent to one char of the encoding's unit size

    constexpr std::size_t encoding_unit(encoding x) noexcept {
        switch (base_encoding(x)) {
        case utf8:  return 1;               break;
        case utf16: return 2;               break;
        case utf32: return 4;               break;
        default:    return std::size_t(-1); break;
        }
    }

    static_assert(encoding_unit(utf8) == 1);
    static_assert(encoding_unit(utf16) == 2);
    static_assert(encoding_unit(utf32) == 4);
    static_assert(encoding_unit(utf16_be) == 2);
    static_assert(encoding_unit(utf16_le) == 2);
    static_assert(encoding_unit(utf32_be) == 4);
    static_assert(encoding_unit(utf32_le) == 4);
    static_assert(encoding_unit(utf8_bom) == 1);
    static_assert(encoding_unit(utf16_bom) == 2);
    static_assert(encoding_unit(utf32_bom) == 4);

    // units_required returns the number of *units* (see above) needed in
    // order to properly encode codepoint cp w/ encoding x

    // units_required returns 0 if is_unicode(cp) == false

    constexpr std::size_t units_required(encoding x, unicode_t cp) noexcept {
        if (!is_unicode(cp)) {
            return 0;
        }
        switch (base_encoding(x)) {
        case utf8:
        {
            if (in_codepoint_range(cp, 0x0000, 0x007f)) return 1;
            else if (in_codepoint_range(cp, 0x0080, 0x07ff)) return 2;
            else if (in_codepoint_range(cp, 0x0800, 0xffff)) return 3;
            else return 4;
        }
        break;
        case utf16:
        {
            if (in_codepoint_range(cp, 0x0000, 0xffff)) return 1;
            else return 2;
        }
        break;
        case utf32:
        {
            return 1;
        }
        break;
        default: return 0; break;
        }
    }

    static_assert(units_required(utf8, unicode_t(0x0000)) == 1);
    static_assert(units_required(utf8, unicode_t(0x007f)) == 1);
    static_assert(units_required(utf8, unicode_t(0x0080)) == 2);
    static_assert(units_required(utf8, unicode_t(0x07ff)) == 2);
    static_assert(units_required(utf8, unicode_t(0x0800)) == 3);
    static_assert(units_required(utf8, unicode_t(0xffff)) == 3);
    static_assert(units_required(utf8, unicode_t(0x10000)) == 4);
    static_assert(units_required(utf8, unicode_t(0x10ffff)) == 4);
    static_assert(units_required(utf8, unicode_t(0x110000)) == 0);
    static_assert(units_required(utf8, unicode_t(-1)) == 0);
    static_assert(units_required(utf8, first_surrogate) == 0);
    static_assert(units_required(utf8, last_surrogate) == 0);
    
    static_assert(units_required(utf16, unicode_t(0x0000)) == 1);
    static_assert(units_required(utf16, unicode_t(0xffff)) == 1);
    static_assert(units_required(utf16, unicode_t(0x10000)) == 2);
    static_assert(units_required(utf16, unicode_t(0x10ffff)) == 2);
    static_assert(units_required(utf16, unicode_t(0x110000)) == 0);
    static_assert(units_required(utf16, unicode_t(-1)) == 0);
    static_assert(units_required(utf16, first_surrogate) == 0);
    static_assert(units_required(utf16, last_surrogate) == 0);

    static_assert(units_required(utf32, unicode_t(0x0000)) == 1);
    static_assert(units_required(utf32, unicode_t(0x10ffff)) == 1);
    static_assert(units_required(utf32, unicode_t(0x110000)) == 0);
    static_assert(units_required(utf32, unicode_t(-1)) == 0);
    static_assert(units_required(utf32, first_surrogate) == 0);
    static_assert(units_required(utf32, last_surrogate) == 0);

    static_assert(units_required(utf16_be, unicode_t(0x0000)) == 1);
    static_assert(units_required(utf16_be, unicode_t(0xffff)) == 1);
    static_assert(units_required(utf16_be, unicode_t(0x10000)) == 2);
    static_assert(units_required(utf16_be, unicode_t(0x10ffff)) == 2);
    static_assert(units_required(utf16_be, unicode_t(0x110000)) == 0);
    static_assert(units_required(utf16_be, unicode_t(-1)) == 0);
    static_assert(units_required(utf16_be, first_surrogate) == 0);
    static_assert(units_required(utf16_be, last_surrogate) == 0);

    static_assert(units_required(utf16_le, unicode_t(0x0000)) == 1);
    static_assert(units_required(utf16_le, unicode_t(0xffff)) == 1);
    static_assert(units_required(utf16_le, unicode_t(0x10000)) == 2);
    static_assert(units_required(utf16_le, unicode_t(0x10ffff)) == 2);
    static_assert(units_required(utf16_le, unicode_t(0x110000)) == 0);
    static_assert(units_required(utf16_le, unicode_t(-1)) == 0);
    static_assert(units_required(utf16_le, first_surrogate) == 0);
    static_assert(units_required(utf16_le, last_surrogate) == 0);

    static_assert(units_required(utf32_be, unicode_t(0x0000)) == 1);
    static_assert(units_required(utf32_be, unicode_t(0x10ffff)) == 1);
    static_assert(units_required(utf32_be, unicode_t(0x110000)) == 0);
    static_assert(units_required(utf32_be, unicode_t(-1)) == 0);
    static_assert(units_required(utf32_be, first_surrogate) == 0);
    static_assert(units_required(utf32_be, last_surrogate) == 0);

    static_assert(units_required(utf32_le, unicode_t(0x0000)) == 1);
    static_assert(units_required(utf32_le, unicode_t(0x10ffff)) == 1);
    static_assert(units_required(utf32_le, unicode_t(0x110000)) == 0);
    static_assert(units_required(utf32_le, unicode_t(-1)) == 0);
    static_assert(units_required(utf32_le, first_surrogate) == 0);
    static_assert(units_required(utf32_le, last_surrogate) == 0);

    static_assert(units_required(utf8_bom, unicode_t(0x0000)) == 1);
    static_assert(units_required(utf8_bom, unicode_t(0x007f)) == 1);
    static_assert(units_required(utf8_bom, unicode_t(0x0080)) == 2);
    static_assert(units_required(utf8_bom, unicode_t(0x07ff)) == 2);
    static_assert(units_required(utf8_bom, unicode_t(0x0800)) == 3);
    static_assert(units_required(utf8_bom, unicode_t(0xffff)) == 3);
    static_assert(units_required(utf8_bom, unicode_t(0x10000)) == 4);
    static_assert(units_required(utf8_bom, unicode_t(0x10ffff)) == 4);
    static_assert(units_required(utf8_bom, unicode_t(0x110000)) == 0);
    static_assert(units_required(utf8_bom, unicode_t(-1)) == 0);
    static_assert(units_required(utf8_bom, first_surrogate) == 0);
    static_assert(units_required(utf8_bom, last_surrogate) == 0);

    static_assert(units_required(utf16_bom, unicode_t(0x0000)) == 1);
    static_assert(units_required(utf16_bom, unicode_t(0xffff)) == 1);
    static_assert(units_required(utf16_bom, unicode_t(0x10000)) == 2);
    static_assert(units_required(utf16_bom, unicode_t(0x10ffff)) == 2);
    static_assert(units_required(utf16_bom, unicode_t(0x110000)) == 0);
    static_assert(units_required(utf16_bom, unicode_t(-1)) == 0);
    static_assert(units_required(utf16_bom, first_surrogate) == 0);
    static_assert(units_required(utf16_bom, last_surrogate) == 0);

    static_assert(units_required(utf32_bom, unicode_t(0x0000)) == 1);
    static_assert(units_required(utf32_bom, unicode_t(0x10ffff)) == 1);
    static_assert(units_required(utf32_bom, unicode_t(0x110000)) == 0);
    static_assert(units_required(utf32_bom, unicode_t(-1)) == 0);
    static_assert(units_required(utf32_bom, first_surrogate) == 0);
    static_assert(units_required(utf32_bom, last_surrogate) == 0);

    // allowed_char_type returns if char type Char may be encoded/decoded
    // using the encoding x

    // our impl allows all encodings to encode/decode to/from strings of
    // one byte chars, but UTF-16 and UTF-32 encodings also allow for 
    // encoding/decoding to/from two and four byte char strings, respectively

    // notice that under these rules UTF-8 and UTF-16 encodings cannot be
    // directly used w/ strings of 32-bit chars, and likewise UTF-8 and 
    // UTF-32 encodings cannot be used w/ strings of 16-bit chars

    template<typename Char>
    constexpr bool allowed_char_type(encoding x) noexcept {
        return sizeof(Char) == 1 || sizeof(Char) == encoding_unit(x);
    }

    static_assert(allowed_char_type<char>(utf8) == true);
    static_assert(allowed_char_type<char>(utf16) == true);
    static_assert(allowed_char_type<char>(utf32) == true);
    static_assert(allowed_char_type<char>(utf16_be) == true);
    static_assert(allowed_char_type<char>(utf16_le) == true);
    static_assert(allowed_char_type<char>(utf32_be) == true);
    static_assert(allowed_char_type<char>(utf32_le) == true);
    static_assert(allowed_char_type<char>(utf8_bom) == true);
    static_assert(allowed_char_type<char>(utf16_bom) == true);
    static_assert(allowed_char_type<char>(utf32_bom) == true);

    static_assert(allowed_char_type<char8_t>(utf8) == true);
    static_assert(allowed_char_type<char8_t>(utf16) == true);
    static_assert(allowed_char_type<char8_t>(utf32) == true);
    static_assert(allowed_char_type<char8_t>(utf16_be) == true);
    static_assert(allowed_char_type<char8_t>(utf16_le) == true);
    static_assert(allowed_char_type<char8_t>(utf32_be) == true);
    static_assert(allowed_char_type<char8_t>(utf32_le) == true);
    static_assert(allowed_char_type<char8_t>(utf8_bom) == true);
    static_assert(allowed_char_type<char8_t>(utf16_bom) == true);
    static_assert(allowed_char_type<char8_t>(utf32_bom) == true);

    static_assert(allowed_char_type<char16_t>(utf8) == false);
    static_assert(allowed_char_type<char16_t>(utf16) == true);
    static_assert(allowed_char_type<char16_t>(utf32) == false);
    static_assert(allowed_char_type<char16_t>(utf16_be) == true);
    static_assert(allowed_char_type<char16_t>(utf16_le) == true);
    static_assert(allowed_char_type<char16_t>(utf32_be) == false);
    static_assert(allowed_char_type<char16_t>(utf32_le) == false);
    static_assert(allowed_char_type<char16_t>(utf8_bom) == false);
    static_assert(allowed_char_type<char16_t>(utf16_bom) == true);
    static_assert(allowed_char_type<char16_t>(utf32_bom) == false);

    static_assert(allowed_char_type<char32_t>(utf8) == false);
    static_assert(allowed_char_type<char32_t>(utf16) == false);
    static_assert(allowed_char_type<char32_t>(utf32) == true);
    static_assert(allowed_char_type<char32_t>(utf16_be) == false);
    static_assert(allowed_char_type<char32_t>(utf16_le) == false);
    static_assert(allowed_char_type<char32_t>(utf32_be) == true);
    static_assert(allowed_char_type<char32_t>(utf32_le) == true);
    static_assert(allowed_char_type<char32_t>(utf8_bom) == false);
    static_assert(allowed_char_type<char32_t>(utf16_bom) == false);
    static_assert(allowed_char_type<char32_t>(utf32_bom) == true);


    // wchar_encoding, if x is UTF-8-based, returns x
    // wchar_encoding, if x is UTF-16-based, and sizeof(wchar_t) == 2, returns x
    // wchar_encoding, if x is UTF-32-based, and sizeof(wchar_t) == 4, returns x
    // wchar_encoding, if x is UTF-16-based, but sizeof(wchar_t) == 4, returns
    // the equivalent UTF-32-based encoding
    // wchar_encoding, if x is UTF-32-based, but sizeof(wchar_t) == 2, returns
    // the equivalent UTF-16-based encoding

    // the return value of wchar_encoding will ensure that the expression
    // allowed_char_type<wchar_t>(wchar_encoding(x)) == true is always true

    // the reason for this function is to solve a portability issue which arises
    // from usage of wchar_t-based strings w/ our system, that being that
    // code written for Windows using it may not work on Linux due to wchar_t
    // changing size between the two, which would cause said code to potentially
    // break as a result

    // this function addresses this problem, this function is provided so that
    // the *internal* encoding used w/ wchar_t-based strings can be dynamically
    // changed between equivalent UTF-16 and UTF-32 encodings based on the size
    // of the wchar_t type, thus solving our problem

    // take note that, as stated, this dynamic changing of internal encodings
    // is to only be used *internally* by a program, and should not be used to
    // discern the encoding used when interacting w/ the external environment

    constexpr encoding wchar_encoding(encoding x) noexcept {
        if constexpr (sizeof(wchar_t) == 2) {
            switch (x) {
            case utf8:      return utf8;        break;
            case utf16:     return utf16;       break;
            case utf32:     return utf16;       break;
            case utf16_be:  return utf16_be;    break;
            case utf16_le:  return utf16_le;    break;
            case utf32_be:  return utf16_be;    break;
            case utf32_le:  return utf16_le;    break;
            case utf8_bom:  return utf8_bom;    break;
            case utf16_bom: return utf16_bom;   break;
            case utf32_bom: return utf16_bom;   break;
            default:        return encoding{};  break;
            }
        }
        else {
            switch (x) {
            case utf8:      return utf8;        break;
            case utf16:     return utf32;       break;
            case utf32:     return utf32;       break;
            case utf16_be:  return utf32_be;    break;
            case utf16_le:  return utf32_le;    break;
            case utf32_be:  return utf32_be;    break;
            case utf32_le:  return utf32_le;    break;
            case utf8_bom:  return utf8_bom;    break;
            case utf16_bom: return utf32_bom;   break;
            case utf32_bom: return utf32_bom;   break;
            default:        return encoding{};  break;
            }
        }
    }

    static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4);

    // Windows tests

    static_assert(sizeof(wchar_t) != 2 || wchar_encoding(utf8) == utf8);
    static_assert(sizeof(wchar_t) != 2 || wchar_encoding(utf16) == utf16);
    static_assert(sizeof(wchar_t) != 2 || wchar_encoding(utf32) == utf16);
    static_assert(sizeof(wchar_t) != 2 || wchar_encoding(utf16_be) == utf16_be);
    static_assert(sizeof(wchar_t) != 2 || wchar_encoding(utf16_le) == utf16_le);
    static_assert(sizeof(wchar_t) != 2 || wchar_encoding(utf32_be) == utf16_be);
    static_assert(sizeof(wchar_t) != 2 || wchar_encoding(utf32_le) == utf16_le);
    static_assert(sizeof(wchar_t) != 2 || wchar_encoding(utf8_bom) == utf8_bom);
    static_assert(sizeof(wchar_t) != 2 || wchar_encoding(utf16_bom) == utf16_bom);
    static_assert(sizeof(wchar_t) != 2 || wchar_encoding(utf32_bom) == utf16_bom);

    // Linux/macOS/etc. tests

    static_assert(sizeof(wchar_t) != 4 || wchar_encoding(utf8) == utf8);
    static_assert(sizeof(wchar_t) != 4 || wchar_encoding(utf16) == utf32);
    static_assert(sizeof(wchar_t) != 4 || wchar_encoding(utf32) == utf32);
    static_assert(sizeof(wchar_t) != 4 || wchar_encoding(utf16_be) == utf32_be);
    static_assert(sizeof(wchar_t) != 4 || wchar_encoding(utf16_le) == utf32_le);
    static_assert(sizeof(wchar_t) != 4 || wchar_encoding(utf32_be) == utf32_be);
    static_assert(sizeof(wchar_t) != 4 || wchar_encoding(utf32_le) == utf32_le);
    static_assert(sizeof(wchar_t) != 4 || wchar_encoding(utf8_bom) == utf8_bom);
    static_assert(sizeof(wchar_t) != 4 || wchar_encoding(utf16_bom) == utf32_bom);
    static_assert(sizeof(wchar_t) != 4 || wchar_encoding(utf32_bom) == utf32_bom);

    static_assert(allowed_char_type<wchar_t>(wchar_encoding(utf8)) == false);
    static_assert(allowed_char_type<wchar_t>(wchar_encoding(utf16)) == true);
    static_assert(allowed_char_type<wchar_t>(wchar_encoding(utf32)) == true);
    static_assert(allowed_char_type<wchar_t>(wchar_encoding(utf16_be)) == true);
    static_assert(allowed_char_type<wchar_t>(wchar_encoding(utf16_le)) == true);
    static_assert(allowed_char_type<wchar_t>(wchar_encoding(utf32_be)) == true);
    static_assert(allowed_char_type<wchar_t>(wchar_encoding(utf32_le)) == true);
    static_assert(allowed_char_type<wchar_t>(wchar_encoding(utf8_bom)) == false);
    static_assert(allowed_char_type<wchar_t>(wchar_encoding(utf16_bom)) == true);
    static_assert(allowed_char_type<wchar_t>(wchar_encoding(utf32_bom)) == true);


    // bom_length_bytes returns the length, in bytes, of the BOM of encoding x

    constexpr std::size_t bom_length_bytes(encoding x) noexcept {
        switch (base_encoding(x)) {
        case utf8: return 3; break;
        case utf16: return 2; break;
        case utf32: return 4; break;
        default: break;
        }
        return std::size_t(-1);
    }

    static_assert(bom_length_bytes(utf8) == 3);
    static_assert(bom_length_bytes(utf16) == 2);
    static_assert(bom_length_bytes(utf32) == 4);
    static_assert(bom_length_bytes(utf16_be) == 2);
    static_assert(bom_length_bytes(utf16_le) == 2);
    static_assert(bom_length_bytes(utf32_be) == 4);
    static_assert(bom_length_bytes(utf32_le) == 4);
    static_assert(bom_length_bytes(utf8_bom) == 3);
    static_assert(bom_length_bytes(utf16_bom) == 2);
    static_assert(bom_length_bytes(utf32_bom) == 4);

    // bom_length_chars is a quality-of-life alternative to the above

    // bom_length_chars<Char> behaviour undefined if allowed_char_type<Char>(x) == false

    template<typename Char>
    inline std::size_t bom_length_chars(encoding x) noexcept {
        TAUL_ASSERT(allowed_char_type<Char>(x));
        return bom_length_bytes(x) / sizeof(Char);
    }


    namespace internal {
        constexpr char utf8_bom_bytes_be[3] = { 0xefui8, 0xbbui8, 0xbfui8 };
        constexpr char utf8_bom_bytes_le[3] = { 0xefui8, 0xbbui8, 0xbfui8 };
        constexpr char utf16_bom_bytes_be[2] = { 0xfeui8, 0xffui8 };
        constexpr char utf16_bom_bytes_le[2] = { 0xffui8, 0xfeui8 };
        constexpr char utf32_bom_bytes_be[4] = { 0x00ui8, 0x00ui8, 0xfeui8, 0xffui8 };
        constexpr char utf32_bom_bytes_le[4] = { 0xffui8, 0xfeui8, 0x00ui8, 0x00ui8 };

        constexpr std::span<const char> encoding_bom_bytes_be(encoding x) noexcept {
            switch (base_encoding(x)) {
            case utf8:  return std::span<const char>{ utf8_bom_bytes_be, 3 };   break;
            case utf16: return std::span<const char>{ utf16_bom_bytes_be, 2 };  break;
            case utf32: return std::span<const char>{ utf32_bom_bytes_be, 4 };  break;
            default:    return std::span<const char>{};                         break;
            }
        }

        constexpr std::span<const char> encoding_bom_bytes_le(encoding x) noexcept {
            switch (base_encoding(x)) {
            case utf8:  return std::span<const char>{ utf8_bom_bytes_le, 3 };   break;
            case utf16: return std::span<const char>{ utf16_bom_bytes_le, 2 };  break;
            case utf32: return std::span<const char>{ utf32_bom_bytes_le, 4 };  break;
            default:    return std::span<const char>{};                         break;
            }
        }
    }

    // TODO: the below BOM functions have not been unit tested (at least not enough to
    //       cover all of its core behaviour, especially regarding endianness)

    // encoding_bom_bytes and encoding_antibom_bytes define what byte order defines
    // the 'BOM' and 'anti-BOM' orders as a function of encoding x, and what will
    // be *correct* from its perspective

    // if explicit_bom(x) == true, then the encoding used will be drop_bom(x)

    constexpr std::span<const char> encoding_bom_bytes(encoding x) noexcept {
        return
            explicit_endian(x) == std::endian::big
            ? internal::encoding_bom_bytes_be(x)
            : internal::encoding_bom_bytes_le(x);
    }

    constexpr std::span<const char> encoding_antibom_bytes(encoding x) noexcept {
        return
            explicit_endian(x) == std::endian::big
            ? internal::encoding_bom_bytes_le(x)
            : internal::encoding_bom_bytes_be(x);
    }

    // test that BOMs/anti-BOMs are of their expected lengths

    static_assert(encoding_bom_bytes(utf8).size() == bom_length_bytes(utf8));
    static_assert(encoding_bom_bytes(utf16).size() == bom_length_bytes(utf16));
    static_assert(encoding_bom_bytes(utf32).size() == bom_length_bytes(utf32));
    static_assert(encoding_bom_bytes(utf16_be).size() == bom_length_bytes(utf16_be));
    static_assert(encoding_bom_bytes(utf16_le).size() == bom_length_bytes(utf16_le));
    static_assert(encoding_bom_bytes(utf32_be).size() == bom_length_bytes(utf32_be));
    static_assert(encoding_bom_bytes(utf32_le).size() == bom_length_bytes(utf32_le));
    static_assert(encoding_bom_bytes(utf8_bom).size() == bom_length_bytes(utf8_bom));
    static_assert(encoding_bom_bytes(utf16_bom).size() == bom_length_bytes(utf16_bom));
    static_assert(encoding_bom_bytes(utf32_bom).size() == bom_length_bytes(utf32_bom));
    
    static_assert(encoding_antibom_bytes(utf8).size() == bom_length_bytes(utf8));
    static_assert(encoding_antibom_bytes(utf16).size() == bom_length_bytes(utf16));
    static_assert(encoding_antibom_bytes(utf32).size() == bom_length_bytes(utf32));
    static_assert(encoding_antibom_bytes(utf16_be).size() == bom_length_bytes(utf16_be));
    static_assert(encoding_antibom_bytes(utf16_le).size() == bom_length_bytes(utf16_le));
    static_assert(encoding_antibom_bytes(utf32_be).size() == bom_length_bytes(utf32_be));
    static_assert(encoding_antibom_bytes(utf32_le).size() == bom_length_bytes(utf32_le));
    static_assert(encoding_antibom_bytes(utf8_bom).size() == bom_length_bytes(utf8_bom));
    static_assert(encoding_antibom_bytes(utf16_bom).size() == bom_length_bytes(utf16_bom));
    static_assert(encoding_antibom_bytes(utf32_bom).size() == bom_length_bytes(utf32_bom));

    // UTF-8's BOM doesn't change based on machine endianness

    static_assert(
        []() constexpr -> bool {
            const auto a = encoding_bom_bytes(utf8);
            const auto b = encoding_antibom_bytes(utf8);
            if (a.size() != b.size()) {
                return false;
            }
            for (std::size_t i = 0; i < a.size(); i++) {
                if (a[i] != b[i]) {
                    return false;
                }
            }
            return true;
        }());

    // these are quality-of-life alternatives to the above

    // encoding_bom_chars<Char> behaviour undefined if allowed_char_type<Char>(x) == false
    // encoding_antibom_chars<Char> behaviour undefined if allowed_char_type<Char>(x) == false

    template<typename Char>
    inline std::basic_string_view<Char> encoding_bom_chars(encoding x) noexcept {
        TAUL_ASSERT(allowed_char_type<Char>(x));
        auto b = encoding_bom_bytes(x);
        TAUL_ASSERT(b.size() % sizeof(Char) == 0);
        auto _len = b.size() / sizeof(Char);
        // reminder that raw pointers are valid random-access iterators
        auto _begin = (const Char*)b.data();
        auto _end = _begin + _len;
        return std::basic_string_view<Char>(_begin, _end);
    }

    template<typename Char>
    inline std::basic_string_view<Char> encoding_antibom_chars(encoding x) noexcept {
        TAUL_ASSERT(allowed_char_type<Char>(x));
        auto b = encoding_antibom_bytes(x);
        TAUL_ASSERT(b.size() % sizeof(Char) == 0);
        auto _len = b.size() / sizeof(Char);
        // reminder that raw pointers are valid random-access iterators
        auto _begin = (const Char*)b.data();
        auto _end = _begin + _len;
        return std::basic_string_view<Char>(_begin, _end);
    }


    enum class bom_status : std::uint8_t {
        no_bom,     // no BOM
        bom,        // non-reversed BOM
        antibom,    // reversed BOM
    };

    // check_bom checks if the beginning of input contains a BOM of encoding e, 
    // returning it's status

    template<typename Char>
    inline bom_status check_bom(encoding e, std::basic_string_view<Char> input) noexcept {
        const auto bom_chars = encoding_bom_chars<Char>(e);
        const auto antibom_chars = encoding_antibom_chars<Char>(e);
        const auto subject = input.substr(0, bom_length_chars<Char>(e));
        if (subject == bom_chars)           return bom_status::bom;
        else if (subject == antibom_chars)  return bom_status::antibom;
        else                                return bom_status::no_bom;
    }


    namespace internal {


        struct encode_args final {
            encoding e;
            char* target;
            unicode_t x;
            std::size_t units;
            std::size_t bytes;
            std::size_t chars;
        };

        inline void encode_utf8_onto(const internal::encode_args& args) {
            if (args.target) {
                if (args.units == 1) {
                    const char8_t chr = char8_t(args.x);
                    std::memcpy((void*)(args.target + 0), (const void*)&chr, sizeof(decltype(chr)));
                }
                else if (args.units == 2) {
                    unicode_t a = 0xc0 + ((args.x >> 6) & 0x1f);
                    unicode_t b = 0x80 + (args.x & 0x3f);
                    args.target[0] = char8_t(a);
                    args.target[1] = char8_t(b);
                }
                else if (args.units == 3) {
                    unicode_t a = 0xe0 + ((args.x >> 12) & 0x0f);
                    unicode_t b = 0x80 + ((args.x >> 6) & 0x3f);
                    unicode_t c = 0x80 + (args.x & 0x3f);
                    args.target[0] = char8_t(a);
                    args.target[1] = char8_t(b);
                    args.target[2] = char8_t(c);
                }
                else if (args.units == 4) {
                    unicode_t a = 0xf0 + ((args.x >> 18) & 0x07);
                    unicode_t b = 0x80 + ((args.x >> 12) & 0x3f);
                    unicode_t c = 0x80 + ((args.x >> 6) & 0x3f);
                    unicode_t d = 0x80 + (args.x & 0x3f);
                    args.target[0] = char8_t(a);
                    args.target[1] = char8_t(b);
                    args.target[2] = char8_t(c);
                    args.target[3] = char8_t(d);
                }
                else TAUL_DEADEND;
            }
            else TAUL_DEADEND;
        }

        inline void encode_utf16_onto(const internal::encode_args& args) {
            TAUL_ASSERT(args.target);
            if (args.units == 1) {
                const char16_t chr = to_endian(explicit_endian(args.e), char16_t(args.x));
                std::memcpy((void*)(args.target + 0), (const void*)&chr, sizeof(decltype(chr)));
            }
            else if (args.units == 2) {
                unicode_t u = args.x - 0x10000;
                unicode_t w1 = 0xd800 + ((u >> 10) & 0x03ff);
                unicode_t w2 = 0xdc00 + (u & 0x03ff);
                const char16_t high = to_endian(explicit_endian(args.e), char16_t(w1));
                const char16_t low = to_endian(explicit_endian(args.e), char16_t(w2));
                std::memcpy((void*)(args.target + 0), (const void*)&high, sizeof(decltype(high)));
                std::memcpy((void*)(args.target + 2), (const void*)&low, sizeof(decltype(low)));
            }
            else TAUL_DEADEND;
        }

        inline void encode_utf32_onto(const internal::encode_args& args) {
            TAUL_ASSERT(args.target);
            TAUL_ASSERT(args.units == 1);
            const char32_t chr_v = char32_t(args.x);
            const char32_t chr = to_endian(explicit_endian(args.e), chr_v);
            std::memcpy((void*)(args.target + 0), (const void*)&chr, sizeof(decltype(chr)));
        }

        inline void encode_onto(const internal::encode_args& args) {
            TAUL_ASSERT(args.target);
            switch (base_encoding(args.e)) {
            case utf8: encode_utf8_onto(args); break;
            case utf16: encode_utf16_onto(args); break;
            case utf32: encode_utf32_onto(args); break;
            default: TAUL_DEADEND; break;
            }
        }


        // TODO: this could be a cool thing to have as part of our frontend

        enum class utf8_byte_type : std::uint8_t {
            unknown,
            ascii,
            continuation,
            two_byte_head,
            three_byte_head,
            four_byte_head,
        };

        inline utf8_byte_type get_utf8_byte_type(const char* x) noexcept {
            if (x) {
                if ((*x & 0b1111'1000i8) == 0b1111'0000i8) return utf8_byte_type::four_byte_head;
                else if ((*x & 0b1111'0000i8) == 0b1110'0000i8) return utf8_byte_type::three_byte_head;
                else if ((*x & 0b1110'0000i8) == 0b1100'0000i8) return utf8_byte_type::two_byte_head;
                else if ((*x & 0b1100'0000i8) == 0b1000'0000i8) return utf8_byte_type::continuation;
                else if ((*x & 0b1000'0000i8) == 0b0000'0000i8) return utf8_byte_type::ascii;
                else return utf8_byte_type::unknown;
            }
            else TAUL_DEADEND;
            return utf8_byte_type{};
        }

        inline bool check_utf8_expected_continuation_bytes(const char* x, std::size_t len, utf8_byte_type t) noexcept {
            if (x) {
                TAUL_ASSERT(len >= 1);
                TAUL_ASSERT(get_utf8_byte_type(x) == t);
                switch (t) {
                case utf8_byte_type::ascii:
                {
                    return true;
                }
                break;
                case utf8_byte_type::continuation: TAUL_DEADEND; break;
                case utf8_byte_type::two_byte_head:
                {
                    return
                        len >= 2 &&
                        get_utf8_byte_type(x + 1) == utf8_byte_type::continuation;
                }
                break;
                case utf8_byte_type::three_byte_head:
                {
                    return
                        len >= 3 &&
                        get_utf8_byte_type(x + 1) == utf8_byte_type::continuation &&
                        get_utf8_byte_type(x + 2) == utf8_byte_type::continuation;
                }
                break;
                case utf8_byte_type::four_byte_head:
                {
                    return
                        len >= 4 &&
                        get_utf8_byte_type(x + 1) == utf8_byte_type::continuation &&
                        get_utf8_byte_type(x + 2) == utf8_byte_type::continuation &&
                        get_utf8_byte_type(x + 3) == utf8_byte_type::continuation;
                }
                break;
                default: TAUL_DEADEND; break;
                }
            }
            else TAUL_DEADEND;
            return bool();
        }

        inline std::optional<decode_result> decode_utf8(const char* x, std::size_t len) noexcept {
            TAUL_ASSERT(len >= 1);
            if (x) {
                const auto head_type = get_utf8_byte_type(x);
                if (head_type == utf8_byte_type::unknown) return std::nullopt;
                if (head_type == utf8_byte_type::continuation) return std::nullopt;
                if (!check_utf8_expected_continuation_bytes(x, len, head_type)) return std::nullopt;
                decode_result result{};
                switch (head_type) {
                case utf8_byte_type::ascii:
                {
                    result = {
                        unicode_t(*x),
                        1,
                    };
                }
                break;
                case utf8_byte_type::continuation: TAUL_DEADEND; break;
                case utf8_byte_type::two_byte_head:
                {
                    unicode_t val{};
                    val |= ((x[0] & 0b0001'1111i8) << 6);
                    val |= (x[1] & 0b0011'1111i8);
                    result = {
                        val,
                        2,
                    };
                }
                break;
                case utf8_byte_type::three_byte_head:
                {
                    unicode_t val{};
                    val |= ((x[0] & 0b0000'1111i8) << 12);
                    val |= ((x[1] & 0b0011'1111i8) << 6);
                    val |= (x[2] & 0b0011'1111i8);
                    result = {
                        val,
                        3,
                    };
                }
                break;
                case utf8_byte_type::four_byte_head:
                {
                    unicode_t val{};
                    val |= ((x[0] & 0b0000'0111i8) << 18);
                    val |= ((x[1] & 0b0011'1111i8) << 12);
                    val |= ((x[2] & 0b0011'1111i8) << 6);
                    val |= (x[3] & 0b0011'1111i8);
                    result = {
                        val,
                        4,
                    };
                }
                break;
                default: TAUL_DEADEND; break;
                }
                const auto required_bytes = units_required(taul::utf8, result.cp);
                if (required_bytes < result.bytes) return std::nullopt; // overlong encoding
                return std::make_optional(result);
            }
            else TAUL_DEADEND;
            return std::nullopt;
        }

        enum class utf16_unit_type : std::uint8_t {
            nonsurrogate,
            leading_surrogate,
            trailing_surrogate,
        };

        inline std::uint16_t load_utf16_unit(const char* x, std::endian bo) noexcept {
            TAUL_ASSERT(x);
            return to_endian(bo, *(const std::uint16_t*)x);
        }

        constexpr utf16_unit_type get_utf16_unit_type(std::uint16_t unit) noexcept {
            if (is_leading_surrogate(unicode_t(unit))) return utf16_unit_type::leading_surrogate;
            else if (is_trailing_surrogate(unicode_t(unit))) return utf16_unit_type::trailing_surrogate;
            else return utf16_unit_type::nonsurrogate;
        }

        inline unicode_t extract_surrogate_value(std::uint16_t unit) noexcept {
            return unit & 0b0000'0011'1111'1111ui16;
        }

        inline std::optional<decode_result> decode_utf16(const char* x, std::size_t len, std::endian bo) noexcept {
            TAUL_ASSERT(x);
            TAUL_ASSERT(len >= 2);
            // potentially unaligned memory access
            const auto high = load_utf16_unit(x, bo);
            switch (get_utf16_unit_type(high)) {
            case utf16_unit_type::nonsurrogate:
            {
                decode_result result{
                    unicode_t(high),
                    2,
                };
                return std::make_optional(result);
            }
            break;
            case utf16_unit_type::leading_surrogate:
            {
                if (len < 4) break;
                // potentially unaligned memory access
                const auto low = load_utf16_unit(x + 2, bo);
                if (get_utf16_unit_type(low) != utf16_unit_type::trailing_surrogate) break;
                const auto high_v = extract_surrogate_value(high);
                const auto low_v = extract_surrogate_value(low);
                decode_result result{
                    ((high_v << 10) | low_v) + unicode_t(0x10000ui32),
                    4,
                };
                return std::make_optional(result);
            }
            break;
            case utf16_unit_type::trailing_surrogate:
            {
                break;
            }
            break;
            default: TAUL_DEADEND; break;
            }
            return std::nullopt;
        }

        inline std::uint32_t load_utf32_unit(const char* x, std::endian bo) noexcept {
            TAUL_ASSERT(x);
            return to_endian(bo, *(const std::uint32_t*)x);
        }

        constexpr unicode_t extract_utf32_unit_value(std::uint32_t unit) noexcept {
            return unicode_t(unit & 0b0000'0000'0001'1111'1111'1111'1111'1111ui32);
        }

        inline std::optional<decode_result> decode_utf32(const char* x, std::size_t len, std::endian bo) noexcept {
            TAUL_ASSERT(x);
            TAUL_ASSERT(len >= 4);
            // potentially unaligned memory access(es)
            const auto unit = load_utf32_unit(x, bo);
            const auto unit_v = extract_utf32_unit_value(unit);
            decode_result result{
                unit_v,
                4,
            };
            return std::make_optional(result);
        }
    }


    template<typename OutChar>
    inline std::optional<std::basic_string<OutChar>> encode(
        encoding out_e, 
        unicode_t x) {
        out_e = drop_bom(out_e);
        if (!is_unicode(x)) return std::nullopt;
        if (!allowed_char_type<OutChar>(out_e)) return std::nullopt;
        constexpr bool byte_chars = sizeof(OutChar) == 1;
        const auto units = units_required(out_e, x);
        const auto bytes = units * encoding_unit(out_e);
        const auto chars = byte_chars ? bytes : units;
        // SSO == no heap alloc
        std::basic_string<OutChar> result(chars, ' ');
        internal::encode_args args{
            out_e,
            (char*)result.data(),
            x,
            units,
            bytes,
            chars,
        };
        internal::encode_onto(args);
        return std::make_optional(std::move(result));
    }

    template<typename InChar>
    inline std::optional<decode_result> decode(
        encoding in_e,
        std::basic_string_view<InChar> x) {
        in_e = drop_bom(in_e);
        if (!allowed_char_type<InChar>(in_e)) return std::nullopt;
        if (x.empty()) return std::nullopt;
        const auto len = x.length() * sizeof(InChar);
        if (len % encoding_unit(in_e) != 0) return std::nullopt;
        std::optional<decode_result> result = std::nullopt;
        switch (base_encoding(in_e)) {
        case utf8: result = internal::decode_utf8((const char*)x.data(), len); break;
        case utf16: result = internal::decode_utf16((const char*)x.data(), len, explicit_endian(in_e)); break;
        case utf32: result = internal::decode_utf32((const char*)x.data(), len, explicit_endian(in_e)); break;
        default: TAUL_DEADEND; break;
        }
        if (result && !is_unicode(result->cp)) return std::nullopt;
        return result;
    }

    template<typename OutChar, typename InChar>
    inline std::optional<std::basic_string<OutChar>> convert_encoding(
        encoding in_e,
        encoding out_e,
        std::basic_string_view<InChar> x,
        unicode_t err) {
        if (!allowed_char_type<InChar>(in_e)) return std::nullopt;
        if (!allowed_char_type<OutChar>(out_e)) return std::nullopt;
        if (!is_unicode(err)) return std::nullopt;
        const auto bom_status_v = check_bom(in_e, x);
        const bool has_bom = bom_status_v != bom_status::no_bom;
        auto effective_in_e = in_e;
        if (has_bom) {
            effective_in_e =
                bom_status_v == bom_status::bom
                ? in_e
                : change_endian(opposite_endian(explicit_endian(in_e)), in_e);
        }
        if (explicit_bom(in_e) && !has_bom) return std::nullopt;
        decoder<InChar> decoder_v(effective_in_e, x);
        encoder<OutChar> encoder_v(out_e);
        if (has_bom) decoder_v.skip(bom_length_chars<InChar>(effective_in_e));
        if (explicit_bom(out_e)) {
            if (!encoder_v.next(bom_codepoint)) TAUL_DEADEND;
        }
        while (!decoder_v.done()) {
            const auto decoded = decoder_v.next();
            if (decoded) {
                if (!encoder_v.next(decoded->cp)) TAUL_DEADEND;
            }
            else {
                for (std::size_t j = 0; j < encoding_unit(effective_in_e); j++) {
                    if (!encoder_v.next(err)) TAUL_DEADEND;
                }
                decoder_v.skip(1);
            }
        }
        return std::make_optional<std::basic_string<OutChar>>(encoder_v.result());
    }

    template<typename OutChar, typename InChar>
    std::optional<std::basic_string<OutChar>> convert_encoding(
        encoding in_e, 
        encoding out_e, 
        const std::basic_string<InChar>& x, 
        unicode_t err) {
        return convert_encoding<OutChar>(in_e, out_e, std::basic_string_view<InChar>(x), err);
    }

    template<typename OutChar, typename InChar>
    std::optional<std::basic_string<OutChar>> convert_encoding(
        encoding in_e, 
        encoding out_e, 
        const InChar* x, 
        unicode_t err) {
        TAUL_ASSERT(x);
        return convert_encoding<OutChar>(in_e, out_e, std::basic_string_view<InChar>(x), err);
    }

    template<typename OutChar>
    inline encoder<OutChar>::encoder(encoding out_e) 
        : _out_e(out_e), 
        _result({}) {}

    template<typename OutChar>
    inline bool encoder<OutChar>::next(unicode_t x) {
        auto temp = encode<OutChar>(_out_e, x);
        if (temp) {
            _result += *temp;
        }
        return (bool)temp;
    }

    template<typename OutChar>
    inline taul::encoder<OutChar>::string_t taul::encoder<OutChar>::result() noexcept {
        return std::move(_result);
    }

    template<typename InChar>
    inline taul::decoder<InChar>::decoder(encoding in_e, string_view_t input) 
        : _in_e(in_e), 
        _input(input), 
        _pos(0) {}

    template<typename InChar>
    inline std::size_t taul::decoder<InChar>::pos() const noexcept {
        return _pos;
    }

    template<typename InChar>
    inline std::optional<decode_result> taul::decoder<InChar>::next() {
        const auto result = peek();
        if (result) {
            skip(result->chars<InChar>());
        }
        return result;
    }

    template<typename InChar>
    inline std::optional<decode_result> taul::decoder<InChar>::peek() {
        return decode<InChar>(_in_e, _input.substr(pos()));
    }

    template<typename InChar>
    inline void taul::decoder<InChar>::skip(std::size_t n) noexcept {
        const auto _remaining = _input.length() - pos();
        _pos += n <= _remaining ? n : _remaining;
    }

    template<typename InChar>
    inline bool taul::decoder<InChar>::done() const noexcept {
        return _pos == _input.length();
    }
}


template<>
struct std::formatter<taul::encoding> final : std::formatter<std::string> {
    auto format(const taul::encoding& x, format_context& ctx) const {
        return formatter<string>::format(taul::fmt_encoding(x), ctx);
    }
};

namespace std {
    template<typename Char>
    inline std::ostream& operator<<(std::ostream& stream, const taul::encoding& x) {
        return stream << taul::fmt_encoding(x);
    }
}

