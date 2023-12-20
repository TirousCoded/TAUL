

#include "parser_pat.h"

#include "../asserts.h"


taul::source_pos taul::internal::map_tkn_to_src_pos(
    const taul::token_seq& tkns,
    std::size_t offset) noexcept {
    TAUL_IN_BOUNDS(offset, 0, tkns.size() + 1);
    if (tkns.empty()) {
        TAUL_ASSERT(offset == 0);
        return 0;
    }
    else if (offset == tkns.size()) {
        TAUL_ASSERT(tkns.has_size());
        return tkns.back().pos() + (source_pos)tkns.back().str().length();
    }
    else {
        return tkns[offset].pos();
    }
}

taul::node taul::internal::create_node(
    node_ctx& ctx) {
    // this creates a *dummy* node
    // perform_eval uses this node as the *context* of its execution
    // finish_node then *patches* the node's state, producing the 
    // end result of the process as a whole
    return ctx.create(taul::token{});
}

taul::internal::parser_match taul::internal::perform_eval(
    taul::node& nd,
    toplevel_parser_pat* toplevel, 
    node_ctx& ctx, 
    const taul::token_seq& tkns, 
    std::size_t offset, 
    const std::shared_ptr<taul::logger>& lgr) {
    if (toplevel && toplevel->gramdat) {
        const auto& gramdat = *(toplevel->gramdat);
        // just forward to underlying eval call; simple enough
        return
            toplevel->eval(
                ctx,
                nd,
                gramdat,
                tkns,
                offset,
                std::size_t(-1),
                lgr);
    }
    else TAUL_DEADEND;
    return parser_match{}; // this should never be reached
}

void taul::internal::finish_node(
    taul::node& nd,
    toplevel_parser_pat* toplevel,
    const parser_match& match,
    const taul::token_seq& tkns) {
    if (toplevel && toplevel->gramdat) {
        const auto& ppr = toplevel->gramdat->_pprs[toplevel->pprInd];
        if (match.success) {
            // find first position of source string section of our match
            source_pos first_pos = 
                map_tkn_to_src_pos(
                    tkns, 
                    match.offset);
            // find last position of source string section of our match
            source_pos final_pos =
                match.tkns.empty()
                ? first_pos
                : match.tkns.back().pos() + (source_pos)match.tkns.back().str().length();
            // resolve string view of source string section of our match
            std::string_view matched_txt =
                tkns.str().substr(
                    first_pos,
                    final_pos - first_pos);
            // perform patching of our *dummy* node
            taul::internal::patch_node(
                nd,
                ppr,
                matched_txt,
                first_pos);
        }
    }
    else TAUL_DEADEND;
}

std::optional<taul::node> taul::internal::pat_parser_fn(
    node_ctx& ctx, 
    const std::shared_ptr<parser_state>& state, 
    const taul::token_seq& tkns,
    std::size_t offset, 
    const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(state);
    const auto toplevel = static_cast<toplevel_parser_pat*>(state.get());
    taul::node nd = 
        create_node(
            ctx);
    const auto match = 
        perform_eval(
            nd, 
            toplevel, 
            ctx, 
            tkns, 
            offset, 
            lgr);
    finish_node(
        nd, 
        toplevel, 
        match, 
        tkns);
    return 
        match.success 
        ? std::make_optional<node>(std::move(nd)) 
        : std::nullopt;
}

taul::internal::parser_pat::parser_pat() 
    : parser_state() {}

taul::internal::toplevel_parser_pat::toplevel_parser_pat(std::size_t pprInd)
    : parser_pat(),
    pprInd(pprInd),
    gramdat(nullptr) {}

taul::internal::parser_match taul::internal::toplevel_parser_pat::eval(
    node_ctx& ctx,
    node& nd,
    const grammar_data& gramdat,
    const taul::token_seq& tkns,
    std::size_t offset,
    const std::size_t localend_offset,
    const std::shared_ptr<taul::logger>& lgr) {
    TAUL_IN_BOUNDS(pprInd, 0, gramdat._pprs.size());
    std::size_t tkns_len = 0;
    node::iterator first{}, last_notpte{};
    bool success = true;
    for (const auto& I : children) {
        TAUL_ASSERT(I);
        const auto matched = I->eval(ctx, nd, gramdat, tkns, offset + tkns_len, localend_offset, lgr);
        if (matched.success) {
            tkns_len += matched.tkns.size();
            if (first == node::iterator{}) {
                first = matched.first;
            }
            // gotta account for if first/last iters are past-the-end (like w/ assertion exprs)
            if (matched.last_notpte != node::iterator{}) {
                last_notpte = matched.last_notpte;
            }
        }
        else {
            success = false;
            break;
        }
    }
    if (success) {
        return
            make_parser_match(
                true,
                std::span<const token>(std::next(tkns.begin(), offset), tkns_len),
                offset,
                first,
                last_notpte);
    }
    else {
        if (first != node::iterator{}) {
            nd.detach_range(first, std::next(last_notpte));
        }
        return
            make_parser_match(
                false,
                std::span<const token>(std::next(tkns.begin(), offset), 0),
                offset,
                node::iterator{},
                node::iterator{});
    }
}

taul::internal::begin_parser_pat::begin_parser_pat() 
    : parser_pat() {}

taul::internal::parser_match taul::internal::begin_parser_pat::eval(
    node_ctx& ctx,
    node& nd,
    const grammar_data& gramdat,
    const taul::token_seq& tkns,
    std::size_t offset,
    const std::size_t localend_offset,
    const std::shared_ptr<taul::logger>& lgr) {
    return 
        make_parser_match(
            offset == 0, 
            std::span<const token>(std::next(tkns.begin(), offset), 0), 
            offset, 
            node::iterator{}, 
            node::iterator{});
}

taul::internal::end_parser_pat::end_parser_pat()
    : parser_pat() {}

taul::internal::parser_match taul::internal::end_parser_pat::eval(
    node_ctx& ctx,
    node& nd,
    const grammar_data& gramdat,
    const taul::token_seq& tkns,
    std::size_t offset,
    const std::size_t localend_offset,
    const std::shared_ptr<taul::logger>& lgr) {
    return
        make_parser_match(
            offset == tkns.size(),
            std::span<const token>(std::next(tkns.begin(), offset), 0),
            offset,
            node::iterator{},
            node::iterator{});
}

taul::internal::any_parser_pat::any_parser_pat() {}

taul::internal::parser_match taul::internal::any_parser_pat::eval(
    node_ctx& ctx,
    node& nd,
    const grammar_data& gramdat,
    const taul::token_seq& tkns,
    std::size_t offset,
    const std::size_t localend_offset,
    const std::shared_ptr<taul::logger>& lgr) {
    if (offset < tkns.size()) {
        nd.attach(ctx.create(tkns[offset]));
        return
            make_parser_match(
                true,
                std::span<const token>(std::next(tkns.begin(), offset), 1),
                offset,
                std::prev(nd.end()),
                std::prev(nd.end()));
    }
    else {
        return
            make_parser_match(
                false,
                std::span<const token>(std::next(tkns.begin(), offset), 0),
                offset,
                node::iterator{},
                node::iterator{});
    }
}

taul::internal::string_parser_pat::string_parser_pat(std::string s) 
    : s(s) {}

taul::internal::parser_match taul::internal::string_parser_pat::eval(
    node_ctx& ctx, 
    node& nd, 
    const grammar_data& gramdat, 
    const taul::token_seq& tkns, 
    std::size_t offset, 
    const std::size_t localend_offset, 
    const std::shared_ptr<taul::logger>& lgr) {
    if (
        offset < tkns.size() && 
        !tkns[offset].is_failure() &&
        tkns[offset].str() == s) {
        nd.attach(ctx.create(tkns[offset]));
        return
            make_parser_match(
                true,
                std::span<const token>(std::next(tkns.begin(), offset), 1),
                offset,
                std::prev(nd.end()),
                std::prev(nd.end()));
    }
    else {
        return
            make_parser_match(
                false,
                std::span<const token>(std::next(tkns.begin(), offset), 0),
                offset,
                node::iterator{},
                node::iterator{});
    }
}

taul::internal::charset_parser_pat::charset_parser_pat(std::string s)
    : s(s) {}

taul::internal::parser_match taul::internal::charset_parser_pat::eval(
    node_ctx& ctx, 
    node& nd, 
    const grammar_data& gramdat, 
    const taul::token_seq& tkns, 
    std::size_t offset, 
    const std::size_t localend_offset, 
    const std::shared_ptr<taul::logger>& lgr) {
    if (
        offset < tkns.size() &&
        !tkns[offset].is_failure() &&
        tkns[offset].str().length() == 1 &&
        [&]() -> bool {
            for (const auto& I : s) {
                if (tkns[offset].str().back() == I) {
                    return true;
                }
            }
            return false;
        }()) {
        nd.attach(ctx.create(tkns[offset]));
        return
            make_parser_match(
                true,
                std::span<const token>(std::next(tkns.begin(), offset), 1),
                offset,
                std::prev(nd.end()),
                std::prev(nd.end()));
    }
    else {
        return
            make_parser_match(
                false,
                std::span<const token>(std::next(tkns.begin(), offset), 0),
                offset,
                node::iterator{},
                node::iterator{});
    }
}

taul::internal::token_parser_pat::token_parser_pat() {}

taul::internal::parser_match taul::internal::token_parser_pat::eval(
    node_ctx& ctx, 
    node& nd, 
    const grammar_data& gramdat, 
    const taul::token_seq& tkns, 
    std::size_t offset, 
    const std::size_t localend_offset, 
    const std::shared_ptr<taul::logger>& lgr) {
    if (
        offset < tkns.size() && 
        !tkns[offset].is_failure()) {
        nd.attach(ctx.create(tkns[offset]));
        return
            make_parser_match(
                true,
                std::span<const token>(std::next(tkns.begin(), offset), 1),
                offset,
                std::prev(nd.end()),
                std::prev(nd.end()));
    }
    else {
        return
            make_parser_match(
                false,
                std::span<const token>(std::next(tkns.begin(), offset), 0),
                offset,
                node::iterator{},
                node::iterator{});
    }
}

taul::internal::failure_parser_pat::failure_parser_pat() {}

taul::internal::parser_match taul::internal::failure_parser_pat::eval(
    node_ctx& ctx, 
    node& nd, 
    const grammar_data& gramdat, 
    const taul::token_seq& tkns, 
    std::size_t offset, 
    const std::size_t localend_offset, 
    const std::shared_ptr<taul::logger>& lgr) {
    if (
        offset < tkns.size() &&
        tkns[offset].is_failure()) {
        nd.attach(ctx.create(tkns[offset]));
        return
            make_parser_match(
                true,
                std::span<const token>(std::next(tkns.begin(), offset), 1),
                offset,
                std::prev(nd.end()),
                std::prev(nd.end()));
    }
    else {
        return
            make_parser_match(
                false,
                std::span<const token>(std::next(tkns.begin(), offset), 0),
                offset,
                node::iterator{},
                node::iterator{});
    }
}

taul::internal::name_parser_pat::name_parser_pat(bool lpr, std::size_t ruleIndOfRef) 
    : lpr(lpr), 
    ruleIndOfRef(ruleIndOfRef) {}

taul::internal::parser_match taul::internal::name_parser_pat::eval(
    node_ctx& ctx, 
    node& nd, 
    const grammar_data& gramdat, 
    const taul::token_seq& tkns, 
    std::size_t offset, 
    const std::size_t localend_offset, 
    const std::shared_ptr<taul::logger>& lgr) {
    std::span<const token> tkns_result{};
    bool success = false;
    if (lpr) {
        // perform this code if we name an lpr
        success =
            offset < tkns.size() &&
            !tkns[offset].is_failure() &&
            tkns[offset].lpr().index == ruleIndOfRef;
        if (success) {
            tkns_result = std::span<const token>(std::next(tkns.begin(), offset), 1);
            nd.attach(ctx.create(tkns[offset]));
        }
    }
    else {
        // perform this code if we name a ppr
        const auto& psr = gramdat._pprs[ruleIndOfRef].parser();
        const auto toplevel = get_parser_state_as<toplevel_parser_pat>(psr);
        TAUL_ASSERT(toplevel);
        taul::node nd0 =
            create_node(
                ctx);
        const auto match =
            perform_eval(
                nd0,
                toplevel,
                ctx,
                tkns,
                offset,
                lgr);
        finish_node(
            nd0,
            toplevel,
            match,
            tkns);
        success = match.success;
        if (success) {
            tkns_result = match.tkns;
            nd.attach(std::move(nd0));
        }
    }
    if (success) {
        return
            make_parser_match(
                true,
                tkns_result,
                offset,
                std::prev(nd.end()),
                std::prev(nd.end()));
    }
    else {
        return
            make_parser_match(
                false,
                std::span<const token>(std::next(tkns.begin(), offset), 0),
                offset,
                node::iterator{},
                node::iterator{});
    }
}

taul::internal::sequence_parser_pat::sequence_parser_pat() {}

taul::internal::parser_match taul::internal::sequence_parser_pat::eval(
    node_ctx& ctx, 
    node& nd, 
    const grammar_data& gramdat, 
    const taul::token_seq& tkns, 
    std::size_t offset, 
    const std::size_t localend_offset, 
    const std::shared_ptr<taul::logger>& lgr) {
    std::size_t tkns_len = 0;
    node::iterator first{}, last_notpte{};
    bool success = true;
    for (const auto& I : children) {
        TAUL_ASSERT(I);
        const auto matched = I->eval(ctx, nd, gramdat, tkns, offset + tkns_len, localend_offset, lgr);
        if (matched.success) {
            tkns_len += matched.tkns.size();
            if (first == node::iterator{}) {
                first = matched.first;
            }
            // gotta account for if first/last iters are past-the-end (like w/ assertion exprs)
            if (matched.last_notpte != node::iterator{}) {
                last_notpte = matched.last_notpte;
            }
        }
        else {
            success = false;
            break;
        }
    }
    if (success) {
        return
            make_parser_match(
                true,
                std::span<const token>(std::next(tkns.begin(), offset), tkns_len),
                offset,
                first,
                last_notpte);
    }
    else {
        if (first != node::iterator{}) {
            nd.detach_range(first, std::next(last_notpte));
        }
        return
            make_parser_match(
                false,
                std::span<const token>(std::next(tkns.begin(), offset), 0),
                offset,
                node::iterator{},
                node::iterator{});
    }
}

taul::internal::set_parser_pat::set_parser_pat(bias b) 
    : b(b) {}

taul::internal::parser_match taul::internal::set_parser_pat::eval(
    node_ctx& ctx,
    node& nd,
    const grammar_data& gramdat,
    const taul::token_seq& tkns,
    std::size_t offset,
    const std::size_t localend_offset,
    const std::shared_ptr<taul::logger>& lgr) {
    parser_match selection = 
        make_parser_match(
            false,
            std::span<const token>(std::next(tkns.begin(), offset), 0),
            offset,
            node::iterator{},
            node::iterator{});
    for (const auto& I : children) {
        TAUL_ASSERT(I);
        parser_match matched =
            I->eval(
                ctx, 
                nd, 
                gramdat, 
                tkns, 
                offset, 
                localend_offset, 
                lgr);
        // skip if fails
        if (!matched.success) {
            continue;
        }
        // select first successful match no matter what
        bool select = !selection.success;
        // otherwise, select if new match is preferrable
        if (!select) {
            switch (b) {
            case bias::first_longest:   select = matched.tkns.size() > selection.tkns.size();   break;
            case bias::first_shortest:  select = matched.tkns.size() < selection.tkns.size();   break;
            case bias::last_longest:    select = matched.tkns.size() >= selection.tkns.size();  break;
            case bias::last_shortest:   select = matched.tkns.size() <= selection.tkns.size();  break;
            case bias::first:           select = false;                                         break;
            case bias::last:            select = true;                                          break;
            default:                    TAUL_DEADEND;                                           break;
            }
        }
        // if above two make select == true, select matched
        if (select) {
            if (selection.first != node::iterator{}) {
                // gotta not forget to call detach_range
                nd.detach_range(
                    selection.first,
                    std::next(selection.last_notpte));
            }
            selection = std::move(matched);
        }
        else {
            if (matched.first != node::iterator{}) {
                // gotta not forget to call detach_range
                nd.detach_range(
                    matched.first,
                    std::next(matched.last_notpte));
            }
        }
        // if our bias is 'first', then we may exit early
        if (select && b == bias::first) {
            break;
        }
    }
    return selection;
}

taul::internal::modifier_parser_pat::modifier_parser_pat(std::uint16_t min, std::uint16_t max)
    : min(min),
    max(max) {}

taul::internal::parser_match taul::internal::modifier_parser_pat::eval(
    node_ctx& ctx,
    node& nd,
    const grammar_data& gramdat,
    const taul::token_seq& tkns,
    std::size_t offset,
    const std::size_t localend_offset,
    const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.size() == 1);
    const auto has_max = max > 0;
    std::size_t tkns_len = 0;
    node::iterator first{}, last_notpte{};
    std::size_t n = 0;
    bool success = false;
    while (true) {
        parser_match matched =
            children.back()->eval(
                ctx, 
                nd, 
                gramdat, 
                tkns, 
                offset + tkns_len, 
                localend_offset, 
                lgr);
        if (matched.success) {
            tkns_len += matched.tkns.size();
            if (first == node::iterator{}) {
                first = matched.first;
            }
            // gotta account for if first/last iters are past-the-end (like w/ assertion exprs)
            if (matched.last_notpte != node::iterator{}) {
                last_notpte = matched.last_notpte;
            }
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
    if (success) {
        return
            make_parser_match(
                true,
                std::span<const token>(std::next(tkns.begin(), offset), tkns_len),
                offset,
                first,
                last_notpte);
    }
    else {
        if (first != node::iterator{}) {
            nd.detach_range(first, std::next(last_notpte));
        }
        return
            make_parser_match(
                false,
                std::span<const token>(std::next(tkns.begin(), offset), 0),
                offset,
                node::iterator{},
                node::iterator{});
    }
}

taul::internal::assertion_parser_pat::assertion_parser_pat(taul::polarity p) 
    : p(p) {}

taul::internal::parser_match taul::internal::assertion_parser_pat::eval(
    node_ctx& ctx,
    node& nd,
    const grammar_data& gramdat,
    const taul::token_seq& tkns,
    std::size_t offset,
    const std::size_t localend_offset,
    const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.size() == 1);
    parser_match matched = 
        children.back()->eval(
            ctx, 
            nd, 
            gramdat, 
            tkns, 
            offset, 
            localend_offset, 
            lgr);
    const bool success =
        p == polarity::positive
        ? matched.success
        : !matched.success;
    if (matched.success) {
        if (matched.first != node::iterator{}) {
            // cleanup if matched succeeded, as we won't need these nodes anyway
            nd.detach_range(matched.begin(), matched.end());
        }
    }
    return
        make_parser_match(
            success,
            std::span<const token>(std::next(tkns.begin(), offset), 0),
            offset,
            node::iterator{},
            node::iterator{});
}

taul::internal::constraint_parser_pat::constraint_parser_pat(taul::polarity p)
    : p(p) {}

taul::internal::parser_match taul::internal::constraint_parser_pat::eval(
    node_ctx& ctx,
    node& nd,
    const grammar_data& gramdat,
    const taul::token_seq& tkns,
    std::size_t offset,
    const std::size_t localend_offset,
    const std::shared_ptr<taul::logger>& lgr) {
    TAUL_ASSERT(children.size() == 2);
    bool success{};
    parser_match constrained = 
        children[0]->eval(
            ctx, 
            nd, 
            gramdat, 
            tkns, 
            offset, 
            localend_offset, 
            lgr);
    if (constrained.success) {
        // putting this in an if-statement so we can skip evaluating it if constrained fails
        parser_match constraining = 
            children[1]->eval(
                ctx,
                nd,
                gramdat,
                tkns,
                offset,
                offset + constrained.tkns.size(),
                lgr);
        success =
            p == polarity::positive
            ? constraining.success
            : !constraining.success;
        if (constraining.success) {
            if (constraining.first != node::iterator{}) {
                // cleanup if matched succeeded, as we won't need these nodes anyway
                nd.detach_range(constraining.begin(), constraining.end());
            }
        }
    }
    else {
        success = false;
    }
    if (success) {
        return constrained;
    }
    else {
        // it's possible failure could be due to constraining expr, so we'll need to do cleanup then
        if (constrained.success) {
            if (constrained.first != node::iterator{}) {
                nd.detach_range(constrained.begin(), constrained.end());
            }
        }
        return
            make_parser_match(
                false,
                std::span<const token>(std::next(tkns.begin(), offset), 0),
                offset,
                node::iterator{},
                node::iterator{});
    }
}

taul::internal::localend_parser_pat::localend_parser_pat() {}

taul::internal::parser_match taul::internal::localend_parser_pat::eval(
    node_ctx& ctx,
    node& nd,
    const grammar_data& gramdat,
    const taul::token_seq& tkns,
    std::size_t offset,
    const std::size_t localend_offset,
    const std::shared_ptr<taul::logger>& lgr) {
    return
        make_parser_match(
            offset == localend_offset,
            std::span<const token>(std::next(tkns.begin(), offset), 0),
            offset,
            node::iterator{},
            node::iterator{});
}
