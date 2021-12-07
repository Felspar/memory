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

        std::array<raw_memory<T>, buffer_size> buffer{};
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
        ~small_ring() {
            while (not empty()) { pop_back(); }
        }

        /// Return true if the ring is empty
        bool empty() const noexcept { return base == top; }
        std::size_t size() const noexcept {
            if (base <= top) {
                return top - base;
            } else {
                return ring_size + base - top - 1;
            }
        }

        /// Return items from the buffer. All of these are undefined behaviour
        /// if the ring doesn't contain enough data
        T &front() { return buffer[top].value(); }
        T const &front() const { return buffer[top].value(); }
        T &back() { return buffer[increment(base)].value(); }
        T const &back() const { return buffer[increment(base)].value(); }
        T &operator[](std::size_t const s) {
            std::size_t const p = base + 1u + s;
            if (p >= buffer_size) {
                return buffer[p - buffer_size].value();
            } else {
                return buffer[p].value();
            }
        }
        T const &operator[](std::size_t const s) const {
            return (*const_cast<small_ring *>(this))[s];
        }

        /// Push an item to the head end of the buffer
        void push(T t) {
            top = increment(top);
            if (top == base) { pop_back(); }
            buffer[top].emplace(std::move(t));
        }
        /// Pop the item off the back of the ring
        void pop_back() {
            buffer[increment(base)].destroy_if(true);
            base = increment(base);
        }
    };


}
