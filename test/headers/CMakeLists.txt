add_library(memory-headers-tests STATIC EXCLUDE_FROM_ALL
        any_buffer.cpp
        atomic_pen.cpp
        bitmap.strategy.cpp
        concepts.cpp
        control.cpp
        holding_pen.cpp
        raw_memory.cpp
        sizes.cpp
        small_vector.cpp
        spaceship.cpp
        stack.storage.cpp
    )
target_link_libraries(memory-headers-tests felspar-memory)
add_dependencies(check memory-headers-tests)
