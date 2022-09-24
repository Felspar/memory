#pragma once


namespace felspar::memory::pool {


    /// Low level implementation of the pool
    template<typename SL>
    inline void *allocate(SL &stack) {
        if (not stack.empty()) {
            void *const item = stack.back();
            stack.pop_back();
            return item;
        } else {
            return nullptr;
        }
    }


    template<typename SL>
    inline void deallocate(SL &stack, void *ptr) {
        stack.push_back(ptr);
    }


}
