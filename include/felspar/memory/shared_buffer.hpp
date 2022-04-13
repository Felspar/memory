#pragma once


#include <felspar/memory/control.hpp>


namespace felspar::memory {


    /// A shared memory buffer
    template<typename T>
    class shared_buffer {
    public:
        using value_type = T;
        using control_type = control;
        using buffer_type = std::span<T>;

    private:
        buffer_type buffer;
        control_type *owner = nullptr;
    };


    /// A view onto a shared memory buffer
    template<typename T>
    class shared_buffer_view {
    public:
        using value_type = T;
    };


}
