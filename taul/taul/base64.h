

#pragma once


#include <cstdint>
#include <string>
#include <vector>
#include <span>
#include <optional>
#include <concepts>


namespace taul {


    // NOTE: in our unit tests, the below overloads get conflated w/ one
    //       another, w/ them being effectively tested *as a unit*


    std::string encode_base64(std::span<const uint8_t> x);

    template<std::convertible_to<std::span<const uint8_t>> T>
    inline std::string encode_base64(const T& x) {
        return encode_base64(std::span<const uint8_t>(x));
    }

    inline std::string encode_base64(std::string_view x) {
        return encode_base64(std::span((const uint8_t*)x.data(), x.length()));
    }

    template<std::convertible_to<std::string_view> T>
    inline std::string encode_base64(const T& x) {
        return encode_base64(std::string_view(x));
    }


    std::optional<std::vector<uint8_t>> decode_base64(std::string_view x);

    template<std::convertible_to<std::string_view> T>
    inline std::optional<std::vector<uint8_t>> decode_base64(const T& x) {
        return decode_base64(std::string_view(x));
    }


    namespace internal {
        constexpr auto base64_charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        constexpr std::optional<char> base64_index_to_char(uint8_t x) noexcept {
            return x <= 63 ? std::make_optional(base64_charset[x]) : std::nullopt;
        }
        constexpr std::optional<uint8_t> base64_char_to_index(char x) noexcept {
            if (x >= 'A' && x <= 'Z')       return uint8_t(x) - (uint8_t)'A';
            else if (x >= 'a' && x <= 'z')  return uint8_t(26) + (uint8_t(x) - (uint8_t)'a');
            else if (x >= '0' && x <= '9')  return uint8_t(52) + (uint8_t(x) - (uint8_t)'0');
            else if (x == '+')              return uint8_t(62);
            else if (x == '/')              return uint8_t(63);
            else                            return std::nullopt;
        }

        static_assert(
            []() constexpr -> bool {
                for (size_t i = 0; i < 64; i++) {
                    if (const auto x = base64_char_to_index(base64_charset[i])) {
                        if (x.value() != i) return false;
                    }
                    else return false;
                }
                return true;
            }());

        struct base64_decoded_unit final {
            uint8_t data[3] = {};
            size_t count = 0;
        };
        struct base64_encoded_unit final {
            char data[4] = {};
            size_t count = 0;
        };

        // for base64_decode_unitleading, if two bits of any of the chars of x contain bit data prior
        // to its trailing 6-bits, this data is ignored, and does not cause the process to fail

        std::optional<base64_encoded_unit> base64_encode_unit(base64_decoded_unit x) noexcept;
        std::optional<base64_decoded_unit> base64_decode_unit(base64_encoded_unit x) noexcept;

        std::optional<std::string> base64_encode_text(std::span<const uint8_t> x);
        std::optional<std::vector<uint8_t>> base64_decode_text(std::string_view x);
    }
}

