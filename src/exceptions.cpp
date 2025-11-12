#include <felspar/exceptions.hpp>
#include <felspar/memory/exceptions.hpp>

#include <new>


void felspar::memory::detail::throw_bad_alloc(
        char const *m, std::source_location const &loc) {
    throw stdexcept::bad_alloc{m, loc};
}


void felspar::memory::detail::throw_logic_error(
        char const *m, std::source_location const &loc) {
    throw stdexcept::logic_error{m, loc};
}


void felspar::memory::detail::throw_length_error(
        char const *m, std::source_location const &loc) {
    throw stdexcept::length_error{m, loc};
}


void felspar::memory::detail::throw_overaligned_memory(
        std::size_t const alignment, std::source_location const &loc) {
    throw stdexcept::logic_error{
            "Requested over-aligned memory from the pool of "
                    + std::to_string(alignment) + " bytes",
            loc};
}
