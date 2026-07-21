cmake_minimum_required(VERSION 3.10)
project(ParallelMergeSort CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Threads REQUIRED)

add_executable(parallel_sort
    main.cpp
    mergeSort.cpp
    parallelMergeSort.cpp
)

target_compile_options(parallel_sort PRIVATE -O2 -Wall -Wextra)
target_link_libraries(parallel_sort PRIVATE Threads::Threads)
