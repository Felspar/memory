add_library(memory-headers-tests STATIC EXCLUDE_FROM_ALL
        accumulation_buffer.cpp
        any_buffer.cpp
        atomic_pen.cpp
        bitmap.strategy.cpp
        concepts.cpp
        control.cpp
        fixed-pool.pmr.cpp
        holding_pen.cpp
        pmr.cpp
        raw_memory.cpp
        shared_buffer.cpp
        shared_view.cpp
        shared_vector.cpp
        sizes.cpp
        small_ring.cpp
        small_vector.cpp
        spaceship.cpp
        stack.storage.cpp
    )
target_link_libraries(memory-headers-tests PRIVATE felspar-memory)
add_dependencies(felspar-check memory-headers-tests)
