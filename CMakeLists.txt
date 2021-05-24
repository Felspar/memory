cmake_minimum_required(VERSION 3.10)
project(felspar-memory)

add_library(felspar-memory INTERFACE)
target_include_directories(felspar-memory INTERFACE include)
target_compile_features(felspar-memory INTERFACE cxx_std_20)
