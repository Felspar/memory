if(TARGET check)
    add_test_run(check felspar-memory TESTS
            raw_storage.cpp
            sizes.cpp
            small_vector.cpp
            stack.storage.cpp
        )
endif()
