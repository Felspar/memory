#pragma once


#ifndef FELSPAR_FORCE_PMR

#if __has_include(<memory_resource>)

#include <memory_resource>


namespace felspar::pmr {
    using memory_resource = std::pmr::memory_resource;
}


#else
#define FELSPAR_FORCE_PMR
#endif

#endif


#ifdef FELSPAR_FORCE_PMR


#include <cstddef>


namespace felspar::pmr {


    class memory_resource {
        virtual void *do_allocate(std::size_t bytes, std::size_t alignment) = 0;
        virtual void do_deallocate(
                void *p, std::size_t bytes, std::size_t alignment) = 0;
        virtual bool
                do_is_equal(memory_resource const &other) const noexcept = 0;

      public:
        [[nodiscard]] void *allocate(
                std::size_t bytes,
                std::size_t alignment = alignof(std::max_align_t)) {
            return do_allocate(bytes, alignment);
        }
        void deallocate(
                void *p,
                std::size_t bytes,
                std::size_t alignment = alignof(std::max_align_t)) {
            return do_deallocate(p, bytes, alignment);
        }
        [[nodiscard]] bool
                is_equal(memory_resource const &other) const noexcept {
            return do_is_equal(other);
        }
    };


}


#endif
