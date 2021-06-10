#pragma once


#include <memory>


namespace felspar::memory {


    /// Optionally holds a value, but does not allow the value to be re-assigned
    /// without being explicitly cleared first
    template<typename T>
    class alignas(T) holding_pen {
        std::array<std::byte, sizeof(T)> pen;
        bool holding;

      public:
        holding_pen() : holding{false} {}
        holding_pen(T &&t) : pen{}, holding{true} {
            new (pen.data()) T{std::move(t)};
        }
        ~holding_pen() { reset(); }

        /// Access to the pen
        T &value() { return *reinterpret_cast<T *>(pen.data()); }
        T const &value() const {
            return *reinterpret_cast<T const *>(pen.data());
        }

        explicit operator bool() const { return holding; }
        T *operator->() { return &value(); }
        T const *operator->() const { return &value(); }
        T &operator*() { return value(); }
        T const &operator*() const { return value(); }

        /// ## Manipulating the holding pen

        /// Assign a new value into the pen destroying any value already held
        void assign(T &&t) {
            reset();
            new (pen.data()) T{std::move(t)};
            holding = true;
        }
        /// Destroy any held value
        void reset() {
            if (holding) {
                std::destroy_at(reinterpret_cast<T *>(pen.data()));
                holding = false;
            }
        }

        /// Fetch the value then clear the pen
        holding_pen transfer_out() && {
            if (not holding) {
                return holding_pen<T>{};
            } else {
                T r = std::move(value());
                reset();
                return r;
            }
        }
    };


}
