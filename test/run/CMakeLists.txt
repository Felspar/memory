if(TARGET check)
    add_test_run(check felspar-memory TESTS
            sizes.cpp
            small_vector.cpp
        )
endif()
