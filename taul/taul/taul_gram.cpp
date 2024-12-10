

#include "taul_gram.h"

#include "internal/srcgen/taul-gram.h"


// NOTE: I tried using a preprocessor directive to let us switch to loading TAUL's
//       grammar dynamically by compiling taul.taul, but I ran into an issue where
//       I got infinite recursion due to taul::load relying on taul::taul_gram

// TODO: when we regenerate taul-gram.h take into account that the '#include <taul/~.h>'
//       bits will likely be invalid, and need to be edited manually
//
//       at some point we'd be best to find a way to have taulc account for this for us


taul::grammar taul::taul_gram() {
    return taul::fetchers::taul();
}

