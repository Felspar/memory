add_library(memory-headers-tests STATIC EXCLUDE_FROM_ALL
        holding_pen.cpp
    )
target_link_libraries(memory-headers-tests felspar-memory)
add_dependencies(check memory-headers-tests)
