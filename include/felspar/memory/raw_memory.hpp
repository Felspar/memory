#pragma once


#include <array>
#include <memory>
#include <new>
#include <type_traits>


namespace felspar::memory {


    /// ## Memory for storage of a type `T`
    /**
     * The `raw_memory` does not have any way to know whether the storage is in
     * use or not. Tracking usage of the storage is the responsibility of the
     * user of this type.
     */
    template<typename T>
    class raw_memory {
        std::array<std::byte, sizeof(T)> pen alignas(T);


      public:
        using value_type = std::remove_cv_t<std::remove_reference_t<T>>;
        using reference_type = std::add_lvalue_reference_t<value_type>;
        using const_reference_type =
                std::add_lvalue_reference_t<value_type const>;
        using pointer_type = std::add_pointer_t<value_type>;
        using const_pointer_type = std::add_pointer_t<value_type const>;


        /// ### Construction and assignment
        /// Users must use the APIs to manage copy and move themselves
        constexpr raw_memory() = default;
        raw_memory(raw_memory const &) = delete;
        raw_memory(raw_memory &&) = delete;
        raw_memory &operator=(raw_memory const &) = delete;
        raw_memory &operator=(raw_memory &&) = delete;


        /// ### Memory location
        /// Returns the memory location of any value stored in the storage
        constexpr pointer_type data() noexcept {
            return std::launder(reinterpret_cast<pointer_type>(pen.data()));
        }
        constexpr const_pointer_type data() const noexcept {
            return std::launder(
                    reinterpret_cast<const_pointer_type>(pen.data()));
        }


        /// ### Access to the value
        /**
         * Returns the value. Undefined behaviour if there is nothing in the
         * storage
         */
        constexpr reference_type value() noexcept { return *data(); }
        constexpr const_reference_type value() const noexcept {
            return *data();
        }


        /// ### Setting the value

        /// #### Construct
        /**
         * Constructs a new item in the memory passing the arguments to the
         * constructor. This is undefined behaviour if the memory is already
         * occupied.
         */
        template<typename... Args>
        reference_type emplace(Args... args) {
            new (data()) value_type{std::forward<Args>(args)...};
            /// TODO The below is not available in libc++ yet
            // std::construct_at<value_type>(data(), std::forward<Args>(args)...);
            return *data();
        }
        /// #### Copy
        reference_type copy(value_type const &v) {
            new (data()) value_type{v};
            return *data();
        }


        /// ### Conditional operations

        /// Conditionally assigns/constructs into the storage
        reference_type assign_or_emplace(bool const holds, value_type t) {
            if (holds) {
                return (*data() = std::move(t));
            } else {
                return emplace(std::move(t));
            }
        }
        /// Conditionally destroys the held item
        void destroy_if(bool const destroy) noexcept {
            if (destroy) { std::destroy_at(data()); }
        }
    };


}
