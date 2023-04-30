#pragma once


#include <felspar/memory/shared_buffer.hpp>


namespace felspar::memory {


    /// ## Accumulation buffer
    /**
     * A buffer into which data can be accumulated at the end, and consumed from
     * the front.
     */
    template<typename T>
    class accumulation_buffer final {
        using buffer_type = shared_buffer<T>;
        using vector_type = typename buffer_type::vector_type;

        buffer_type buffer = {};
        std::span<T> occupied = {};
        vector_type *pvector = {};
        std::size_t min_buffer = 128;

      public:
        using value_type = T;

        /// ### Constructors
        accumulation_buffer() {}
        explicit accumulation_buffer(std::size_t mb) : min_buffer{mb} {}
        accumulation_buffer(accumulation_buffer const &) = default;
        accumulation_buffer(accumulation_buffer &&) = default;

        /// ### Assignment
        accumulation_buffer &operator=(accumulation_buffer const &) = default;
        accumulation_buffer &operator=(accumulation_buffer &&) = default;

        /// ### Information about the current state of the buffer
        bool empty() const noexcept { return occupied.empty(); }
        auto size() const noexcept { return occupied.size(); }

        /// ### Grow the buffer
        template<typename V = value_type>
        void ensure_length(std::size_t const count, V &&t = {}) {
            if (occupied.size() < count) {
                auto const needed = count - occupied.size();
                if (not pvector
                    or pvector->size() + needed > pvector->capacity()) {
                    vector_type v;
                    v.reserve(std::max(count, min_buffer) + occupied.size());
                    v.insert(v.begin(), occupied.begin(), occupied.end());
                    v.insert(v.begin() + v.size(), count - v.size(), t);
                    auto alloc = buffer_type::control_type::wrap_existing(
                            std::move(v));
                    pvector = alloc.second;
                    buffer = buffer_type{std::move(alloc)};
                    occupied = buffer;
                } else {
                    pvector->insert(
                            pvector->begin() + pvector->size(), needed, t);
                    occupied = {occupied.data(), count};
                }
            }
        }

        /// ### Access to the buffer
        value_type &operator[](std::size_t const i) { return occupied[i]; }
        value_type const &operator[](std::size_t const i) const {
            return occupied[i];
        }
        auto begin() { return occupied.begin(); }
        auto end() { return occupied.end(); }

        /// ### Split the buffer
        auto first(std::size_t const count) {
            auto const part = occupied.first(count);
            occupied = occupied.subspan(count);
            return buffer_type{buffer.owner, part};
        }
    };


}
