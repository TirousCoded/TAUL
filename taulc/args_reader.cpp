

#include "args_reader.h"

#include <taul/all.h>


args_reader::args_reader(int32_t argc, const char** argv)
    : _args(_generate_args(argc, argv)) {}

std::optional<std::string> args_reader::next() {
    if (_index >= _args.size()) return std::nullopt;
    return std::make_optional(_args[_index++]);
}

std::optional<std::string> args_reader::peek() {
    if (_index >= _args.size()) return std::nullopt;
    return std::make_optional(_args[_index]);
}

std::optional<std::string> args_reader::expect(const std::string& x) {
    if (_index >= _args.size()) return std::nullopt;
    if (_args[_index] != x) return std::nullopt;
    return std::make_optional(_args[_index++]);
}

std::vector<std::string> args_reader::_generate_args(int32_t argc, const char** argv) {
    std::vector<std::string> result{};
    for (const auto& I : std::span<const char*>(argv, argc)) {
        result.push_back(std::string(I));
    }
    TAUL_ASSERT(result.size() >= 1); // <- shouldn't happen unless OS is broken, lol
    return result;
}
