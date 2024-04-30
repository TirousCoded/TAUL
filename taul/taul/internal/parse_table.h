

#pragma once


#include <variant>
#include <unordered_set>
#include <unordered_map>

#include "../hashing.h"
#include "../symbol_range.h"
#include "../symbol_set.h"

#include "id_grouper.h"


#define _DUMP_FIRST_SET_BUILD_PROCESS_LOG 0
#define _DUMP_FOLLOW_SET_BUILD_PROCESS_LOG 0
#define _DUMP_PARSE_TABLE_USE_CASE_ADDING_LOG 0


namespace taul::internal {


    // see this wikipedia article for the theory behind LL(1) parsing
    //      https://en.wikipedia.org/wiki/LL_parser#Parser_implementation_in_C++


    // the below is a quick write-up detailing the design of our backend

    // this write-up is a tad messy, and can be improved, but will suffice


    // (basics)

    // TODO: in our impl code, comments may sometimes get terms like 
    //       'non-terminal' and 'rule' mixed up

    // our system will break-up LPRs and PPRs into a lower-level
    // form which represents each non-terminal w/ much simpler formal
    // language semantics in which each non-terminal rule is kept 
    // vary simple, and a number of 'helper' non-terminal are used 
    // to build larger composite patterns

    // these lower-level forms will come in the form of two parse
    // tables: one for the lexer, and one for the parser

    // 'non-terminals' refer to symbol IDs used to identify LPRs/PPRs 
    // and helpers, and 'rules' refer to definitions which map 
    // non-terminals to an array of 'terms', w/ each non-terminal 
    // being able to have multiple rules, defining each of the 
    // alternative productions for it

    // 'terms' each specify the symbols rewritten to by the application
    // of a rule during processing


    // (visual example)

    // the following is a visual example of our main/helper non-terminal
    // distinction, and how complex rules are broken down into much
    // simpler lower-level ones using helpers:

    // (before)

    //      A           : a ( b c ) d ( e f | g h i ) j ;

    // (after)

    //      A           : a A/1 d A/2 j ;
    //      A/1         : b c ;
    //      A/2         : e f ;
    //      A/2         : g h i ;

    // above A is the main non-terminal, and A/1 and A/2 are helpers


    // (assertions)

    // in order to impl TAUL's '&' (lookahead) and '-' (lookahead-not) 
    // exprs, we'll have it be that parse table entry items may be 
    // marked as 'assertions'

    // assertion-marked items correspond to parse stack items which 
    // are instructed to not consume any input upon their processing, 
    // thus letting us 'assert' things, w/out consuming input

    // take note that ONLY terminals may be assertion-marked

    // I'm fairly certain that assertion-marked terms don't need to be
    // treated differently w/ regards to FIRST/FOLLOW sets

    // there are four principle reasons I think this:

    //      1) given a non-terminal defined by a rule starting w/ an 
    //         assertion-marked term, it's FIRST set shouldn't really
    //         care, as it just cares about the *required* inputs needed
    //         to branch, not if those inputs are consumed

    //      2) given a non-terminal defined by a rule starting w/ an
    //         assertion-marked term, and w/ epsilon in its FIRST set,
    //         its FOLLOW set should be fine, as its follow set is defined
    //         according to other non-terminals following it, which act
    //         according to their immediate input, w/out caring if
    //         that input was or wasn't *asserted upon* prior

    //      3) given a rule starting w/ terms 'A' and 'B', and A is assertion-marked,
    //         and B's FIRST set is a *superset* of A's, this should be fine,
    //         as A defines the inputs that will actually survive both A and B,
    //         so what else is in B's FIRST set is irrelevant

    //      4) given a rule starting w/ terms 'A' and 'B', and A is assertion-marked,
    //         and B's FIRST set is a *subset* of A's, this should be fine, as
    //         A will define inputs that won't survive past B, but this shouldn't
    //         break the parse table, just add some redundency
    //              * #4 here provides a potential for parse table optimization


    // (helper transparency)

    // when it comes to things like the parser output, notice that
    // helper non-terminals are to be 'transparent'

    // transparent here means that when the non-terminal symbol of the
    // helper is being processed, it should not result in any reporting
    // of the non-terminal to the output, w/ neither the beginning, nor
    // ending of its scope being acknowledged
    
    // also, all terminals and non-terminals reported therein are 
    // associated instead w/ the inner-most nested non-terminal who's
    // scope the transparent helper symbol exists within

    // this is used to allow for the family of a main non-terminal and
    // its helpers to be able to be used together such that the end-user 
    // is provided w/ a view of them forming one large composite pattern

    // below is an example to better visualize what this means:

    // (what's actually going on)

    //  > begin non-terminal A
    //      > terminal #1
    //      > begin non-terminal B      <- let B be our transparent helper
    //          > terminal #2
    //          > terminal #3
    //      > end non-terminal B
    //      > terminal #4
    //  > end non-terminal A

    // (what the end-user sees)

    //  > begin non-terminal A
    //      > terminal #1
    //      > terminal #2               <- B is not acknowledged
    //      > terminal #3               <- thus, #2 and #3 get associated w/ A
    //      > terminal #4
    //  > end non-terminal A


    // (ID mappings)

    // for each parse table, a range of IDs will be defined which 
    // may be used to identify each non-terminal

    // given a range of non-terminal IDs [START, END] usable, they 
    // will be mapped in the following manner:


    //      [ START ][ main IDs ][ helper IDs ][ extra ][ END ]


    // [ START ] and [ END ] are the start/end of the ID range
    // (notice that these two are bounds indicators, and do not 
    // represent any allocation of IDs)

    // [ main IDs ] are the IDs of the main LPRs/PPRs themselves, 
    // w/ these ALWAYS starting at the vary beginning of the range

    // [ helper IDs ] are the IDs of the helper non-terminals used
    // to help define the main LPR/PPR non-terminals, and one another

    // [ extra ] is all the extra, unused, non-terminal IDs


    // seperated this out so we can properly give it a std::hash impl

    struct pt_key final {
        symbol_id nonterminal;
        group_id terminal_group;


        inline bool operator==(const pt_key& other) const noexcept {
            return 
                nonterminal == other.nonterminal && 
                terminal_group == other.terminal_group;
        }
        inline bool operator!=(const pt_key& other) const noexcept { return !(*this == other); }


        template<typename Symbol>
        inline std::string fmt(const id_grouper<Symbol>& grouper) const {
            return std::format(
                "{} / {} {}",
                nonterminal, terminal_group, grouper.get_symbol_range(terminal_group));
        }
    };
}

namespace std {
    template<>
    struct std::hash<taul::internal::pt_key> {
        inline std::size_t operator()(const taul::internal::pt_key& k) const noexcept {
            return taul::hash_combine(taul::hash(k.nonterminal), taul::hash(k.terminal_group));
        }
    };
}

namespace taul::internal {


    // these encapsulate parse table 'terms' which is what we'll call the component
    // terminal/non-terminals of each rule

    template<typename Symbol>
    struct pt_terminal final {
        symbol_range<Symbol> ids; // the IDs of the terminal symbols
        bool assertion; // if the terminal term is an assertion


        inline std::string fmt() const {
            return
                assertion
                ? std::format("assert{}", ids)
                : std::format("{}", ids);
        }
    };

    struct pt_nonterminal final {
        symbol_id id; // the ID of the non-terminal symbol


        inline std::string fmt() const {
            return std::format("{}", id);
        }
    };

    template<typename Symbol>
    struct pt_term final {
        std::variant<pt_terminal<Symbol>, pt_nonterminal> u;


        inline bool is_terminal() const noexcept { return u.index() == 0; }
        inline bool is_nonterminal() const noexcept { return u.index() == 1; }

        inline const pt_terminal<Symbol>& terminal() const noexcept { TAUL_ASSERT(is_terminal()); return std::get<0>(u); }
        inline const pt_nonterminal& nonterminal() const noexcept { TAUL_ASSERT(is_nonterminal()); return std::get<1>(u); }


        inline std::string fmt() const {
            return
                is_terminal()
                ? terminal().fmt()
                : nonterminal().fmt();
        }


        inline static pt_term<Symbol> init_terminal(symbol_range<Symbol> ids, bool assertion = false) {
            return pt_term<Symbol>{
                decltype(u)(std::in_place_index_t<0>{}, std::move(ids), std::move(assertion)),
            };
        }
        template<typename U>
        inline static pt_term<Symbol> init_terminal(const U& low, const U& high, bool assertion = false) {
            return init_terminal(symbol_range<Symbol>::create(low, high), assertion);
        }
        inline static pt_term<Symbol> init_nonterminal(symbol_id id) {
            return pt_term<Symbol>{
                decltype(u)(std::in_place_index_t<1>{}, std::move(id)),
            };
        }
    };


    // this encapsulates a 'rule' in our parse table

    template<typename Symbol>
    struct pt_rule final {
        symbol_id id; // the ID of the non-terminal symbol of the rule

        std::vector<pt_term<Symbol>> terms = {}; // the terms defining the rule's production


        inline std::string fmt() const {
            std::string terms_str{};
            for (const auto& I : terms) {
                terms_str += std::format(" {}", I.fmt());
            }
            return std::format("{} :{} ;", id, terms_str);
        }
    };


    // this struct contains data used during build_mappings, and diagnostic data
    // generated therein, w/ this data being for diagnostic purposes

    template<typename Symbol>
    struct parse_table_build_details final {

        // build data

        std::unordered_set<symbol_id> defined_nonterminals; // non-terminals defined in rules vector

        // IMPORTANT: the final step in parse table building move-assigns first_sets_A, follow_sets_A,
        //            and prefix_sets_A from *this into the final parse table

        // given A -> w, for each A there may be multiple possible w values

        // thus below, the '_A' sets are for each A, and the '_w' sets are
        // for each unique w (specified by a rules vector index)

        std::unordered_map<symbol_id, symbol_set<Symbol>> first_sets_A; // FIRST sets (Fi(A))
        std::unordered_map<symbol_id, symbol_set<Symbol>> follow_sets_A; // FOLLOW sets (Fo(A))

        std::vector<symbol_set<Symbol>> first_sets_w; // FIRST sets (Fi(w))

        // these 'prefix sets' are the sets that actually define the prefixes of
        // each non-terminal, w/ these being equal to their FIRST sets (Fi(A)), plus
        // their FOLLOW sets (Fo(A)) if the FIRST set contained epsilon

        std::unordered_map<symbol_id, symbol_set<Symbol>> prefix_sets_A; // prefix sets (Pfx(A))

        std::vector<symbol_set<Symbol>> prefix_sets_w; // prefix sets (Pfx(w))

        // error diagnostics

        // if the build process aborts due to the below, then it means that the larger impl
        // is likely broken, as code external to this part of the impl is responsible for
        // ensuring that such error states are never allowed to arise

        // if build_mappings sets any of the below error states to true, then it will abort 
        // the rest of the build process

        // TODO: the below error states don't actually cover *all* the internal errors possible,
        //       w/ things like, for example, checking that non-terminal IDs are in fact actual
        //       non-terminal IDs (ie. not *just* not a terminal ID) not being checked for, or
        //       checking to see if a terminal ID is one actual valid for use (ie. LPR IDs have
        //       a whole 16-bit range defined, but that doesn't mean all of them are valid for use)
        //
        //       these are oversights that I've decided not to bother w/ for now but do keep this
        //       in mind w/ regards to later refactors/revisions to this impl code
        //
        //       the reason for the above is due to how addressing them *properly* would require 
        //       additional subsystems to be made to test for them
        //
        //       in specific, it would require us adding symbol_traits stuff for the domain of
        //       non-terminal IDs for the specialization, which isn't too much, but I'm still to 
        //       lazy to do it, lol (note: I did add this, just need to add the internal errors)

        bool nonterminal_id_is_terminal_id = false; // if rule ID is (improperly) a legal terminal ID
        bool terminal_ids_not_in_legal_range = false; // if rule term(s) ref IDs not in legal terminal range as terminal IDs
        bool nonterminal_not_in_rules_vector = false; // if rule term(s) ref non-terminal not defined in rules vector

        inline bool no_internal_errors() const noexcept {
            return 
                !nonterminal_id_is_terminal_id && 
                !terminal_ids_not_in_legal_range && 
                !nonterminal_not_in_rules_vector;
        }

        // it's okay if during TAUL compilation collisions is populated during build_mappings,
        // as it's the mechanism by which we detect such errors

        std::unordered_set<pt_key> collisions = {}; // set of keys w/ multiple potential mappings


        inline std::string fmt(const id_grouper<Symbol>& grouper, const char* tab = "    ") const;
    };

    template<typename Symbol>
    inline std::string taul::internal::parse_table_build_details<Symbol>::fmt(const id_grouper<Symbol>& grouper, const char* tab) const {
        TAUL_ASSERT(tab);
        std::string result{};
        result += "(parse_table_build_details)";
        result += "\ndefined non-terminals:";
        for (const auto& I : defined_nonterminals) {
            result += std::format(
                "\n{}{}",
                tab, I);
        }
        result += "\nFIRST sets (Fi(A)):";
        for (const auto& I : first_sets_A) {
            result += std::format(
                "\n{}{} -> {}",
                tab, I.first, I.second);
        }
        result += "\nFIRST sets (Fi(w)):";
        {
            std::size_t i = 0;
            for (const auto& I : first_sets_w) {
                result += std::format(
                    "\n{}(rule {}) -> {}",
                    tab, i, I);
                i++;
            }
        }
        result += "\nFOLLOW sets (Fo(A)):";
        for (const auto& I : follow_sets_A) {
            result += std::format(
                "\n{}{} -> {}",
                tab, I.first, I.second);
        }
        result += "\nprefix sets (Pfx(A)):";
        for (const auto& I : prefix_sets_A) {
            result += std::format(
                "\n{}{} -> {}",
                tab, I.first, I.second);
        }
        result += "\nprefix sets (Pfx(w)):";
        {
            std::size_t i = 0;
            for (const auto& I : prefix_sets_w) {
                result += std::format(
                    "\n{}(rule {}) -> {}",
                    tab, i, I);
                i++;
            }
        }
        result += std::format("\nnonterminal_id_is_terminal_id == {}", nonterminal_id_is_terminal_id);
        result += std::format("\nterminal_ids_not_in_legal_range == {}", terminal_ids_not_in_legal_range);
        result += std::format("\nnonterminal_not_in_rules_vector == {}", nonterminal_not_in_rules_vector);
        result += "\ncollisions:";
        for (const auto& I : collisions) {
            result += std::format(
                "\n{}{}",
                tab, I.fmt(grouper));
        }
        return result;
    }


    // our main parse table type

    template<typename Symbol>
    struct parse_table final {
        std::vector<pt_rule<Symbol>> rules = {}; // vector of parse table rules
        id_grouper<Symbol> grouper = {}; // ID grouper used to help define terminals
        std::unordered_map<pt_key, std::size_t> mappings = {}; // maps keys to rule indices

        // these are for giving our LPRs/PPRs their FIRST/FOLLOW/prefix sets, w/ these
        // being moved from parse_table_build_details during the final step in building

        std::unordered_map<symbol_id, symbol_set<Symbol>> first_sets_A;
        std::unordered_map<symbol_id, symbol_set<Symbol>> follow_sets_A;
        std::unordered_map<symbol_id, symbol_set<Symbol>> prefix_sets_A;


        // add_rule adds new rules to the end of the parse table's rule vector

        inline parse_table<Symbol>& add_rule(symbol_id nonterminal);

        // add_terminal and add_nonterminal adds new terms to the rule at index
        // in the rules vector

        // behaviour is undefined if index is out-of-bounds

        inline parse_table<Symbol>& add_terminal(std::size_t index, symbol_id low, symbol_id high, bool assertion = false);
        inline parse_table<Symbol>& add_terminal(std::size_t index, const typename symbol_traits<Symbol>::preferred_type& low, const typename symbol_traits<Symbol>::preferred_type& high, bool assertion = false);

        inline parse_table<Symbol>& add_nonterminal(std::size_t index, symbol_id id);

        // build_mappings performs generation of parse table mappings

        // if a mapping would result in overwriting an existing one, it is forgone,
        // and its key is added to the collision set

        inline parse_table<Symbol>& build_mappings(parse_table_build_details<Symbol>& details);


    private:

        // helper methods to break-up build_mappings

        inline void _build_defined_nonterminals(parse_table_build_details<Symbol>& details);
        inline void _check_for_nonterminal_id_is_terminal_id(parse_table_build_details<Symbol>& details);
        inline void _check_for_refs_to_terminal_ids_not_in_legal_range(parse_table_build_details<Symbol>& details);
        inline void _check_for_refs_to_nonterminal_not_in_rules_vector(parse_table_build_details<Symbol>& details);
        inline void _build_first_sets(parse_table_build_details<Symbol>& details);
        inline void _build_follow_sets(parse_table_build_details<Symbol>& details);
        inline void _build_prefix_sets(parse_table_build_details<Symbol>& details);
        inline void _populate_id_grouper(parse_table_build_details<Symbol>& details);
        inline void _populate_parse_table_and_check_for_collisions(parse_table_build_details<Symbol>& details);
        inline void _move_assign_first_follow_and_prefix_sets(parse_table_build_details<Symbol>& details);


    public:

        // lookup returns the index, if any, of the parse table rule under k

        // if multiple such rules exist, the one returned is arbitrary

        // lookup behaviour is undefined prior to build_mappings being called

        inline std::optional<std::size_t> lookup(const pt_key& k) const noexcept;
        inline std::optional<std::size_t> lookup(symbol_id nonterminal, group_id terminal_group) const noexcept;


        inline std::string fmt(const char* tab = "    ") const;
    };

    template<typename Symbol>
    inline parse_table<Symbol>& taul::internal::parse_table<Symbol>::add_rule(symbol_id nonterminal) {
        rules.push_back(pt_rule<Symbol>{ nonterminal });
        return *this;
    }

    template<typename Symbol>
    inline parse_table<Symbol>& taul::internal::parse_table<Symbol>::add_terminal(std::size_t index, symbol_id low, symbol_id high, bool assertion) {
        TAUL_ASSERT(index < rules.size());
        rules[index].terms.push_back(pt_term<Symbol>::init_terminal(symbol_range<Symbol>::create(low, high), assertion));
        return *this;
    }

    template<typename Symbol>
    inline parse_table<Symbol>& taul::internal::parse_table<Symbol>::add_terminal(std::size_t index, const typename symbol_traits<Symbol>::preferred_type& low, const typename symbol_traits<Symbol>::preferred_type& high, bool assertion) {
        return add_terminal(index, symbol_traits<Symbol>::id(low), symbol_traits<Symbol>::id(high), assertion);
    }

    template<typename Symbol>
    inline parse_table<Symbol>& taul::internal::parse_table<Symbol>::add_nonterminal(std::size_t index, symbol_id id) {
        TAUL_ASSERT(index < rules.size());
        rules[index].terms.push_back(pt_term<Symbol>::init_nonterminal(id));
        return *this;
    }

    template<typename Symbol>
    inline parse_table<Symbol>& taul::internal::parse_table<Symbol>::build_mappings(parse_table_build_details<Symbol>& details) {
        details = std::remove_reference_t<decltype(details)>(); // reset details
        _build_defined_nonterminals(details);
        _check_for_nonterminal_id_is_terminal_id(details);
        _check_for_refs_to_terminal_ids_not_in_legal_range(details);
        _check_for_refs_to_nonterminal_not_in_rules_vector(details);
        // abort on error
        if (details.nonterminal_id_is_terminal_id) return *this;
        if (details.terminal_ids_not_in_legal_range) return *this;
        if (details.nonterminal_not_in_rules_vector) return *this;
        _build_first_sets(details);
        _build_follow_sets(details);
        _build_prefix_sets(details);
        _populate_id_grouper(details);
        _populate_parse_table_and_check_for_collisions(details);
        _move_assign_first_follow_and_prefix_sets(details);
        return *this;
    }

    template<typename Symbol>
    inline void taul::internal::parse_table<Symbol>::_build_defined_nonterminals(parse_table_build_details<Symbol>& details) {
        for (const auto& I : rules) {
            details.defined_nonterminals.insert(I.id);
        }
    }

    template<typename Symbol>
    inline void taul::internal::parse_table<Symbol>::_check_for_nonterminal_id_is_terminal_id(parse_table_build_details<Symbol>& details) {
        for (const auto& I : details.defined_nonterminals) {
            if (!symbol_traits<Symbol>::legal_id(I)) continue;
            details.nonterminal_id_is_terminal_id = true;
            return;
        }
    }

    template<typename Symbol>
    inline void taul::internal::parse_table<Symbol>::_check_for_refs_to_terminal_ids_not_in_legal_range(parse_table_build_details<Symbol>& details) {
        for (const auto& I : rules) {
            for (const auto& J : I.terms) {
                if (J.is_nonterminal()) continue;
                const bool a = symbol_traits<Symbol>::legal_id(J.terminal().ids.low);
                const bool b = symbol_traits<Symbol>::legal_id(J.terminal().ids.high);
                if (a && b) continue;
                details.terminal_ids_not_in_legal_range = true;
                return;
            }
        }
    }
    
    template<typename Symbol>
    inline void taul::internal::parse_table<Symbol>::_check_for_refs_to_nonterminal_not_in_rules_vector(parse_table_build_details<Symbol>& details) {
        for (const auto& I : rules) {
            for (const auto& J : I.terms) {
                if (J.is_terminal()) continue;
                if (details.defined_nonterminals.contains(J.nonterminal().id)) continue;
#if 0
                TAUL_LOG(make_stderr_logger(), 
                    "nonterminal_not_in_rules_vector arose due to {} (is LPR parse table: {})",
                    J.fmt(), std::is_same_v<Symbol, glyph>);
#endif
                details.nonterminal_not_in_rules_vector = true;
                return;
            }
        }
    }
    
    template<typename Symbol>
    inline void taul::internal::parse_table<Symbol>::_build_first_sets(parse_table_build_details<Symbol>& details) {
        details.first_sets_A.reserve(details.defined_nonterminals.size());
        // ensure every rule has an associated FIRST set
        for (const auto& I : details.defined_nonterminals) {
            details.first_sets_A.try_emplace(I, symbol_set<Symbol>{});
        }
        details.first_sets_w.resize(rules.size());
#if _DUMP_FIRST_SET_BUILD_PROCESS_LOG
        TAUL_LOG(make_stderr_logger(), "building FIRST sets!");
#endif
        while (true) {
#if _DUMP_FIRST_SET_BUILD_PROCESS_LOG
            TAUL_LOG(make_stderr_logger(), "*** cycle ***");
#endif
            bool changed = false;
            for (std::size_t i = 0; i < rules.size(); i++) {
#if _DUMP_FIRST_SET_BUILD_PROCESS_LOG
                TAUL_LOG(make_stderr_logger(), "> rule {}", i);
#endif
                const auto& I = rules[i];
                auto& set_A = details.first_sets_A[I.id];
                auto& set_w = details.first_sets_w[i];
#if _DUMP_FIRST_SET_BUILD_PROCESS_LOG
                TAUL_LOG(make_stderr_logger(), "before:");
                TAUL_LOG(make_stderr_logger(), "    set_A == {}", set_A);
                TAUL_LOG(make_stderr_logger(), "    set_w == {}", set_w);
#endif

                bool found_nonempty_term = false;
                for (const auto& J : I.terms) {
                    // add ID's of front terminal
                    if (J.is_terminal()) {
#if _DUMP_FIRST_SET_BUILD_PROCESS_LOG
                        TAUL_LOG(make_stderr_logger(), "    -> adding {}", front.terminal().ids);
#endif
                        if (!set_w.includes_range(J.terminal().ids)) changed = true;
                        set_w.add_range(J.terminal().ids);

                        found_nonempty_term = true;
                        break;
                    }
                    // add FIRST set of front non-terminal
                    if (J.is_nonterminal()) {
                        TAUL_ASSERT(details.first_sets_A.contains(J.nonterminal().id));
                        const auto& J_first_set_A = details.first_sets_A.at(J.nonterminal().id);
                        const auto other =
                            symbol_set<Symbol>(J_first_set_A)
                            .remove_epsilon(); // gotta remember to remove epsilon!
#if _DUMP_FIRST_SET_BUILD_PROCESS_LOG
                        TAUL_LOG(make_stderr_logger(), "    -> adding {}", other);
#endif
                        if (!set_w.includes_set(other)) changed = true;
                        set_w.add_set(other);

                        if (J_first_set_A.includes_epsilon()) continue;
                        found_nonempty_term = true;
                        break;
                    }
                }
                if (!found_nonempty_term) {
#if _DUMP_FIRST_SET_BUILD_PROCESS_LOG
                    TAUL_LOG(make_stderr_logger(), "    -> adding epsilon");
#endif
                    if (!set_w.includes_epsilon()) changed = true;
                    set_w.add_epsilon();
                }

                // add latest from set_w to set_A
                set_A.add_set(set_w);
#if _DUMP_FIRST_SET_BUILD_PROCESS_LOG
                TAUL_LOG(make_stderr_logger(), "after:");
                TAUL_LOG(make_stderr_logger(), "    set_A == {}", set_A);
                TAUL_LOG(make_stderr_logger(), "    set_w == {}", set_w);
#endif
            }
            // keep looping until FIRST sets stop changing
            if (!changed) break;
        }
    }

    template<typename Symbol>
    inline void taul::internal::parse_table<Symbol>::_build_follow_sets(parse_table_build_details<Symbol>& details) {
        details.follow_sets_A.reserve(details.defined_nonterminals.size());
#if _DUMP_FOLLOW_SET_BUILD_PROCESS_LOG
        TAUL_LOG(make_stderr_logger(), "building FOLLOW sets!");
#endif
        // ensure every rule has an associated FOLLOW set w/ expected defaults
        for (const auto& I : details.defined_nonterminals) {
            auto& set_A = details.follow_sets_A[I];
#if _DUMP_FOLLOW_SET_BUILD_PROCESS_LOG
            TAUL_LOG(make_stderr_logger(), "*** adding defaults (for {}) ***", I);
            TAUL_LOG(make_stderr_logger(), "before:");
            TAUL_LOG(make_stderr_logger(), "    set_A == {}", set_A);
#endif
            // for FOLLOW sets for parse tables, Fo(A) is equal to the inverse of Fi(A),
            // as symbols can be followed up by ANYTHING, however that *everything*
            // needs to have Fi(A) subtracted from it, so as to ensure that it does
            // not create ambiguity w/ the non-nullable rules of the non-terminal
            set_A =
                symbol_set<Symbol>()
                .add_all()
                .remove_set(details.first_sets_A[I])
                .remove_epsilon();
#if _DUMP_FOLLOW_SET_BUILD_PROCESS_LOG
            TAUL_LOG(make_stderr_logger(), "after:");
            TAUL_LOG(make_stderr_logger(), "    set_A == {}", set_A);
#endif
        }

        // TODO: this is our old FOLLOW set populating code, which impls traditional LL(1) semantics,
        //       w/ me holding out on deleting this in case we may need it due to our design breaking
#if 0
        // populate FOLLOW sets
        if constexpr (std::is_same_v<Symbol, token>) {
#if _DUMP_FOLLOW_SET_BUILD_PROCESS_LOG
            TAUL_LOG(make_stderr_logger(), "*** populating FOLLOW sets ***");
#endif
            while (true) {
                bool changed = false;
                for (std::size_t i = 0; i < rules.size(); i++) {
#if _DUMP_FOLLOW_SET_BUILD_PROCESS_LOG
                    TAUL_LOG(make_stderr_logger(), "> rule {}", i);
#endif
                    const auto& I = rules[i];
                    // loop across all terms and update corresponding FOLLOW sets of any non-terminals
                    for (std::size_t j = 0; j < I.terms.size(); j++) {
                        const auto& J = I.terms[j];
#if _DUMP_FOLLOW_SET_BUILD_PROCESS_LOG
                        TAUL_LOG(make_stderr_logger(), "> term (index {}) {}", i, J.fmt());
#endif
                        const bool is_last_term = j + 1 == I.terms.size();

                        if (J.is_terminal()) continue;

                        auto& set_A = details.follow_sets_A[J.nonterminal().id];
#if _DUMP_FOLLOW_SET_BUILD_PROCESS_LOG
                        TAUL_LOG(make_stderr_logger(), "before:");
                        TAUL_LOG(make_stderr_logger(), "    set_A == {}", set_A);
#endif
                        if (is_last_term) {
                            // if this is last term, add I's FOLLOW set to non-terminal's FOLLOW set
                            const auto& other_set_A = details.follow_sets_A[I.id];
#if _DUMP_FOLLOW_SET_BUILD_PROCESS_LOG
                            TAUL_LOG(make_stderr_logger(), "    -> adding (A) {}", other_set_A);
#endif
                            if (!set_A.includes_set(other_set_A)) changed = true;
                            set_A.add_set(other_set_A);
                        }
                        else {
                            const auto& after_J = I.terms[j + 1];
                            if (after_J.is_terminal()) {
#if _DUMP_FOLLOW_SET_BUILD_PROCESS_LOG
                                TAUL_LOG(make_stderr_logger(), "    -> adding (B) {}", after_J.terminal().ids);
#endif
                                // if next term is a terminal, add it to J's FOLLOW set
                                if (!set_A.includes_range(after_J.terminal().ids)) changed = true;
                                set_A.add_range(after_J.terminal().ids);
                            }
                            else if (after_J.is_nonterminal()) {
                                // if next term is a non-terminal, add its FOLLOW set to J's FOLLOW set
                                const auto& after_J_set_A = details.follow_sets_A[after_J.nonterminal().id];
#if _DUMP_FOLLOW_SET_BUILD_PROCESS_LOG
                                TAUL_LOG(make_stderr_logger(), "    -> adding (C) {}", after_J_set_A);
#endif
                                if (!set_A.includes_set(after_J_set_A)) changed = true;
                                set_A.add_set(after_J_set_A);
                            }
                            else TAUL_DEADEND;
                        }
#if _DUMP_FOLLOW_SET_BUILD_PROCESS_LOG
                        TAUL_LOG(make_stderr_logger(), "after:");
                        TAUL_LOG(make_stderr_logger(), "    set_A == {}", set_A);
#endif
                    }
                }
                // keep looping until FOLLOW sets stop changing
                if (!changed) break;
            }
        }
#endif
    }

    template<typename Symbol>
    inline void parse_table<Symbol>::_build_prefix_sets(parse_table_build_details<Symbol>& details) {
        // populate prefix sets w/ FIRST sets
        details.prefix_sets_A = details.first_sets_A;
        details.prefix_sets_w = details.first_sets_w;
        // add FOLLOW sets if FIRST set contained epsilon
        for (auto& I : details.prefix_sets_A) {
            if (!I.second.includes_epsilon()) continue;

            TAUL_ASSERT(details.follow_sets_A.contains(I.first));
            I.second.add_set(details.follow_sets_A.at(I.first));
        }
        for (std::size_t i = 0; i < rules.size(); i++) {
            const auto& I = rules[i];
            
            auto& set_w = details.prefix_sets_w[i];

            if (!set_w.includes_epsilon()) continue;

            TAUL_ASSERT(details.follow_sets_A.contains(I.id));
            set_w.add_set(details.follow_sets_A.at(I.id));
        }
    }
    
    template<typename Symbol>
    inline void taul::internal::parse_table<Symbol>::_populate_id_grouper(parse_table_build_details<Symbol>& details) {
        for (const auto& I : details.first_sets_A) {
#if _DUMP_PARSE_TABLE_USE_CASE_ADDING_LOG
            TAUL_LOG(make_stderr_logger(), "    > adding from Fi(A) {} (of {})", I.second, I.first);
#endif
            for (const auto& J : I.second.ranges()) {
                grouper.add_use_case(J.low, J.high);
            }
        }
        for (const auto& I : details.follow_sets_A) {
#if _DUMP_PARSE_TABLE_USE_CASE_ADDING_LOG
            TAUL_LOG(make_stderr_logger(), "    > adding from Fo(A) {} (of {})", I.second, I.first);
#endif
            for (const auto& J : I.second.ranges()) {
                grouper.add_use_case(J.low, J.high);
            }
        }
        for (const auto& I : details.first_sets_w) {
#if _DUMP_PARSE_TABLE_USE_CASE_ADDING_LOG
            TAUL_LOG(make_stderr_logger(), "    > adding from Fi(w) {}", I);
#endif
            for (const auto& J : I.ranges()) {
                grouper.add_use_case(J.low, J.high);
            }
        }
    }
    
    template<typename Symbol>
    inline void taul::internal::parse_table<Symbol>::_populate_parse_table_and_check_for_collisions(parse_table_build_details<Symbol>& details) {
        for (std::size_t i = 0; i < rules.size(); i++) {
            const auto& I = rules[i];

            const auto& prefix_set_w = details.prefix_sets_w[i];

            // given two prefix_set_w ranges A and B, our system may break if
            // grouper(A.high) == grouper(B.low), so we'll use minimum_group
            // to assert that this condition never arises

            // our code in _populate_id_grouper is written to ensure that the
            // grouper has the right ID groups to ensure the above doesn't arise

            group_id minimum_group = 0;

            for (const auto& J : prefix_set_w.ranges()) {
                const group_id low_group = grouper(J.low);
                const group_id high_group = grouper(J.high);

                TAUL_ASSERT(low_group >= minimum_group);
                minimum_group = high_group + 1;

                for (group_id ii = low_group; ii <= high_group; ii++) {
                    pt_key key{ I.id, ii };
                    if (!mappings.contains(key)) {
                        mappings[key] = i;
                    }
                    else {
                        details.collisions.emplace(key);
                    }
                }
            }
        }
    }

    template<typename Symbol>
    inline void parse_table<Symbol>::_move_assign_first_follow_and_prefix_sets(parse_table_build_details<Symbol>& details) {
        std::swap(first_sets_A, details.first_sets_A);
        std::swap(follow_sets_A, details.follow_sets_A);
        std::swap(prefix_sets_A, details.prefix_sets_A);
    }

    template<typename Symbol>
    inline std::optional<std::size_t> taul::internal::parse_table<Symbol>::lookup(const pt_key& k) const noexcept {
        const auto it = mappings.find(k);
        return
            it != mappings.end()
            ? std::make_optional(it->second)
            : std::nullopt;
    }

    template<typename Symbol>
    inline std::optional<std::size_t> taul::internal::parse_table<Symbol>::lookup(symbol_id nonterminal, group_id terminal_group) const noexcept {
        return lookup(pt_key{ nonterminal, terminal_group });
    }

    template<typename Symbol>
    inline std::string taul::internal::parse_table<Symbol>::fmt(const char* tab) const {
        TAUL_ASSERT(tab);
        std::string result{};
        result += "(parse_table)";
        result += "\nrules:";
        if (rules.size() <= 100) {
            std::size_t i = 0;
            for (const auto& I : rules) {
                result += std::format(
                    "\n{}(rule {}) {}",
                    tab, i, I.fmt());
                i++;
            }
        }
        else {
            result += std::format("\n{}rules.size() > 100; will not display to ensure readability", tab);
        }
        result += "\nmappings:";
        if (mappings.size() <= 300) {
            for (const auto& I : mappings) {
                result += std::format(
                    "\n{}{} -> {}",
                    tab, I.first.fmt(grouper), I.second);
            }
        }
        else {
            result += std::format("\n{}mappings.size() > 300; will not display to ensure readability", tab);
        }
        result += "\nFIRST sets (Fi(A)):";
        for (const auto& I : first_sets_A) {
            result += std::format(
                "\n{}{} -> {}",
                tab, I.first, I.second);
        }
        result += "\nFOLLOW sets (Fo(A)):";
        for (const auto& I : follow_sets_A) {
            result += std::format(
                "\n{}{} -> {}",
                tab, I.first, I.second);
        }
        result += "\nprefix sets (Pfx(A)):";
        for (const auto& I : prefix_sets_A) {
            result += std::format(
                "\n{}{} -> {}",
                tab, I.first, I.second);
        }
        return result;
    }
}

