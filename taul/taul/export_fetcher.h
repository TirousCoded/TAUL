

#pragma once


#include <optional>
#include <mutex>

#include "source_code.h"
#include "grammar.h"


namespace taul {


    // TODO: this function has not been unit tested (I just *eyeballed* it)

    // export creates a source code object encapsulating a C++ header file defining
    // a 'fetcher' function which returns a copy of the grammar

    source_code export_fetcher(
        const grammar& gram,
        const char* fetcher,
        const char* taul_include_path = "taul",
        bool taul_includes_use_triangle_brackets = false,
        const char* tab = "    ");


    namespace internal {
        template<typename T>
        concept export_fetcher_singleton_traits =
            requires
        {
            { T::get_serial_str() } noexcept -> std::convertible_to<std::string_view>;
        };

        template<export_fetcher_singleton_traits Traits>
        class export_fetcher_singleton final {
        public:
            export_fetcher_singleton() = default;


            static inline grammar fetch() {
                if (!_gram) {
                    std::scoped_lock lock(_mtx);
                    _gram = grammar::deserialize(Traits::get_serial_str());
                }
                return deref_assert(_gram);
            }


        private:
            static std::optional<grammar> _gram;
            static std::mutex _mtx;
        };

        template<export_fetcher_singleton_traits Traits>
        std::optional<grammar> export_fetcher_singleton<Traits>::_gram = std::nullopt;
        template<export_fetcher_singleton_traits Traits>
        std::mutex export_fetcher_singleton<Traits>::_mtx = std::mutex();
    }
}

