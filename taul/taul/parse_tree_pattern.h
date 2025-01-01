

#pragma once


#include <optional>

#include "parse_tree.h"


namespace taul {


    // parse_tree_pattern encapsulates a pattern in a parse_tree which parse_tree
    // objects can be compared against

    // parse_tree_pattern patterns do not check if a parse_tree is marked as aborted

    // parse_tree_pattern primarily exists to ease unit testing of grammars

    class parse_tree_pattern final {
    public:

        class autocloser final {
        public:
            autocloser(parse_tree_pattern& client) noexcept;

            autocloser() = delete;
            autocloser(const autocloser&) = delete;
            autocloser(autocloser&&) noexcept = default;

            ~autocloser() noexcept;
            
            autocloser& operator=(const autocloser&) = delete;
            autocloser& operator=(autocloser&&) noexcept = default;


        private:
            parse_tree_pattern* _client = nullptr;
        };


        parse_tree_pattern(grammar gram);

        parse_tree_pattern() = delete;
        parse_tree_pattern(const parse_tree_pattern&) = default;
        parse_tree_pattern(parse_tree_pattern&&) noexcept = default;

        ~parse_tree_pattern() noexcept = default;

        parse_tree_pattern& operator=(const parse_tree_pattern&) = default;
        parse_tree_pattern& operator=(parse_tree_pattern&&) noexcept = default;


        // is_sealed returns if the parse_tree_pattern is *sealed*

        // a parse_tree_pattern is sealed if it has a root, and has no syntactic
        // nodes not yet closed

        // if a parse_tree_pattern is sealed it can no longer be ammended

        bool is_sealed() const noexcept;


        // match checks x against the parse tree pattern, returning if it
        // matches it or not

        // behaviour is undefined if is_sealed() == false

        // behaviour is undefined if x.is_sealed() == false

        bool match(const parse_tree& x) const noexcept;


        // TODO: only the first overload for lexical has been unit tested, w/ the
        //       others, and the failure/end methods, being expected just to wrap it

        // lexical creates a new node encapsulating a token, making it the
        // root node, or a child of the current node

        // behaviour is undefined if is_sealed() == true

        // behaviour is undefined if lpr is not part of the parse
        // tree pattern's associated grammar

        parse_tree_pattern& lexical(token tkn);
        parse_tree_pattern& lexical(lpr_ref lpr, source_pos pos, source_len len);

        // behaviour is undefined if the grammar has no LPR under name

        parse_tree_pattern& lexical(const str& name, source_pos pos, source_len len);

        // these are used to summarize adding tokens

        parse_tree_pattern& failure(source_pos pos, source_len len = 0);
        parse_tree_pattern& end(source_pos pos);

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
        // tree pattern's associated grammar

        parse_tree_pattern& syntactic(ppr_ref ppr, source_pos pos);

        // behaviour is undefined if the grammar has no PPR under name

        parse_tree_pattern& syntactic(const str& name, source_pos pos);

        // TODO: these ***_autoclose overloads have not been unit tested

        // these overloads return an autocloser to automate closing

        autocloser syntactic_autoclose(ppr_ref ppr, source_pos pos);
        autocloser syntactic_autoclose(const str& name, source_pos pos);

        // close makes the parent of the current node the current node,
        // sealing the parse_tree if the current node is the root node

        // behaviour is undefined if there is no current node

        parse_tree_pattern& close() noexcept;

        // skip adds len to the length of the current syntactic node of
        // the parse tree pattern

        // behaviour is undefined if there is no current node

        parse_tree_pattern& skip(source_len len) noexcept;

        // TODO: this skip overload has not been unit tested

        // this overload skips the length of x

        parse_tree_pattern& skip(std::string_view x) noexcept;

        // TODO: only the first overload for loose_syntactic has been unit tested,
        //       w/ the other being expected just to wrap it

        // loose_syntactic creates a new node encapsulating syntax, making
        // it the root node, or a child of the current node, and in either
        // case NOT making it the current node

        // loose_syntactic acts like a regular syntax node, except that the
        // internal structure of the syntax node is not asserted, only that
        // the outer-most node of the subtree exists

        // loose_syntactic given the above also specifies the expected aggregate
        // length of matched syntax nodes' internal contents

        // loose_syntactic exists to allow grammar production rule unit tests
        // to be decoupled from the particulars of other production rules which
        // aren't important to the test at hand

        // behaviour is undefined if is_sealed() == true

        // behaviour is undefined if ppr is not part of the parse
        // tree pattern's associated grammar

        parse_tree_pattern& loose_syntactic(ppr_ref ppr, source_pos pos, source_len len);

        // behaviour is undefined if the grammar has no PPR under name

        parse_tree_pattern& loose_syntactic(const str& name, source_pos pos, source_len len);


    private:

        static constexpr size_t _no_node = std::numeric_limits<size_t>::max();

        struct _node_t final {
            size_t parent = _no_node; // the parent node of this node, if any
            symbol_id id; // sym ID specifies if node is lexical or syntactic
            source_pos pos;
            source_len len;
            size_t children = 0;
            bool loose = false; // if node is a loose (syntactic) node


            bool has_parent() const noexcept;

            source_pos low_pos() const noexcept;
            source_pos high_pos() const noexcept;

            bool valid() const noexcept; // checks if id is valid for use in pattern

            bool is_lexical() const noexcept;
            bool is_syntactic() const noexcept;
        };


        grammar _gram;
        std::vector<_node_t> _nodes;
        size_t _current = _no_node;


        bool _has_current() const noexcept;

        _node_t& _get_current() noexcept;
        const _node_t& _get_current() const noexcept;

        void _try_incr_current_node_child_count();
        void _try_contribute_to_current_node_len(source_pos high_pos);
        void _make_latest_node_the_current_node();
        void _make_parent_node_the_current_node();


        // seperate matcher state from class body so match is thread-safe

        struct _match_state_t final {
            decltype(_nodes)::const_iterator    it;     // current pattern node iter
            decltype(_nodes)::const_iterator    end;    // past-the-end pattern node iter
            parse_tree::const_iterator          curr;   // current node iter in match target
            parse_tree::const_iterator          last;   // past-the-end iter in match target


            bool has_it() const;
            bool has_curr() const;

            void consume_one();
            void consume_subtree();
        };

        bool _match(const parse_tree& x) const;
        bool _match_step(_match_state_t& s, parse_tree::const_iterator parent) const;
        bool _match_lexical(_match_state_t& s, decltype(_nodes)::const_iterator our_it, parse_tree::const_iterator parent) const;
        bool _match_syntactic(_match_state_t& s, decltype(_nodes)::const_iterator our_it, parse_tree::const_iterator parent) const;
        bool _match_loose_syntactic(_match_state_t& s, decltype(_nodes)::const_iterator our_it, parse_tree::const_iterator parent) const;
        bool _match_children(_match_state_t& s, parse_tree::const_iterator parent) const;
    };
}

