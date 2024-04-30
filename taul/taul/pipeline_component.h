

#pragma once


#include "api_component.h"


namespace taul {


    // our API operates via the end-user building pull-based pipelines of
    // components which together form a parsing system

    // pipeline_component is the abstract base class of these components

    // pipeline_component is only for the pull-based components of the
    // pipeline, w/ push-based portions like observers and listeners
    // not being derived from pipeline_component


    // one thing to note is that in our API, when two pipeline components,
    // or other components like observers/listeners are *bound* to one
    // another, we are to use raw pointers in order to give the end-user
    // more control over heap usage (ie. not forcing multiple heap allocs)


    class pipeline_component : public api_component {
    public:

        pipeline_component(std::shared_ptr<logger> lgr = nullptr);

        virtual ~pipeline_component() noexcept = default;


        // reset is used to reset the usage state of the pipeline, allowing
        // for reuse of the pipeline

        // reset calls do not reinitialize all aspects of the pipeline's
        // components, only state relating to pipeline usage

        // reset calls are expected to propagate upwards, so to reset the
        // entire pipeline, reset must be called on the downstream-most
        // component of the pipeline

        // after reconfiguring main pipeline components, such as changing
        // the upstream stream of a component, the state of the pipeline
        // is invalidated until the pipeline is reset

        virtual void reset() = 0;
    };
}

