if(TARGET felspar-check)
    add_test_run(felspar-check felspar-memory TESTS
            bitmap.cpp
            bitmap_strategy.cpp
            buffers.cpp
            fallback.strategy.cpp
            fixed-pool.pmr.cpp
            hexdump.cpp
            holding_pen.cpp
            pmr.cpp
            raw_memory.cpp
            shared_buffer.cpp
            sizes.cpp
            size_split.strategy.cpp
            slab.storage.cpp
            small_ring.cpp
            small_vector.cpp
            stable_vector.cpp
            stack.storage.cpp
        )
endif()
