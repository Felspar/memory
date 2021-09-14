#pragma once


#include <felspar/memory/holding_pen.hpp>

#include <typeinfo>


namespace felspar::memory {


    /// A buffer that can be used to transport any type and from which a value
    /// of the correct type can be later retrieved
    struct any_buffer {
        static std::size_t constexpr alignment = 8;
        static std::size_t constexpr buffer_size = 64;

        any_buffer() {}
        any_buffer(any_buffer &&o)
        : type{o.type}, move_into{o.move_into}, deleter{o.deleter} {
            if (move_into) { move_into(buffer.data(), o.buffer.data()); }
        }
        any_buffer(any_buffer const &) = delete;
        ~any_buffer() {
            if (deleter) { deleter(buffer.data()); }
        }

        any_buffer &operator=(any_buffer &&) = delete;
        any_buffer &operator=(any_buffer const &) = delete;

        /// Place the object into the buffer
        template<typename T>
        any_buffer(T object) : type(&typeid(T)) {
            static_assert(
                    std::alignment_of_v<T> <= alignment,
                    "Alignment requirement of held type is too high");
            static_assert(
                    sizeof(T) <= buffer_size,
                    "Large object support not yet implemented");
            new (buffer.data()) T{std::move(object)};
            move_into = [](std::byte *into, std::byte *from) {
                new (into) T{std::move(*reinterpret_cast<T *>(from))};
            };
            deleter = [](std::byte *d) {
                std::destroy_at(reinterpret_cast<T *>(d));
            };
        }

        /// Return true if there is a held object
        explicit operator bool() const { return type and deleter; }

        /// Retrieve the value out of the buffer
        template<typename T>
        holding_pen<T> fetch() {
            if (not type) {
                return {};
            } else if (typeid(T) == *type) {
                return {std::move(*reinterpret_cast<T *>(buffer.data()))};
            } else {
                return {};
            }
        }

        /// The typeid for the held object
        auto const &held_type() const {
            if (not type) {
                throw std::runtime_error{"There is no held object"};
            } else {
                return *type;
            }
        }

      private:
        std::type_info const *type = nullptr;
        void (*move_into)(std::byte *, std::byte *) = nullptr;
        void (*deleter)(std::byte *) = nullptr;
        std::array<std::byte, buffer_size> buffer alignas(alignment);
    };


}
