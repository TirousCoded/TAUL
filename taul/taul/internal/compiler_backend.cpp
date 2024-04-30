

#include "compiler_backend.h"


using namespace taul::string_literals;


void taul::internal::compiler_backend::push_expr() {
    expr_stk.push_back(expr{});
}

void taul::internal::compiler_backend::pop_expr() {
    auto result = _build_expr_spec();
    expr_stk.pop_back();
    _merge_down(result);
}

void taul::internal::compiler_backend::push_suffix(suffix x) {
    TAUL_ASSERT(in_expr());
    suffix_stk.push_back(x);
    expr_stk.back().suffixes++;
}

void taul::internal::compiler_backend::pop_suffix() {
    TAUL_ASSERT(in_expr());
    TAUL_ASSERT(expr_stk.back().suffixes >= 1);
    suffix_stk.pop_back();
    expr_stk.back().suffixes--;
}

void taul::internal::compiler_backend::push_nonterminal(ppr_ref x, source_pos pos) {
    nonterminal_stk.push_back(nonterminal{ x, pos });
}

void taul::internal::compiler_backend::pop_nonterminal() {
    TAUL_ASSERT(in_nonterminal());
    nonterminal_stk.pop_back();
}

taul::internal::compiler_backend::compiler_backend(const source_code& src, spec_error_counter& ec, std::shared_ptr<logger> lgr)
    : listener(lgr),
    src(&src),
    ec(&ec) {}

void taul::internal::compiler_backend::on_startup() {
    // do nothing
}

void taul::internal::compiler_backend::on_shutdown() {
    _build_result();
}

void taul::internal::compiler_backend::on_lexical(token tkn) {
    _handle_lexical(tkn);
}

void taul::internal::compiler_backend::on_syntactic(ppr_ref ppr, source_pos pos) {
    push_nonterminal(ppr, pos);
    _handle_syntactic_begin();
}

void taul::internal::compiler_backend::on_close() {
    _handle_syntactic_end();
    pop_nonterminal();
}

void taul::internal::compiler_backend::on_abort(source_pos pos) {
    raise(spec_error::syntax_error, "syntax error!");
}

taul::spec taul::internal::compiler_backend::_build_expr_spec() {
    spec_writer sw{};
    // write prefixes for suffixes (in reverse order of push)
    while (expr_stk.back().suffixes >= 1) {
        switch (suffix_stk.back()) {
        case suffix::optional: sw.optional(); break;
        case suffix::kleene_star: sw.kleene_star(); break;
        case suffix::kleene_plus: sw.kleene_plus(); break;
        default: TAUL_DEADEND; break;
        }
        pop_suffix();
        // also gotta remember to write close to sw_main for each suffix
        expr_stk.back().sw_main.close();
    }
    // write main body of expr
    sw.write_spec(expr_stk.back().sw_main);
    return sw.done();
}

void taul::internal::compiler_backend::_merge_down(const spec& x) {
    if (in_expr()) expr_stk.back().sw_main.write_spec(x);
    else if (in_lpr()) sw_lpr_defs.write_spec(x);
    else if (in_ppr()) sw_ppr_defs.write_spec(x);
    else TAUL_DEADEND;
}

void taul::internal::compiler_backend::_build_result() {
    if (cancelled) return;
    spec_writer sw{};
    sw.write_spec(sw_lpr_decls);
    sw.write_spec(sw_ppr_decls);
    sw.write_spec(sw_lpr_defs);
    sw.write_spec(sw_ppr_defs);
    result = std::make_optional(sw.done());
}

void taul::internal::compiler_backend::_handle_lexical(token tkn) {
    TAUL_DEREF_SAFE(src && ec) {
        if (!in_nonterminal()) return; // just in case
        auto nonterminal = nonterminal_stk.back().ppr.name();
        if (tkn.is_normal()) {
            auto name = tkn.lpr.value().name();
            auto text = tkn.str(*src);
            if (name == "IDENTIFIER"_str) {
                if (nonterminal == "Rule_Name"_str) {
                    rule_name = text;
                }
                else if (nonterminal == "Name"_str) {
                    TAUL_ASSERT(in_expr());
                    expr_stk.back().sw_main.name(text);
                }
            }
            else if (name == "STRING"_str) {
                if (nonterminal == "String"_str) {
                    TAUL_ASSERT(in_expr());
                    expr_stk.back().sw_main.string(text.substr(1, text.length() - 2));
                }
            }
            else if (name == "CHARSET"_str) {
                if (nonterminal == "Charset"_str) {
                    TAUL_ASSERT(in_expr());
                    expr_stk.back().sw_main.charset(text.substr(1, text.length() - 2));
                }
            }
        }
    }
}

void taul::internal::compiler_backend::_handle_syntactic_begin() {
    TAUL_ASSERT(in_nonterminal());
    TAUL_DEREF_SAFE(src && ec) {
        auto name = nonterminal_stk.back().ppr.name();
        if (name == "LexerSection"_str) {
            current_section = section::lexer;
        }
        else if (name == "ParserSection"_str) {
            current_section = section::parser;
        }
        else if (name == "Rule"_str) {
            current_qualifier = qualifier::none;
            qualifier_count = 0;
        }
        else if (name == "Rule_Alts"_str) {
            if (in_lpr()) {
                sw_lpr_decls.lpr_decl(rule_name);
                sw_lpr_defs.lpr(rule_name, current_qualifier);
            }
            if (in_ppr()) {
                sw_ppr_decls.ppr_decl(rule_name);
                sw_ppr_defs.ppr(rule_name, current_qualifier);
            }
        }
        else if (name == "Qualifier"_str) {
            qualifier_count++;
        }
        else if (name == "Qualifier_Skip"_str) {
            current_qualifier = qualifier::skip;
        }
        else if (name == "Qualifier_Support"_str) {
            current_qualifier = qualifier::support;
        }
        else if (name == "Expr"_str) {
            push_expr();
        }
        else if (name == "Expr_NoSuffix"_str) {
            push_expr();
        }
        else if (name == "Sequence"_str) {
            TAUL_ASSERT(in_expr());
            expr_stk.back().sw_main.sequence();
        }
        else if (name == "LookAhead"_str) {
            TAUL_ASSERT(in_expr());
            expr_stk.back().sw_main.lookahead();
        }
        else if (name == "LookAheadNot"_str) {
            TAUL_ASSERT(in_expr());
            expr_stk.back().sw_main.lookahead_not();
        }
        else if (name == "Not"_str) {
            TAUL_ASSERT(in_expr());
            expr_stk.back().sw_main.not0();
        }
        else if (name == "Optional_Suffix"_str) {
            push_suffix(suffix::optional);
        }
        else if (name == "KleeneStar_Suffix"_str) {
            push_suffix(suffix::kleene_star);
        }
        else if (name == "KleenePlus_Suffix"_str) {
            push_suffix(suffix::kleene_plus);
        }
        else if (name == "Alt_Divider"_str) {
            // the way we have it setup we gotta account for 
            // toplevel Alts not being in an Expr
            if (in_expr()) expr_stk.back().sw_main.alternative();
            else {
                if (in_lpr()) sw_lpr_defs.alternative();
                if (in_ppr()) sw_ppr_defs.alternative();
            }
        }
    }
}

void taul::internal::compiler_backend::_handle_syntactic_end() {
    TAUL_ASSERT(in_nonterminal());
    TAUL_DEREF_SAFE(src && ec) {
        auto name = nonterminal_stk.back().ppr.name();
        if (name == "Rule_Qualifiers"_str) {
            if (qualifier_count >= 2) {
                raise(spec_error::illegal_multiple_qualifiers, "illegal multiple qualifiers on rule!");
            }
        }
        else if (name == "Rule_Alts"_str) {
            if (in_lpr()) sw_lpr_defs.close();
            if (in_ppr()) sw_ppr_defs.close();
        }
        else if (name == "Expr"_str) {
            pop_expr();
        }
        else if (name == "Expr_NoSuffix"_str) {
            pop_expr();
        }
        else if (name == "End"_str) {
            TAUL_ASSERT(in_expr());
            expr_stk.back().sw_main.end();
        }
        else if (name == "Any"_str) {
            TAUL_ASSERT(in_expr());
            expr_stk.back().sw_main.any();
        }
        else if (name == "Token"_str) {
            TAUL_ASSERT(in_expr());
            expr_stk.back().sw_main.token();
        }
        else if (name == "Failure"_str) {
            TAUL_ASSERT(in_expr());
            expr_stk.back().sw_main.failure();
        }
        else if (name == "Sequence"_str) {
            TAUL_ASSERT(in_expr());
            expr_stk.back().sw_main.close();
        }
        else if (name == "LookAhead"_str) {
            TAUL_ASSERT(in_expr());
            expr_stk.back().sw_main.close();
        }
        else if (name == "LookAheadNot"_str) {
            TAUL_ASSERT(in_expr());
            expr_stk.back().sw_main.close();
        }
        else if (name == "Not"_str) {
            TAUL_ASSERT(in_expr());
            expr_stk.back().sw_main.close();
        }
    }
}

