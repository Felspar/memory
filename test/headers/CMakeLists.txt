add_library(memory-headers-tests STATIC EXCLUDE_FROM_ALL
        accumulation_buffer.cpp
        any_buffer.cpp
        atomic_pen.cpp
        bitmap.strategy.cpp
        concepts.cpp
        concepts-polyfill.cpp
        control.cpp
        fallback.strategy.cpp
        fixed-pool.pmr.cpp
        holding_pen.cpp
        new_delete.strategy.cpp
        pmr.cpp
        pmr.allocator.cpp
        raw_memory.cpp
        shared_buffer.cpp
        shared_view.cpp
        shared_vector.cpp
        sizes.cpp
        small_ring.cpp
        small_vector.cpp
        size_split.strategy.cpp
        spaceship.cpp
        stack.storage.cpp
        std.allocator.cpp
    )
target_link_libraries(memory-headers-tests PRIVATE felspar-memory)
add_dependencies(felspar-check memory-headers-tests)
