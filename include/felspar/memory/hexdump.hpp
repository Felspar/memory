#pragma once

#include <ostream>
#include <span>


namespace felspar::memory {


    /// Print a hex dump of the memory to the provided stream of only up to the
    /// first `bytes` bytes
    std::ostream &hexdump(
            std::ostream &, std::span<std::byte>, std::size_t bytes = 256);


}
