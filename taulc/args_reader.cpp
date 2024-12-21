

#include "args_reader.h"

#include <taul/all.h>


args_reader::args_reader(int32_t argc, const char** argv)
    : _args(_generate_args(argc, argv)) {}

std::optional<std::string> args_reader::next() {
    if (const auto result = peek()) {
        _index++;
        return result;
    }
    return std::nullopt;
}

std::optional<std::string> args_reader::peek() {
    if (_index >= _args.size()) return std::nullopt;
    return _args[_index];
}

bool args_reader::expect(std::string_view x) {
    if (peek() != x) return false;
    _index++;
    return true;
}

bool args_reader::expect(std::initializer_list<std::string_view> x) {
    for (const auto& I : x) {
        if (expect(I)) return true;
    }
    return false;
}

std::optional<std::string> args_reader::expect_ext(std::string_view x) {
    if (
        const auto arg = peek();
        arg &&
        std::string_view(*arg).substr(0, x.length()) == x &&
        std::string_view(*arg).substr(x.length(), 1) == "=") {
        _index++;
        return arg->substr(x.length() + 1);
    }
    return std::nullopt;
}

std::optional<std::string> args_reader::expect_ext(std::initializer_list<std::string_view> x) {
    for (const auto& I : x) {
        if (const auto result = expect_ext(I)) return result;
    }
    return std::nullopt;
}

std::vector<std::string> args_reader::_generate_args(int32_t argc, const char** argv) {
    std::vector<std::string> result{};
    for (const auto& I : std::span<const char*>(argv, argc)) {
        result.push_back(std::string(I));
    }
    TAUL_ASSERT(result.size() >= 1); // <- shouldn't happen unless OS is broken, lol
    return result;
}

