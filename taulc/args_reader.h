

#pragma once


#include <cstdint>
#include <string>
#include <vector>
#include <span>
#include <optional>


// this 'args_reader' abstracts the process of evaluating the driver's args in
// the expected CLI-like manner


class args_reader final {
public:

    args_reader(int32_t argc, const char** argv);


    // fetch next arg string, if any, unconditionally, w/ advancing reader

    std::optional<std::string> next();

    // fetch next arg string, if any, unconditionally, w/out advancing reader

    std::optional<std::string> peek();

    // checks if next arg string, if any, is x, w/ advancing reader

    bool expect(std::string_view x);
    bool expect(std::initializer_list<std::string_view> x);

    // extends 'expect' w/ the requirement for a '=<value>' part following string
    // x to exist, returning <value> found, if any

    std::optional<std::string> expect_ext(std::string_view x);
    std::optional<std::string> expect_ext(std::initializer_list<std::string_view> x);


private:

    std::vector<std::string> _args;
    size_t _index = 1; // <- 1 skips the first arg describing the process's environment


    static std::vector<std::string> _generate_args(int32_t argc, const char** argv);
};

