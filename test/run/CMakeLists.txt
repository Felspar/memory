if(TARGET check)
    add_test_run(check felspar-memory TESTS
            small_vector.cpp
        )
endif()
