#pragma once
#include <string_view>
#include <optional>
#include <taul/grammar.h>
#include <taul/export_fetcher.h>

namespace taul::fetchers {
    namespace details {
        struct abc_singleton_traits final {
            static constexpr std::string_view get_serial_str() noexcept {
                return "TAULAQAAAAAAAAAFAAAAAAAAAAIAAAAAAAAAAQAAAEEAAQAAAEIAAQAAAEMAAQAAAEQAAgAAAEpQAAUAAABTdGFydAADAAAAQUJDAAUAAAAAAAAAAQARAAEAAAAAAAAAAGEAAABhAAAAAAIAEQABAAAAAAAAAABiAAAAYgAAAAADABEAAQAAAAAAAAAAYwAAAGMAAAAABAARAAEAAAAAAAAAAGQAAABkAAAAAAUAEQAFAAAAAAAAAABDUQAAQ1EAAAAAF2wAABdsAAAAAGcwAABnMAAAAABZMAAAWTAAAAAASzAAAEswAAAACAAAAAAAAAADABIAAQAAAAAAAAABBQASAP////8FABIAAgAAAAAAAAABBwASAP////8BBgASAP////8GABIAAAAAAAAAAAAGABIAAgAAAAAAAAABBwASAP////8BBgASAP////8HABIAAQAAAAAAAAABBAASAAAAAAAEABIAAwAAAAAAAAAAAQARAAEAEQAAAQgAEgD/////AAMAEQADABEAAAgAEgABAAAAAAAAAAACABEAAgARAAAIABIAAQAAAAAAAAAABAARAAQAEQAA";
            }
        };
    }
    inline taul::grammar abc() {
        return taul::internal::export_fetcher_singleton<details::abc_singleton_traits>::fetch();
    }
}