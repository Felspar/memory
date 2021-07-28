#pragma once


#include <felspar/memory/raw_storage.hpp>


namespace felspar::memory {


    /// Optionally holds a value, but does not allow the value to be re-assigned
    /// without being explicitly cleared first
    template<typename T>
    class holding_pen {
        raw_storage<T> store;
        bool holding;

      public:
        holding_pen() : holding{false} {}
        holding_pen(T &&t) : holding{true} { store.emplace(std::move(t)); }
        holding_pen(holding_pen const &) = delete;
        holding_pen(holding_pen &&) = delete;
        ~holding_pen() { reset(); }

        /// Access to the pen
        T &value() { return *store.data(); }
        T const &value() const { return *store.data(); }

        explicit operator bool() const { return holding; }
        T *operator->() { return &value(); }
        T const *operator->() const { return &value(); }
        T &operator*() { return value(); }
        T const &operator*() const { return value(); }

        /// ## Manipulating the holding pen
        holding_pen &operator=(holding_pen const &) = delete;
        holding_pen &operator=(holding_pen &&) = delete;

        /// Assign a new value into the pen destroying any value already held
        void assign(T t) {
            store.destroy_if(std::exchange(holding, true));
            store.emplace(std::move(t));
        }
        template<typename... Args>
        T &emplace(Args... args) {
            store.destroy_if(std::exchange(holding, true));
            return store.emplace(std::forward<Args>(args)...);
        }
        /// Destroy any held value
        void reset() { store.destroy_if(std::exchange(holding, false)); }

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
