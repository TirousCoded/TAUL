

#pragma once


#include <string>
#include <vector>
#include <span>
#include <optional>

#include "str.h"
#include "source_code.h"
#include "polarity.h"
#include "qualifier.h"
#include "spec_opcode.h"


namespace taul {


    // usage of invalid spec will result in undefined behaviour

    struct spec final {
        std::vector<std::uint8_t>       bin;                // the spec binary itself
        std::shared_ptr<source_code>    src = nullptr;      // the source_code the spec will be associated with, if any


        // associate binds a source_code to the spec, associating it w/ it, such
        // that it will be used when loading requires source location information

        // if x == nullptr, the spec will be imbued will be disassociated from
        // any current source_code association it may have

        inline void associate(const std::shared_ptr<source_code>& x) noexcept {
            src = x;
        }

        inline bool associated(const std::shared_ptr<source_code>& x) const noexcept {
            return src == x;
        }


        // concat concatenates two specs together, producing a new specification

        // the source_code association (or lack thereof) of spec 'a' is the one imbued in
        // the spec resulting from the concatenation

        static spec concat(const spec& a, const spec& b);
    };


    // IMPORTANT: the below use std::string_view rather than taul::str as in this case the
    //            former is better to use to avoid heap usage


    class spec_writer final {
    public:

        // make everything public to auto-gen copy/move ctor/assign

        spec _temp = spec{}; // internal, do not use


        static_assert(spec_opcodes == 21);

        // TODO: the taul::str overloads have not been unit tested

        // the taul::str overloads below are to ensure that taul::str objects
        // passed in live for the entire duration of the call, as converting
        // a short-lived taul::str into std::string_view could result in a
        // dangling pointer

        spec_writer& pos(source_pos new_pos);
        spec_writer& close();
        spec_writer& alternative();
        spec_writer& lpr_decl(std::string_view name);
        spec_writer& lpr_decl(const taul::str& name);
        spec_writer& ppr_decl(std::string_view name);
        spec_writer& ppr_decl(const taul::str& name);
        spec_writer& lpr(std::string_view name, qualifier qualifier = qualifier::none);
        spec_writer& lpr(const taul::str& name, qualifier qualifier = qualifier::none);
        spec_writer& ppr(std::string_view name, qualifier qualifier = qualifier::none);
        spec_writer& ppr(const taul::str& name, qualifier qualifier = qualifier::none);

        spec_writer& end();
        spec_writer& any();
        spec_writer& string(std::string_view s);
        spec_writer& string(const taul::str& s);
        spec_writer& charset(std::string_view s);
        spec_writer& charset(const taul::str& s);
        spec_writer& token();
        spec_writer& failure();
        spec_writer& name(std::string_view name);
        spec_writer& name(const taul::str& name);
        spec_writer& sequence();
        spec_writer& lookahead();
        spec_writer& lookahead_not();
        spec_writer& not0();
        spec_writer& optional();
        spec_writer& kleene_star();
        spec_writer& kleene_plus();

        // write_spec write the entirety of x to the spec being written

        // write_spec behaviour is undefined if &x == this

        // TODO: write_spec hasn't been unit tested

        spec_writer& write_spec(const spec& x);
        spec_writer& write_spec(const spec_writer& x);


        // finishes writing, returning the finished spec, resetting
        // the writer's state thereafter

        spec done();
    };


    // spec_interpreter is a base class used to specify a set of state and events
    // which are used to interpret a spec

    class spec_interpreter {
    public:

        spec_interpreter() = default;
        spec_interpreter(const spec_interpreter&) = delete;
        spec_interpreter(spec_interpreter&&) noexcept = delete;

        ~spec_interpreter() noexcept = default;

        spec_interpreter& operator=(const spec_interpreter&) = delete;
        spec_interpreter& operator=(spec_interpreter&&) noexcept = delete;


        void interpret(const spec& x);


    protected:

        virtual void on_startup() {}
        virtual void on_shutdown() {}

        static_assert(spec_opcodes == 21);

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

        std::size_t _step(const spec& s, std::size_t offset);
    };


    namespace internal {


        void spec_write_u8(spec& s, std::uint8_t x) noexcept;
        void spec_write_u16(spec& s, std::uint16_t x) noexcept;
        void spec_write_u32(spec& s, std::uint32_t x) noexcept;
        void spec_write_char(spec& s, char x) noexcept;
        void spec_write_opcode(spec& s, spec_opcode opcode) noexcept;
        void spec_write_qualifier(spec& s, qualifier q) noexcept;
        void spec_write_str(spec& s, std::string_view x) noexcept;

        std::uint8_t spec_read_u8(const spec& s, std::size_t offset, std::size_t& len) noexcept;
        std::uint16_t spec_read_u16(const spec& s, std::size_t offset, std::size_t& len) noexcept;
        std::uint32_t spec_read_u32(const spec& s, std::size_t offset, std::size_t& len) noexcept;
        char spec_read_char(const spec& s, std::size_t offset, std::size_t& len) noexcept;
        taul::spec_opcode spec_read_opcode(const spec& s, std::size_t offset, std::size_t& len) noexcept;
        taul::qualifier spec_read_qualifier(const spec& s, std::size_t offset, std::size_t& len) noexcept;
        std::string_view spec_read_str(const spec& s, std::size_t offset, std::size_t& len) noexcept;


        class write_spec_method_spec_interpreter final : public spec_interpreter {
        public:

            spec_writer* client;


            write_spec_method_spec_interpreter(spec_writer& client) 
                : client(&client) {}


        protected:

            static_assert(spec_opcodes == 21);

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
}

