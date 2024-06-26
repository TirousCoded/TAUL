

#pragma once


#include "logger.h"
#include "str.h"
#include "source_code.h"
#include "spec_error.h"
#include "spec.h"


namespace taul {


    // these taul::compile functions are used for syntax compilation,
    // producing taul::spec objects, which are then loaded

    // overloads accepting a taul::source_code shared-pointer will fail
    // if their src argument is nullptr, raising a source-code-not-found
    // spec error

    // overloads which accept an std::filesystem::path const-lvalue 
    // reference will fail if their specified file could not be loaded, 
    // raising a source-code-not-found spec error

    // the ec parameters are used for reporting pre-loading errors,
    // such as syntax errors


    // only this first taul::compile function will be unit tested, 
    // w/ all other overloads being expected to be straight-forward 
    // usages of this first one, w/ some extra behaviour which can be
    // presumed to be handled via things like taul::source_code

    // the overloads which load source code from a file will also be
    // unit tested, but only w/ regards to their raising of a spec
    // error if their specified file could not be found

    // the behaviour of debug symbol 'pos' instruction generation has
    // not been unit tested, and is disabled in our unit tests


    std::optional<spec> compile(
        const std::shared_ptr<source_code>& src,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr,
        bool dbgsyms = true);

    std::optional<spec> compile(
        const std::shared_ptr<source_code>& src,
        const std::shared_ptr<logger>& lgr = nullptr,
        bool dbgsyms = true);

    std::optional<spec> compile(
        const str& src,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr,
        bool dbgsyms = true);

    std::optional<spec> compile(
        const str& src,
        const std::shared_ptr<logger>& lgr = nullptr,
        bool dbgsyms = true);

    std::optional<spec> compile(
        const std::filesystem::path& src_path,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr,
        bool dbgsyms = true);

    std::optional<spec> compile(
        const std::filesystem::path& src_path,
        const std::shared_ptr<logger>& lgr = nullptr,
        bool dbgsyms = true);
}

