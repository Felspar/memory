#pragma once


#include <felspar/memory/holding_pen.hpp>

#include <mutex>


namespace felspar::memory {


    /// Mimics the `holding_pen`, but makes it thread safe. Acts like a `std::atomic`, but lifts most of the type requirements
    template<typename T>
    class atomic_pen final {
        std::mutex mutex;
        holding_pen<T> pen;

      public:
        shared_pen() {}
        shared_pen(T &&t) : pen{std::move(t)} {}
        shared_pen(shared_pen const &) = delete;
        shared_pen(shared_pen &&h) : pen{std::move(h)} {}

        T value() {
            std::scoped_lock _{mutex};
            return pen.value();
        }

        void assign(T t) {
            std::scoped_lock _{mutex};
            pen.assign(std::move(t));
        }

        void reset() {
            std::scoped_lock _{mutex};
            pen.reset();
        }

        holding_pen<T> transfer_out() && {
            std::scoped_lock _{mutex};
            return pen.transfer_out();
        }
    };


}
