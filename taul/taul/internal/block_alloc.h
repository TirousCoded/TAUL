

#pragma once


#include <cstddef>
#include <stdint.h>
#include <array>
#include <vector>

#include "../asserts.h"
#include "../logger.h"


namespace taul::internal {


    // this is a paged arena allocator for fixed size memory blocks

    // this uses incremental slicing of new pages, and recycling of existing
    // blocks via a linked list stack, in order to achieve high speed allocation
    // and deallocation that has really good data cache performance

    // this is based on code in a previous project of mine, in which I used
    // a similar system to create a more general purpose allocator, w/ me
    // having done a lot of optimizing of that one, and so I'm bringing what
    // I learned into this one


    template<std::size_t Bytes>
    struct alignas(alignof(std::max_align_t)) block final {

        static_assert(Bytes >= alignof(std::max_align_t));
        static_assert(Bytes >= sizeof(std::uintptr_t));

        alignas(alignof(std::max_align_t)) std::uint8_t _bytes[Bytes];
    };

    template<std::size_t Bytes, std::size_t BlocksPerPage>
    class block_alloc final {
    public:

        using block_t = block<Bytes>;

        static_assert(std::is_trivially_constructible_v<block_t>);
        static_assert(std::is_trivially_copyable_v<block_t>);

        using page_t = std::array<block_t, BlocksPerPage>;


        inline block_alloc(const std::shared_ptr<logger>& lgr);

        inline ~block_alloc() noexcept;


        // these report system diagnostics

        inline std::size_t pages() const noexcept;                 // returns pages allocated
        inline std::size_t blocks_recycled() const noexcept;       // returns blocks freed, awaiting recycling
        inline std::size_t blocks_outstanding() const noexcept;    // returns blocks allocated, awaiting freeing


        // these perform allocation/deallocation

        inline block_t* alloc();

        inline void free(block_t* x) noexcept;


    private:

        std::shared_ptr<logger> _lgr;

        // this is the pointer to our recycled block linked list stack

        block_t* _top_recycled_block = nullptr;

        // while *much slower* than our linked list stack, since pages are 
        // allocated infrequently, and that this is only used to clean them
        // all up at block_alloc destruction, we'll just use a vector

        std::vector<page_t*> _pages;

        // this is the latest page bound for incremental slicing

        page_t* _latest_page = nullptr;

        // this is the number of blocks incrementally sliced thus far, w/
        // there being BlocksPerPage blocks per page

        std::size_t _blocks_incr_sliced = 0;

        // this count the total and recycled block counts

        std::size_t _total_blocks = 0;
        std::size_t _recycled_blocks = 0;


        inline void _cleanup_pages() noexcept;
        
        inline block_t* _try_get_recycled_block() noexcept;
        inline block_t* _try_incr_slice_block_from_latest_page() noexcept;
        inline void _alloc_new_page();

        inline void _recycle_block(block_t* x) noexcept;
    };

    template<std::size_t Bytes, std::size_t BlocksPerPage>
    inline block_alloc<Bytes, BlocksPerPage>::block_alloc(const std::shared_ptr<logger>& lgr)
        : _lgr(lgr) {}
    
    template<std::size_t Bytes, std::size_t BlocksPerPage>
    inline block_alloc<Bytes, BlocksPerPage>::~block_alloc() noexcept {
        TAUL_ASSERT(blocks_outstanding() == 0); // crash if still have blocks outstanding
        _cleanup_pages();
    }
    
    template<std::size_t Bytes, std::size_t BlocksPerPage>
    inline std::size_t block_alloc<Bytes, BlocksPerPage>::pages() const noexcept {
        return _pages.size();
    }
    
    template<std::size_t Bytes, std::size_t BlocksPerPage>
    inline std::size_t block_alloc<Bytes, BlocksPerPage>::blocks_recycled() const noexcept {
        return _recycled_blocks;
    }
    
    template<std::size_t Bytes, std::size_t BlocksPerPage>
    inline std::size_t block_alloc<Bytes, BlocksPerPage>::blocks_outstanding() const noexcept {
        return _total_blocks - blocks_recycled();
    }
    
    template<std::size_t Bytes, std::size_t BlocksPerPage>
    inline block_alloc<Bytes, BlocksPerPage>::block_t* block_alloc<Bytes, BlocksPerPage>::alloc() {
        block_t* result{};
        result = _try_get_recycled_block();
        if (!result) {
            result = _try_incr_slice_block_from_latest_page();
        }
        if (!result) {
            _alloc_new_page();
            TAUL_LOG(_lgr, "alloc new page ({} bytes) ({} blocks-per-page)", sizeof(page_t), BlocksPerPage);
            result = _try_incr_slice_block_from_latest_page();
        }
        TAUL_ASSERT(result);
        TAUL_LOG(_lgr, "alloc block ({} bytes) at {}", Bytes, (std::uintptr_t)result);
        return result;
    }
    
    template<std::size_t Bytes, std::size_t BlocksPerPage>
    inline void block_alloc<Bytes, BlocksPerPage>::free(block_t* x) noexcept {
        TAUL_ASSERT(x != nullptr);
        TAUL_LOG(_lgr, "free block ({} bytes) at {}", Bytes, (std::uintptr_t)x);
        _recycle_block(x);
    }

    template<std::size_t Bytes, std::size_t BlocksPerPage>
    inline void block_alloc<Bytes, BlocksPerPage>::_cleanup_pages() noexcept {
        TAUL_LOG(_lgr, "cleaning up {} pages", pages());
        for (page_t*& I : _pages) {
            std::free(I);
        }
    }

    template<std::size_t Bytes, std::size_t BlocksPerPage>
    inline block_alloc<Bytes, BlocksPerPage>::block_t* block_alloc<Bytes, BlocksPerPage>::_try_get_recycled_block() noexcept {
        if (blocks_recycled() == 0) {
            //TAUL_LOG(_lgr, "_try_get_recycled_block failed!");
            return nullptr;
        }
        block_t* result{};
        if (_top_recycled_block) {
            // make top recycled block our result
            result = _top_recycled_block;
            // bind next block down in linked list stack as the new top
            _top_recycled_block = *(block_t**)_top_recycled_block;
            _recycled_blocks--;
        }
        else TAUL_DEADEND;
        TAUL_ASSERT(result);
        return result;
    }
    
    template<std::size_t Bytes, std::size_t BlocksPerPage>
    inline block_alloc<Bytes, BlocksPerPage>::block_t* block_alloc<Bytes, BlocksPerPage>::_try_incr_slice_block_from_latest_page() noexcept {
        if (_blocks_incr_sliced == BlocksPerPage) {
            return nullptr;
        }
        if (!_latest_page) {
            return nullptr;
        }
        block_t* result{};
        if (_latest_page) {
            result = _latest_page->data() + _blocks_incr_sliced;
            _blocks_incr_sliced++;
            _total_blocks++;
        }
        else TAUL_DEADEND;
        TAUL_ASSERT(result);
        return result;
    }
    
    template<std::size_t Bytes, std::size_t BlocksPerPage>
    inline void block_alloc<Bytes, BlocksPerPage>::_alloc_new_page() {
        page_t* _new_page = (page_t*)std::malloc(sizeof(page_t));
        TAUL_ASSERT(_new_page);
        _pages.push_back(_new_page);
        _latest_page = _new_page;
        _blocks_incr_sliced = 0;
    }

    template<std::size_t Bytes, std::size_t BlocksPerPage>
    inline void block_alloc<Bytes, BlocksPerPage>::_recycle_block(block_t* x) noexcept {
        TAUL_ASSERT(x != nullptr);
        // write current top's address to x
        *(block_t**)x = _top_recycled_block;
        // make x the new top
        _top_recycled_block = x;
        _recycled_blocks++;
    }
}

