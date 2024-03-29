

#include "lexer_pat.h"

#include "../asserts.h"
#include "../string_and_charset.h"


taul::token taul::internal::pat_lexer_fn(
    const std::shared_ptr<lexer_state>& state,
    std::string_view txt,
    source_pos offset,
    const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(state);
    const auto toplevel = static_cast<toplevel_lexer_pat*>(state.get());
    TAUL_ASSERT(toplevel);
    TAUL_ASSERT(toplevel && toplevel->gramdat);
    taul::token result{};
    if (toplevel && toplevel->gramdat) {
        const auto& lpr = toplevel->gramdat->_lprs[toplevel->lprInd];
        const auto matched = toplevel->eval(*(toplevel->gramdat), txt, offset, source_pos(-1), lgr);
        result = 
            matched.success 
            ? taul::token(lpr, matched.txt, matched.offset) 
            : taul::token::failure("", matched.offset);
    }
    else TAUL_DEADEND;
    return result;
}

taul::internal::lexer_pat::lexer_pat()
    : lexer_state() {}

taul::internal::toplevel_lexer_pat::toplevel_lexer_pat(std::size_t lprInd)
    : lexer_pat(),
    lprInd(lprInd),
    gramdat(nullptr) {}

taul::internal::lexer_match taul::internal::toplevel_lexer_pat::eval(const grammar_data& gramdat,  std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_IN_BOUNDS(lprInd, 0, gramdat._lprs.size());
    std::uint32_t len = 0;
    bool success = true;
    for (const auto& I : children) {
        TAUL_ASSERT(I);
        const auto matched = I->eval(gramdat, txt, offset + len, localend_offset, lgr);
        if (matched.success) {
            len += (std::uint32_t)matched.txt.length();
        }
        else {
            success = false;
            break;
        }
    }
    return
        success
        ? make_lexer_match(true, txt.substr(offset, len), offset)
        : make_lexer_match(false, "", offset);
}

taul::internal::begin_lexer_pat::begin_lexer_pat()
    : lexer_pat() {}

taul::internal::lexer_match taul::internal::begin_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.empty());
    return make_lexer_match(offset == 0, "", offset);
}

taul::internal::end_lexer_pat::end_lexer_pat()
    : lexer_pat() {}

taul::internal::lexer_match taul::internal::end_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.empty());
    return make_lexer_match(offset == txt.size(), "", offset);
}

taul::internal::any_lexer_pat::any_lexer_pat()
    : lexer_pat() {}

taul::internal::lexer_match taul::internal::any_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.empty());
    const auto _substr = txt.substr(offset, 1);
    return
        !_substr.empty()
        ? make_lexer_match(true, _substr, offset)
        : make_lexer_match(false, "", offset);
}

taul::internal::string_lexer_pat::string_lexer_pat(std::string s)
    : lexer_pat(), 
    s(std::move(s)) {}

taul::internal::lexer_match taul::internal::string_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.empty());
    const auto _substr = txt.substr(offset, s.size());
    return
        _substr == std::string_view(s)
        ? make_lexer_match(true, _substr, offset)
        : make_lexer_match(false, "", offset);
}

taul::internal::charset_lexer_pat::charset_lexer_pat(std::string s)
    : lexer_pat(),
    s(std::move(s)) {}

taul::internal::lexer_match taul::internal::charset_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.empty());
    const auto _substr = txt.substr(offset, 1);
    bool success = false;
    if (!_substr.empty()) {
        success = in_charset_str(_substr[0], s);
    }
    return
        success
        ? make_lexer_match(true, _substr, offset)
        : make_lexer_match(false, "", offset);
}

taul::internal::sequence_lexer_pat::sequence_lexer_pat()
    : lexer_pat() {}

taul::internal::range_lexer_pat::range_lexer_pat(char a, char b)
    : a(a),
    b(b) {}

taul::internal::lexer_match taul::internal::range_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.empty());
    const auto _substr = txt.substr(offset, 1);
    bool success =
        !_substr.empty() &&
        _in_range(_substr[0]);
    return make_lexer_match(success, _substr, offset);
}

bool taul::internal::range_lexer_pat::_in_range(char x) const noexcept {
    return
        a <= b
        ? ((x >= a) && (x <= b))
        : ((x >= b) && (x <= a));
}

taul::internal::name_lexer_pat::name_lexer_pat(std::size_t lprIndOfRef)
    : lexer_pat(),
    lprIndOfRef(lprIndOfRef) {}

taul::internal::lexer_match taul::internal::name_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(lprIndOfRef < gramdat._lprs.size());
    const auto tkn = gramdat._lprs[lprIndOfRef].fnobj(txt, offset, lgr);
    return
        (bool)tkn
        ? make_lexer_match(true, tkn.str(), offset)
        : make_lexer_match(false, "", offset);
}

taul::internal::lexer_match taul::internal::sequence_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    std::uint32_t len = 0;
    bool success = true;
    for (const auto& I : children) {
        TAUL_ASSERT(I);
        const auto matched = I->eval(gramdat, txt, offset + len, localend_offset, lgr);
        if (matched.success) {
            len += (std::uint32_t)matched.txt.length();
        }
        else {
            success = false;
            break;
        }
    }
    return
        success
        ? make_lexer_match(true, txt.substr(offset, len), offset)
        : make_lexer_match(false, "", offset);
}

taul::internal::set_lexer_pat::set_lexer_pat()
    : lexer_pat() {}

taul::internal::lexer_match taul::internal::set_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    lexer_match selection = make_lexer_match(false, "", offset);
    // TODO: could we make bias::last run *faster* by iterating backwards? or would increase be too small?
    for (const auto& I : children) {
        TAUL_ASSERT(I);
        const auto matched = I->eval(gramdat, txt, offset, localend_offset, lgr);
        // select first successful match no matter what
        if (matched.success) {
            selection = matched;
            break;
        }
    }
    return selection;
}

taul::internal::not_lexer_pat::not_lexer_pat() 
    : lexer_pat() {}

taul::internal::lexer_match taul::internal::not_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.size() == 1);
    const auto matched = children.back()->eval(gramdat, txt, offset, localend_offset, lgr);
    const bool success = 
        !matched.success && 
        !txt.substr(offset).empty();
    return
        success
        ? make_lexer_match(true, txt.substr(offset, 1), offset)
        : make_lexer_match(false, "", offset);
}

taul::internal::modifier_lexer_pat::modifier_lexer_pat(std::uint16_t min, std::uint16_t max) 
    : lexer_pat(), 
    min(min), 
    max(max) {}

taul::internal::lexer_match taul::internal::modifier_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.size() == 1);
    const auto has_max = max > 0;
    std::uint32_t len = 0;
    std::size_t n = 0;
    bool success = false;
    while (true) {
        const auto matched = children.back()->eval(gramdat, txt, offset + len, localend_offset, lgr);
        if (matched.success) {
            len += (std::uint32_t)matched.txt.length();
            n++;
            if (has_max && n == max) {
                break;
            }
        }
        else break;
    }
    if (n >= min) {
        success = true;
    }
    return
        success
        ? make_lexer_match(true, txt.substr(offset, len), offset)
        : make_lexer_match(false, "", offset);
}

taul::internal::assertion_lexer_pat::assertion_lexer_pat(bool positive)
    : lexer_pat(),
    positive(positive) {}

taul::internal::lexer_match taul::internal::assertion_lexer_pat::eval(const grammar_data& gramdat, std::string_view txt, source_pos offset, const source_pos localend_offset, const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.size() == 1);
    const auto matched = children.back()->eval(gramdat, txt, offset, localend_offset, lgr);
    const bool success =
        positive 
        ? matched.success
        : !matched.success;
    return make_lexer_match(success, "", offset);
}

