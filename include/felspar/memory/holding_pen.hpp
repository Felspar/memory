#pragma once


#include <felspar/memory/exceptions.hpp>
#include <felspar/memory/raw_memory.hpp>

#include <utility>


namespace felspar::memory {


    /// ## Holding pen
    /**
     * Optionally holds a value, but does not allow the value to be re-assigned
     * without being explicitly cleared first
     */
    template<typename T>
    class holding_pen final {
        raw_memory<T> store;
        bool holding;


      public:
        using value_type = T;


        constexpr holding_pen() noexcept : holding{false} {}
        holding_pen(T &&t) : holding{true} { store.emplace(std::move(t)); }
        holding_pen(holding_pen const &) = delete;
        holding_pen(holding_pen &&h) : holding{h.holding} {
            if (holding) { store.emplace(std::move(h.store.value())); }
        }
        ~holding_pen() { reset(); }


        /// ### Access to the pen
        T &
                value(felspar::source_location const &loc =
                              felspar::source_location::current()) {
            if (holding) [[likely]] {
                return *store.data();
            } else {
                detail::throw_logic_error("The holding  pen is empty", loc);
            }
        }
        T const &
                value(felspar::source_location const &loc =
                              felspar::source_location::current()) const {
            if (holding) [[likely]] {
                return *store.data();
            } else {
                detail::throw_logic_error("The holding  pen is empty", loc);
            }
        }

        /// #### Get pen value or default
        template<typename U>
        T value_or(U &&default_value) const {
            if (holding) {
                return *store.data();
            } else {
                return static_cast<T>(std::forward<U>(default_value));
            }
        }

        std::remove_reference_t<T> *operator->() { return &value(); }
        std::remove_reference_t<T> const *operator->() const {
            return &value();
        }
        T &operator*() { return value(); }
        T const &operator*() const { return value(); }


        /// ### Queries
        bool has_value() const noexcept { return holding; }
        explicit operator bool() const noexcept { return holding; }

        friend bool operator==(holding_pen const &l, holding_pen const &r) {
            if (l.holding != r.holding) {
                return false;
            } else if (not l.holding and not r.holding) {
                return true;
            } else {
                return *(r.store).data() == *(l.store).data();
            }
        }


        /// ### Manipulating the holding pen
        holding_pen &operator=(holding_pen const &) = delete;
        holding_pen &operator=(holding_pen &&hp) {
            if (hp.holding) {
                assign(std::move(hp.store.value()));
            } else {
                reset();
            }
            return *this;
        }
        holding_pen &operator=(value_type &&v) {
            assign(std::move(v));
            return *this;
        }
        holding_pen &operator=(value_type const &v) {
            store.destroy_if(std::exchange(holding, true));
            store.copy(v);
            return *this;
        }

        /// #### Assign a new value into the pen destroying any value already held
        void assign(T t) {
            store.destroy_if(std::exchange(holding, true));
            store.emplace(std::move(t));
        }
        template<typename... Args>
        T &emplace(Args... args) {
            store.destroy_if(std::exchange(holding, true));
            return store.emplace(std::forward<Args>(args)...);
        }
        /// #### Destroy any held value
        void reset() noexcept {
            store.destroy_if(std::exchange(holding, false));
        }

        /// #### Fetch the value then clear the pen
        /// TODO Deprecate?
        holding_pen transfer_out() && {
            if (not holding) {
                return holding_pen{};
            } else {
                T r{std::move(value())};
                reset();
                return r;
            }
        }
    };


}
