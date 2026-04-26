#pragma once


#include <cstddef>
#include <new>


namespace felspar::memory {


    /**
     * ## New/Delete allocator strategy
     *
     * A stateless strategy that delegates directly to the global
     * `::operator new` and `::operator delete`. This provides heap allocation
     * as a first-class strategy, useful as a fallback in compositors or as a
     * baseline for telemetry.
     *
     * This strategy is default-constructible, copyable, and movable because it
     * has no state.
     */
    struct new_delete_strategy final {
        /// ### Allocate from the global heap
        [[nodiscard]] std::byte *allocate(std::size_t const bytes) {
            return static_cast<std::byte *>(::operator new(bytes));
        }


        /// ### Deallocate back to the global heap
        void deallocate(void *ptr, std::size_t bytes) noexcept {
#if __cpp_sized_deallocation
            ::operator delete(ptr, bytes);
#else
            ::operator delete(ptr);
#endif
        }
    };


}
