cmake_minimum_required(VERSION 3.14...3.28)
project(felspar-memory)


set(FELSPAR_FORCE_PMR NO CACHE BOOL
    "Force use of polyfilled version of std::pmr")


if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_custom_target(felspar-check)
    set_property(TARGET felspar-check PROPERTY EXCLUDE_FROM_ALL TRUE)
    include(requirements.cmake)
endif()

add_subdirectory(src)

if(TARGET felspar-check)
    add_subdirectory(test)
endif()
