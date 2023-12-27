

#pragma once


#include "../logger.h"
#include "../spec_error.h"


namespace taul::internal {


    struct for_internal_use_tag final {};


    template<typename... Args>
    inline void raise_spec_error(
        spec_error_counter* ec,
        spec_error err,
        const std::shared_ptr<taul::logger>& lgr,
        std::format_string<Args...> fmt,
        Args&&... args) {
        if (ec) {
            ec->raise(err);
        }
        TAUL_LOG(lgr, "TAUL error: ({}) {}", err, std::format(fmt, std::forward<Args&&>(args)...));
    }
}

