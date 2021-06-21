#pragma once


#include <array>
#include <felspar/exceptions.hpp>


namespace felspar::memory {


    namespace detail {


        /// Calculate how big a space a type will need in a contiguous memory
        /// area taking into account its size and alignment
        constexpr std::size_t
                block_size(std::size_t const size, std::size_t const align) {
            return (align * ((size + align - 1) / align));
        }


    }


    /// Stack allocated dynamic vector
    template<typename T, std::size_t N = 32>
    class small_vector final {
        static std::size_t constexpr block_size =
                detail::block_size(sizeof(T), alignof(T));

        std::size_t entries = {};
        std::array<std::byte, block_size * N> storage alignas(T);

      public:
        using value_type = T;

        /// Constructors
        small_vector() noexcept {};
        template<typename... Args>
        small_vector(Args... args) {
            (push_back(std::forward<Args>(args)), ...);
        }

        /// Capacity and meta-data
        auto capacity() const noexcept { return N; }
        auto size() const noexcept { return entries; }

        /// Access
        T const &operator[](std::size_t const i) const { return *(data() + i); }
        T &operator[](std::size_t const i) { return *(data() + i); }
        T &back() { return *(data() + entries - 1); }
        T const &back() const { return *(data() + entries - 1); }
        T const *data() const noexcept {
            return reinterpret_cast<T const *>(storage.data());
        }
        T *data() noexcept { return reinterpret_cast<T *>(storage.data()); }
        T &front() { return *data(); }
        T const &front() const { return *data(); }

        /// Modifiers
        template<typename... Args>
        void emplace_back(Args... args) {
            if (entries >= capacity()) {
                throw felspar::length_error{"Over small_vector capacity"};
            }
            new (storage.data() + block_size * entries++)
                    T{std::forward<Args>(args)...};
        }
        void push_back(T t) {
            if (entries >= capacity()) {
                throw felspar::length_error{"Over small_vector capacity"};
            }
            new (storage.data() + block_size * entries++) T{std::move(t)};
        }
    };


}
