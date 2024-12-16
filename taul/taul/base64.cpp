

#include "base64.h"


std::string taul::encode_base64(std::span<const uint8_t> x) {
    return internal::base64_encode_text(x).value();
}

std::optional<std::vector<uint8_t>> taul::decode_base64(std::string_view x) {
    return internal::base64_decode_text(x);
}

std::optional<taul::internal::base64_encoded_unit> taul::internal::base64_encode_unit(base64_decoded_unit x) noexcept {
    if (x.count > 3) return std::nullopt; // check if x is valid

    // discard unneeded data in x so the below binary math can safely 
    // presume the excess terms are all zero bits
    if (x.count < 2) x.data[1] = 0;
    if (x.count < 3) x.data[2] = 0;

    base64_encoded_unit r{};

    // build the array of four 6-bit values describing the 24-bits of Base64 data
    // 
    // during this process also resolve r's count value
    uint8_t _data[4]{};

    _data[0] = x.data[0] >> 2;
    _data[1] = ((x.data[0] & (uint8_t)0b00000011U) << 4) | (x.data[1] >> 4);

    r.count = 2;

    if (x.count >= 2) {
        _data[2] = ((x.data[1] & (uint8_t)0b00001111U) << 2) | (x.data[2] >> 6);
        r.count++;
    }
    else _data[2] = 0;

    if (x.count >= 3) {
        _data[3] = x.data[2] & (uint8_t)0b00111111U;
        r.count++;
    }
    else _data[3] = 0;

    // finish up by building the final character array of r, translating _data's 6-bit index values
    //
    // here we also add padding to the unused array values
    for (size_t i = 0; i < 4; i++) {
        r.data[i] =
            i < r.count
            ? base64_index_to_char(_data[i]).value()
            : '=';
    }

    return r;
}

std::optional<taul::internal::base64_decoded_unit> taul::internal::base64_decode_unit(base64_encoded_unit x) noexcept {
    if (x.count < 2 || x.count > 4) return std::nullopt; // check if x is valid

    base64_decoded_unit r{};

    // translate x from an array of four Base64 characters to an array of 
    // four 6-bit Base64 indices
    //
    // along the way we resolve r's final count
    //
    // we also resolve the unused terms to zero, so the binary math below
    // can safely presume that said terms will be composed solely of zero bits
    uint8_t _data[4]{};

    constexpr uint8_t err = 64;

    _data[0] = base64_char_to_index(x.data[0]).value_or(err);
    _data[1] = base64_char_to_index(x.data[1]).value_or(err);
    if (_data[0] == err) return std::nullopt;
    if (_data[1] == err) return std::nullopt;

    r.count = 1;

    if (x.count >= 3 && x.data[2] != '=') {
        _data[2] = base64_char_to_index(x.data[2]).value_or(err);
        if (_data[2] == err) return std::nullopt;
        r.count++;
    }
    else _data[2] = 0;

    if (x.count >= 4 && x.data[3] != '=') {
        _data[3] = base64_char_to_index(x.data[3]).value_or(err);
        if (_data[3] == err) return std::nullopt;
        r.count++;
    }
    else _data[3] = 0;

    // use the above resolved information to resolve our final r data entries
    //
    // this takes advantage of the above presumption of unused terms being composed
    // solely of zero bits to avoid having to do *count tests* like we do above,
    // meaning that if the math below resolves r.data[1] or r.data[2] in a scenario
    // where they shouldn't be, it's fine since they'll just be doing binary math
    // on all-zero values, and thus come to an all-zero conclusion
    r.data[0] = (_data[0] << 2) | (_data[1] >> 4);
    r.data[1] = ((_data[1] & (uint8_t)0b00001111U) << 4) | (_data[2] >> 2);
    r.data[2] = ((_data[2] & (uint8_t)0b00000011U) << 6) | _data[3];

    return r;
}

std::optional<std::string> taul::internal::base64_encode_text(std::span<const uint8_t> x) {
    std::string r{};
    size_t bytes_encoded = 0;
    while (bytes_encoded < x.size_bytes()) {
        // prepare du
        base64_decoded_unit du{};
        du.count = std::min<size_t>(3, x.size_bytes() - bytes_encoded);
        std::copy_n(x.begin() + bytes_encoded, du.count, &du.data[0]);
        // encode du into eu
        if (auto eu = base64_encode_unit(du)) {
            // grow r, and copy data from eu to new memory on end of r
            r.resize(r.length() + eu->count);
            std::copy_n(&(eu->data)[0], eu->count, r.begin() + (r.length() - eu->count));
            // add '=' padding
            for (size_t i = 0; i < 4 - eu->count; i++) {
                r += '=';
            }
        }
        else return std::nullopt; // fail
        bytes_encoded += du.count; // count encoded bytes
    }
    return r;
}

std::optional<std::vector<uint8_t>> taul::internal::base64_decode_text(std::string_view x) {
    std::vector<uint8_t> r{};
    size_t chars_decoded = 0;
    while (chars_decoded < x.length()) {
        // prepare eu
        base64_encoded_unit eu{};
        eu.count = std::min<size_t>(4, x.length() - chars_decoded);
        std::copy_n(x.begin() + chars_decoded, eu.count, &eu.data[0]);
        // decode eu into du
        if (auto du = base64_decode_unit(eu)) {
            // grow r, and copy data from du to new memory on end of r
            r.resize(r.size() + du->count);
            std::copy_n(&(du->data)[0], du->count, r.begin() + (r.size() - du->count));
        }
        else return std::nullopt; // fail
        chars_decoded += eu.count; // count decoded chars
    }
    return r;
}

