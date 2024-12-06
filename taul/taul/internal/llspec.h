

#pragma once


#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <format>

#include "../spec.h"


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


    static_assert(spec_opcodes == 21); // update llspec_opcode whenever we add to spec_opcode

    enum class llspec_opcode : uint8_t {

        // these function identically to their spec_opcode counterparts

        pos,
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
        name,
        sequence,
        lookahead,
        lookahead_not,
        not0,
        optional,
        kleene_star,
        kleene_plus,

        _end_of_spec_mirror = kleene_plus, // marks end of portion mirrored from spec_opcode

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


        static_assert(llspec_opcodes == 21);

        llspec_writer& pos(source_pos new_pos);
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

        llspec_writer& name(std::string_view name);
        template<typename StringLike>
        inline llspec_writer& name(const StringLike& name) {
            return this->name(std::string_view(name));
        }

        llspec_writer& sequence();
        llspec_writer& lookahead();
        llspec_writer& lookahead_not();
        llspec_writer& not0();
        llspec_writer& optional();
        llspec_writer& kleene_star();
        llspec_writer& kleene_plus();

        llspec_writer& write_spec(const llspec& x);
        llspec_writer& write_spec(const llspec_writer& x);


        llspec done();


    private:

        llspec _temp = llspec{};


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


        std::optional<llspec_opcode> lookahead() const noexcept;
        void interpret(const llspec& x);


    protected:

        virtual void on_startup() {}
        virtual void on_shutdown() {}

        static_assert(llspec_opcodes == 21);

        virtual void on_pos(source_pos new_pos) {}
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
        virtual void on_name(std::string_view name) {}
        virtual void on_sequence() {}
        virtual void on_lookahead() {}
        virtual void on_lookahead_not() {}
        virtual void on_not() {}
        virtual void on_optional() {}
        virtual void on_kleene_star() {}
        virtual void on_kleene_plus() {}


    private:

        std::optional<llspec_opcode> _lookahead;


        size_t _step(const llspec& s, size_t offset);
    };


    class llspec_writer::write_spec_method_llspec_interpreter final : public llspec_interpreter {
    public:

        llspec_writer* client;


        write_spec_method_llspec_interpreter(llspec_writer& client)
            : llspec_interpreter(),
            client(&client) {}


    protected:

        static_assert(llspec_opcodes == 21);

        void on_pos(source_pos new_pos) override final { TAUL_DEREF_SAFE(client) client->pos(new_pos); }
        void on_close() override final { TAUL_DEREF_SAFE(client) client->close(); }
        void on_alternative() override final { TAUL_DEREF_SAFE(client) client->alternative(); }
        void on_lpr_decl(std::string_view name) override final { TAUL_DEREF_SAFE(client) client->lpr_decl(name); }
        void on_ppr_decl(std::string_view name) override final { TAUL_DEREF_SAFE(client) client->ppr_decl(name); }
        void on_lpr(std::string_view name, qualifier qualifier) override final { TAUL_DEREF_SAFE(client) client->lpr(name, qualifier); }
        void on_ppr(std::string_view name, qualifier qualifier) override final { TAUL_DEREF_SAFE(client) client->ppr(name, qualifier); }
        void on_end() override final { TAUL_DEREF_SAFE(client) client->end(); }
        void on_any() override final { TAUL_DEREF_SAFE(client) client->any(); }
        void on_string(std::string_view s) override final { TAUL_DEREF_SAFE(client) client->string(s); }
        void on_charset(std::string_view s) override final { TAUL_DEREF_SAFE(client) client->charset(s); }
        void on_token() override final { TAUL_DEREF_SAFE(client) client->token(); }
        void on_failure() override final { TAUL_DEREF_SAFE(client) client->failure(); }
        void on_name(std::string_view name) override final { TAUL_DEREF_SAFE(client) client->name(name); }
        void on_sequence() override final { TAUL_DEREF_SAFE(client) client->sequence(); }
        void on_lookahead() override final { TAUL_DEREF_SAFE(client) client->lookahead(); }
        void on_lookahead_not() override final { TAUL_DEREF_SAFE(client) client->lookahead_not(); }
        void on_not() override final { TAUL_DEREF_SAFE(client) client->not0(); }
        void on_optional() override final { TAUL_DEREF_SAFE(client) client->optional(); }
        void on_kleene_star() override final { TAUL_DEREF_SAFE(client) client->kleene_star(); }
        void on_kleene_plus() override final { TAUL_DEREF_SAFE(client) client->kleene_plus(); }
    };
}

