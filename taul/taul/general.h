

#pragma once


#include <type_traits>


namespace taul {


    // IMPORTANT: don't forget to keep this up-to-date, lol

    constexpr auto api_version = "version 0.1 (beta)"; // TAUL's *official* version

    
    template<typename T>
    concept trivially_copyable_type = std::is_trivially_copyable_v<T>;
}

