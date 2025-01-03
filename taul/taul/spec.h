

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

#include "internal/buff.h"


namespace taul {


    // IMPORTANT: at present, spec binaries are NOT PORTABLE, and exist simply to decouple
    //            TAUL's notions of 'loading' and 'syntax compilation' from one another

    // IMPORTANT: update llspec stuff whenever we modify spec stuff


    // usage of invalid spec will result in undefined behaviour

    class spec final {
    public:

        spec() = default;
        spec(const spec&) = default;
        spec(spec&&) noexcept = default;

        ~spec() noexcept = default;

        spec& operator=(const spec&) = default;
        spec& operator=(spec&&) noexcept = default;


        inline size_t size() const noexcept { return _bin.size(); }


        // associate binds a source_code to the spec, associating it w/ it, such
        // that it will be used when loading requires source location information

        // if x == nullptr, the spec will be imbued will be disassociated from
        // any current source_code association it may have

        inline std::shared_ptr<source_code> src() const noexcept { return _src; }
        inline void associate(const std::shared_ptr<source_code>& x) noexcept { _src = x; }
        inline bool associated(const std::shared_ptr<source_code>& x) const noexcept { return _src == x; }


        // concat concatenates two specs together, producing a new specification

        // the source_code association (or lack thereof) of spec 'a' is the one imbued in
        // the spec resulting from the concatenation

        static spec concat(const spec& a, const spec& b);


    private:
        internal::buff _bin;
        std::shared_ptr<source_code> _src = nullptr;


        friend class spec_writer;
        friend class spec_interpreter;
    };


    // IMPORTANT: the below use std::string_view rather than taul::str as in this case the
    //            former is better to use to avoid heap usage


    class spec_writer final {
    public:

        spec_writer() = default;
        spec_writer(const spec_writer&) = default;
        spec_writer(spec_writer&&) noexcept = default;

        ~spec_writer() noexcept = default;

        spec_writer& operator=(const spec_writer&) = default;
        spec_writer& operator=(spec_writer&&) noexcept = default;


        // TODO: pos has not been unit tested

        // every instr in a spec has associated w/ it a 'pos' value, describing
        // a source_pos offset into the source code which got compiled into this
        // spec, if any, where that instr is located

        // by default, the pos value given to instrs is 0

        // the 'pos' method changes this pos value, affecting instr writes thereafter

        spec_writer& pos(source_pos new_pos);


        static_assert(spec_opcodes == 21);

        // TODO: the StringLike overloads have not been unit tested

        // below 'StringLike' refers to a type which may be explicitly converted
        // into an std::string_view of said string-like object's contents

        // the 'StringLike' overloads below are to ensure that std::string and
        // taul::str objects may be used as input, as converting a short-lived 
        // taul::str into std::string_view could otherwise result in a the
        // ladder becoming dangling

        spec_writer& close();
        spec_writer& alternative();
        spec_writer& right_assoc();

        spec_writer& lpr_decl(std::string_view name);
        template<typename StringLike>
        inline spec_writer& lpr_decl(const StringLike& name) {
            return lpr_decl(std::string_view(name));
        }
        
        spec_writer& ppr_decl(std::string_view name);
        template<typename StringLike>
        inline spec_writer& ppr_decl(const StringLike& name) {
            return ppr_decl(std::string_view(name));
        }
        
        spec_writer& lpr(std::string_view name, qualifier qualifier = qualifier::none);
        template<typename StringLike>
        inline spec_writer& lpr(const StringLike& name, qualifier qualifier = qualifier::none) {
            return lpr(std::string_view(name), qualifier);
        }

        spec_writer& ppr(std::string_view name, qualifier qualifier = qualifier::none);
        template<typename StringLike>
        inline spec_writer& ppr(const StringLike& name, qualifier qualifier = qualifier::none) {
            return ppr(std::string_view(name), qualifier);
        }

        spec_writer& end();
        spec_writer& any();
        
        spec_writer& string(std::string_view s);
        template<typename StringLike>
        inline spec_writer& string(const StringLike& s) {
            return string(std::string_view(s));
        }

        spec_writer& charset(std::string_view s);
        template<typename StringLike>
        inline spec_writer& charset(const StringLike& s) {
            return charset(std::string_view(s));
        }
        
        spec_writer& token();
        spec_writer& failure();
        
        spec_writer& name(std::string_view name);
        template<typename StringLike>
        inline spec_writer& name(const StringLike& name) {
            return this->name(std::string_view(name));
        }

        spec_writer& sequence();
        spec_writer& lookahead();
        spec_writer& lookahead_not();
        spec_writer& not0();
        spec_writer& optional();
        spec_writer& kleene_star();
        spec_writer& kleene_plus();

        // TODO: write_spec hasn't been unit tested

        // write_spec writes the entirety of x to the spec being written

        // write_spec behaviour is undefined if &x == this

        spec_writer& write_spec(const spec& x);
        spec_writer& write_spec(const spec_writer& x);


        // finishes writing, returning the finished spec, resetting
        // the writer's state thereafter

        spec done();


    private:
        spec _temp = spec{};
        source_pos _pos = 0;


        class write_spec_method_spec_interpreter;
    };


    // spec_interpreter is a base class used to specify a set of state and events
    // which are used to interpret a spec

    class spec_interpreter {
    public:

        spec_interpreter() = default;
        spec_interpreter(const spec_interpreter&) = delete;
        spec_interpreter(spec_interpreter&&) noexcept = delete;

        virtual ~spec_interpreter() noexcept = default;

        spec_interpreter& operator=(const spec_interpreter&) = delete;
        spec_interpreter& operator=(spec_interpreter&&) noexcept = delete;


        // TODO: pos has not been unit tested

        // pos returns the source_pos corresponding to the current instr

        // behaviour is undefined if used outside of a on_### method, except for
        // on_startup and on_shutdown, where behaviour is still undefined

        source_pos pos() const noexcept;


        // TODO: peek has not been unit tested

        // peek returns the spec_opcode, if any, of the opcode of the
        // next instruction in the interpretation after the current one

        // behaviour is undefined if used outside the context of the on_###
        // methods of a spec_interpreter impl

        std::optional<spec_opcode> peek() const noexcept;


        void interpret(const spec& x);


    protected:

        virtual void on_startup() {}
        virtual void on_shutdown() {}

        static_assert(spec_opcodes == 21);

        virtual void on_close() {}
        virtual void on_alternative() {}
        virtual void on_right_assoc() {}
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
        source_pos _pos = 0;
        std::optional<spec_opcode> _peek;


        bool _step(internal::buff_reader& rdr);
    };


    class spec_writer::write_spec_method_spec_interpreter final : public spec_interpreter {
    public:

        spec_writer* client;


        write_spec_method_spec_interpreter(spec_writer& client)
            : spec_interpreter(),
            client(&client) {}


    protected:

        static_assert(spec_opcodes == 21);

        void on_close() override final { deref_assert(client).pos(pos()).close(); }
        void on_alternative() override final { deref_assert(client).pos(pos()).alternative(); }
        void on_right_assoc() override final { deref_assert(client).pos(pos()).right_assoc(); }
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
        void on_name(std::string_view name) override final { deref_assert(client).pos(pos()).name(name); }
        void on_sequence() override final { deref_assert(client).pos(pos()).sequence(); }
        void on_lookahead() override final { deref_assert(client).pos(pos()).lookahead(); }
        void on_lookahead_not() override final { deref_assert(client).pos(pos()).lookahead_not(); }
        void on_not() override final { deref_assert(client).pos(pos()).not0(); }
        void on_optional() override final { deref_assert(client).pos(pos()).optional(); }
        void on_kleene_star() override final { deref_assert(client).pos(pos()).kleene_star(); }
        void on_kleene_plus() override final { deref_assert(client).pos(pos()).kleene_plus(); }
    };
}

