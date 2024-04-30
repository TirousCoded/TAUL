

#pragma once


#include "logger.h"


namespace taul {


    // this is the base class of all frontend API components


    class api_component {
    public:

        // this being immutable makes it easier to propagate lgr down to
        // subcomponents w/out having to worry about toplevel lgr changing

        const std::shared_ptr<logger> lgr;


        api_component(std::shared_ptr<logger> lgr = nullptr);

        virtual ~api_component() noexcept = default;
    };
}

