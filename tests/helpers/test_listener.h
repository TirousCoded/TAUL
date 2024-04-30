

#pragma once


#include <string>
#include <format>

#include <taul/listener.h>


class test_listener final : public taul::listener {
public:

    std::string output;


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

    inline void on_abort(taul::source_pos pos) override {
        output += std::format("\non_abort({})", pos);
    }
};

