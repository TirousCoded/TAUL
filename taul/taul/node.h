

#pragma once


#include <cstdint>
#include <memory>
#include <stdexcept>
#include <iterator>
#include <type_traits>
#include <atomic>
#include <optional>

#include "logger.h"
#include "rules.h"

#include "internal/util.h"
#include "internal/block_alloc.h"


namespace taul {


    class node;

    template<bool Const>
    class node_iter;

    namespace internal {


        struct node_data;


        node_data& get_node_data(node& target) noexcept;


        void patch_node(
            node& target,
            token tkn);

        void patch_node(
            node& target,
            const parser_rule& ppr,
            std::string_view str,
            source_pos pos);
    }

    class node_ctx;


    // node objects will operate as std::unique_ptr-like objects encapsulating exclusive
    // ownership of mutable node tree state

    // these nodes will be liberally mutable regarding their child/parent node associations

    class node final {
    public:

        friend struct taul::internal::node_data;
        friend taul::internal::node_data& taul::internal::get_node_data(node& target) noexcept;
        friend class taul::node_ctx;


        using iterator                  = node_iter<false>;
        using const_iterator            = node_iter<true>;
        using reverse_iterator          = std::reverse_iterator<iterator>;
        using const_reverse_iterator    = std::reverse_iterator<const_iterator>;


        // TODO:
        //      currently, our system allows for mutable iterators to be used to
        //      move ctor/assign via the state of nodes owned by parent nodes,
        //      which will BREAK our current system if performed...
        //
        //      thus, for now we'll file such move ctor/assign usage under the
        //      category of 'undefined behaviour', and just move on, w/ me not
        //      really being interested in changing much at the moment
        //
        //      later on though, we may want to revisit this code, and in addition
        //      to general refactoring, revise how it works to resolve this issue,
        //      and improve its design more generally
        //
        //      I'm thinking to replace internal::node_data w/ a frontent taul::node_info,
        //      which is then given taul::node's frontent API, and is the thing that
        //      taul::node and taul::node_iter then encapsulate, or something along
        //      these lines...


        node() = delete;
        node(const node&) = delete;
        node(node&& other) noexcept;

        ~node() noexcept;

        node& operator=(const node&) = delete;
        node& operator=(node&& rhs) noexcept;


        // returns a pointer to the node context associated w/ this node

        // this is used to check/compare node context associations

        const node_ctx* ctx() const noexcept;


        // these return if the node encapsulates a lexical or syntactic node

        // lexical nodes encapsulate tokens, syntactic nodes do not

        bool is_lexical() const noexcept;
        bool is_syntactic() const noexcept;

        // if lexical, this returns if the node encapsulates a failure token
        // if syntactic, this returns false

        bool is_failure() const noexcept;

        // these returns the lpr/ppr the node encapsulates an association w/, if any
        
        // lpr throws taul::lpr_association_error if the token has no lpr association
        // ppr throws taul::ppr_association_error if the token has no ppr association

        const lexer_rule& lpr() const;
        const parser_rule& ppr() const;

        // this returns the portion of string matched by the node

        std::string_view str() const noexcept;

        // this returns the source code position of the node's str() string in the
        // broader text from which it was matched

        source_pos pos() const noexcept;

        // returns the token, if any, encapsulated by the node

        std::optional<token> tkn() const noexcept;


        // these return iterators to this node, or other nodes in relation to it

        // if no such association exists, a past-the-end iterator is returned as
        // though returned by a call to end

        iterator iter() noexcept;
        iterator parent() noexcept;
        iterator left_child() noexcept;
        iterator right_child() noexcept;
        iterator left_sibling() noexcept;
        iterator right_sibling() noexcept;

        const_iterator iter() const noexcept;
        const_iterator parent() const noexcept;
        const_iterator left_child() const noexcept;
        const_iterator right_child() const noexcept;
        const_iterator left_sibling() const noexcept;
        const_iterator right_sibling() const noexcept;

        // children returns the number of child nodes attached to this node

        std::size_t children() const noexcept;


        // these return iterators encapsulating the range of child nodes
        // attached to this node, if any

        // these return non-decrementable past-the-end iterators if the
        // node has no child nodes

        iterator begin() noexcept;
        const_iterator cbegin() const noexcept;
        const_iterator begin() const noexcept;

        reverse_iterator rbegin() noexcept;
        const_reverse_iterator crbegin() const noexcept;
        const_reverse_iterator rbegin() const noexcept;

        iterator end() noexcept;
        const_iterator cend() const noexcept;
        const_iterator end() const noexcept;

        reverse_iterator rend() noexcept;
        const_reverse_iterator crend() const noexcept;
        const_reverse_iterator rend() const noexcept;


        // equiv returns if this node tree and other are structurally equivalent

        // node tree equivalence does not consider parent node associations

        bool equiv(const node& other) const noexcept;


        // attach attaches child nodes to this node

        // ownership of the node will be acquired by this node
        
        // the overload w/out place_before attaches the nodes to the end of the sequence
        // of attached child nodes

        // the overload w/ place_before attaches the nodes such that it comes before 
        // the place_before node, w/ this overload acting the same as the overload w/out
        // place_before when place_before is a past-the-end iterator

        // behaviour is undefined if place_before is not a child node of this node

        // behaviour is undefined if attached node is not associated w/ the same node 
        // context as this node is

        void attach(node&& x);
        void attach(node&& x, iterator place_before);
        
        // detach detaches the node at x from this node, returning a node which has
        // taken ownership of it

        // behaviour is undefined if x is not a child node of this node
        
        // detach_range detaches the nodes in the range of first and last, if any,
        // discarding these nodes

        // the last iterator is the range's past-the-end iterator

        // behaviour is undefined if the range of first and last is invalid

        // detach_all detaches *all* children of this node, if any,
        // discarding these nodes

        node detach(iterator x);
        void detach_range(iterator first, iterator last);
        void detach_all();


        std::string fmt() const;
        std::string fmt_tree(const char* tab = "    ") const;


    private:

        node_ctx* _ctx;
        internal::node_data* _data;


        node(
            internal::for_internal_use_tag, 
            node_ctx& ctx,
            const lexer_rule& lpr,
            std::string_view str,
            source_pos pos);

        node(
            internal::for_internal_use_tag, 
            node_ctx& ctx,
            token tkn);

        node(
            internal::for_internal_use_tag, 
            node_ctx& ctx,
            const parser_rule& ppr,
            std::string_view str,
            source_pos pos);


        // _is_bound returns if node is *unbound*, which is an intermediate
        // state used internally when the node is, thusly, *unbound*

        bool _is_bound() const noexcept;


        void _drop() noexcept;
        void _forget() noexcept;

        void _retarget() noexcept;


        void _attach_first_child(node&& x);
        void _attach_nonfirst_child_to_end(node&& x);
        void _attach_nonfirst_child_behind(node&& x, iterator place_before);

        node _detach_first_child(iterator x);
        node _detach_nonfirst_child(iterator x);


        void _fmt_tree(std::string& result, std::size_t tabs, const char* tab) const;
    };


    template<bool Const>
    class node_iter final {
    public:

        friend class node;
        friend class node_iter<false>;


        using this_t = node_iter<Const>;


        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = std::conditional_t<Const, const node, node>;
        using difference_type   = std::ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;


        inline node_iter();
        inline node_iter(const this_t& other);
        inline node_iter(this_t&& other) noexcept;

        inline ~node_iter() noexcept;

        inline this_t& operator=(const this_t& rhs);
        inline this_t& operator=(this_t&& rhs) noexcept;


        constexpr bool is_const() const noexcept { return Const; }


        // this returns if the iterator is a past-the-end iterator

        inline bool is_past_the_end() const noexcept;

        // this returns if the iterator is a decrementable past-the-end iterator

        // past-the-end node iterators may be 'decrementable' or 'non-decrementable'

        // decrementable past-the-end iterators are created by incrementing a 
        // non-past-the-end iterator but which has no right sibling, w/ these
        // iterators storing a referencing to this previous node and will revert
        // to it upon decrementing

        // non-decrementable past-the-end iterators lack the above association,
        // and are created w/ default construction

        // this returns false if the iterator is not a past-the-end iterator

        inline bool is_decrementable_pte() const noexcept;


        // all past-the-end iterators, no matter their decrement behaviour, will be
        // considered to all be equal to one another

        inline bool equal(const this_t& other) const noexcept;

        inline bool operator==(const this_t& rhs) const noexcept;
        inline bool operator!=(const this_t& rhs) const noexcept;


        // below behaviour is undefined if the iterator is past-the-end

        inline reference operator*() const;
        inline pointer operator->() const;


        // below behaviour is undefined if increment/decrement has no
        // node to iterate to (including for non-decrementable 
        // past-the-end iterators)

        // ++x and --x

        inline this_t& operator++();
        inline this_t& operator--();

        // x++ and x--

        inline this_t operator++(int);
        inline this_t operator--(int);


        inline node_iter<true> to_const() const noexcept;

        // node iterators may be implicitly converted to const node iterators

        inline operator node_iter<true>() const noexcept;


        inline std::string fmt() const;


    private:
        
        inline node_iter(internal::node_data* nd, internal::node_data* revert);


        // this is the node iterated to, w/ this being nullptr for 
        // past-the-end iterators

        internal::node_data* _nd;

        // for decrementable past-the-end iterators, this is the node to
        // revert back to on decrement

        internal::node_data* _revert;
    };


    namespace internal {


        struct node_data final {

            // this _node field is used to communicate to iterators which taul::node
            // they should dereference to, w/ this being updated upon move in order
            // to maintain these associations

            node* _node;

            bool _lexical = true;
            union {
                const lexer_rule* _lpr = nullptr;
                const parser_rule* _ppr;
            };
            std::string_view _str = "";
            source_pos _pos = 0;

            // nodes will take ownership of their left-child, and right-sibling, thus
            // creating a *chain* of node ownership for the tree

            node_data* _parent = nullptr;
            std::optional<node> _lc;
            node_data* _rc = nullptr;
            node_data* _ls = nullptr;
            std::optional<node> _rs;

            std::uint32_t _children = 0;


            node_data* _get_lc_data() const noexcept;
            node_data* _get_rs_data() const noexcept;
        };
    }


    class node_ctx final {
    public:

        friend class taul::node;


        node_ctx(const std::shared_ptr<logger>& lgr = nullptr);

        node_ctx(const node_ctx&) = delete;
        node_ctx(node_ctx&&) noexcept = delete;

        ~node_ctx() noexcept;

        node_ctx& operator=(const node_ctx&) = delete;
        node_ctx& operator=(node_ctx&&) noexcept = delete;


        // returns the number of *unique* nodes which currently exist

        std::size_t count() const noexcept;


        // this is used to create a new node via this node context

        // the implementation may throw exceptions if it failed due to something
        // like internal exhaustion of available resources

        node create(
            const lexer_rule& lpr,
            std::string_view str,
            source_pos pos = 0);

        node create(
            token tkn);

        node create(
            const parser_rule& ppr,
            std::string_view str,
            source_pos pos = 0);

        // performs a deep-copy of target, returning a clone associated w/
        // this node context

        // the implementation may throw exceptions if it failed due to something
        // like internal exhaustion of available resources

        node clone(const node& target);


    private:

        static constexpr std::size_t nodes_per_page = 48;


        std::shared_ptr<logger> _lgr;
        internal::block_alloc<sizeof(internal::node_data), nodes_per_page> _ba;


        // this is called to create and init new node_data

        internal::node_data* _create_node_data();

        // this is called to deinit and destroy old node_data

        void _cleanup_node_data(internal::node_data* x) noexcept;

        // these are used to perform node_data init/deinit

        void _init_node_data(internal::node_data* x) noexcept;
        void _deinit_node_data(internal::node_data* x) noexcept;

        // this updates the _node field of the node_data at x

        void _retarget_node_data(internal::node_data* x, taul::node* new_target) noexcept;
    };


    template<bool Const>
    inline node_iter<Const>::node_iter() 
        : node_iter(nullptr, nullptr) {}

    template<bool Const>
    inline node_iter<Const>::node_iter(const this_t& other) 
        : node_iter(other._nd, other._revert) {}

    template<bool Const>
    inline node_iter<Const>::node_iter(this_t&& other) noexcept
        : node_iter() {
        std::swap(_nd, other._nd);
        std::swap(_revert, other._revert);
    }

    template<bool Const>
    inline node_iter<Const>::~node_iter() noexcept {}

    template<bool Const>
    inline node_iter<Const>::this_t& node_iter<Const>::operator=(const this_t& rhs) {
        _nd = rhs._nd;
        _revert = rhs._revert;
        return *this;
    }

    template<bool Const>
    inline node_iter<Const>::this_t& node_iter<Const>::operator=(this_t&& rhs) noexcept {
        std::swap(_nd, rhs._nd);
        std::swap(_revert, rhs._revert);
        return *this;
    }

    template<bool Const>
    inline bool node_iter<Const>::is_past_the_end() const noexcept {
        return _nd == nullptr;
    }

    template<bool Const>
    inline bool node_iter<Const>::is_decrementable_pte() const noexcept {
        return is_past_the_end() && _revert != nullptr;
    }

    template<bool Const>
    inline bool node_iter<Const>::equal(const this_t& other) const noexcept {
        // if both are nullptr, then they're equal, as ALL past-the-end iterators
        // are considered equal, not matter their _revert values
        return _nd == other._nd;
    }

    template<bool Const>
    inline bool node_iter<Const>::operator==(const this_t& rhs) const noexcept {
        return equal(rhs);
    }

    template<bool Const>
    inline bool node_iter<Const>::operator!=(const this_t& rhs) const noexcept {
        return !equal(rhs);
    }

    template<bool Const>
    inline node_iter<Const>::reference node_iter<Const>::operator*() const {
        TAUL_ASSERT(!is_past_the_end());
        return *(_nd->_node);
    }

    template<bool Const>
    inline node_iter<Const>::pointer node_iter<Const>::operator->() const {
        TAUL_ASSERT(!is_past_the_end());
        return _nd->_node;
    }

    template<bool Const>
    inline node_iter<Const>::this_t& node_iter<Const>::operator++() {
        if (_nd) {
            const auto _rs = _nd->_get_rs_data();
            if (!_rs) {
                _revert = _nd;
            }
            _nd = _rs;
        }
        else throw std::runtime_error("illegal iterator increment!");
        return *this;
    }

    template<bool Const>
    inline node_iter<Const>::this_t& node_iter<Const>::operator--() {
        if (_nd && _nd->_ls) {
            _nd = _nd->_ls;
        }
        else if (_revert) {
            std::swap(_nd, _revert);
        }
        else throw std::runtime_error("illegal iterator decrement!");
        return *this;
    }

    template<bool Const>
    inline node_iter<Const>::this_t node_iter<Const>::operator++(int) {
        const auto old = *this;
        std::advance(*this, 1);
        return old;
    }

    template<bool Const>
    inline node_iter<Const>::this_t node_iter<Const>::operator--(int) {
        const auto old = *this;
        std::advance(*this, -1);
        return old;
    }

    template<bool Const>
    inline node_iter<true> node_iter<Const>::to_const() const noexcept {
        return node_iter<true>(_nd, _revert);
    }

    template<bool Const>
    inline node_iter<Const>::operator node_iter<true>() const noexcept {
        return to_const();
    }

    template<bool Const>
    inline std::string node_iter<Const>::fmt() const {
        if constexpr (Const) {
            return
                is_past_the_end() 
                ? "<past-the-end> (const)" 
                : std::format("{} (const)", **this);
        }
        else {
            return
                is_past_the_end()
                ? "<past-the-end>"
                : std::format("{}", **this);
        }
    }

    template<bool Const>
    inline node_iter<Const>::node_iter(internal::node_data* nd, internal::node_data* revert) 
        : _nd(nd), 
        _revert(revert) {}
}


template<>
struct std::formatter<taul::node> final : std::formatter<std::string> {
    auto format(const taul::node& x, format_context& ctx) const {
        return formatter<string>::format(x.fmt(), ctx);
    }
};

template<>
struct std::formatter<taul::node_iter<false>> final : std::formatter<std::string> {
    auto format(const taul::node_iter<false>& x, format_context& ctx) const {
        return formatter<string>::format(x.fmt(), ctx);
    }
};

template<>
struct std::formatter<taul::node_iter<true>> final : std::formatter<std::string> {
    auto format(const taul::node_iter<true>& x, format_context& ctx) const {
        return formatter<string>::format(x.fmt(), ctx);
    }
};

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::node& x) {
        return stream << x.fmt();
    }
}

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::node_iter<false>& x) {
        return stream << x.fmt();
    }
}

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::node_iter<true>& x) {
        return stream << x.fmt();
    }
}

