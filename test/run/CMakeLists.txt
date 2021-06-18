if(TARGET check)
    add_test_run(check felspar-memory TESTS
            detail.cpp
            small_vector.cpp
        )
endif()
