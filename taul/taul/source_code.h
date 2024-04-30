

#pragma once


#include <vector>
#include <span>
#include <filesystem>

#include "logger.h"
#include "str.h"


namespace taul {


    // these 'source code' objects encapsulate a collection of source code 'pages',
    // corresponding to the various different strings, files, etc. where the source
    // code of a compilation may come from

    // the collection is represented as a single string composed of the concatenation
    // of the pages, such that every character/line/origin location has a corresponding
    // 32-bit source_pos value uniquely identifying it, allowing all of said information
    // to thus be reduced down to this 32-bit source_pos value

    // each page is likewise also accessed via an index in the source code object

    // a string identifier is included in each page's metadata called its 'origin',
    // which generally refers to either the filepath to the page's associated source
    // code file, or some other string identifier naming its *origin* in some way

    // character/line values index from 1, not 0


    // take note that source_pos can be used to refer to positions in ANY source
    // string, not just taul::source_code, w/ this being the case so that we're
    // not semantically coupled stricly to JUST using taul::source_code

    // want this to be 32-bit, as it's large enough for pretty much all use cases,
    // but small enough to be nice-and-compact for use in end-user's code

    using source_pos = std::uint32_t;

    // this type is is paired w/ source_pos and measures source code length

    using source_len = source_pos;


    // this format function is used to get a formatted string of a source_pos, for
    // situations where full source_location information isn't otherwise available

    std::string fmt_pos(source_pos pos);
    std::string fmt_pos_and_len(source_pos pos, source_len len);


    struct source_page final {
        source_pos      pos     = 0;
        source_len      length  = 0;
        str	            origin;         // a descriptor of the page's origin, such as a file path
    };


    // I'm using std::string so that source_location can outlive its source_code object

    struct source_location final {
        source_pos      pos     = 0;
        str	            origin;
        size_t          chr     = 1;
        size_t          line    = 1;


        std::string fmt() const;
    };
}


template<>
struct std::formatter<taul::source_location> final : std::formatter<std::string> {
    auto format(const taul::source_location& x, format_context& ctx) const {
        return formatter<string>::format(x.fmt(), ctx);
    }
};

namespace std {
    inline std::ostream& operator<<(std::ostream& stream, const taul::source_location& x) {
        return stream << x.fmt();
    }
}


namespace taul {


    // source_code objects should generally be wrapped in shared pointers, but may
    // be used w/ move-assignment to reduce overhead

    class source_code final {
    public:

        source_code() = default;
        source_code(const source_code&) = delete;
        source_code(source_code&& x) noexcept;

        ~source_code() noexcept = default;

        source_code& operator=(const source_code&) = delete;
        source_code& operator=(source_code&& rhs) noexcept;


        // this returns the concatenated source code string

        str str() const noexcept;

        // source code objects can implicit convert into their source code strings

        inline operator taul::str() const noexcept { return str(); }


        bool pos_in_bounds(source_pos pos) const noexcept;


        std::span<const source_page> pages() const noexcept;


        // this, if successful, returns the page index of the page pos is within

        std::optional<size_t> page_at(source_pos pos) const noexcept;


        std::optional<source_location> location_at(source_pos pos) const noexcept;


        // these allocate a new taul::str, replacing the old one, if any

        void add_str(taul::str origin, taul::str x);

        // returns if successful

        bool add_file(
            const std::filesystem::path& src_path, 
            const std::shared_ptr<logger>& lgr = nullptr);


        void reset() noexcept;


    private:

        taul::str _concat;
        std::vector<source_page> _pages;

        // each _pages entry gets an entry in this, detailing where in _lineStarts
        // begins its lines

        std::vector<size_t> _pageLineStartOffsets;

        // TODO: I'm sure there's a way to do this better, but we'll just
        //       record the positions of all the line starts, then linearly
        //       iterate across when resolving a position's line number

        // TODO: we could seperate out and generalize our 'ID grouper' code,
        //       and then we could impl below as a grouper + hash map

        // here 'line start' refers to the position AFTER the newline, which
        // we're considering to be the final character of the *previous* line

        std::vector<source_pos> _lineStarts;


        // this is to be called BEFORE doing anything else when adding a new page

        void _addLineStarts(taul::str s);

        std::pair<size_t, size_t> resolveChrAndLine(size_t pageInd, source_pos pos) const noexcept;
    };
}

