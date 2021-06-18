cmake_minimum_required(VERSION 3.10)
project(felspar-memory)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_custom_target(check)
    include(requirements.cmake)
endif()

add_library(felspar-memory INTERFACE)
target_include_directories(felspar-memory INTERFACE include)
target_compile_features(felspar-memory INTERFACE cxx_std_20)

if(TARGET check)
    set_property(TARGET check PROPERTY EXCLUDE_FROM_ALL TRUE)
    add_subdirectory(test)
endif()
