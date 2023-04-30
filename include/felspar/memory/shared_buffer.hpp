#pragma once


#include <felspar/exceptions.hpp>
#include <felspar/memory/control.hpp>

#include <vector>


namespace felspar::memory {


    template<typename T>
    class accumulation_buffer;
    template<typename T>
    class shared_buffer_view;


    /// ## A shared memory buffer
    template<typename T>
    class shared_buffer final {
        friend class accumulation_buffer<T>;
        friend class shared_buffer_view<T>;
        friend class shared_buffer_view<T const>;
        using vector_type = std::vector<T>;
        using control_type = control;
        using buffer_type = std::span<T>;

        /// ### Private constructors
        explicit shared_buffer(
                std::pair<std::unique_ptr<control_type>, vector_type *> alloc)
        : buffer{alloc.second->data(), alloc.second->size()},
          owner{alloc.first.release()} {}
        shared_buffer(control_type *o, buffer_type b)
        : buffer{b}, owner{control_type::increment(o)} {}

      public:
        using value_type = T;

        /// ### Construction, destruction and assignment
        shared_buffer() {}
        shared_buffer(shared_buffer const &sb)
        : buffer{sb.buffer}, owner{control_type::increment(sb.owner)} {}
        shared_buffer &operator=(shared_buffer const &sb) {
            buffer = sb.buffer;
            control_type::decrement(owner);
            owner = control_type::increment(sb.owner);
            return *this;
        }
        shared_buffer(shared_buffer &&sb)
        : buffer{std::exchange(sb.buffer, {})},
          owner{std::exchange(sb.owner, {})} {}
        shared_buffer &operator=(shared_buffer &&sb) {
            control_type::decrement(owner);
            owner = std::exchange(sb.owner, {});
            buffer = std::exchange(sb.buffer, {});
            return *this;
        }
        ~shared_buffer() { control_type::decrement(owner); }

        /// ### Allocation
        template<typename V = value_type>
        static shared_buffer allocate(std::size_t const count, V &&v = {}) {
            return shared_buffer{control_type::wrap_existing(
                    vector_type(count, std::forward<V>(v)))};
        }

        /// ### Information about the buffer
        bool empty() const noexcept { return buffer.empty(); }
        auto size() const noexcept { return buffer.size(); }
        /// Return the memory control block
        control_type *control_block() const noexcept { return owner; }

        /// ### Access to the buffer
        value_type &operator[](std::size_t const i) { return buffer[i]; }
        value_type const &operator[](std::size_t const i) const {
            return buffer[i];
        }
        value_type &
                at(std::size_t const i,
                   felspar::source_location const &loc =
                           felspar::source_location::current()) {
            if (i >= buffer.size()) {
                throw felspar::stdexcept::logic_error{"Buffer overrun", loc};
            } else {
                return buffer[i];
            }
        }
        value_type const &
                at(std::size_t const i,
                   felspar::source_location const &loc =
                           felspar::source_location::current()) const {
            if (i >= buffer.size()) {
                throw felspar::stdexcept::logic_error{"Buffer overrun", loc};
            } else {
                return buffer[i];
            }
        }
        auto begin() { return buffer.begin(); }
        auto end() { return buffer.end(); }

        value_type const *data() const noexcept { return buffer.data(); }

        /// ### Implicit conversions
        operator std::span<value_type>() { return buffer; }

        /// ### Sharing the buffer
        shared_buffer first(std::size_t const items) {
            return {owner, buffer.first(items)};
        }

      private:
        buffer_type buffer;
        control_type *owner = nullptr;
    };


    /// ## A view onto a shared memory buffer
    template<typename T>
    class shared_buffer_view {
      public:
        using value_type = T;
        using control_type = control;
        using buffer_type = std::span<T>;

        shared_buffer_view(shared_buffer<std::remove_cv_t<value_type>> const &sb)
        : buffer{sb.buffer}, owner{sb.owner} {}

        /// ### Information about the buffer
        bool empty() const noexcept { return buffer.empty(); }
        auto size() const noexcept { return buffer.size(); }
        /// ### Return the memory control block
        control_type *control_block() const noexcept { return owner; }

      private:
        buffer_type buffer;
        control_type *owner = nullptr;
    };
    template<typename T>
    shared_buffer_view(shared_buffer<T>) -> shared_buffer_view<T>;


}
