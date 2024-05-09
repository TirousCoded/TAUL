

#include "export_fetcher.h"

#include "hex.h"


using namespace taul::string_literals;


// IMPORTANT: the generated code of taul::export_fetcher works by creating a translation-unit
//            local global variable, which gets initialized ONCE AND ONLY ONCE immediately
//            BEFORE the main function executes, and then the fetcher launders an lvalue to
//            this program-wide singleton to the end-user
//
//            I did some quick tests to see that 'static' global vars in *.cpp files work this
//            way, and I found that it did initialize only once, and prior to the main function,
//            and I used std::async to quickly check that this held true w/ regards to the 
//            usage of these global vars in the context of multiple threads
//
//            the detail about multiple threads is IMPORTANT, as I'd like us to be able to 
//            access this global var from multiple threads concurrently w/out synchronization
//
//            TODO: not super big deal, but maybe investigate the below further
// 
//            I don't know about the nuance of "what about code also called prior to the start
//            of the main function which interacts w/ the global var?", but from what I've 
//            seen so far I'm comfirtable *assuming* that everything should be fine


// just some quick-n'-dirty string *sanitizing* in order to ensure that they're
// able to be put in C++ source code w/out issue

// as we're working w/ specs, we presume input is UTF-8

static std::string _sanitize_s(std::string_view s) {
    std::string result{};
    result.reserve(s.length());
    taul::decoder<char> d(taul::utf8, s);
    while (!d.done()) {
        auto decoded = d.next();
        if (!decoded) break;
        if (taul::is_visible_ascii(decoded->cp)) {
            result += char(decoded->cp);
        }
        // quickly throw together an escape sequence to *sanitize* w/
        else if (taul::is_ascii(decoded->cp)) {
            result += std::format("\\x{}", taul::fmt_hex_u8(uint8_t(decoded->cp)));
        }
        else {
            result += std::format("\\U{}", taul::fmt_hex_u32(uint32_t(decoded->cp)));
        }
    }
    return result;
}

static std::string _fmt_string(std::string_view s) {
    return std::format("taul::str::lit(\"{}\")", _sanitize_s(s));
}

static std::string _fmt_qualifier(taul::qualifier q) {
    return std::format("taul::qualifier::{}", q);
}


// apparently putting class in unnamed namespace in *.cpp file has similar
// affects to having function w/ static linkage
//      https://stackoverflow.com/questions/43686876/class-with-static-linkage

namespace {
    class export_fetcher_spec_helper final : public taul::spec_interpreter {
    public:

        std::string* src = nullptr;
        const char* tab = nullptr;


        export_fetcher_spec_helper(std::string& src, const char* tab)
            : src(&src), 
            tab(tab) {}


        static_assert(taul::spec_opcodes == 21);

        void on_pos(taul::source_pos new_pos) override final {
            TAUL_DEREF_SAFE(src) *src += std::format("{0}sw.pos({1});\n", tab, size_t(new_pos));
        }
        void on_close() override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.close();\n", tab);
        }
        void on_alternative() override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.alternative();\n", tab);
        }
        void on_lpr_decl(std::string_view name) override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.lpr_decl({1});\n", tab, _fmt_string(name));
        }
        void on_ppr_decl(std::string_view name) override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.ppr_decl({1});\n", tab, _fmt_string(name));
        }
        void on_lpr(std::string_view name, taul::qualifier qualifier) override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.lpr({1}, {2});\n", tab, _fmt_string(name), _fmt_qualifier(qualifier));
        }
        void on_ppr(std::string_view name, taul::qualifier qualifier) override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.ppr({1}, {2});\n", tab, _fmt_string(name), _fmt_qualifier(qualifier));
        }
        void on_end() override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.end();\n", tab);
        }
        void on_any() override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.any();\n", tab);
        }
        void on_string(std::string_view s) override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.string({1});\n", tab, _fmt_string(s));
        }
        void on_charset(std::string_view s) override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.charset({1});\n", tab, _fmt_string(s));
        }
        void on_token() override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.token();\n", tab);
        }
        void on_failure() override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.failure();\n", tab);
        }
        void on_name(std::string_view name) override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.name({1});\n", tab, _fmt_string(name));
        }
        void on_sequence() override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.sequence();\n", tab);
        }
        void on_lookahead() override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.lookahead();\n", tab);
        }
        void on_lookahead_not() override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.lookahead_not();\n", tab);
        }
        void on_not() override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.not0();\n", tab);
        }
        void on_optional() override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.optional();\n", tab);
        }
        void on_kleene_star() override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.kleene_star();\n", tab);
        }
        void on_kleene_plus() override final {
            TAUL_DEREF_SAFE(src) (*src) += std::format("{0}sw.kleene_plus();\n", tab);
        }
    };
}


taul::source_code taul::export_fetcher(
    const spec& s,
    const char* fetcher_name,
    const char* taul_include_path_prefix,
    const char* tab) {
    TAUL_ASSERT(fetcher_name);
    TAUL_ASSERT(taul_include_path_prefix);
    TAUL_ASSERT(tab);

    std::string src{};
    src += std::format("#pragma once\n");
    src += std::format("#include <optional>\n");
    src += std::format("#include <{0}/logger.h>\n", taul_include_path_prefix);
    src += std::format("#include <{0}/grammar.h>\n", taul_include_path_prefix);
    src += std::format("#if TAUL_SRCGEN_IMPLEMENTATION\n");
    src += std::format("#include <{0}/spec.h>\n", taul_include_path_prefix);
    src += std::format("#include <{0}/load.h>\n", taul_include_path_prefix);
    src += std::format("#endif\n");
    src += std::format("\n");
    src += std::format("// write \"#define TAUL_SRCGEN_IMPLEMENTATION 1\" in a *.cpp file to host the implementation,\n");
    src += std::format("// placing it prior to the #include for the generated fetcher header file");
    src += std::format("\n");
    src += std::format("namespace taul::fetchers {{\n");
    src += std::format("{0}taul::grammar {1}() noexcept;\n", tab, fetcher_name);
    src += std::format("}}\n");
    src += std::format("\n");
    src += std::format("#if TAUL_SRCGEN_IMPLEMENTATION\n");
    src += std::format("static taul::grammar _TAUL_SRCGEN_init_{0}() {{\n", fetcher_name);
    src += std::format("{0}auto lgr = taul::make_stderr_logger();\n", tab);
    src += std::format("{0}taul::spec_writer sw{{}};\n", tab);
    
    export_fetcher_spec_helper(src, tab).interpret(s);

    src += std::format("{0}auto loaded = taul::load(sw.done(), lgr);\n", tab);
    src += std::format("{0}TAUL_LOG_IF(!loaded, lgr, \"TAUL fetcher 'abc' backing data didn't initialize properly!\");\n", tab);
    src += std::format("{0}return loaded.value();\n", tab);
    src += std::format("}}\n");
    src += std::format("static const taul::grammar _TAUL_SRCGEN_{0}_object = _TAUL_SRCGEN_init_{0}();\n", fetcher_name);
    src += std::format("\n");
    src += std::format("taul::grammar taul::fetchers::{0}() noexcept {{\n", fetcher_name);
    src += std::format("{0}return _TAUL_SRCGEN_{1}_object;\n", tab, fetcher_name);
    src += std::format("}}\n");
    src += std::format("#endif");

    taul::source_code result{};
    result.add_str("<src>"_str, str(src));
    return result;
}

