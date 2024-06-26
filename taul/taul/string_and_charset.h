

#pragma once


#include <string>
#include <string_view>
#include <optional>

#include "unicode.h"


namespace taul {


    // in order to simplify testing, we define here a procedural API for resolving
    // TAUL language strings and charsets, such that things like escape sequences,
    // and char ranges within charsets, are resolved

    // this API also covers escape sequences

    // the unit tests for these will replace testing spec loader behaviour in these
    // regards, w/ spec loader behaviour only testing in summary for these things,
    // w/ the impl being expected to use this API in the spec loader


    // this system is part of the TAUL frontend, w/ things like charset strings
    // being used to then generate backend structures which actually get incorporated
    // into the behaviour of grammars


    // -- CHARS & ESCAPE SEQUENCES --


    // see taul.taul for a complete list of TAUL language escape sequences

    // this little API only parses/formats a single (potentially multi-byte) 
    // char or escape sequence


    // parse_taul_char parses a single regular character from the start of x, 
    // resolving an escape sequence if needed, returning it, or std::nullopt
    // if parsing failed due to x.empty() == true

    // parse_taul_char allows for x to contain UTF-8 encoded sequences

    // parse_taul_char allows for TAUL escape sequences to specify otherwise
    // illegal Unicode codepoints

    // the TAUL string x is expected to NOT have enclosing '\'' characters

    // if chars != nullptr, *chars will be assigned the number of characters which
    // where parsed to resolve the escaped character, w/ no assignment occurring
    // if parsing failed

    std::optional<unicode_t> parse_taul_char(std::string_view x, std::size_t* chars = nullptr);
    
    // this overload ensures x doesn't become dangling during call

    template<typename StringLike>
    inline std::optional<unicode_t> parse_taul_char(const StringLike& x, std::size_t* chars = nullptr) {
        return parse_taul_char(std::string_view(x), chars);
    }

    // fmt_taul_char takes a character and creates a valid representation of 
    // it in TAUL, escaping it if needed

    // fmt_taul_char will, for characters that are not visible ASCII or which lack
    // a specific escape sequence dedicated to them, format such characters via the
    // use of hexadecimal TAUL escape sequences to aid readability

    // fmt_taul_char will also uses hexadecimal TAUL escape sequences in order to
    // be able to output a formatted string containing potentially illegal Unicode
    // codepoints specified in it

    // formatted hexadecimal escape seqs will have lowercase hex digits

    // if string_not_charset == true, '\'' will be escaped, but ']' and '-' will not
    
    // if string_not_charset == false, ']' and '-' will be escaped, but '\'' will not

    std::string fmt_taul_char(unicode_t x, bool string_not_charset = true);


    // -- STRINGS --


    // parse_taul_string takes a TAUL string and returns a string equivalent to
    // it, but w/ any escape sequences resolved

    // the TAUL string x is expected to NOT have enclosing '\'' characters

    // parse_taul_string allows for non-ASCII chars to be included in the string
    // as UTF-8 encoded sequences

    // parse_taul_string allows for TAUL escape sequences to specify otherwise
    // illegal Unicode codepoints

    // unescaped '\'' chars are not given any special treatment herein; they're
    // only notable in TAUL syntax compilation

    // if the final char is an unescaped '\\' char, it will likewise also not
    // be given any special treatment

    // (the above two are for ensuring we can't ever *fail* parsing, to ease
    // usage of this code in taul::load)

    std::u32string parse_taul_string(std::string_view x);

    // this overload ensures x doesn't become dangling during call

    template<typename StringLike>
    inline std::u32string parse_taul_string(const StringLike& x) {
        return parse_taul_string(std::string_view(x));
    }

    // fmt_taul_string takes a string x and returns a valid TAUL string, w/ 
    // characters being escaped as needed

    // formatted hexadecimal escape seqs will have lowercase hex digits

    std::string fmt_taul_string(std::u32string_view x);

    // this overload ensures x doesn't become dangling during call

    template<typename U32StringLike>
    inline std::string fmt_taul_string(const U32StringLike& x) {
        return fmt_taul_string(std::u32string_view(x));
    }


    // -- CHARSETS --


    // IMPORTANT: the chars in the below format may encode non-ASCII Unicode
    //            as UTF-8 encoded sequences

    // in C++, parsed TAUL charsets are represented by strings w/ a vary
    // specific format

    // in this format, the charset is represented as an array of char ranges,
    // each formed by char pairs, the first being the *low* codepoint char
    // of the range, and the second being the *high* codepoint char, w/ these
    // two chars describing an *inclusive set* of chars (ie. [first, last]
    // not [first, last))

    // for example, 'aabbcc' is equiv to the TAUL charset [abc], w/ 'ac' 
    // likewise equiv to [a-c], and 'ac11xz' being equiv to [a-c1x-z]

    // chars in these charset strings may be invisible chars like \r or \f,
    // or they may be spaces, other visible ASCII, etc. w/ these strings
    // being vary *raw* representations

    // these strings may also contain stray nulls, as nulls are valid chars,
    // so the impl must keep this in mind

    // notice that unlike TAUL syntax, the first char of each pair MUST be
    // the low codepoint, NEVER the high codepoint, w/ behaviour involving
    // these invalid charsets being generally undefined

    // chars not forming charsets in the parsed TAUL charset string are also
    // represented like this, treating individual chars and explicit char
    // ranges the same


    // in TAUL, charsets allow for ANTLR-style '[a-z]' ranges

    // the '-' must be an escaped '\-' in order to not form a char range, w/
    // exception of '-' chars at the vary start or end of the charset, which
    // do not need to be escaped

    // furthermore, TAUL charsets allow the first char of these char ranges
    // to be the *high* codepoint, and the second char of the pair the *low*
    // codepoint of the two


    // parse_taul_charset takes a TAUL charset and returns a parsed charset 
    // string equivalent to it, but w/ any escape sequences resolved

    // the TAUL charset x is expected to NOT have enclosing '[' or ']' characters

    // parse_taul_charset allows for non-ASCII chars to be included in the charset
    // as UTF-8 encoded sequences

    // parse_taul_charset allows for TAUL escape sequences to specify otherwise
    // illegal Unicode codepoints

    // unescaped ']' chars are not given any special treatment herein; they're
    // only notable in TAUL syntax compilation

    // if the final char is an unescaped '\\' char, it will likewise also not
    // be given any special treatment

    // (the above two are for ensuring we can't ever *fail* parsing, to ease
    // usage of this code in taul::load)

    std::u32string parse_taul_charset(std::string_view x);

    // this overload ensures x doesn't become dangling during call

    template<typename StringLike>
    inline std::u32string parse_taul_charset(const StringLike& x) {
        return parse_taul_charset(std::string_view(x));
    }

    // fmt_taul_charset takes a charset string x and returns a valid TAUL charset, 
    // w/ characters being escaped as needed
    
    // formatted hexadecimal escape seqs will have lowercase hex digits

    // behaviour is undefined if x is not a valid charset string

    std::string fmt_taul_charset(std::u32string_view x);

    // this overload ensures x doesn't become dangling during call

    template<typename U32StringLike>
    inline std::string fmt_taul_charset(const U32StringLike& x) {
        return fmt_taul_charset(std::u32string_view(x));
    }



    // TODO: in_charset_str has not been unit tested

    // NOTE: while not *really* needed, in_charset_str is needed for unit tests

    // in_charset_str returns if x exists in charset

    // behaviour is undefined if charset is not a valid charset string

    bool in_charset_str(unicode_t x, std::u32string_view charset) noexcept;


    namespace internal {


        bool in_char_range(unicode_t x, unicode_t low, unicode_t high) noexcept;

        // this does in_charset_str, but returns the index of the char pair
        // in the charset which matched, or charset length upon failure

        std::size_t where_in_charset_str(unicode_t x, std::u32string_view charset) noexcept;
    }
}

