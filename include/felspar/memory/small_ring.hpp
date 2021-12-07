#pragma once


#include <felspar/memory/raw_memory.hpp>


namespace felspar::memory {


    /**
     * ## Small ring buffer
     *
     * Ring buffer with an API based around pushing single items to the head and
     * which has a fixed compile-time size.
     */
    template<typename T, std::size_t S>
    class small_ring {
        static constexpr std::size_t ring_size = S;
        static constexpr std::size_t buffer_size = ring_size + 1u;

        std::array<raw_memory<T>, buffer_size> buffer;
        std::size_t base{}, top{};

        /// Return the increment of top/base wrapping around at the top
        static std::size_t increment(std::size_t const s) {
            if (s == ring_size) {
                return 0;
            } else {
                return s + 1u;
            }
        }

      public:
        /// Return true if the ring is empty
        bool empty() const noexcept { return base == top; }
        std::size_t size() const noexcept {
            if (base <= top) {
                return top - base;
            } else {
                return ring_size + base - top - 1;
            }
        }

        /// Return items from the buffer
        T const &front() { return buffer[top].value(); }
        T const &back() { return buffer[increment(base)].value(); }

        /// Push an item to the head end of the buffer
        void push(T t) {
            top = increment(top);
            if (top == base) { pop_back(); }
            buffer[top].emplace(std::move(t));
        }
        /// Pop the item off the back of the ring
        void pop_back() {
            buffer[base].destroy_if(true);
            base = increment(base);
        }
    };


}
