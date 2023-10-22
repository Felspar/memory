#pragma once


#include <felspar/memory/pmr.hpp>

#include <stdexcept>
#include <string>
#include <vector>


namespace felspar::memory {


    /// A pool of a single fixed size. Any allocation larger than this is given
    /// to the backing allocator
    class fixed_pool : public pmr::memory_resource {
        void *do_allocate(std::size_t bytes, std::size_t alignment) override {
            if (alignment > alignof(std::max_align_t)) {
                throw std::logic_error{
                        "Requested over-aligned memory from the pool of "
                        + std::to_string(alignment) + " bytes"};
            } else if (bytes <= max_size and not pool.empty()) {
                std::byte *const v = pool.back();
                pool.pop_back();
                return v;
            } else {
                return allocator->allocate(std::max(max_size, bytes));
            }
        }
        void do_deallocate(void *p, std::size_t bytes, std::size_t) override {
            if (bytes <= max_size) {
                pool.push_back(reinterpret_cast<std::byte *>(p));
            } else {
                allocator->deallocate(p, bytes);
            }
        }
        bool do_is_equal(memory_resource const &other) const noexcept override {
            return this == &other;
        }

        std::size_t max_size;
        pmr::memory_resource *allocator;
        std::vector<std::byte *> pool;

      public:
        fixed_pool(
                std::size_t const max_size,
                pmr::memory_resource *const allocator)
        : max_size{max_size}, allocator{allocator} {}
        ~fixed_pool() {
            if (allocator) {
                for (auto v : pool) { allocator->deallocate(v, max_size); }
            }
        }

        fixed_pool(fixed_pool const &) = delete;
        fixed_pool &operator=(fixed_pool const &) = delete;
    };


}
