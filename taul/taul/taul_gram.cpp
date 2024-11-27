

#include "taul_gram.h"

#define _TAUL_USE_OLD_TAUL_SPEC 0

#if _TAUL_USE_OLD_TAUL_SPEC
#include "load.h"
#include "internal/taul_spec.h"
#else
#include "internal/srcgen/taul-gram.h"
#endif


// TODO: at some point, maybe add a preprocessor definition-based branch for letting us
//       load TAUL grammar dynamically from taul.taul, w/ this replacing us using our
//       old internal::taul_spec function, which can be replaced 100% at this point

// TODO: when we regenerate taul-gram.h take into account that the '#include <taul/~.h>'
//       bits will likely be invalid, and need to be edited manually
//
//       at some point we'd be best to find a way to have taulc account for this for us


taul::grammar taul::taul_gram() {
#if _TAUL_USE_OLD_TAUL_SPEC
    return load(internal::taul_spec()).value();
#else
    return taul::fetchers::taul();
#endif
}

