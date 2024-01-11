

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


    // returns 16 if char is not hex digit

    template<typename Char>
    inline std::size_t hex_digit(Char x) noexcept {
        switch (x) {
        case Char('0'): return 0;   break;
        case Char('1'): return 1;   break;
        case Char('2'): return 2;   break;
        case Char('3'): return 3;   break;
        case Char('4'): return 4;   break;
        case Char('5'): return 5;   break;
        case Char('6'): return 6;   break;
        case Char('7'): return 7;   break;
        case Char('8'): return 8;   break;
        case Char('9'): return 9;   break;
        case Char('a'): return 10;  break;
        case Char('b'): return 11;  break;
        case Char('c'): return 12;  break;
        case Char('d'): return 13;  break;
        case Char('e'): return 14;  break;
        case Char('f'): return 15;  break;
        case Char('A'): return 10;  break;
        case Char('B'): return 11;  break;
        case Char('C'): return 12;  break;
        case Char('D'): return 13;  break;
        case Char('E'): return 14;  break;
        case Char('F'): return 15;  break;
        default:        return 16;  break;
        }
    }
}

