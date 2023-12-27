

#pragma once


#include <string>
#include <vector>
#include <span>
#include <optional>

#include "source_code.h"
#include "bias.h"
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

        inline void associate(const std::shared_ptr<source_code>& x) noexcept {
            src = x;
        }
    };


    class spec_writer final {
    public:

        // writer's need not be copied, nor moved, just used and thrown away

        spec_writer() = default;
        spec_writer(const spec_writer&) = delete;
        spec_writer(spec_writer&&) noexcept = delete;

        ~spec_writer() noexcept = default;

        spec_writer& operator=(const spec_writer&) = delete;
        spec_writer& operator=(spec_writer&&) noexcept = delete;


        static_assert(spec_opcodes == 22);

        spec_writer& grammar_bias(bias b);
        spec_writer& close();
        spec_writer& lpr_decl(std::string_view name);
        spec_writer& ppr_decl(std::string_view name);
        spec_writer& lpr(std::string_view name, qualifier qualifier = qualifier::none);
        spec_writer& ppr(std::string_view name);

        spec_writer& begin();
        spec_writer& end();
        spec_writer& any();
        spec_writer& string(std::string_view s);
        spec_writer& charset(std::string_view s);
        spec_writer& range(char a, char b);
        spec_writer& token();
        spec_writer& failure();
        spec_writer& name(std::string_view name);
        spec_writer& sequence();
        spec_writer& set(bias b = bias::fl);
        spec_writer& modifier(std::uint16_t min, std::uint16_t max);
        spec_writer& assertion(polarity p = polarity::positive);
        spec_writer& constraint(polarity p = polarity::positive);
        spec_writer& junction();
        spec_writer& localend();


        // finishes writing, returning the finished spec, resetting
        // the writer's state thereafter

        spec done();


    private:

        spec _temp;
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

        static_assert(spec_opcodes == 22);

        virtual void on_grammar_bias(bias b) {}
        virtual void on_close() {}
        virtual void on_lpr_decl(std::string_view name) {}
        virtual void on_ppr_decl(std::string_view name) {}
        virtual void on_lpr(std::string_view name, qualifier qualifier) {}
        virtual void on_ppr(std::string_view name) {}

        virtual void on_begin() {}
        virtual void on_end() {}
        virtual void on_any() {}
        virtual void on_string(std::string_view s) {}
        virtual void on_charset(std::string_view s) {}
        virtual void on_range(char a, char b) {}
        virtual void on_token() {}
        virtual void on_failure() {}
        virtual void on_name(std::string_view name) {}
        virtual void on_sequence() {}
        virtual void on_set(bias b) {}
        virtual void on_modifier(std::uint16_t min, std::uint16_t max) {}
        virtual void on_assertion(polarity p) {}
        virtual void on_constraint(polarity p) {}
        virtual void on_junction() {}
        virtual void on_localend() {}


    private:

        std::size_t _step(const spec& s, std::size_t offset);
    };


    namespace internal {


        void spec_write_u8(spec& s, std::uint8_t x) noexcept;
        void spec_write_u16(spec& s, std::uint16_t x) noexcept;
        void spec_write_u32(spec& s, std::uint32_t x) noexcept;
        void spec_write_char(spec& s, char x) noexcept;
        void spec_write_opcode(spec& s, spec_opcode opcode) noexcept;
        void spec_write_bias(spec& s, bias b) noexcept;
        void spec_write_polarity(spec& s, polarity p) noexcept;
        void spec_write_qualifier(spec& s, qualifier q) noexcept;
        void spec_write_str(spec& s, std::string_view x) noexcept;

        std::uint8_t spec_read_u8(const spec& s, std::size_t offset, std::size_t& len) noexcept;
        std::uint16_t spec_read_u16(const spec& s, std::size_t offset, std::size_t& len) noexcept;
        std::uint32_t spec_read_u32(const spec& s, std::size_t offset, std::size_t& len) noexcept;
        char spec_read_char(const spec& s, std::size_t offset, std::size_t& len) noexcept;
        taul::spec_opcode spec_read_opcode(const spec& s, std::size_t offset, std::size_t& len) noexcept;
        taul::bias spec_read_bias(const spec& s, std::size_t offset, std::size_t& len) noexcept;
        taul::polarity spec_read_polarity(const spec& s, std::size_t offset, std::size_t& len) noexcept;
        taul::qualifier spec_read_qualifier(const spec& s, std::size_t offset, std::size_t& len) noexcept;
        std::string_view spec_read_str(const spec& s, std::size_t offset, std::size_t& len) noexcept;
    }
}

