

#pragma once


#include <string>
#include <format>

#include <taul/error_handler.h>


class test_error_handler_for_input_recording final : public taul::error_handler {
public:

    std::string output;

    taul::error_handler* forward_to = nullptr;


    test_error_handler_for_input_recording(taul::error_handler* forward_to = nullptr) 
        : forward_to(forward_to) {}


    void startup(taul::base_parser* client) override final {
        output += std::format("\nstartup({})", uintptr_t(client));
        if (forward_to) forward_to->startup(client);
    }
    void shutdown() override final {
        output += "\nshutdown()";
        if (forward_to) forward_to->shutdown();
    }
    void terminal_error(taul::token_range ids, taul::token input) override final {
        output += std::format("\nterminal_error({}, {})", ids, input);
        if (forward_to) forward_to->terminal_error(ids, input);
    }
    void nonterminal_error(taul::symbol_id id, taul::token input) override final {
        output += std::format("\nnonterminal_error({}, {})", id, input);
        if (forward_to) forward_to->nonterminal_error(id, input);
    }
};

