

#include "taul_gram.h"

#include "taul_spec.h"
#include "load.h"


// TODO: bootstrap taul.taul later and replace below w/ call to fetcher
//
//       then, we'll commit/push srcgen fetcher code to github, and we
//       can use this code as a backup if we ever break something

taul::grammar taul::taul_gram() {
    return load(taul_spec()).value();
}

