#pragma once


#include <felspar/memory/control.hpp>
#include <felspar/memory/raw_memory.hpp>

#include <vector>


namespace felspar::memory {


    template<typename T>
    class shared_buffer_view;


    /// A shared memory buffer
    template<typename T>
    class shared_buffer final {
        friend class shared_buffer_view<T>;
        friend class shared_buffer_view<T const>;

      public:
        using value_type = T;
        using control_type = control;
        using buffer_type = std::span<T>;

        /// Construction, destruction and assignment
        shared_buffer() {}
        shared_buffer(shared_buffer const &) = delete;
        shared_buffer &operator=(shared_buffer const &) = delete;
        shared_buffer(shared_buffer &&sb)
        : buffer{std::exchange(sb.buffer, {})},
          owner{std::exchange(sb.owner, {})} {}
        shared_buffer &operator=(shared_buffer &&) = delete;
        ~shared_buffer() { control_type::decrement(owner); }

        /// Allocation
        static shared_buffer allocate(std::size_t const count) {
            auto alloc =
                    control_type::wrap_existing(std::vector<value_type>(count));
            shared_buffer sb;
            sb.owner = alloc.first.release();
            sb.buffer = {alloc.second->data(), count};
            return sb;
        }

        /// Information about the buffer
        bool empty() const noexcept { return buffer.empty(); }
        auto size() const noexcept { return buffer.size(); }
        /// Return the memory control block
        control_type *control_block() const noexcept { return owner; }

      private:
        buffer_type buffer;
        control_type *owner = nullptr;
    };


    /// A view onto a shared memory buffer
    template<typename T>
    class shared_buffer_view {
      public:
        using value_type = T;
        using control_type = control;
        using buffer_type = std::span<T>;

        shared_buffer_view(shared_buffer<std::remove_cv_t<value_type>> const &sb)
        : buffer{sb.buffer}, owner{sb.owner} {}

        /// Information about the buffer
        bool empty() const noexcept { return buffer.empty(); }
        auto size() const noexcept { return buffer.size(); }
        /// Return the memory control block
        control_type *control_block() const noexcept { return owner; }

      private:
        buffer_type buffer;
        control_type *owner = nullptr;
    };
    template<typename T>
    shared_buffer_view(shared_buffer<T>) -> shared_buffer_view<T>;


}
