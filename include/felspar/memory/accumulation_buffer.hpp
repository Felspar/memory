#pragma once


#include <felspar/memory/shared_buffer.hpp>


namespace felspar::memory {


    /// A buffer into which data can be accumulated at the end, and consumed
    /// from the front
    template<typename T>
    class accumulation_buffer final {
        using buffer_type = shared_buffer<T>;
        using vector_type = typename buffer_type::vector_type;
        buffer_type buffer = {};
        std::span<T> occupied = {};
        vector_type *pvector = {};

      public:
        using value_type = T;

        /// Information about the current state of the buffer
        bool empty() const noexcept { return buffer.empty(); }
        auto size() const noexcept { return buffer.size(); }

        /// Grow the buffer
        template<typename V = value_type>
        void ensure_length(std::size_t const count, V &&t = {}) {
            if (buffer.size() < count) {
                vector_type v;
                v.reserve(count + buffer.size());
                v.insert(v.begin(), buffer.begin(), buffer.end());
                v.insert(v.begin() + v.size(), count - v.size(), t);
                auto alloc =
                        buffer_type::control_type::wrap_existing(std::move(v));
                pvector = alloc.second;
                buffer = buffer_type{std::move(alloc)};
                occupied = buffer;
            }
        }

        /// Access to the buffer
        value_type &operator[](std::size_t const i) { return buffer[i]; }
        value_type const &operator[](std::size_t const i) const {
            return buffer[i];
        }
    };


}
