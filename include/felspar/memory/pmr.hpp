#pragma once


#ifndef FELSPAR_FORCE_PMR

#if __has_include(<memory_resource>)

#include <memory_resource>


namespace felspar {
    namespace pmr = std::pmr;
}


#else
#define FELSPAR_FORCE_PMR
#endif

#endif


#ifdef FELSPAR_FORCE_PMR


#include <typeinfo>
#include <new>


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


    namespace detail {
        class new_delete_allocator : public memory_resource {
            void *do_allocate(std::size_t bytes, std::size_t alignment) {
                return ::operator new(
                        bytes, static_cast<std::align_val_t>(alignment));
            }
            void do_deallocate(
                    void *p, std::size_t bytes, std::size_t alignment) {
                ::operator delete(
                        p, bytes, static_cast<std::align_val_t>(alignment));
            }

            bool do_is_equal(memory_resource const &other) const noexcept {
                return typeid(other) == typeid(*this);
            }
        };
        inline new_delete_allocator g_new_delete_allocator;
    }
    inline auto *new_delete_resource() noexcept {
        return &detail::g_new_delete_allocator;
    }


}


#endif
