#include <felspar/memory/bitmap.strategy.hpp>

#include <cstdint>

static_assert(felspar::memory::overallocating_allocator_strategy<
              felspar::memory::bitmap_strategy<std::uint8_t>>);
