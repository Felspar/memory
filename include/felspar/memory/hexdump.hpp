#pragma once

#include <ostream>
#include <span>


namespace felspar::memory {


    constexpr std::size_t default_hexdump_byte_range = 256;


    /// Print a hex dump of the memory to the provided stream of only up to the
    /// first `bytes` bytes
    std::ostream &
            hexdump(std::ostream &,
                    std::span<std::byte const>,
                    std::size_t bytes = default_hexdump_byte_range);

    template<typename C>
    inline std::ostream &
            hexdump(std::ostream &s,
                    std::span<C> b,
                    std::size_t bytes = default_hexdump_byte_range) {
        static_assert(sizeof(C) == 1, "Can only hexdump byte like types");
        return hexdump(
                s,
                std::span<std::byte const>(
                        reinterpret_cast<std::byte const *>(b.data()), b.size()),
                bytes);
    }


    /// The following code allows for the use of hexdump as a wrapper when used
    /// with `operator<<` to print to a stream.
    namespace detail {
        template<typename C>
        struct hexdump_proxy {
            std::span<C const> b;
        };
        template<typename C>
        inline std::ostream &operator<<(std::ostream &s, hexdump_proxy<C> p) {
            return hexdump(s, p.b);
        }
    }

    template<typename C, std::size_t N>
    inline detail::hexdump_proxy<C> hexdump(std::array<C, N> const &b) {
        return {b};
    }
    template<typename C>
    inline detail::hexdump_proxy<C> hexdump(std::span<C> b) {
        return {b};
    }


}
