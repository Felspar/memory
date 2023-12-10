add_library(felspar-memory
        hexdump.cpp
    )
target_include_directories(felspar-memory PUBLIC ../include)
target_link_libraries(felspar-memory PUBLIC felspar-exceptions)
