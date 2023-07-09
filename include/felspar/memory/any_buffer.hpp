#pragma once


#include <felspar/exceptions.hpp>

#include <array>
#include <typeinfo>


namespace felspar::memory {


    /// ## A buffer for any object
    /**
     * A buffer that can be used to transport any type and from which a value of
     * the correct type can be later retrieved.
     *
     * The buffer contains a small amount of memory meaning that the object can
     * be embedded.
     */
    template<std::size_t BS = 128, std::size_t AL = 16>
    struct any_buffer {
        static std::size_t constexpr alignment = AL;
        static std::size_t constexpr buffer_size = BS;


        /// ### Construction
        constexpr any_buffer() noexcept {}
        any_buffer(any_buffer &&o)
        : typeptr{o.typeptr}, move_into{o.move_into}, deleter{o.deleter} {
            if (move_into) { move_into(buffer.data(), o.buffer.data()); }
        }
        any_buffer(any_buffer const &) = delete;
        ~any_buffer() { destroy(); }

        /// #### Place the object into the buffer
        template<typename T>
        any_buffer(T object) {
            emplace<T>(std::move(object));
        }


        /// ### Queries

        /// #### Return true if there is a held object
        bool has_value() const noexcept {
            return typeptr and move_into and deleter;
        }
        explicit operator bool() const noexcept { return has_value(); }

        /// #### The typeid for the held object
        auto const &type() const {
            if (not typeptr) {
                return typeid(void);
            } else {
                return *typeptr;
            }
        }

        /// #### Return a reference to the contained object
        template<typename T>
        T &value() {
            if (typeid(T) == *typeptr) {
                return unsafe_value<T>();
            } else {
                throw felspar::stdexcept::logic_error{
                        "This any_buffer is empty"};
            }
        }
        template<typename T>
        T const &value() const {
            if (typeid(T) == *typeptr) {
                return unsafe_value<T>();
            } else {
                throw felspar::stdexcept::logic_error{
                        "This any_buffer is empty"};
            }
        }


        /// ### Modification

        template<typename T, typename... Args>
        void emplace(Args... args) {
            static_assert(
                    std::alignment_of_v<T> <= alignment,
                    "Alignment requirement of held type is too high");
            static_assert(
                    sizeof(T) <= buffer_size,
                    "Large object support not yet implemented");
            typeptr = &typeid(T);
            new (buffer.data()) T{std::forward<Args>(args)...};
            move_into = [](std::byte *into, std::byte *from) {
                new (into) T{std::move(*reinterpret_cast<T *>(from))};
            };
            deleter = [](std::byte *d) {
                std::destroy_at(std::launder(reinterpret_cast<T *>(d)));
            };
        }

        any_buffer &operator=(any_buffer &&o) {
            destroy();
            typeptr = std::exchange(o.typeptr, nullptr);
            move_into = std::exchange(o.move_into, nullptr);
            deleter = std::exchange(o.deleter, nullptr);
            if (o.move_into) { move_into(buffer.data(), o.buffer.data()); }
            return *this;
        }
        template<typename T>
        any_buffer &operator=(T t) {
            destroy();
            emplace<T>(std::move(t));
            return *this;
        }

        any_buffer &operator=(any_buffer const &) = delete;


      private:
        void destroy() {
            if (deleter) { deleter(buffer.data()); }
        }
        template<typename T>
        T &unsafe_value() {
            return *reinterpret_cast<T *>(buffer.data());
        }
        template<typename T>
        T const &unsafe_value() const {
            return *reinterpret_cast<T *>(buffer.data());
        }

        std::array<std::byte, buffer_size> buffer alignas(alignment);
        std::type_info const *typeptr = nullptr;
        void (*move_into)(std::byte *, std::byte *) = nullptr;
        void (*deleter)(std::byte *) = nullptr;
    };


}
