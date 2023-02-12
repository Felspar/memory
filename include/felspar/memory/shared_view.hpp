#pragma once


#include <felspar/memory/control.hpp>

#include <span>


namespace felspar::memory {


    template<typename T>
    class shared_vector;


    /// ## View into a shared vector
    /**
     * The view is taken from a `shared_vector` or from any other contiguous
     * memory. If the view is over a `shared_vector` then a `shared_vector` can
     * be reconstituted without the need for any more memory allocations.
     */
    template<typename T>
    class shared_view final {
        template<typename Tt>
        friend class shared_vector;

        std::span<std::byte> buffer;
        felspar::memory::control *owner = nullptr;

        constexpr shared_view(
                std::span<std::byte> s, felspar::memory::control *o)
        : buffer{s}, owner{o} {}

      public:
        /// ### Types
        using span_type = std::span<std::byte>;
        using const_span_type = std::span<std::byte const>;
        using control_type = felspar::memory::control;

        /// ### Conversions
        explicit operator shared_vector<T>() const;

        /// ### Queries
        constexpr std::size_t size() const noexcept { return buffer.size(); }
        constexpr std::byte *data() noexcept { return buffer.data(); }
        constexpr std::byte const *data() const noexcept {
            return buffer.data();
        }

        constexpr span_type memory() const noexcept { return buffer; }
        constexpr const_span_type cmemory() const noexcept { return buffer; }
    };


    /// ### Type aliases
    using shared_byte_view = shared_view<std::byte>;


}
