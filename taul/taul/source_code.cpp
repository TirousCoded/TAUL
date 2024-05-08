

#include "source_code.h"

#include <fstream>

#include "asserts.h"


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
    std::swap(_pageLineStartOffsets, x._pageLineStartOffsets);
    std::swap(_lineStarts, x._lineStarts);
}

taul::source_code& taul::source_code::operator=(source_code&& rhs) noexcept {
    if (this != &(rhs)) {
        std::swap(_concat, rhs._concat);
        std::swap(_pages, rhs._pages);
        std::swap(_pageLineStartOffsets, rhs._pageLineStartOffsets);
        std::swap(_lineStarts, rhs._lineStarts);
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
    if (!pos_in_bounds(pos)) {
        return std::nullopt;
    }
    size_t ind = 0;
    for (const auto& I : pages()) {
        if (pos >= I.pos && pos < I.pos + I.length) {
            break;
        }
        ind++;
    }
    TAUL_ASSERT(ind < pages().size());
    return std::make_optional(ind);
}

std::optional<taul::source_location> taul::source_code::location_at(source_pos pos) const noexcept {
    const auto pageIndOpt = page_at(pos);
    if (!pageIndOpt) {
        return std::nullopt;
    }
    const auto pageInd = *pageIndOpt;
    const auto chrAndLine = resolveChrAndLine(pageInd, pos);
    taul::source_location loc{
        pos,
        pages()[pageInd].origin,
        chrAndLine.first,
        chrAndLine.second,
    };
    return std::make_optional(std::move(loc));
}

bool taul::source_code::to_file(const std::filesystem::path& out_path) const {
    if (!out_path.has_filename()) return false;
    std::ofstream stream(out_path);
    stream.seekp(0);
    stream.write(str().data(), str().length());
    stream.close();
    return true;
}

void taul::source_code::add_str(
    taul::str origin,
    taul::str x,
    encoding in_e) {
    if (in_e != utf8) {
        // TODO: this way of doing things should be refactored
        auto temp = convert_encoding<char>(in_e, utf8, std::string_view(x));
        TAUL_ASSERT(temp);
        x = taul::str(temp.value());
    }
    _addLineStarts(x);
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
            buff = convert_encoding<char>(utf8, utf8, buff).value();
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

void taul::source_code::_addLineStarts(taul::str s) {
    _pageLineStartOffsets.push_back(_lineStarts.size());
    source_pos offset = source_pos(str().length());
    for (const auto& I : s) {
        if (I == '\n') {
            _lineStarts.push_back(offset + 1);
        }
        offset++;
    }
}

std::pair<size_t, size_t> taul::source_code::resolveChrAndLine(size_t pageInd, source_pos pos) const noexcept {
    TAUL_ASSERT(pos_in_bounds(pos));
    TAUL_ASSERT(pageInd < pages().size());
    size_t line = 1;
    size_t offset = pages()[pageInd].pos;
    for (size_t i = _pageLineStartOffsets[pageInd]; i < _lineStarts.size(); i++) {
        const auto& lineStart = _lineStarts[i];
        if (pos < lineStart) {
            break;
        }
        line++;
        offset = lineStart;
    }
    const size_t chr = (pos - offset) + 1;
    return std::make_pair(chr, line);
}

