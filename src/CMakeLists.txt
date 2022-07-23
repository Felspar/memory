add_library(felspar-memory
        hexdump.cpp
    )
target_include_directories(felspar-memory PUBLIC ../include)
target_compile_features(felspar-memory PUBLIC cxx_std_20)
target_link_libraries(felspar-memory PUBLIC felspar-exceptions)
