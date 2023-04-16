#pragma once


#include <array>
#include <span>

#include <felspar/concepts.hpp>
#include <felspar/exceptions.hpp>
#include <felspar/memory/sizes.hpp>


namespace felspar::memory {


    /// ## Stack allocated dynamic vector
    /**
     * Grows up to a fixed size determined at compile time. Attempts to add more
     * items than the vector can support will lead to an exception being thrown.
     *
     * Because the storage area is embedded iterators are never invalidated by
     * adding or removing items to the array, but they are invalided by moving
     * the `small_vector`.
     */
    template<typename T, std::size_t N = 32>
    class small_vector final {
        static std::size_t constexpr block_size =
                memory::block_size(sizeof(T), alignof(T));

        std::size_t entries = {};
        std::array<std::byte, block_size * N> storage alignas(T);

      public:
        /// ### Types
        using value_type = std::remove_reference_t<T>;
        using reference_type = std::add_lvalue_reference_t<value_type>;
        using const_reference_type =
                std::add_lvalue_reference_t<value_type const>;
        using pointer_type = std::add_pointer_t<value_type>;
        using const_pointer_type = std::add_pointer_t<value_type const>;

        /// ### Constructors
        constexpr small_vector() noexcept {};
        template<typename... Args>
        small_vector(Args... args) {
            (push_back(std::forward<Args>(args)), ...);
        }
        small_vector(small_vector &&sv) {
            for (auto &&i : sv) { push_back(std::move(i)); }
        }
        constexpr ~small_vector() {
            for (auto &i : *this) { std::destroy_at(&i); }
        }
        /// ### No copy/move
        small_vector(small_vector const &) = delete;
        small_vector &operator=(small_vector const &) = delete;
        small_vector &operator=(small_vector &&) = delete;

        /// ### Capacity and meta-data
        [[nodiscard]] constexpr bool empty() const noexcept {
            return entries == 0;
        }
        [[nodiscard]] constexpr auto capacity() const noexcept { return N; }
        [[nodiscard]] constexpr auto size() const noexcept { return entries; }

        /// ### Access
        [[nodiscard]] constexpr const_reference_type
                operator[](std::size_t const i) const {
            return *(data() + i);
        }
        [[nodiscard]] constexpr reference_type operator[](std::size_t const i) {
            return *(data() + i);
        }

        [[nodiscard]] constexpr const_pointer_type data() const noexcept {
            return std::launder(reinterpret_cast<T const *>(storage.data()));
        }
        [[nodiscard]] constexpr pointer_type data() noexcept {
            return std::launder(reinterpret_cast<T *>(storage.data()));
        }

        [[nodiscard]] constexpr reference_type back() {
            return *(data() + entries - 1);
        }
        [[nodiscard]] constexpr const_reference_type back() const {
            return *(data() + entries - 1);
        }
        [[nodiscard]] constexpr reference_type front() { return *data(); }
        [[nodiscard]] constexpr const_reference_type front() const {
            return *data();
        }

        /// ### Automatic conversion
        operator std::span<value_type>() noexcept { return {data(), size()}; }
        operator std::span<value_type const>() const noexcept {
            return {data(), size()};
        }

        /// ### Iteration
        using iterator = pointer_type;
        [[nodiscard]] constexpr iterator begin() noexcept { return data(); }
        [[nodiscard]] constexpr iterator end() noexcept {
            return data() + size();
        }
        using const_iterator = const_pointer_type;
        [[nodiscard]] constexpr const_iterator begin() const noexcept {
            return data();
        }
        [[nodiscard]] constexpr const_iterator end() const noexcept {
            return data() + size();
        }
        [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
            return data();
        }
        [[nodiscard]] constexpr const_iterator cend() const noexcept {
            return data() + size();
        }

        /// ### Adding data
        template<typename... Args>
        void emplace_back(Args... args) {
            if (entries >= capacity()) {
                throw felspar::stdexcept::length_error{
                        "Over small_vector capacity"};
            }
            new (storage.data() + block_size * entries++)
                    T{std::forward<Args>(args)...};
        }
        void push_back(T t) {
            if (entries >= capacity()) {
                throw felspar::stdexcept::length_error{
                        "Over small_vector capacity"};
            }
            new (storage.data() + block_size * entries++) T{std::move(t)};
        }
        /// ### Removing data
        void clear() {
            for (auto &v : *this) { std::destroy_at(&v); }
            entries = {};
        }
        void erase(iterator pos)
            requires assignable_from<value_type &, value_type &&>
        {
            for (auto from = pos + 1u, e = end(); from != e; ++from, ++pos) {
                *pos = std::move(*from);
            }
            std::destroy_at(pos);
            --entries;
        }
        void erase(iterator pos) {
            for (auto from = pos + 1u, e = end(); from != e; ++from, ++pos) {
                std::destroy_at(pos);
                new (pos) T(std::move(*from));
            }
            std::destroy_at(pos);
            --entries;
        }
    };

    template<typename... Args>
    small_vector(Args...) -> small_vector<std::common_type_t<Args...>>;


}
