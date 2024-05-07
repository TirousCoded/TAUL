

#pragma once


#include "source_code.h"
#include "spec.h"


namespace taul {


    // TODO: this function has not been unit tested (I just *eyeballed* it)

    // export creates a source code object encapsulating a C++ header file defining
    // a 'fetcher' function used to return a grammar loaded by the specified spec.

    source_code export_fetcher(
        const spec& s,
        const char* fetcher_name,
        const char* include_prefix = "taul",
        const char* tab = "    ");
}

