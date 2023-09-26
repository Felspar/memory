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
        explicit stable_vector(std::size_t const m, value_type const &v = {}) {
            grow_to(m, v);
        }


        /// ### Queries
        bool empty() const noexcept { return m_size == 0u; }
        auto size() const noexcept { return m_size; }
        value_type const &operator[](std::size_t const idx) const {
            return (*m_storage.at(v_index(idx)))[sv_index(idx)];
        }


        /// ### Mutation
        void clear() {
            for (auto &s : m_storage) { s->clear(); }
            m_size = 0;
        }
        value_type &push_back(value_type t) {
            auto const section = grow_storage_if_needed(m_size++);
            return m_storage[section]->push_back(std::move(t));
        }


      private:
        std::size_t v_index(std::size_t const s) const noexcept {
            return s / section_size;
        }
        std::size_t sv_index(std::size_t const s) const noexcept {
            return s % section_size;
        }
        void grow_to(std::size_t const target, value_type const &v) {
            for (; m_size < target; ++m_size) {
                auto const section = grow_storage_if_needed(m_size);
                m_storage[section]->push_back(v);
            }
        }
        std::size_t grow_storage_if_needed(std::size_t const target) {
            auto const section = v_index(target);
            if (m_storage.size() <= section) {
                m_storage.push_back(std::make_unique<sv_type>());
            }
            return section;
        }
    };


}
