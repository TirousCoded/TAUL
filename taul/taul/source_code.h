

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


    // source code objects store injected source code as UTF-8 internally, converting
    // from other encodings as needed


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


        // TODO: if we figure out how to do it properly, maybe add option for 
        //       exporting non-UTF-8 text files

        // to_file writes the source code object to a file at out_path, encoded as UTF-8

        // to_file returns if it succeeded

        // to_file will fail if out_path does not have a valid filename component

        bool to_file(const std::filesystem::path& out_path) const;


        // TODO: maybe add support for other char types later on, not just encodings

        // these allocate a new taul::str, replacing the old one, if any
        
        void add_str(
            taul::str origin, 
            taul::str x,
            encoding in_e = utf8);

        // TODO: if we figure out how to do it properly, maybe add option for 
        //       importing non-UTF-8 text files

        // add_file imports a text file from src_path, encoded as UTF-8, adding 
        // it as a source code page

        // add_file returns if successful

        // add_file can handle also text files encoded as UTF-8 BOM, in which
        // case it'll be converted into UTF-8

        bool add_file(
            const std::filesystem::path& src_path,
            const std::shared_ptr<logger>& lgr = nullptr);


        void reset() noexcept;


    private:

        taul::str _concat;
        std::vector<source_page> _pages;

        // this maps each source_pos in the source code to the line/chr/page-index
        // its associated w/, so that we can look them up

        // this feels a tad *expensive* w/ regards to space, but it gives us O(1)
        // lookup time complexity, and we *need* to do it like this in order for
        // us to properly handle having 

        struct _pos_map_entry final {
            uint32_t chr, ln, page_index; // using uint32_t herein to try and reduce memory usage
        };

        std::unordered_map<source_pos, _pos_map_entry> _pos_map;


        void _populate_pos_map_for_new_page(taul::str new_page_txt);
    };
}

