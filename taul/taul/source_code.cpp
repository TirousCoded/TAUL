

#include "source_code.h"

#include <fstream>

#include "asserts.h"


using namespace taul::string_literals;


std::string taul::fmt_pos(source_pos pos) {
    return std::format("[pos {}]", pos);
}

std::string taul::fmt_pos_and_len(source_pos pos, source_len len) {
    return std::format("[pos {}, len {}]", pos, len);
}

std::string taul::source_location::fmt() const {
    return std::format("[ln {}, ch {}; \"{}\"]", line, chr, origin);
}

taul::source_code::source_code(source_code&& x) noexcept {
    std::swap(_concat, x._concat);
    std::swap(_pages, x._pages);
    std::swap(_pos_map, x._pos_map);
}

taul::source_code& taul::source_code::operator=(source_code&& rhs) noexcept {
    if (this != &(rhs)) {
        std::swap(_concat, rhs._concat);
        std::swap(_pages, rhs._pages);
        std::swap(_pos_map, rhs._pos_map);
    }
    return *this;
}

taul::str taul::source_code::str() const noexcept {
    return _concat;
}

bool taul::source_code::pos_in_bounds(source_pos pos) const noexcept {
    return size_t(pos) < str().length();
}

std::span<const taul::source_page> taul::source_code::pages() const noexcept {
    return std::span<const taul::source_page>(_pages.begin(), _pages.size());
}

std::optional<size_t> taul::source_code::page_at(source_pos pos) const noexcept {
    const auto it = _pos_map.find(pos);
    return
        it != _pos_map.end()
        ? std::make_optional(it->second.page_index)
        : std::nullopt;
}

std::optional<taul::source_location> taul::source_code::location_at(source_pos pos) const noexcept {
    const auto it = _pos_map.find(pos);
    if (it == _pos_map.end()) return std::nullopt;
    source_location result{
        pos,
        pages()[it->second.page_index].origin,
        it->second.chr,
        it->second.ln,
    };
    return std::make_optional(std::move(result));
}

bool taul::source_code::to_file(const std::filesystem::path& out_path) const {
    if (!out_path.has_filename()) return false;
    std::ofstream stream(out_path, std::ios::binary);
    stream.seekp(0);
    stream.write(str().data(), str().length());
    stream.close();
    return true;
}

void taul::source_code::add_str(
    taul::str origin,
    taul::str x,
    encoding in_e) {
    if (in_e != utf8) { // convert x to UTF-8 if input isn't UTF-8 already
        auto temp = convert_encoding<char>(in_e, utf8, std::string_view(x));
        TAUL_ASSERT(temp);
        x = taul::str(temp.value());
    }
    _populate_pos_map_for_new_page(x);
    source_page _new_page{
        (source_pos)str().size(),
        (source_len)x.length(),
        std::move(origin),
    };
    _pages.push_back(std::move(_new_page));
    _concat = _concat + x;
}

bool taul::source_code::add_file(
    const std::filesystem::path& src_path,
    const std::shared_ptr<logger>& lgr) {
    // try make a short path string to use as origin
    auto short_path = std::filesystem::proximate(src_path, std::filesystem::current_path());

    // using this to format short_path, but avoiding heap allocating if lgr == nullptr
    const std::string short_path_s = bool(lgr) ? short_path.string() : std::string();
    
    TAUL_LOG(lgr, "loading source code page from \"{}\"...", short_path_s);
    std::ifstream ifs(src_path, std::ios::binary | std::ios::ate);
    if (ifs.is_open()) {
        const size_t file_size = (size_t)ifs.tellg();
        ifs.seekg(0);
        std::string buff{};
        buff.resize(file_size, '\0');
        ifs.read(buff.data(), file_size);
        
        // gotta do this to *filter out* BOM if src_path file is actually UTF-8 BOM
        if (check_bom(utf8, std::string_view(buff)) != bom_status::no_bom) {
            buff = convert_encoding<char>(utf8_bom, utf8, buff).value();
        }

        add_str(taul::str(short_path.string()), taul::str(buff));
        TAUL_LOG(lgr, "loaded source code page ({} char) from \"{}\"!", ifs.gcount(), short_path_s);
    }
    TAUL_LOG_IF(!ifs.is_open(), lgr, "failed source code page load due to \"{}\" not found!", short_path_s);
    return ifs.is_open();
}

void taul::source_code::reset() noexcept {
    *this = std::move(source_code{});
}

void taul::source_code::_populate_pos_map_for_new_page(taul::str new_page_txt) {
    source_pos offset = source_pos(str().length()); // starting offset of new page
    decoder<char> decoder(utf8, new_page_txt);
    uint32_t chr = 1;
    uint32_t ln = 1;
    while (!decoder.done()) {
        const auto decoded = decoder.next();
        TAUL_ASSERT(decoded);
        {
            // populate w/ new entries
            const _pos_map_entry entry{
                chr,
                ln,
                uint32_t(pages().size()),
            };
            for (size_t i = 0; i < decoded->bytes; i++) {
                _pos_map[offset + source_pos(i)] = entry;
            }
        }
        {
            // update chr and ln based on newlines encountered
            if (decoded->cp == U'\r') {
                const auto lookahead = decoder.peek();
                const bool forms_crlf = lookahead && lookahead->cp == U'\n';
                if (!forms_crlf) {
                    chr = 0; // will incr to 1 below
                    ln++;
                }
            }
            else if (decoded->cp == U'\n') {
                chr = 0; // will incr to 1 below
                ln++;
            }
            chr++; // always incr, no matter what
        }
        offset += source_pos(decoded->bytes); // advance offset
    }
}

