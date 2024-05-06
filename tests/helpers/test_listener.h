

#pragma once


#include <string>
#include <format>

#include <taul/listener.h>


class test_listener final : public taul::listener {
public:

    std::string output;

    // since they don't participate in playback, we'll use counters for errors instead

    size_t terminal_errors = 0, nonterminal_errors = 0;


    inline void on_startup() override {
        output += "\non_startup()";
    }
    
    inline void on_shutdown() override {
        output += "\non_shutdown()";
    }
    
    inline void on_lexical(taul::token tkn) override {
        output += std::format("\non_lexical({})", tkn);
    }
    
    inline void on_syntactic(taul::ppr_ref ppr, taul::source_pos pos) override {
        output += std::format("\non_syntactic({}, {})", ppr, pos);
    }
    
    inline void on_close() override {
        output += "\non_close()";
    }

    inline void on_abort() override {
        output += std::format("\non_abort()");
    }

    inline void on_terminal_error(taul::token_range ids, taul::token input) override {
        terminal_errors++;
    }

    inline void on_nonterminal_error(taul::symbol_id id, taul::token input) override {
        nonterminal_errors++;
    }
};

