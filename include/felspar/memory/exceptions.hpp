#pragma once


#include <cstddef>
#include <source_location>


namespace felspar::memory::detail {


    [[noreturn]] void
            throw_bad_alloc(char const *, std::source_location const &);
    [[noreturn]] void
            throw_length_error(char const *, std::source_location const &);
    [[noreturn]] void
            throw_logic_error(char const *, std::source_location const &);
    [[noreturn]] void throw_overaligned_memory(
            std::size_t alignment, std::source_location const &);


}
