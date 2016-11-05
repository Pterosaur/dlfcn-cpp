cmake_minimum_required(VERSION 3.6)
project(dlfcn_cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -export-dynamic")

link_libraries(dl)
include_directories(include)

set(SOURCE_FILES main.cpp include/handle.h include/library.h examplelib.cpp)
add_executable(dlfcn_cpp ${SOURCE_FILES})

add_library(examplelib SHARED examplelib.cpp)