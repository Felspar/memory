add_library(felspar-memory
        exceptions.cpp
        hexdump.cpp
    )
target_compile_features(felspar-memory INTERFACE cxx_std_20)
target_include_directories(felspar-memory PUBLIC ../include)
target_link_libraries(felspar-memory PUBLIC felspar-exceptions)
