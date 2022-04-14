#pragma once


#include <felspar/memory/shared_buffer.hpp>


namespace felspar::memory {


    /// A buffer into which data can be accumulated at the end, and consumed
    /// from the front
    template<typename T>
    class accumulation_buffer {
        using buffer_type = shared_buffer<T>;
        buffer_type buffer;

      public:
        using value_type = T;

        /// Information about the current state of the buffer
        bool empty() const noexcept { return buffer.empty(); }
        auto size() const noexcept { return buffer.size(); }

        /// Grow the buffer
        template<typename V = value_type>
        void ensure_length(std::size_t const count, V &&v = {}) {
            if (buffer.size() < count) {
                auto nb = buffer_type::allocate(count, std::forward<V>(v));
                std::copy(buffer.begin(), buffer.end(), nb.begin());
                buffer = nb;
            }
        }

        /// Access to the buffer
        value_type &operator[](std::size_t const i) { return buffer[i]; }
        value_type const &operator[](std::size_t const i) const {
            return buffer[i];
        }
    };


}
