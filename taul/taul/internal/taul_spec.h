

#pragma once


#include "../spec.h"


namespace taul::internal {


    // TODO: taul_spec is the original, hand written, TAUL spec which was used to
    //       create the TAUL compiler which later bootstrapped taul.taul
    //
    //       taul_spec is obsolete at this point, and can be gotten rid of, but hasn't
    //       been mainly due to me not caring enough to actually do it, lol


    // taul_spec returns the spec for the TAUL grammar

    spec taul_spec();
}

