

#include "grammar_data.h"


void taul::internal::grammar_data::build_lookup() {
    TAUL_ASSERT(_lookup.empty());
    for (const auto& I : _lprs) {
        _lookup[I.name] = entry{ true, I.index };
    }
    for (const auto& I : _pprs) {
        _lookup[I.name] = entry{ false, I.index };
    }
}

std::string taul::internal::lexer_rule::fmt() const {
    return
        qualifier != qualifier::none
        ? std::format("[{0}] {1} ({2})", index, name, qualifier)
        : std::format("[{0}] {1}", index, name);
}

std::string taul::internal::parser_rule::fmt() const {
    return
        std::format("[{0}] {1}", index, name);
}
