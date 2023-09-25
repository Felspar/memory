#pragma once


#include <felspar/memory/small_vector.hpp>

#include <memory>
#include <vector>


namespace felspar::memory {


    /// ## Stable vector
    /**
     * This vector type mimics a `std::vector` except that the cells themselves
     * have stable memory addresses even when the vector is appended to. To
     * achieve this the vector isn't contiguous though.
     */
    template<typename T, std::size_t S>
    class stable_vector {
        using sv_type = small_vector<T, S>;
        using sv_ptr_type = std::unique_ptr<sv_type>;
        using v_type = std::vector<sv_ptr_type>;
        v_type m_storage = {};
        std::size_t m_size = {};


      public:
        using value_type = T;
        static constexpr std::size_t section_size = S;


        /// ### Construction
        stable_vector() {}
        explicit stable_vector(std::size_t const m) { grow_to(m); }


        /// ### Queries
        bool empty() const noexcept { return m_size == 0u; }
        auto size() const noexcept { return m_size; }


      private:
        std::size_t v_index(std::size_t const s) const noexcept {
            return s / section_size;
        }
        std::size_t sv_index(std::size_t const s) const noexcept {
            return s % section_size;
        }
        void grow_to(std::size_t const target) {
            for (; m_size < target; ++m_size) {
                if (m_storage.size() <= v_index(m_size)) {
                    m_storage.push_back(std::make_unique<sv_type>());
                }
                m_storage.back()->push_back({});
            }
        }
    };


}
