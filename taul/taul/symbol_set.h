

#pragma once


#include <iostream>
#include <format>
#include <vector>

#include "symbol_id.h"
#include "symbols.h"
#include "symbol_traits.h"
#include "symbol_range.h"
#include "symbol_set_decls.h"


namespace taul {


    // TODO: our current impl performs a lot of linear-time sequential
    //       traversal of symbol_set objects, in ways that could *maybe*
    //       (or maybe not) be improved by log-time binary-search-like
    //       traversal methods
    //
    //       being so small though, log-time binary-search-like traversal
    //       could also result in unhelpful increase in up-front costs
    //       regarding performance, which may not make them worth it

    // TODO: it may be a good idea to invest some time at some point into
    //       developing some kind of SSO-like ('SSO' being 'Small String
    //       Optimization') optimization feature for symbol_set, as that
    //       could provide a nice performance boost to them


    // symbol_set describes symbol sets by recording the contiguous symbol
    // ID ranges of the symbols in the set
    //
    // doing things like this means that symbol_set time complexity regarding
    // looking-up things in the set grows w/ the degree of *fragmentation* of
    // the set (ie. how many contiguous ranges are needed to describe it),
    // rather than the number of symbols encapsulated by the set
    //
    // this is useful, as it means we can have, for example, a set that contains
    // ALL non-ASCII Unicode codepoints, w/out having to incur a huge additional
    // cost to looking things up therein, so long as we don't start creating
    // lots of *holes* in said range


    // while not an ID, for things like FIRST/FOLLOW sets, we need our
    // sets to be able to contain the formal language 'epsilon', which
    // is a special set member representing an empty set of symbols
    // (w/ the rest of the set members being single symbol sets)


    // for symbol_set, we'll define the following operators:

    //      'x + y' == 'the set of IDs in x or y'
    //      'x - y' == 'the set of IDs in x, but not in y'
    //      '-y'    == 'the set of IDs not in y'

    // the -y description is referring to all legal IDs for the portion
    // of the broader ID space actually being used by the symbol_set type

    // this isn't proper set-theory notation, but whatever


    template<typename Symbol, typename Traits>
    class symbol_set final {
    public:

        template<typename Symbol, typename Traits>
        friend struct taul::symbol_set_iterator;


        using symbol_t = Symbol;
        using traits_t = Traits;

        using this_t = symbol_set<symbol_t, traits_t>;

        using symbol_range_t = symbol_range<symbol_t, traits_t>;


        // preferred integer-like type for specifying IDs

        // values of preferred_type NEED NOT THEMSELVES BE ID VALUES!

        // thus, given preferred_type value x, ID is traits_t::id(x)

        using preferred_type = typename traits_t::preferred_type;


        using const_iterator = symbol_set_iterator<symbol_t, traits_t>;
        using iterator = const_iterator;


        inline symbol_set();
        inline symbol_set(const this_t& other);
        inline symbol_set(this_t&& other) noexcept;

        ~symbol_set() noexcept = default;

        inline this_t& operator=(const this_t& rhs);
        inline this_t& operator=(this_t&& rhs) noexcept;


        // size returns the number of elements in the set, excluding epsilon

        inline std::size_t size() const noexcept;

        inline bool has_size() const noexcept { return size() > 0; }
        inline bool empty() const noexcept { return !has_size(); }


        // iterators are guaranteed to be in ascending order of symbol_id value

        // iterators do not traverse epsilon

        inline iterator cbegin() const noexcept;
        inline iterator begin() const noexcept { return cbegin(); }

        inline iterator cend() const noexcept;
        inline iterator end() const noexcept { return cend(); }


        // ranges returns a span over the symbol ranges of the symbol set

        inline std::span<const symbol_range_t> ranges() const noexcept;


        // includes(_id) returns if x is in the set

        inline bool includes_id(symbol_id x) const noexcept;
        
        inline bool includes(const preferred_type& x) const noexcept { return includes_id(traits_t::id(x)); }
        inline bool includes(const symbol_t& x) const noexcept { return includes_id(traits_t::id(x)); }

        // includes(_id)_range returns if inclusive (ie. not exclusive!) range [low, high] is in the set

        // if low > high, low and high will be swapped

        inline bool includes_id_range(symbol_id low, symbol_id high) const noexcept;

        inline bool includes_range(const preferred_type& low, const preferred_type& high) const noexcept { return includes_id_range(traits_t::id(low), traits_t::id(high)); }
        inline bool includes_range(const symbol_t& low, const symbol_t& high) const noexcept { return includes_id_range(traits_t::id(low), traits_t::id(high)); }

        inline bool includes_range(symbol_range_t x) const noexcept { return includes_id_range(x.low, x.high); }

        // includes_epsilon checks if epsilon is in the set

        inline bool includes_epsilon() const noexcept;

        // includes_set checks if x is a proper subset of the set (including w/ regards to epsilon)

        inline bool includes_set(const this_t& x) const noexcept;


        // add(_id) adds x to the set, if it wasn't already

        // add(_id) behaviour is undefined if traits_t::legal_id(x) == false

        inline this_t& add_id(symbol_id x);

        inline this_t& add(const preferred_type& x) { return add_id(traits_t::id(x)); }
        inline this_t& add(const symbol_t& x) { return add_id(traits_t::id(x)); }
        
        // add(_id)_range adds the IDs from the inclusive (ie. not exclusive!) range [low, high]
        // to the set that weren't already

        // if low > high, low and high will be swapped

        // add(_id)_range behaviour is undefined if traits_t::legal_id(low) == false
        // add(_id)_range behaviour is undefined if traits_t::legal_id(high) == false

        inline this_t& add_id_range(symbol_id low, symbol_id high);

        inline this_t& add_range(const preferred_type& low, const preferred_type& high) { return add_id_range(traits_t::id(low), traits_t::id(high)); }
        inline this_t& add_range(const symbol_t& low, const symbol_t& high) { return add_id_range(traits_t::id(low), traits_t::id(high)); }

        inline this_t& add_range(symbol_range_t x) { return add_id_range(x.low, x.high); }

        // add_epsilon adds epsilon to the set, if it wasn't already

        inline this_t& add_epsilon() noexcept;

        // add_set adds the symbols of set x to this set that weren't already in it

        // add_set also adds epsilon to this set if x has it

        inline this_t& add_set(const this_t& x);

        // add_all adds all (legal) symbol IDs and epsilon to the set

        inline this_t& add_all() { return add_id_range(traits_t::first_id, traits_t::last_id).add_epsilon(); }

        inline this_t& operator+=(symbol_id rhs) { return add_id(rhs); return *this; }
        inline this_t& operator+=(const preferred_type& rhs) { return add(rhs); return *this; }
        inline this_t& operator+=(const symbol_t& rhs) { return add(rhs); return *this; }
        inline this_t& operator+=(const this_t& rhs) { return add_set(rhs); return *this; }


        // remove(_id) removes x from the set, if it's part of it

        // remove(_id) potentially needs to alloc memory to handle a removal, and so it's 
        // not noexcept, and all iterators are invalidated
        
        inline this_t& remove_id(symbol_id x);

        inline this_t& remove(const preferred_type& x) { return remove_id(traits_t::id(x)); }
        inline this_t& remove(const symbol_t& x) { return remove_id(traits_t::id(x)); }

        // remove(_id)_range removes the IDs of inclusive (ie. not exclusive!) range [low, high]
        // from the set that there are currently in it

        // if low > high, low and high will be swapped

        // remove(_id)_range potentially needs to alloc memory to handle a removal, and so it's 
        // not noexcept, and all iterators are invalidated

        inline this_t& remove_id_range(symbol_id low, symbol_id high);

        inline this_t& remove_range(const preferred_type& low, const preferred_type& high) { return remove_id_range(traits_t::id(low), traits_t::id(high)); }
        inline this_t& remove_range(const symbol_t& low, const symbol_t& high) { return remove_id_range(traits_t::id(low), traits_t::id(high)); }

        inline this_t& remove_range(symbol_range_t x) { return remove_id_range(x.low, x.high); }

        // remove_epsilon removes epsilon from the set, if it's part of it

        inline this_t& remove_epsilon() noexcept;

        // remove_set removes the symbols of set x from this set which are in this set

        // remove_set also removes epsilon from this set if x has it

        inline this_t& remove_set(const this_t& x);

        // remove_all removes all symbol IDs and epsilon from the set

        inline this_t& remove_all() noexcept;

        inline this_t& operator-=(symbol_id rhs) { return remove_id(rhs); }
        inline this_t& operator-=(const preferred_type& rhs) { return remove(rhs); }
        inline this_t& operator-=(const symbol_t& rhs) { return remove(rhs); }
        inline this_t& operator-=(const this_t& rhs) { return remove_set(rhs); }


        inline bool equal(const this_t& other) const noexcept;

        inline bool operator==(const this_t& rhs) const noexcept { return equal(rhs); }
        inline bool operator!=(const this_t& rhs) const noexcept { return !equal(rhs); }


        // inverse returns the set of IDs not in this set

        // inverse also accounts for epsilon

        inline this_t inverse() const;

        inline this_t operator-() const { return inverse(); }


        inline std::string fmt() const;


    private:

        std::vector<symbol_range_t> _ranges;
        std::size_t _count;
        bool _epsilon;


        inline bool _includes_unbroken_id_range(symbol_id low, symbol_id high) const noexcept;

        inline void _split_id_range(std::size_t range_to_split_index, symbol_id split_low, symbol_id split_high);
        inline void _remove_id_range(symbol_id low, symbol_id high);

        inline void _merge_neighbor_pair(std::size_t i);
        inline void _merge_neighbors();
        inline void _recount();
        inline void _sort_merge_and_recount();


    public:

#if 0
        inline void dump() const {
            TAUL_LOG(make_stderr_logger(), "dump:");
            for (const auto& I : _ranges) {
                TAUL_LOG(make_stderr_logger(), "    -> low=={}, high=={}", std::size_t(I.low), std::size_t(I.high));
            }
        }
#endif
    };

    template<typename Symbol, typename Traits>
    struct symbol_set_iterator final {

        using symbol_t = Symbol;
        using traits_t = Traits;

        using this_t = symbol_set_iterator<symbol_t, traits_t>;


        using difference_type = std::ptrdiff_t;
        using value_type = symbol_id;
        using pointer = void;
        using reference = symbol_id;
        using iterator_category = std::forward_iterator_tag;


        // having these public ensures we can auto-gen copy/move ctor/assign,
        // and that we can aggregate init iters, which makes things easier

        const symbol_set<Symbol, Traits>* _client = nullptr; // internal, do not use
        std::size_t _index = std::size_t(-1); // internal, do not use
        symbol_id _id = symbol_id(-1); // internal, do not use


        inline symbol_id operator*() const noexcept {
            TAUL_ASSERT(_client);
            return _id;
        }


        inline this_t& operator++();
        inline this_t operator++(int);


        inline bool equal(const this_t& other) const noexcept;

        inline bool operator==(const this_t& other) const noexcept { return equal(other); }
        inline bool operator!=(const this_t& other) const noexcept { return !equal(other); }
    };


    template<typename Symbol, typename Traits>
    inline symbol_set<Symbol, Traits> operator+(const symbol_set<Symbol, Traits>& lhs, const symbol_set<Symbol, Traits>& rhs) {
        symbol_set<Symbol, Traits> result = lhs;
        result += rhs;
        return result;
    }
    
    template<typename Symbol, typename Traits>
    inline symbol_set<Symbol, Traits> operator-(const symbol_set<Symbol, Traits>& lhs, const symbol_set<Symbol, Traits>& rhs) {
        symbol_set<Symbol, Traits> result = lhs;
        result -= rhs;
        return result;
    }


    template<typename Symbol, typename Traits>
    inline symbol_set<Symbol, Traits>::symbol_set() 
        : _ranges({}), 
        _count(0), 
        _epsilon(false) {}

    template<typename Symbol, typename Traits>
    inline symbol_set<Symbol, Traits>::symbol_set(const this_t& other) 
        : _ranges(other._ranges), 
        _count(other._count), 
        _epsilon(other._epsilon) {}
    
    template<typename Symbol, typename Traits>
    inline symbol_set<Symbol, Traits>::symbol_set(this_t&& other) noexcept 
        : _ranges(std::move(other._ranges)), 
        _count(std::move(other._count)),
        _epsilon(std::move(other._epsilon)) {}
    
    template<typename Symbol, typename Traits>
    inline typename symbol_set<Symbol, Traits>::this_t& symbol_set<Symbol, Traits>::operator=(const this_t& rhs) {
        _ranges = rhs._ranges;
        _count = rhs._count;
        _epsilon = rhs._epsilon;
        return *this;
    }
    
    template<typename Symbol, typename Traits>
    inline typename symbol_set<Symbol, Traits>::this_t& symbol_set<Symbol, Traits>::operator=(this_t&& rhs) noexcept {
        if (&rhs == this) {
            _ranges = std::move(rhs._ranges);
            _count = std::move(rhs._count);
            _epsilon = std::move(rhs._epsilon);
        }
        return *this;
    }

    template<typename Symbol, typename Traits>
    inline std::size_t symbol_set<Symbol, Traits>::size() const noexcept {
        return _count;
    }

    template<typename Symbol, typename Traits>
    inline typename symbol_set<Symbol, Traits>::iterator symbol_set<Symbol, Traits>::cbegin() const noexcept {
        if (empty()) return end();
        return iterator{
            this,
            0,
            _ranges.front().low,
        };
    }

    template<typename Symbol, typename Traits>
    inline typename symbol_set<Symbol, Traits>::iterator symbol_set<Symbol, Traits>::cend() const noexcept {
        if (empty()) return iterator{};
        return iterator{
            this,
            _ranges.size(),
            symbol_id(-1),
        };
    }

    template<typename Symbol, typename Traits>
    inline std::span<const typename symbol_set<Symbol, Traits>::symbol_range_t> symbol_set<Symbol, Traits>::ranges() const noexcept {
        return std::span<const symbol_range_t>(_ranges.begin(), _ranges.size());
    }
    
    template<typename Symbol, typename Traits>
    inline bool symbol_set<Symbol, Traits>::includes_id(symbol_id x) const noexcept {
        for (const auto& I : _ranges) {
            if (I.contains(x)) return true;
        }
        return false;
    }

    template<typename Symbol, typename Traits>
    inline bool taul::symbol_set<Symbol, Traits>::includes_id_range(symbol_id low, symbol_id high) const noexcept {
        if (!is_symbol_id(low)) return false;
        if (!is_symbol_id(high)) return false;
        if (low > high) std::swap(low, high);
        return _includes_unbroken_id_range(low, high);
    }

    template<typename Symbol, typename Traits>
    inline bool symbol_set<Symbol, Traits>::includes_epsilon() const noexcept {
        return _epsilon;
    }

    template<typename Symbol, typename Traits>
    inline bool symbol_set<Symbol, Traits>::includes_set(const this_t& x) const noexcept {
        if (!includes_epsilon() && x.includes_epsilon()) return false;
        if (x.empty()) return true;
        for (const auto& I : x._ranges) {
            if (!_includes_unbroken_id_range(I.low, I.high)) return false;
        }
        return true;
    }

    template<typename Symbol, typename Traits>
    inline typename symbol_set<Symbol, Traits>::this_t& symbol_set<Symbol, Traits>::add_id(symbol_id x) {
        return add_id_range(x, x);
    }

    template<typename Symbol, typename Traits>
    inline typename symbol_set<Symbol, Traits>::this_t& taul::symbol_set<Symbol, Traits>::add_id_range(symbol_id low, symbol_id high) {
        TAUL_ASSERT(traits_t::legal_id(low));
        TAUL_ASSERT(traits_t::legal_id(high));
        if (low > high) std::swap(low, high);
        _ranges.push_back(symbol_range_t{ low, high });
        _sort_merge_and_recount();
        return *this;
    }

    template<typename Symbol, typename Traits>
    inline typename symbol_set<Symbol, Traits>::this_t& taul::symbol_set<Symbol, Traits>::add_epsilon() noexcept {
        _epsilon = true;
        return *this;
    }

    template<typename Symbol, typename Traits>
    inline typename symbol_set<Symbol, Traits>::this_t& taul::symbol_set<Symbol, Traits>::add_set(const this_t& x) {
        if (this == &x) return *this; // avoid potential corruption on self usage
        for (const auto& I : x._ranges) _ranges.push_back(I);
        _sort_merge_and_recount();
        if (x.includes_epsilon()) add_epsilon();
        return *this;
    }

    template<typename Symbol, typename Traits>
    inline typename symbol_set<Symbol, Traits>::this_t& symbol_set<Symbol, Traits>::remove_id(symbol_id x) {
        // don't need to manually incr i since if we delete during loop, we'll then exit it
        for (std::size_t i = 0; i < _ranges.size(); i++) {
            if (_ranges[i].contains(x)) {
                const bool a = x == _ranges[i].low;
                const bool b = x == _ranges[i].high;
                if (a && b) _ranges.erase(_ranges.begin() + i);
                else if (a && !b) _ranges[i].low = x + 1;
                else if (!a && b) _ranges[i].high = x - 1;
                else { // split range into two, dividing along x
                    symbol_range_t left{ _ranges[i].low, x - 1 };
                    symbol_range_t right{ x + 1, _ranges[i].high };
                    _ranges[i] = left;
                    _ranges.insert(_ranges.begin() + i + 1, right);
                }
                _count--;
                return *this;
            }
        }
        return *this;
    }

    template<typename Symbol, typename Traits>
    inline typename symbol_set<Symbol, Traits>::this_t& taul::symbol_set<Symbol, Traits>::remove_id_range(symbol_id low, symbol_id high) {
        if (low > high) std::swap(low, high);
        _remove_id_range(low, high);
        _recount();
        return *this;
    }

    template<typename Symbol, typename Traits>
    inline typename symbol_set<Symbol, Traits>::this_t& taul::symbol_set<Symbol, Traits>::remove_epsilon() noexcept {
        _epsilon = false;
        return *this;
    }

    template<typename Symbol, typename Traits>
    inline typename symbol_set<Symbol, Traits>::this_t& taul::symbol_set<Symbol, Traits>::remove_set(const this_t& x) {
        if (this == &x) return remove_all(); // avoid potential corruption on self usage
        for (const auto& I : x._ranges) _remove_id_range(I.low, I.high);
        _recount();
        if (x.includes_epsilon()) remove_epsilon();
        return *this;
    }

    template<typename Symbol, typename Traits>
    inline typename symbol_set<Symbol, Traits>::this_t& symbol_set<Symbol, Traits>::remove_all() noexcept {
        _ranges.clear();
        _count = 0;
        remove_epsilon();
        return *this;
    }

    template<typename Symbol, typename Traits>
    inline bool symbol_set<Symbol, Traits>::equal(const this_t& other) const noexcept {
        if (includes_epsilon() != other.includes_epsilon()) return false;
        if (_ranges.size() != other._ranges.size()) return false;
        for (std::size_t i = 0; i < _ranges.size(); i++) {
            if (_ranges[i].low != other._ranges[i].low) return false;
            if (_ranges[i].high != other._ranges[i].high) return false;
        }
        return true;
    }

    template<typename Symbol, typename Traits>
    inline symbol_set<Symbol, Traits>::this_t symbol_set<Symbol, Traits>::inverse() const {
        this_t result{};
        if (empty()) {
            result.add_id_range(traits_t::first_id, traits_t::last_id);
        }
        else {
            // if IDs exist prior to our first range, add them
            // if IDs exist after our last range, add them
            if (_ranges.front().low > traits_t::first_id) result.add_id_range(traits_t::first_id, _ranges.front().low - 1);
            if (_ranges.back().high < traits_t::last_id) result.add_id_range(_ranges.back().high + 1, traits_t::last_id);
            // iterate until the second-to-last range (as last one is account for above)
            for (std::size_t i = 0; i < _ranges.size() - 1; i++) {
                // if we have a next range, then we know there's a gap between this
                // and that one, so add those IDs
                result.add_id_range(_ranges[i].high + 1, _ranges[i + 1].low - 1);
            }
        }
        // if we DON'T have epsilon, add epsilon
        if (!includes_epsilon()) result.add_epsilon();
        return result;
    }

    template<typename Symbol, typename Traits>
    inline std::string symbol_set<Symbol, Traits>::fmt() const {
        std::string result{};
        for (const auto& I : _ranges) result += I.fmt();
        return 
            includes_epsilon() 
            ? std::format("[{}+epsilon]", result)
            : std::format("[{}]", result);
    }

    template<typename Symbol, typename Traits>
    inline bool symbol_set<Symbol, Traits>::_includes_unbroken_id_range(symbol_id low, symbol_id high) const noexcept {
        TAUL_ASSERT(low <= high);
        for (const auto& I : _ranges) {
            // to include an *unbroken* ID range, low and high must exist in the
            // same range, as existing in >1 ranges means their being a gap between
            // those ranges containing IDs not in the set, but are in [low, high]
            if (I.contains(low) && I.contains(high)) return true;
        }
        return false;
    }

    template<typename Symbol, typename Traits>
    inline void taul::symbol_set<Symbol, Traits>::_split_id_range(std::size_t range_to_split_index, symbol_id split_low, symbol_id split_high) {
        // users of _split_id_range gotta remember to call _recount after
        TAUL_ASSERT(range_to_split_index < _ranges.size());
        TAUL_ASSERT(split_low <= split_high);
        // _split_id_range requires us to be able to have two ID ranges after splitting
        TAUL_ASSERT(split_low > _ranges[range_to_split_index].low);
        TAUL_ASSERT(split_high < _ranges[range_to_split_index].high);
        symbol_range_t a{
            _ranges[range_to_split_index].low,
            split_low - 1, // '- 1' since we want first before split_low
        };
        symbol_range_t b{
            split_high + 1, // '+ 1' since we want first after split_high
            _ranges[range_to_split_index].high,
        };
        _ranges[range_to_split_index] = std::move(a);
        _ranges.insert(_ranges.begin() + range_to_split_index + 1, std::move(b));
    }

    template<typename Symbol, typename Traits>
    inline void symbol_set<Symbol, Traits>::_remove_id_range(symbol_id low, symbol_id high) {
        // users of _remove_id_range gotta remember to call _recount after
        TAUL_ASSERT(low <= high);
        // we can skip more complicated work if we know [low, high] will remove *everything*
        // users still need to call _recount though, as this control path isn't guaranteed to occur
        if (_ranges.empty() || (_ranges.front().low >= low && _ranges.back().high <= high)) {
            remove_all();
            return;
        }
        TAUL_ASSERT(low < high + 1); // check overflow
        for (std::size_t i = 0; i < _ranges.size();) {
            TAUL_ASSERT(_ranges[i].low < _ranges[i].high + 1); // check overflow
            // incr i by 1, unless we have reason to change incr amount
            std::size_t incr_i_by = 1;
            // do the '+ 1' below to make our two ranges exclusive ranges
            if (auto d = internal::range_overlap_details(_ranges[i].low, _ranges[i].high + 1, low, high + 1)) {
                // make exclusive [d->first, d->second) into inclusive [d_low, d_high]
                const auto d_low = d->first;
                const auto d_high = d->second - 1;
                //TAUL_LOG(make_stderr_logger(), "low=={}\nhigh=={}", low, high);
                //TAUL_LOG(make_stderr_logger(), "_ranges[i].low=={}\n_ranges[i].high=={}", _ranges[i].low, _ranges[i].high);
                //TAUL_LOG(make_stderr_logger(), "d_low=={}\nd_high=={}", d_low, d_high);
                const bool eq_low = d_low == _ranges[i].low;
                const bool eq_high = d_high == _ranges[i].high;
                if (eq_low && eq_high) {
                    _ranges.erase(_ranges.begin() + i); // delete if [_ranges[i].low, _ranges[i].high] == [d_low, d_high]
                    incr_i_by = 0; // deleted from _ranges, so don't incr i
                }
                else if (eq_low && !eq_high) _ranges[i].low = d_high + 1; // '+ 1' since we want first after d_high
                else if (!eq_low && eq_high) _ranges[i].high = d_low - 1; // '- 1' since we want first before d_low
                else {
                    // we know now that [d_low, d_high] splits [_ranges[i].low, _ranges[i].high],
                    // so we thus need to [_ranges[i].low, _ranges[i].high] along [d_low, d_high]
                    _split_id_range(i, d_low, d_high);
                    // if neither end of [d_low, d_high] is touching our the edges of our
                    // [_ranges[i].low, _ranges[i].high] range, then we can actually
                    // conclude that [low, high] will NOT AT ALL touch any other range
                    // in _ranges, and thus we can actually return right now
                    return;
                }
            }
            i += incr_i_by;
        }
    }

    template<typename Symbol, typename Traits>
    inline void symbol_set<Symbol, Traits>::_merge_neighbor_pair(std::size_t i) {
        TAUL_ASSERT(i + 1 < _ranges.size());
        // gotta remeber to account for when _ranges[i + 1].high < _ranges[i].high, which CAN happen!
        _ranges[i].high = std::max(_ranges[i].high, _ranges[i + 1].high);
        _ranges.erase(_ranges.begin() + i + 1);
    }

    template<typename Symbol, typename Traits>
    inline void symbol_set<Symbol, Traits>::_merge_neighbors() {
        if (_ranges.size() == 0) return; // edge case
        for (std::size_t i = 0; i < _ranges.size() - 1;) {
            const bool a = _ranges[i].overlaps(_ranges[i + 1]); // merge if overlap w/ neighbor
            const bool b = _ranges[i].high + 1 == _ranges[i + 1].low; // merge if neighber perfectly extends ID range
            if (a || b) _merge_neighbor_pair(i);
            else i++; // if we erase a range, don't incr
        }
    }

    template<typename Symbol, typename Traits>
    inline void symbol_set<Symbol, Traits>::_recount() {
        _count = 0;
        for (const auto& I : _ranges) _count += I.count();
    }

    template<typename Symbol, typename Traits>
    inline void symbol_set<Symbol, Traits>::_sort_merge_and_recount() {
        std::sort(
            _ranges.begin(), 
            _ranges.end(), 
            [](const symbol_range_t& a, const symbol_range_t& b) -> bool {
                return a.low < b.low;
            });
        _merge_neighbors();
        _recount();
    }


    template<typename Symbol, typename Traits>
    inline symbol_set_iterator<Symbol, Traits>::this_t& symbol_set_iterator<Symbol, Traits>::operator++() {
        if (_client) {
            TAUL_ASSERT(*this != _client->end());
            TAUL_ASSERT(_index < _client->_ranges.size());
            if (_id != _client->_ranges[_index].high) {
                _id = _id + 1;
            }
            else {
                _index++;
                if (_index < _client->_ranges.size()) {
                    _id = _client->_ranges[_index].low;
                }
                else _id = symbol_id(-1); // <- ensures important invariant!
            }
        }
        else TAUL_DEADEND;
        return *this;
    }
    
    template<typename Symbol, typename Traits>
    inline symbol_set_iterator<Symbol, Traits>::this_t symbol_set_iterator<Symbol, Traits>::operator++(int) {
        auto r = *this;
        ++r;
        return r;
    }

    template<typename Symbol, typename Traits>
    inline bool symbol_set_iterator<Symbol, Traits>::equal(const this_t& other) const noexcept {
        return 
            _client == other._client &&
            _index == other._index &&
            _id == other._id;
    }
}

template<typename Symbol, typename Traits>
struct std::formatter<taul::symbol_set<Symbol, Traits>> final : std::formatter<std::string> {
    auto format(const taul::symbol_set<Symbol, Traits>& x, format_context& ctx) const {
        return formatter<string>::format(x.fmt(), ctx);
    }
};

namespace std {
    template<typename Symbol, typename Traits>
    inline std::ostream& operator<<(std::ostream& stream, const taul::symbol_set<Symbol, Traits>& x) {
        return stream << x.fmt();
    }
}

