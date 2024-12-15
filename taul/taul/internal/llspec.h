

#pragma once


#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <format>

#include "../spec.h"
#include "parse_table.h" // for preced_t


namespace taul::internal {


    // 'low-level specifications' or 'llspecs' is a low-level form TAUL specs get translated
    // into as part of the loading process

    // TODO: impl notions of 'precedence values' and 'precedence value limits' into parse
    //       tables, w/ llspec referencing this hitherto not yet added parse table feature

    // llspecs are vary similar to specs, except w/ the inclusion of lower-level features
    // such as precedence values on name exprs and precedence value limits
    
    // the process of translating a spec to an llspec will be called 'lowering'

    // things like errors are checked at the TAUL spec level, and thus no such safety checks
    // are made at the llspec level


    static_assert(spec_opcodes == 20); // update llspec_opcode whenever we add to spec_opcode

    enum class llspec_opcode : uint8_t {

        // these function identically to their spec_opcode counterparts

        close,
        alternative,
        lpr_decl,
        ppr_decl,
        lpr,
        ppr,
        end,
        any,
        string,
        charset,
        token,
        failure,
        name, // the llspec version of name expr also carries a 'precedence value'
        sequence,
        lookahead,
        lookahead_not,
        not0,
        optional,
        kleene_star,
        kleene_plus,

        _end_of_spec_mirror = kleene_plus, // marks end of portion mirrored from spec_opcode

        preced_pred, // puts precedence predicate term
        pylon, // puts pylon term

        num, // this is not a valid llspec opcode
    };

    constexpr size_t llspec_opcodes = (size_t)llspec_opcode::num;

    std::string fmt_llspec_opcode(llspec_opcode x);
}


template<>
struct std::formatter<taul::internal::llspec_opcode> final : std::formatter<std::string> {
    auto format(taul::internal::llspec_opcode x, format_context& ctx) const {
        return formatter<string>::format(taul::internal::fmt_llspec_opcode(x), ctx);
    }
};

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::internal::llspec_opcode& x) {
        return stream << taul::internal::fmt_llspec_opcode(x);
    }
}


namespace taul::internal {


    struct llspec final {
        std::vector<uint8_t>            bin;                // the llspec binary itself
        std::shared_ptr<source_code>    src = nullptr;      // the source_code the llspec will be associated with, if any


        inline void associate(const std::shared_ptr<source_code>& x) noexcept { src = x; }
        inline bool associated(const std::shared_ptr<source_code>& x) const noexcept { return src == x; }
        static llspec concat(const llspec& a, const llspec& b);
    };


    class llspec_writer final {
    public:

        llspec_writer() = default;
        llspec_writer(const llspec_writer&) = default;
        llspec_writer(llspec_writer&&) noexcept = default;

        ~llspec_writer() noexcept = default;

        llspec_writer& operator=(const llspec_writer&) = default;
        llspec_writer& operator=(llspec_writer&&) noexcept = default;


        llspec_writer& pos(source_pos new_pos);


        static_assert(llspec_opcodes == 22);

        llspec_writer& close();
        llspec_writer& alternative();

        llspec_writer& lpr_decl(std::string_view name);
        template<typename StringLike>
        inline llspec_writer& lpr_decl(const StringLike& name) {
            return lpr_decl(std::string_view(name));
        }

        llspec_writer& ppr_decl(std::string_view name);
        template<typename StringLike>
        inline llspec_writer& ppr_decl(const StringLike& name) {
            return ppr_decl(std::string_view(name));
        }

        llspec_writer& lpr(std::string_view name, qualifier qualifier = qualifier::none);
        template<typename StringLike>
        inline llspec_writer& lpr(const StringLike& name, qualifier qualifier = qualifier::none) {
            return lpr(std::string_view(name), qualifier);
        }

        llspec_writer& ppr(std::string_view name, qualifier qualifier = qualifier::none);
        template<typename StringLike>
        inline llspec_writer& ppr(const StringLike& name, qualifier qualifier = qualifier::none) {
            return ppr(std::string_view(name), qualifier);
        }

        llspec_writer& end();
        llspec_writer& any();

        llspec_writer& string(std::string_view s);
        template<typename StringLike>
        inline llspec_writer& string(const StringLike& s) {
            return string(std::string_view(s));
        }

        llspec_writer& charset(std::string_view s);
        template<typename StringLike>
        inline llspec_writer& charset(const StringLike& s) {
            return charset(std::string_view(s));
        }

        llspec_writer& token();
        llspec_writer& failure();

        llspec_writer& name(std::string_view name, preced_t preced_val);
        template<typename StringLike>
        inline llspec_writer& name(const StringLike& name, preced_t preced_val) {
            return this->name(std::string_view(name), preced_val);
        }

        llspec_writer& sequence();
        llspec_writer& lookahead();
        llspec_writer& lookahead_not();
        llspec_writer& not0();
        llspec_writer& optional();
        llspec_writer& kleene_star();
        llspec_writer& kleene_plus();

        llspec_writer& preced_pred(preced_t preced_max, preced_t preced_val);
        llspec_writer& pylon();

        llspec_writer& write_spec(const llspec& x);
        llspec_writer& write_spec(const llspec_writer& x);


        llspec done();


    private:

        llspec _temp = llspec{};
        source_pos _pos = 0;


        class write_spec_method_llspec_interpreter;
    };


    class llspec_interpreter {
    public:

        llspec_interpreter() = default;
        llspec_interpreter(const llspec_interpreter&) = delete;
        llspec_interpreter(llspec_interpreter&&) noexcept = delete;

        virtual ~llspec_interpreter() noexcept = default;

        llspec_interpreter& operator=(const llspec_interpreter&) = delete;
        llspec_interpreter& operator=(llspec_interpreter&&) noexcept = delete;


        source_pos pos() const noexcept;
        std::optional<llspec_opcode> peek() const noexcept;
        void interpret(const llspec& x);


    protected:

        virtual void on_startup() {}
        virtual void on_shutdown() {}

        static_assert(llspec_opcodes == 22);

        virtual void on_close() {}
        virtual void on_alternative() {}
        virtual void on_lpr_decl(std::string_view name) {}
        virtual void on_ppr_decl(std::string_view name) {}
        virtual void on_lpr(std::string_view name, qualifier qualifier) {}
        virtual void on_ppr(std::string_view name, qualifier qualifier) {}

        virtual void on_end() {}
        virtual void on_any() {}
        virtual void on_string(std::string_view s) {}
        virtual void on_charset(std::string_view s) {}
        virtual void on_token() {}
        virtual void on_failure() {}
        virtual void on_name(std::string_view name, preced_t preced_val) {}
        virtual void on_sequence() {}
        virtual void on_lookahead() {}
        virtual void on_lookahead_not() {}
        virtual void on_not() {}
        virtual void on_optional() {}
        virtual void on_kleene_star() {}
        virtual void on_kleene_plus() {}

        virtual void on_preced_pred(preced_t preced_max, preced_t preced_val) {}
        virtual void on_pylon() {}


    private:

        source_pos _pos = 0;
        std::optional<llspec_opcode> _peek;


        size_t _step(const llspec& s, size_t offset);
    };


    class llspec_writer::write_spec_method_llspec_interpreter final : public llspec_interpreter {
    public:

        llspec_writer* client;


        write_spec_method_llspec_interpreter(llspec_writer& client)
            : llspec_interpreter(),
            client(&client) {}


    protected:

        static_assert(llspec_opcodes == 22);

        void on_close() override final { deref_assert(client).pos(pos()).close(); }
        void on_alternative() override final { deref_assert(client).pos(pos()).alternative(); }
        void on_lpr_decl(std::string_view name) override final { deref_assert(client).pos(pos()).lpr_decl(name); }
        void on_ppr_decl(std::string_view name) override final { deref_assert(client).pos(pos()).ppr_decl(name); }
        void on_lpr(std::string_view name, qualifier qualifier) override final { deref_assert(client).pos(pos()).lpr(name, qualifier); }
        void on_ppr(std::string_view name, qualifier qualifier) override final { deref_assert(client).pos(pos()).ppr(name, qualifier); }
        void on_end() override final { deref_assert(client).pos(pos()).end(); }
        void on_any() override final { deref_assert(client).pos(pos()).any(); }
        void on_string(std::string_view s) override final { deref_assert(client).pos(pos()).string(s); }
        void on_charset(std::string_view s) override final { deref_assert(client).pos(pos()).charset(s); }
        void on_token() override final { deref_assert(client).pos(pos()).token(); }
        void on_failure() override final { deref_assert(client).pos(pos()).failure(); }
        void on_name(std::string_view name, preced_t preced_val) override final { deref_assert(client).pos(pos()).name(name, preced_val); }
        void on_sequence() override final { deref_assert(client).pos(pos()).sequence(); }
        void on_lookahead() override final { deref_assert(client).pos(pos()).lookahead(); }
        void on_lookahead_not() override final { deref_assert(client).pos(pos()).lookahead_not(); }
        void on_not() override final { deref_assert(client).pos(pos()).not0(); }
        void on_optional() override final { deref_assert(client).pos(pos()).optional(); }
        void on_kleene_star() override final { deref_assert(client).pos(pos()).kleene_star(); }
        void on_kleene_plus() override final { deref_assert(client).pos(pos()).kleene_plus(); }
        void on_preced_pred(preced_t preced_max, preced_t preced_val) override final { deref_assert(client).pos(pos()).preced_pred(preced_max, preced_val); }
        void on_pylon() override final { deref_assert(client).pos(pos()).pylon(); }
    };
}

