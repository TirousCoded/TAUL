

#pragma once


#include <string>
#include <vector>
#include <format>

#include "rules.h"


namespace taul {


    namespace internal {


        // we'll use no_index to specify a lack of association

        constexpr auto no_index = std::size_t(-1);
    }


    // taul::parse_tree encapsulates a parse tree which is immutable, except
    // that it allows for new incremental additions to be ammended to it

    // taul::parse_tree stores its parse tree nodes in an internal array,
    // organizing them in a depth-first manner, such that iteration across
    // this internal array performs a depth-first traversal of the tree

    // nodes identify themselves and one another via indices in this array

    // the parse_tree's node tree is built via an internal stack, w/ the
    // top node being the 'current' node, which the next node added will be
    // added as a child of

    // the usage of this internal stack is via 'open' and 'close' operations

    // the open operation adds a new node, making it a child of the current
    // node if there is one, and pushes it to the top of the stack

    // the close operation pops the current node from the top of the stack

    class parse_tree final {
    public:

        class node;


        using iterator          = std::vector<node>::const_iterator;
        using const_iterator    = iterator;


        parse_tree();
        parse_tree(const parse_tree& x);
        parse_tree(parse_tree&& x) noexcept;

        ~parse_tree() noexcept = default;

        parse_tree& operator=(const parse_tree& rhs);
        parse_tree& operator=(parse_tree&& rhs) noexcept;


        // finished returns if the parse_tree is *finished*, which
        // is defined as it both having a root node, and having
        // an empty node stack

        // if a parse_tree is finished it can no longer be ammended

        bool finished() const noexcept;


        // nodes returns the total node count of the parse_tree

        std::size_t nodes() const noexcept;

        bool has_nodes() const noexcept;


        // at returns the node at index ind in the parse_tree's
        // internal node array

        // throws std::out_of_range if there is no node at ind

        const node& at(std::size_t ind) const;

        // current returns the latest node added to the parse_tree

        // throws std::out_of_range if there is no current node

        const node& current() const;

        // root returns the root node parse_tree, which is always
        // the node at index 0

        // throws std::out_of_range if there is no current node

        const node& root() const;


        // the iterators of the parse_tree perform depth-first
        // traversals of the nodes of the tree

        // we might add in *local* iterator later if we need to

        iterator cbegin() const noexcept;
        iterator begin() const noexcept;

        iterator cend() const noexcept;
        iterator end() const noexcept;


        // parse_tree objects may be equality compared by value/structure

        // the mutable state regarding parse_tree construction is
        // not required to match for two parse_tree objects to be
        // deemed structurally equivalent

        bool equal(const parse_tree& other) const noexcept;

        bool operator==(const parse_tree& rhs) const noexcept;
        bool operator!=(const parse_tree& rhs) const noexcept;


        // open_lexical creates a new node encapsulating a token, 
        // pushing it to the node stack

        // if there is no current node, this node becomes the root,
        // otherwise, it is made a child of the current node

        // behaviour is undefined if finished() == true

        parse_tree& open_lexical(
            const lexer_rule& lpr,
            std::string_view str,
            source_pos pos = 0);
        
        parse_tree& open_lexical(token tkn);

        // open_syntactic creates a new node encapsulating syntax, 
        // pushing it to the node stack

        // if there is no current node, this node becomes the root,
        // otherwise, it is made a child of the current node

        // behaviour is undefined if finished() == true

        parse_tree& open_syntactic(
            const parser_rule& ppr,
            std::string_view str,
            source_pos pos = 0);

        // close pops the current node from the node stack, reverting
        // back to its parent node, if any, being the current node

        // behaviour is undefined if there is no current node

        parse_tree& close() noexcept;


        std::string fmt(const char* tab = "    ") const;


    private:

        std::vector<node> _nodes;

        // for our 'node stack', we'll not actually use a real stack

        std::size_t _current = internal::no_index;
    };

    class parse_tree::node final {
    public:

        friend class parse_tree;


        node();
        node(const node& x);
        node(node&& x) noexcept;

        ~node() noexcept = default;

        node& operator=(const node& rhs);
        node& operator=(node&& rhs) noexcept;


        // index returns the node's index in its parse_tree

        std::size_t index() const noexcept;


        // level returns the number of parent-to-child jumps it takes to
        // reach this node from the root

        std::size_t level() const noexcept;


        // these returns the nodes associated w/ this one in the parse_tree,
        // or past-the-end iterators if no such association has been established

        iterator parent() const noexcept;
        iterator left_sibling() const noexcept;
        iterator right_sibling() const noexcept;
        iterator left_child() const noexcept;
        iterator right_child() const noexcept;

        bool has_parent() const noexcept;
        bool has_left_sibling() const noexcept;
        bool has_right_sibling() const noexcept;

        std::size_t children() const noexcept;

        bool has_children() const noexcept;


        // these return if the node encapsulates a lexical or syntactic node

        // lexical nodes encapsulate tokens, syntactic nodes do not

        bool is_lexical() const noexcept;
        bool is_syntactic() const noexcept;

        // if lexical, this returns if the node encapsulates a failure token
        // if syntactic, this returns false

        bool is_failure() const noexcept;

        // these returns the LPR/PPR the node encapsulates an association w/, if any

        // lpr throws taul::lpr_association_error if the node has no LPR association
        // ppr throws taul::ppr_association_error if the node has no PPR association

        const lexer_rule& lpr() const;
        const parser_rule& ppr() const;

        // this returns the portion of string matched by the node

        std::string_view str() const noexcept;

        // this returns the source code position of the node's str() string in the
        // broader text from which it was matched

        source_pos pos() const noexcept;

        // returns the token, if any, encapsulated by the node

        std::optional<token> tkn() const noexcept;


        std::string fmt() const;


    private:

        struct data final {
            parse_tree* _owner = nullptr;

            std::size_t _index = internal::no_index;
            std::size_t _level = 0;

            bool _lexical = true;
            union {
                const lexer_rule* _lpr = nullptr;
                const parser_rule* _ppr;
            };
            std::string_view _str = "";
            source_pos _pos = 0;

            std::size_t _parent_index = internal::no_index;
            std::size_t _left_sibling_index = internal::no_index;
            std::size_t _right_sibling_index = internal::no_index;

            std::size_t _children = 0;

            std::size_t _right_child_index = internal::no_index;
        };

        data _data;


        parse_tree& _get_owner() const noexcept;

        bool _equal(const node& other) const noexcept;
    };
}


template<>
struct std::formatter<taul::parse_tree> final : std::formatter<std::string> {
    auto format(const taul::parse_tree& x, format_context& ctx) const {
        return formatter<string>::format(x.fmt(), ctx);
    }
};

template<>
struct std::formatter<taul::parse_tree::node> final : std::formatter<std::string> {
    auto format(const taul::parse_tree::node& x, format_context& ctx) const {
        return formatter<string>::format(x.fmt(), ctx);
    }
};

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::parse_tree& x) {
        return stream << x.fmt();
    }
}

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::parse_tree::node& x) {
        return stream << x.fmt();
    }
}

