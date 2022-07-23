if(TARGET felspar-check)
    add_test_run(felspar-check felspar-memory TESTS
            bitmap.cpp
            fixed-pool.pmr.cpp
            hexdump.cpp
            holding_pen.cpp
            pmr.cpp
            raw_memory.cpp
            sizes.cpp
            slab.storage.cpp
            small_ring.cpp
            small_vector.cpp
            stack.storage.cpp
        )
endif()
