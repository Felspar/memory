#pragma once


namespace felspar::memory::pool {


    /// Low level implementation of the pool
    template<typename SL>
    [[nodiscard]] inline auto allocate(SL &p) noexcept {
        if (not p.empty()) {
            auto const item = p.back();
            p.pop_back();
            return item;
        } else {
            return typename SL::value_type{};
        }
    }


    /// We put the memory back into the pool only if the underlying `pool` will
    /// be able to take it. Otherwise the user of this strategy will need to
    template<typename SL, typename P>
    [[nodiscard]] inline bool deallocate(SL &p, P ptr) {
        if (p.size() < p.max_size()) {
            p.push_back(ptr);
            return true;
        } else {
            return false;
        }
    }


}
