#pragma once


#include <felspar/memory/pmr.hpp>
#include <felspar/memory/pool.strategy.hpp>

#include <string>
#include <vector>


namespace felspar::memory::auto_pool {


    /// A pool of a single fixed size. Any allocation larger than this is given
    /// to the backing allocator
    class pmr : public felspar::pmr::memory_resource {
        void *do_allocate(
                std::size_t const bytes, std::size_t alignment) override {
            if (alignment > alignof(std::max_align_t)) {
                throw std::logic_error{
                        "Requested over-aligned memory from the pool of "
                        + std::to_string(alignment) + " bytes"};
            } else {
                void *const memory =
                        bytes <= max_size ? pool::allocate(pool) : nullptr;
                if (not memory) {
                    return allocator->allocate(std::max(max_size, bytes));
                } else {
                    return memory;
                }
            }
        }
        void do_deallocate(
                void *p, std::size_t const bytes, std::size_t) override {
            if (bytes <= max_size) {
                pool::deallocate(pool, p);
            } else {
                allocator->deallocate(p, bytes);
            }
        }
        bool do_is_equal(memory_resource const &other) const noexcept override {
            return this == &other;
        }

        std::size_t max_size;
        pmr::memory_resource *allocator;
        std::vector<void *> pool;

      public:
        pmr(std::size_t const max_size, pmr::memory_resource *const allocator)
        : max_size{max_size}, allocator{allocator} {}
        ~pmr() {
            if (allocator) {
                for (auto v : pool) { allocator->deallocate(v, max_size); }
            }
        }

        pmr(pmr const &) = delete;
        pmr &operator=(pmr const &) = delete;
    };


}
