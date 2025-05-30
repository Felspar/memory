#pragma once


#include <felspar/concepts.hpp>
#include <felspar/memory/exceptions.hpp>
#include <felspar/memory/sizes.hpp>

#include <array>
#include <concepts>
#include <memory>
#include <new>
#include <span>


namespace felspar::memory {


    /// ## Stack allocated dynamic vector
    /**
     * Grows up to a fixed size determined at compile time. Attempts to add more
     * items than the vector can support will lead to an exception being thrown.
     *
     * Because the storage area is embedded iterators are never invalidated by
     * adding or removing items to the end of the array.
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
        constexpr ~small_vector() { clear(); }


        /// ### Copy/move
        small_vector(small_vector const &sv) {
            for (auto const &i : sv) { push_back(i); }
        }
        small_vector(small_vector &&sv) {
            for (auto &&i : sv) { push_back(std::move(i)); }
        }
        small_vector &operator=(small_vector const &sv) {
            clear();
            for (auto &&i : sv) { push_back(i); }
            return *this;
        }
        small_vector &operator=(small_vector &&sv) {
            clear();
            for (auto &&i : sv) { push_back(std::move(i)); }
            return *this;
        }


        /// ### Capacity and meta-data
        [[nodiscard]] constexpr bool empty() const noexcept {
            return entries == 0;
        }
        [[nodiscard]] constexpr auto capacity() const noexcept { return N; }
        [[nodiscard]] constexpr auto size() const noexcept { return entries; }
        [[nodiscard]] constexpr bool has_room() const noexcept {
            return entries < capacity();
        }


        /// ### Comparison
        friend bool operator==(small_vector const &l, small_vector const &r)
            requires(std::equality_comparable<value_type>)
        {
            if (l.size() == r.size()) {
                for (std::size_t index{}; index < l.size(); ++index) {
                    if (l[index] != r[index]) { return false; }
                }
                return true;
            } else {
                return false;
            }
        }

        /// ### Access
        [[nodiscard]] constexpr const_reference_type
                operator[](std::size_t const i) const {
            return *(data() + i);
        }
        [[nodiscard]] constexpr reference_type operator[](std::size_t const i) {
            return *(data() + i);
        }
        [[nodiscard]] constexpr const_reference_type at(
                std::size_t const i,
                source_location const &loc = source_location::current()) const {
            if (i >= size()) {
                detail::throw_length_error(
                        "Out of bounds access to small_vector", loc);
            }
            return (*this)[i];
        }
        [[nodiscard]] constexpr reference_type
                at(std::size_t const i,
                   source_location const &loc = source_location::current()) {
            if (i >= size()) {
                detail::throw_length_error(
                        "Out of bounds access to small_vector", loc);
            }
            return (*this)[i];
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
        template<typename V>
        void resize(std::size_t const new_size, V const &v) {
            if (new_size < size()) {
                do { erase(data() + size() - 1u); } while (new_size < size());
            } else if (new_size > size()) {
                do { push_back(v); } while (new_size > size());
            }
        }
        void resize(std::size_t const new_size) {
            resize(new_size, value_type{});
        }

        template<typename... Args>
        value_type &emplace_back(Args... args) {
            if (not has_room()) {
                detail::throw_length_error(
                        "Over small_vector capacity",
                        source_location::current());
            }
            return *(new (storage.data() + block_size * entries++)
                             T{std::forward<Args>(args)...});
        }
        value_type &push_back(
                value_type t,
                source_location const &loc = source_location::current()) {
            if (not has_room()) {
                detail::throw_length_error("Over small_vector capacity", loc);
            }
            return *(new (storage.data() + block_size * entries++)
                             T{std::move(t)});
        }


        /// ### Removing data
        void clear() {
            while (entries) {
                std::destroy_at(data() + size() - 1u);
                --entries;
            }
        }
        void erase(iterator pos)
            requires assignable_from<value_type &, value_type &&>
        {
            if (pos == end()) { return; }
            for (auto from = pos + 1u, e = end(); from != e; ++from, ++pos) {
                *pos = std::move(*from);
            }
            std::destroy_at(pos);
            --entries;
        }
        void erase(iterator pos) {
            if (pos == end()) { return; }
            for (auto from = pos + 1u, e = end(); from != e; ++from, ++pos) {
                std::destroy_at(pos);
                new (pos) value_type(std::move(*from));
            }
            std::destroy_at(pos);
            --entries;
        }
        /// #### Erase any item that the predicate matches
        template<typename Predicate>
        std::size_t erase_if(Predicate pred) {
            for (std::size_t idx{}; idx < entries; ++idx) {
                if (pred((*this)[idx])) {
                    std::size_t erased{1};
                    std::destroy_at(data() + idx);
                    for (std::size_t from{idx + 1}; from < entries; ++from) {
                        if (not pred((*this)[from])) {
                            new (data() + idx)
                                    value_type(std::move((*this)[from]));
                            ++idx;
                        } else {
                            std::destroy_at(data() + from);
                            ++erased;
                        }
                    }
                    entries -= erased;
                    return erased;
                }
            }
            return {};
        }
    };


    template<typename... Args>
    small_vector(Args...) -> small_vector<std::common_type_t<Args...>>;


}
