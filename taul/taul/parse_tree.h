

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


    // taul::parse_tree encapsulates a parse tree which is immutable, except
    // that it allows for new incremental additions to be ammended to it

    // taul::parse_tree stores its parse tree nodes in an internal array,
    // organizing them in a depth-first manner, such that iteration across
    // this internal array performs a depth-first traversal of the tree

    // nodes identify themselves and one another via indices in this array

    class parse_tree final {
    public:

        class node;


        using iterator          = std::vector<node>::const_iterator;
        using const_iterator    = iterator;


        parse_tree(grammar gram);

        parse_tree() = delete;
        parse_tree(const parse_tree& rhs);
        parse_tree(parse_tree&&) noexcept = default;

        ~parse_tree() noexcept = default;

        parse_tree& operator=(const parse_tree& rhs);
        parse_tree& operator=(parse_tree&&) noexcept = default;


        // is_sealed returns if the parse_tree is *sealed*

        // a parse_tree is sealed if it has a root, and has no syntactic
        // nodes not yet closed

        // if a parse_tree is sealed it can no longer be ammended

        bool is_sealed() const noexcept;


        // TODO: for sanity, the below query methods have largely NOT been unit
        //       tested in the context of an unsealed parse_tree, as trying to do
        //       so would require unit testing each and every little detail of each
        //       state transition of the parse_tree


        // aborted returns if the parse_tree described is *incomplete*
        // as a result of it being a product of aborted parsing

        bool is_aborted() const noexcept;


        // nodes returns the total node count of the parse_tree

        size_t nodes() const noexcept;

        bool has_nodes() const noexcept;


        // at returns the node at index ind in the parse_tree's
        // internal node array

        // throws std::out_of_range if there is no node at ind

        const node& at(size_t ind) const;

        // root returns the root node parse_tree, which is always
        // the node at index 0

        // behaviour is undefined if is_sealed() == false

        const node& root() const;


        // TODO: we might add in *local* iterator later if we need to

        // the iterators of the parse_tree perform depth-first
        // traversals of the nodes of the tree

        iterator cbegin() const noexcept;
        iterator begin() const noexcept;

        iterator cend() const noexcept;
        iterator end() const noexcept;


        // parse_tree objects may be equality compared by value

        // whether *this and other are marked as 'aborted' or not
        // does not matter when judging structural equivalence

        // behaviour is undefined if is_sealed() == false

        bool equal(const parse_tree& other) const noexcept;

        bool operator==(const parse_tree& rhs) const noexcept;
        bool operator!=(const parse_tree& rhs) const noexcept;


        // TODO: only the first overload for lexical has been unit tested, w/ the
        //       others, and the failure/end methods, being expected just to wrap it

        // lexical creates a new node encapsulating a token, making it the
        // root node, or a child of the current node

        // behaviour is undefined if is_sealed() == true

        // behaviour is undefined if lpr is not part of the parse tree's
        // associated grammar

        parse_tree& lexical(token tkn);
        parse_tree& lexical(lpr_ref lpr, source_pos pos, source_len len);

        // behaviour is undefined if the grammar has no LPR under name

        parse_tree& lexical(const str& name, source_pos pos, source_len len);

        // these are used to summarize adding tokens

        parse_tree& failure(source_pos pos, source_len len = 0);
        parse_tree& end(source_pos pos);

        // TODO: only the first overload for syntactic has been unit tested, w/ the
        //       other being expected just to wrap it

        // syntactic creates a new node encapsulating syntax, making
        // it the root node, or a child of the current node, and in
        // either case making it the current node

        // the syntactic node's len value is defined by the largest
        // high_pos() value of its child nodes, or is defined as 0
        // if none of those values are above the low_pos() value of
        // the syntactic node itself

        // the syntactic node tolerates child nodes being defined w/
        // pos values which are below the low_pos() of the syntactic
        // node itself, w/ these child nodes' pos values not modifying
        // the source code range associated w/ the syntactic node
        // (the child node's high_pos() value will still, however)

        // behaviour is undefined if is_sealed() == true

        // behaviour is undefined if ppr is not part of the parse
        // tree's associated grammar

        parse_tree& syntactic(ppr_ref ppr, source_pos pos);

        // behaviour is undefined if the grammar has no PPR under name

        parse_tree& syntactic(const str& name, source_pos pos);

        // close makes the parent of the current node the current node,
        // sealing the parse_tree if the current node is the root node

        // behaviour is undefined if there is no current node

        parse_tree& close() noexcept;

        // skip adds len to the length of the current syntactic node of
        // the parse tree

        // behaviour is undefined if there is no current node

        parse_tree& skip(source_len len);

        // abort marks the parse_tree as being the result of an aborted
        // parsing process

        parse_tree& abort();


        std::string fmt(const char* tab = "    ") const;


    private:

        static constexpr auto _no_index = size_t(-1); // we'll use _no_index to specify a lack of association


        struct _state_t final {
            grammar _gram;
            std::vector<node> _nodes;
            size_t _current = _no_index;
            bool _aborted = false;
        };

        _state_t _state;


        inline bool _has_current() const noexcept { return _state._current != _no_index; }

        const node& _current() const;


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


        void _update_owner_ptrs();
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

        size_t index() const noexcept;


        // level returns the number of parent-to-child jumps it takes to
        // reach this node from the root

        size_t level() const noexcept;


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

        size_t children() const noexcept;

        bool has_children() const noexcept;


        // these are used to query what type of node this is

        bool is_lexical() const noexcept;
        bool is_syntactic() const noexcept;

        bool is_normal() const noexcept;
        bool is_failure() const noexcept;
        bool is_end() const noexcept;


        // id returns the symbol ID of the node

        symbol_id id() const noexcept;

        // pos/len return the position/length of the portion of source string 
        // matched by the node

        source_pos pos() const noexcept;
        source_len len() const noexcept;

        // low_pos returns pos()
        // high_pos returns pos() + len() (as source_pos)

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

            size_t _index = _no_index;
            size_t _level = 0;

            symbol_id _id = symbol_id{};
            source_pos _pos = 0;
            source_len _len = 0;
            std::optional<std::variant<lpr_ref, ppr_ref>> _rule = std::nullopt;

            size_t _parent_index = _no_index;
            size_t _left_sibling_index = _no_index;
            size_t _right_sibling_index = _no_index;

            size_t _children = 0;

            size_t _right_child_index = _no_index;
        };

        data _data;

        public:
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

