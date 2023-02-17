#include <felspar/memory/hexdump.hpp>

#include <iomanip>


std::ostream &felspar::memory::hexdump(
        std::ostream &s, std::span<std::byte const> b, std::size_t const l) {
    s << "Showing " << std::min(l, b.size()) << " bytes out of " << b.size()
      << '\n'
      << std::hex << std::setw(2);
    if (b.size() > l) { b = b.first(l); }
    while (b.size()) {
        auto const length = std::min(b.size(), std::size_t{16u});
        auto const line = b.first(length);
        s << "| ";
        for (std::size_t index{}; auto const ch : line) {
            s << std::setw(2) << std::setfill('0') << static_cast<int>(ch)
              << ' ';
            if (++index == 8) s << "| ";
        }
        if (length < 8) {
            s << std::string(3 * (8 - length), ' ') << '|'
              << std::string(24 + 1, ' ') << '|' << std::string(3, ' ') << '|';
        } else {
            s << std::string(3 * (16 - length), ' ') << '|'
              << std::string(3, ' ') << '|';
        }
        for (std::size_t index{}; auto const ch : line) {
            if (ch < std::byte{' '} || ch >= std::byte{0x7f}) {
                s << "\xC2\xB0";
            } else {
                s << static_cast<char>(ch);
            }
            if (++index == 8) s << '|';
        }
        s << std::string(16 - length + (length < 8 ? 1 : 0), ' ') << "|\n";
        b = b.subspan(length);
    }
    return s << std::dec << std::flush;
}
