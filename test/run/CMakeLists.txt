if(TARGET check)
    add_test_run(check felspar-memory TESTS
            holding_pen.cpp
            raw_memory.cpp
            sizes.cpp
            slab.storage.cpp
            small_vector.cpp
            stack.storage.cpp
        )
endif()
