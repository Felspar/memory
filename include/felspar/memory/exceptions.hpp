#pragma once


#include <felspar/test/source.hpp>

#include <cstddef>


namespace felspar::memory::detail {


    [[noreturn]] void throw_bad_alloc(char const *, source_location const &);
    [[noreturn]] void throw_length_error(char const *, source_location const &);
    [[noreturn]] void throw_logic_error(char const *, source_location const &);
    [[noreturn]] void throw_overaligned_memory(
            std::size_t alignment, source_location const &);


}
