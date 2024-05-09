#pragma once
#include <optional>
#include <taul/logger.h>
#include <taul/grammar.h>
#if TAUL_SRCGEN_IMPLEMENTATION
#include <taul/spec.h>
#include <taul/load.h>
#endif

// write "#define TAUL_SRCGEN_IMPLEMENTATION 1" in a *.cpp file to host the implementation,
// placing it prior to the #include for the generated fetcher header file
namespace taul::fetchers {
    taul::grammar abc() noexcept;
}

#if TAUL_SRCGEN_IMPLEMENTATION
static taul::grammar _TAUL_SRCGEN_init_abc() {
    auto lgr = taul::make_stderr_logger();
    taul::spec_writer sw{};
    sw.pos(28);
    sw.lpr_decl(taul::str::lit("A"));
    sw.pos(41);
    sw.lpr_decl(taul::str::lit("B"));
    sw.pos(54);
    sw.lpr_decl(taul::str::lit("C"));
    sw.pos(67);
    sw.lpr_decl(taul::str::lit("D"));
    sw.pos(81);
    sw.lpr_decl(taul::str::lit("JP"));
    sw.pos(132);
    sw.ppr_decl(taul::str::lit("Start"));
    sw.pos(150);
    sw.ppr_decl(taul::str::lit("ABC"));
    sw.pos(28);
    sw.lpr(taul::str::lit("A"), taul::qualifier::none);
    sw.pos(28);
    sw.string(taul::str::lit("a"));
    sw.pos(28);
    sw.close();
    sw.pos(41);
    sw.lpr(taul::str::lit("B"), taul::qualifier::none);
    sw.pos(41);
    sw.string(taul::str::lit("b"));
    sw.pos(41);
    sw.close();
    sw.pos(54);
    sw.lpr(taul::str::lit("C"), taul::qualifier::none);
    sw.pos(54);
    sw.string(taul::str::lit("c"));
    sw.pos(54);
    sw.close();
    sw.pos(67);
    sw.lpr(taul::str::lit("D"), taul::qualifier::none);
    sw.pos(67);
    sw.string(taul::str::lit("d"));
    sw.pos(67);
    sw.close();
    sw.pos(81);
    sw.lpr(taul::str::lit("JP"), taul::qualifier::none);
    sw.pos(81);
    sw.string(taul::str::lit("\U00005143\U00006c17\U00003067\U00003059\U0000304b"));
    sw.pos(81);
    sw.close();
    sw.pos(132);
    sw.ppr(taul::str::lit("Start"), taul::qualifier::none);
    sw.pos(135);
    sw.kleene_plus();
    sw.pos(132);
    sw.name(taul::str::lit("ABC"));
    sw.pos(135);
    sw.close();
    sw.pos(132);
    sw.close();
    sw.pos(150);
    sw.ppr(taul::str::lit("ABC"), taul::qualifier::none);
    sw.pos(150);
    sw.name(taul::str::lit("A"));
    sw.pos(152);
    sw.sequence();
    sw.pos(154);
    sw.name(taul::str::lit("B"));
    sw.pos(156);
    sw.alternative();
    sw.pos(158);
    sw.name(taul::str::lit("D"));
    sw.pos(152);
    sw.close();
    sw.pos(162);
    sw.name(taul::str::lit("C"));
    sw.pos(150);
    sw.close();
    auto loaded = taul::load(sw.done(), lgr);
    TAUL_LOG_IF(!loaded, lgr, "TAUL fetcher 'abc' backing data didn't initialize properly!");
    return loaded.value();
}
static const taul::grammar _TAUL_SRCGEN_abc_object = _TAUL_SRCGEN_init_abc();

taul::grammar taul::fetchers::abc() noexcept {
    return _TAUL_SRCGEN_abc_object;
}
#endif