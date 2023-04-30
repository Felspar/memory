#pragma once


#include <felspar/memory/shared_view.hpp>


namespace felspar::memory {


    /// ### Shared memory vector
    /**
     * Allows for memory allocations of a vector like type to be shared between
     * instances. Access into the held data is not thread safe, but updates to
     * the shared counts are, so immutable data may be safely shared between
     * `shared_vector` instances.
     */
    template<typename T>
    class shared_vector final {
        template<typename Tt>
        friend class shared_view;

        typename shared_view<T>::span_type buffer;
        typename shared_view<T>::control_type *owner = nullptr;

        shared_vector(
                typename shared_view<T>::span_type s,
                typename shared_view<T>::control_type *o)
        : buffer{s}, owner{control_type::increment(o)} {}

      public:
        using view_type = shared_view<T>;
        using span_type = typename view_type::span_type;
        using const_span_type = typename view_type::const_span_type;
        using control_type = typename view_type::control_type;

        /// ### Constructors
        shared_vector() {}
        explicit shared_vector(std::size_t const elements) {
            auto created =
                    control_type::allocate(elements * sizeof(T), alignof(T));
            owner = created.first.release();
            buffer = created.second;
        }

        /// Copy/move/assignment etc.
        shared_vector(shared_vector const &sb)
        : shared_vector{sb.buffer, sb.owner} {}

        /// Destructor
        ~shared_vector() { control_type::decrement(owner); }

        /// ### Manipulation
        /**
         * Returns the first part of the shared vector without requiring a new
         * memory allocation. There is no locking for shared memory access, so
         * updates are not thread safe.
         */
        shared_vector consume_first(std::size_t const elements) {
            span_type consumed = {buffer.data(), elements};
            buffer = buffer.subspan(elements);
            return {consumed, owner};
        }

        /// ### Conversions
        constexpr operator shared_view<T>() const { return {buffer, owner}; }
        constexpr operator span_type() const noexcept { return buffer; }

        /// ### Queries
        constexpr std::size_t size() const noexcept { return buffer.size(); }
        constexpr std::byte *data() noexcept { return buffer.data(); }
        constexpr std::byte const *data() const noexcept {
            return buffer.data();
        }

        constexpr span_type memory() const noexcept { return buffer; }
        constexpr const_span_type cmemory() const noexcept { return buffer; }
    };

    template<typename T>
    inline shared_view<T>::operator shared_vector<T>() const {
        return {buffer, owner};
    }


    /// ### Type aliases
    using shared_bytes = shared_vector<std::byte>;


}
