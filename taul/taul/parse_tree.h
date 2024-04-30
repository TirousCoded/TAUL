

#pragma once


#include <string>
#include <vector>
#include <format>
#include <optional>
#include <variant>

#include "str.h"
#include "source_code.h"
#include "grammar.h"
#include "symbols.h"


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

    // parse_tree nodes are either 'lexical' or 'syntactic'

    // lexical nodes are simply added as leaf nodes, w/ no children

    // syntactic nodes are added via 'open' and 'close' operations, w/ any
    // nodes added between their open/close calls being added as children 
    // of said syntactic node

    // internally, syntactic nodes are handled via a stack which dictates
    // which syntactic nodes are currently dictating scope, w/ the top
    // node being called the 'current' node, which the next node added will
    // be added as a child of

    // the open operation is encapsulated by the 'syntactic' method

    // the close operation is encapsulated by the 'close' method

    class parse_tree final {
    public:

        class node;


        using iterator          = std::vector<node>::const_iterator;
        using const_iterator    = iterator;


        parse_tree(grammar gram);

        parse_tree() = delete;
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


        // when a fatal syntax error occurs, an 'abort point' may
        // be added to the parse_tree, which indicates where the
        // fatal syntax error occurred

        // abort points don't otherwise interfere w/ the open/close
        // semantics of the parse_tree

        // contains_abort queries if the parse_tree contains an
        // abort point

        bool contains_abort() const noexcept;


        // nodes returns the total node count of the parse_tree

        std::size_t nodes() const noexcept;

        bool has_nodes() const noexcept;


        // at returns the node at index ind in the parse_tree's
        // internal node array

        // throws std::out_of_range if there is no node at ind

        const node& at(std::size_t ind) const;

        // current returns the latest syntactic node added to 
        // the parse_tree

        // throws std::out_of_range if there is no current node

        const node& current() const;

        // root returns the root node parse_tree, which is always
        // the node at index 0

        // throws std::out_of_range if there is no current node

        const node& root() const;


        // the iterators of the parse_tree perform depth-first
        // traversals of the nodes of the tree

        // TODO: we might add in *local* iterator later if we need to

        iterator cbegin() const noexcept;
        iterator begin() const noexcept;

        iterator cend() const noexcept;
        iterator end() const noexcept;


        // parse_tree objects may be equality compared by value

        // structural comparison requires both parse trees to
        // be 'finished'

        // behaviour is undefined if finished() == false
        // behaviour is undefined if other.finished() == false

        bool equal(const parse_tree& other) const noexcept;

        bool operator==(const parse_tree& rhs) const noexcept;
        bool operator!=(const parse_tree& rhs) const noexcept;


        // skip adds len to the length of the current syntactic node of
        // the parse tree

        // behaviour is undefined if there is no current node

        parse_tree& skip(source_len len);

        // lexical creates a new node encapsulating a token, making 
        // it a child of the current node

        // lexical nodes are leaf nodes, not allowing children

        // behaviour is undefined if there is no current node

        // behaviour is undefined if finished() == true

        // behaviour is undefined if lpr is not part of the parse
        // tree's associated grammar

        parse_tree& lexical(token tkn);
        parse_tree& lexical(lpr_ref lpr, source_pos pos, source_len len);

        // behaviour is undefined if the grammar has no LPR under name

        parse_tree& lexical(const str& name, source_pos pos, source_len len);

        // these are used to summarize adding tokens

        parse_tree& failure(source_pos pos, source_len len = 0);
        parse_tree& end(source_pos pos);

        // syntactic creates a new node encapsulating syntax, making 
        // it a child of the current node, then pushing it to the node
        // stack, making it the new current node

        // the source code length associated w/ the syntactic node
        // will be defined by the aggregate length of its children

        // if there is no root node, the node created becomes the root

        // behaviour is undefined if finished() == true

        // behaviour is undefined if ppr is not part of the parse
        // tree's associated grammar

        parse_tree& syntactic(ppr_ref ppr, source_pos pos);

        // behaviour is undefined if the grammar has no PPR under name

        parse_tree& syntactic(const str& name, source_pos pos);

        // close pops the current node from the node stack, reverting
        // back to its parent node, if any, being the current node

        // behaviour is undefined if there is no current node

        parse_tree& close() noexcept;

        // abort adds an abort point node to the parse_tree

        // abort point nodes are classified as syntactic nodes, as 
        // their symbol ID is a PPR ID sentinel, however they function
        // as leaf nodes, not allowing children

        // behaviour is undefined if there is no current node

        // behaviour is undefined if finished() == true

        parse_tree& abort(source_pos pos);


        std::string fmt(const char* tab = "    ") const;


    private:

        grammar _gram;
        std::vector<node> _nodes;

        // for our 'node stack', we'll not actually use a real stack

        std::size_t _current = internal::no_index;

        bool _contains_abort = false;


        inline bool _has_current() const noexcept { return _current != internal::no_index; }


        void _create_latest_node();
        
        void _setup_latest_node_basic_info(
            symbol_id id,
            source_pos pos,
            source_len len,
            std::optional<std::variant<lpr_ref, ppr_ref>> rule);
        
        void _setup_latest_node_relationships();
        
        void _setup_latest_node(
            symbol_id id,
            source_pos pos,
            source_len len,
            std::optional<std::variant<lpr_ref, ppr_ref>> rule);
        
        void _make_latest_node_the_current_node();

        void _contribute_latest_node_to_parent_len();
        void _contribute_current_node_to_parent_len();

        void _close_current_node();

        void _contribute_to_current_len(source_len len);


        void _leaf(
            symbol_id id,
            source_pos pos,
            source_len len,
            std::optional<std::variant<lpr_ref, ppr_ref>> rule);
        
        void _open_branch(
            symbol_id id,
            source_pos pos,
            std::optional<std::variant<lpr_ref, ppr_ref>> rule);

        void _close_branch();

        void _mark_abort();


        std::optional<std::variant<lpr_ref, ppr_ref>> _make_no_rule() const;
        std::optional<std::variant<lpr_ref, ppr_ref>> _make_lpr_rule(lpr_ref ref) const;
        std::optional<std::variant<lpr_ref, ppr_ref>> _make_ppr_rule(ppr_ref ref) const;
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


        // these are used to query what type of node this is

        bool is_lexical() const noexcept;
        bool is_syntactic() const noexcept;

        bool is_normal() const noexcept;
        bool is_failure() const noexcept;
        bool is_end() const noexcept;
        bool is_abort() const noexcept;


        // id returns the symbol ID of the node

        symbol_id id() const noexcept;

        // pos/len return the position/length of the portion of source string 
        // matched by the node

        source_pos pos() const noexcept;
        source_len len() const noexcept;

        // low_pos returns pos()
        // high_pos returns pos() + len() (as source_pos)

        // TODO: these two have not been unit tested

        inline source_pos low_pos() const noexcept { return pos(); }
        inline source_pos high_pos() const noexcept { return pos() + len(); }

        // lpr/ppr returns the LPR/PPR the node encapsulates, if any

        std::optional<lpr_ref> lpr() const;
        std::optional<ppr_ref> ppr() const;

        // str returns the portion of string matched by the node

        // str behaviour is undefined if src is not the correct source string to use

        taul::str str(str src) const;

        // tkn returns the token, if any, encapsulated by the node

        std::optional<token> tkn() const;


        std::string fmt() const;


    private:

        struct data final {
            parse_tree* _owner = nullptr;

            std::size_t _index = internal::no_index;
            std::size_t _level = 0;

            symbol_id _id = symbol_id{};
            source_pos _pos = 0;
            source_len _len = 0;
            std::optional<std::variant<lpr_ref, ppr_ref>> _rule = std::nullopt;

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

