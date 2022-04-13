#pragma once


#include <felspar/memory/shared_buffer.hpp>


namespace felspar::memory {


    /// A buffer into which data can be accumulated at the end, and consumed from the front
    template<typename T>
    class accumulation_buffer {
        shared_buffer<T> buffer;
    public:
        using value_type = T;
    };


}
