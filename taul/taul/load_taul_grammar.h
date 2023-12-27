

#pragma once


#include "logger.h"
#include "grammar.h"


namespace taul {


    // this is a special load function specifically for loading the grammar of the
    // TAUL language itself, which is expected to always succeed

    grammar load_taul_grammar(const std::shared_ptr<logger>& lgr = nullptr);
}

