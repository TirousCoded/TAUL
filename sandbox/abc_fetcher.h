#pragma once
#include <optional>
#include <taul/logger.h>
#include <taul/grammar.h>
#if TAUL_SRCGEN_IMPLEMENTATION
#include <taul/spec.h>
#include <taul/load.h>
#endif

// write "#define TAUL_SRCGEN_IMPLEMENTATION 1" in a *.cpp file to host the implementation

namespace taul::fetchers {
    const taul::grammar& abc() noexcept;
}

#if TAUL_SRCGEN_IMPLEMENTATION
static taul::grammar _TAUL_SRCGEN_init_abc() {
    auto lgr = taul::make_stderr_logger();
    taul::spec_writer sw{};
    sw.pos(28);
    sw.lpr_decl("A"_str);
    sw.pos(41);
    sw.lpr_decl("B"_str);
    sw.pos(54);
    sw.lpr_decl("C"_str);
    sw.pos(67);
    sw.lpr_decl("D"_str);
    sw.pos(81);
    sw.lpr_decl("JP"_str);
    sw.pos(132);
    sw.ppr_decl("Start"_str);
    sw.pos(150);
    sw.ppr_decl("ABC"_str);
    sw.pos(28);
    sw.lpr("A"_str, taul::qualifier::none);
    sw.pos(28);
    sw.string("a"_str);
    sw.pos(28);
    sw.close();
    sw.pos(41);
    sw.lpr("B"_str, taul::qualifier::none);
    sw.pos(41);
    sw.string("b"_str);
    sw.pos(41);
    sw.close();
    sw.pos(54);
    sw.lpr("C"_str, taul::qualifier::none);
    sw.pos(54);
    sw.string("c"_str);
    sw.pos(54);
    sw.close();
    sw.pos(67);
    sw.lpr("D"_str, taul::qualifier::none);
    sw.pos(67);
    sw.string("d"_str);
    sw.pos(67);
    sw.close();
    sw.pos(81);
    sw.lpr("JP"_str, taul::qualifier::none);
    sw.pos(81);
    sw.string("\U00005143\U00006c17\U00003067\U00003059\U0000304b"_str);
    sw.pos(81);
    sw.close();
    sw.pos(132);
    sw.ppr("Start"_str, taul::qualifier::none);
    sw.pos(135);
    sw.kleene_plus();
    sw.pos(132);
    sw.name("ABC"_str);
    sw.pos(135);
    sw.close();
    sw.pos(132);
    sw.close();
    sw.pos(150);
    sw.ppr("ABC"_str, taul::qualifier::none);
    sw.pos(150);
    sw.name("A"_str);
    sw.pos(152);
    sw.sequence();
    sw.pos(154);
    sw.name("B"_str);
    sw.pos(156);
    sw.alternative();
    sw.pos(158);
    sw.name("D"_str);
    sw.pos(152);
    sw.close();
    sw.pos(162);
    sw.name("C"_str);
    sw.pos(150);
    sw.close();
    auto loaded = taul::load(sw.done(), lgr);
    TAUL_LOG_IF(!loaded, lgr, "TAUL fetcher 'abc' backing data didn't initialize properly!");
    return loaded.value();
}
static const taul::grammar _TAUL_SRCGEN_abc_object = _TAUL_SRCGEN_init_abc();

const taul::grammar& taul::fetchers::abc() noexcept {
    return _TAUL_SRCGEN_abc_object;
}
#endif