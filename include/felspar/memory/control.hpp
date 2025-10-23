#pragma once


#include <felspar/memory/sizes.hpp>


#include <atomic>
#include <iterator>
#include <memory>
#include <span>
#include <utility>

#ifndef assert
#include <cassert>
#endif


namespace felspar::memory {


    /// ## Control block for shared memory
    /**
     * A thread safe control block that supports atomic increment and decrements
     * on a counter. When the decrement reaches zero then the (virtual) `free()`
     * function is called.
     */
    struct control {
        /// Use a virtual destructor for type erasure
        virtual ~control() = default;


        /// ### Creation
        /**
         * Creates a new control block with an ownership count of one that wraps
         * the specified object and returns a pointer to both the control block
         * and the object after it has been moved into its new location.
         */
        template<typename S>
        static std::pair<std::unique_ptr<control>, S *> wrap_existing(S &&s) {
            struct sub final : public control {
                S item;
                sub(S &&s) : item{std::move(s)} {}
                ~sub() = default;
                void free() noexcept { delete this; }
            };
            auto made = std::make_unique<sub>(std::move(s));
            S *pitem = &made->item;
            return {std::move(made), pitem};
        }
        /**
         * Allocate a chunk of memory. Return the control block together with a
         * span that encompasses the memory allocated. The initial ownership
         * count will be one.
         */
        static auto
                allocate(std::size_t const bytes, std::size_t const alignment) {
            struct sub final : public control {
                std::byte *memory;
                sub(std::byte *m) noexcept : memory{m} {}
                ~sub() = default;
                void free() noexcept {
                    auto *const m = memory;
                    this->~sub();
                    delete[] m;
                }
            };
            std::size_t const data_offset = block_size(sizeof(sub), alignment);
            std::byte *made = new std::byte[data_offset + bytes];
            return std::pair{
                    std::unique_ptr<control>(new (made) sub{made}),
                    std::span<std::byte>{made + data_offset, bytes}};
        }

        /// ### Count management
        /**
         * Increment and decrement the usage count. We never need to do anything
         * after the increment so we don't need to know the exact count.
         * Decrementing is another matter though as we have to get the exact
         * right count for zero as we will destruct the control block at that
         * point (which in turn will destruct the owned memory.
         *
         * [The memory ordering requirements are described by Raymond
         * Chen.](https://devblogs.microsoft.com/oldnewthing/20251015-00/?p=111686)
         */
        static control *increment(control *c) noexcept {
            if (c) {
                c->ownership_count.fetch_add(1, std::memory_order::release);
            }
            return c;
        }
        static void decrement(control *&cr) noexcept {
            control *c = std::exchange(cr, nullptr);
            if (c
                and c->ownership_count.fetch_sub(1, std::memory_order::acq_rel)
                        == 1u) {
                /**
                 * The call to `fetch_sub` returns the old value, not the value
                 * we set it to, so in practice the free needs to happen when
                 * the old value was one.
                 */
                c->free();
            }
        }


      private:
        virtual void free() noexcept = 0;
        std::atomic<std::size_t> ownership_count = 1u;
    };


    /// ## Ownership tracking iterator
    /**
     * This type can be used to wrap an iterator so that it will also carry a
     * pointer to a control block.
     */
    template<typename Iter, typename Control>
    struct owner_tracking_iterator final {
        using difference_type =
                typename std::iterator_traits<Iter>::difference_type;
        using value_type = typename std::iterator_traits<Iter>::value_type;
        using pointer = typename std::iterator_traits<Iter>::pointer;
        using reference = typename std::iterator_traits<Iter>::reference;
        using iterator_category =
                typename std::iterator_traits<Iter>::iterator_category;

        using iterator_type = Iter;
        using control_type = std::add_pointer_t<Control>;


        constexpr owner_tracking_iterator() : iterator{}, owner{} {}

        constexpr owner_tracking_iterator(
                Iter i, control_type c = nullptr) noexcept
        : iterator{i}, owner{c} {}

        constexpr decltype(auto) operator*() const { return *iterator; }
        constexpr owner_tracking_iterator &operator++() {
            ++iterator;
            return *this;
        }
        constexpr owner_tracking_iterator operator++(int) {
            auto ret = *this;
            iterator++;
            return ret;
        }
        constexpr bool operator==(owner_tracking_iterator i) const noexcept {
            assert(owner == i.owner);
            return iterator == i.iterator;
        }
        constexpr bool operator!=(owner_tracking_iterator i) const noexcept {
            assert(owner == i.owner);
            return iterator != i.iterator;
        }

        decltype(auto) operator-(const owner_tracking_iterator &it) {
            return iterator - it.iterator;
        }

        Iter iterator;
        control_type owner;
    };


}
