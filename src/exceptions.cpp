#include <felspar/memory/exceptions.hpp>

#include <new>


void felspar::memory::detail::throw_bad_alloc() { throw std::bad_alloc{}; }
