

#pragma once


#include "logger.h"
#include "spec_error.h"
#include "spec.h"
#include "grammar.h"


namespace taul {


    // our unit tests will only test the spec_error_counter overloads, and presume the ones 
    // w/out spec_error_counter simply *forward* to the ones that do

    std::optional<grammar> load(
        const spec& s, 
        const std::shared_ptr<logger>& lgr = nullptr);
    
    std::optional<grammar> load(
        const spec& s, 
        spec_error_counter& ec, 
        const std::shared_ptr<logger>& lgr = nullptr);


    // these wrap usage of taul::compile

    // these have not been unit tested
    /*
    std::optional<grammar> load(
        node_ctx& ctx,
        const std::shared_ptr<source_code>& src,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        node_ctx& ctx,
        const std::shared_ptr<source_code>& src,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        const std::shared_ptr<source_code>& src,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        const std::shared_ptr<source_code>& src,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        node_ctx& ctx,
        const std::string& src,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        node_ctx& ctx,
        const std::string& src,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        const std::string& src,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        const std::string& src,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        node_ctx& ctx,
        const std::filesystem::path& src_path,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        node_ctx& ctx,
        const std::filesystem::path& src_path,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        const std::filesystem::path& src_path,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<grammar> load(
        const std::filesystem::path& src_path,
        const std::shared_ptr<logger>& lgr = nullptr);
    */
}

