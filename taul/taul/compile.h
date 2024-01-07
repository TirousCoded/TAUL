

#pragma once


#include "logger.h"
#include "source_code.h"
#include "spec_error.h"
#include "spec.h"
#include "node.h"
#include "traverser.h"


namespace taul {


    // these taul::compile functions are used for syntax compilation,
    // producing taul::spec objects, which are then loaded

    // overloads which accept a taul::source_code shared-pointer will
    // fail if their src argument is nullptr, raising a source-code-not-found
    // spec error

    // overloads which accept an std::filesystem::path const-lvalue reference
    // will fail if their specified file could not be loaded, raising a
    // source-code-not-found spec error

    // the ctx parameters are used for allocating the internal AST of
    // the spec, which is then traversed to produce the spec, w/ overloads
    // w/out ctx creating a node context internally

    // the ec parameters are used for reporting pre-loading errors,
    // such as syntax errors

    // only this first taul::compile function will be unit tested, 
    // w/ all other overloads being expected to be straight-forward 
    // usages of this first one, w/ some extra behaviour which can be
    // presumed to be handled via things like taul::source_code

    // the overloads which load source code from a file will also be
    // unit tested, but only w/ regards to their raising of a spec
    // error if their specified file could not be found

    std::optional<spec> compile(
        node_ctx& ctx,
        const std::shared_ptr<source_code>& src,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<spec> compile(
        node_ctx& ctx,
        const std::shared_ptr<source_code>& src,
        const std::shared_ptr<logger>& lgr = nullptr);
    
    std::optional<spec> compile(
        const std::shared_ptr<source_code>& src,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<spec> compile(
        const std::shared_ptr<source_code>& src,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<spec> compile(
        node_ctx& ctx,
        const std::string& src,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<spec> compile(
        node_ctx& ctx,
        const std::string& src,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<spec> compile(
        const std::string& src,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<spec> compile(
        const std::string& src,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<spec> compile(
        node_ctx& ctx,
        const std::filesystem::path& src_path,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<spec> compile(
        node_ctx& ctx,
        const std::filesystem::path& src_path,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<spec> compile(
        const std::filesystem::path& src_path,
        spec_error_counter& ec,
        const std::shared_ptr<logger>& lgr = nullptr);

    std::optional<spec> compile(
        const std::filesystem::path& src_path,
        const std::shared_ptr<logger>& lgr = nullptr);


    namespace internal {


        // this traverser produces the spec, but does not imbue it w/
        // any source_code association, so be sure that gets done


        class compile_traverser final : public traverser {
        public:

            bool success = true;
            spec output;


            compile_traverser();


        protected:

            void on_begin() override final;
            void on_end() override final;

            void on_enter(const node& nd, bool& skip_children) override final;
            void on_exit(const taul::node& nd) override final;


        private:

            bool lexerSection = true;

            std::string ruleName;
            bool ruleSkip = false;

            spec_writer swForDecls;
            spec_writer swForDefs;
        };
    }
}

