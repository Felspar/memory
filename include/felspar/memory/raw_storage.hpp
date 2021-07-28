#pragma once


#include <array>
#include <memory>
#include <type_traits>


namespace felspar::memory {


    /// Memory for storage of a type T
    /**
     * The `raw_storage` does not have any way to know whether the storage is in
     * use or not. Tracking usage of the storage is the responsibility of the
     * user of this type.
     */
    template<typename T>
    class alignas(T) raw_storage {
        std::array<std::byte, sizeof(T)> pen;

      public:
        using value_type = std::remove_cv_t<T>;
        using pointer_type = std::add_pointer_t<value_type>;
        using const_pointer_type = std::add_pointer_t<value_type const>;
        using reference_type = value_type &;

        /// Returns the memory location of any value stored in the storage
        pointer_type data() {
            return reinterpret_cast<pointer_type>(pen.data());
        }
        const_pointer_type data() const {
            return reinterpret_cast<const_pointer_type>(pen.data());
        }

        /// Constructs a new item in the memory passing the arguments to the
        /// constructor. This is undefined behaviour if the memory is already
        /// occupied.
        template<typename... Args>
        reference_type emplace(Args... args) {
            std::construct_at<value_type>(data(), std::forward<Args>(args)...);
            return *data();
        }

        /// Conditionally destroys the held item
        void destroy_if(bool const destroy) {
            if (destroy) { std::destroy_at(reinterpret_cast<T *>(pen.data())); }
        }
    };


}
