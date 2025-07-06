cmake_minimum_required(VERSION 3.16)

project(QuickCompress)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find threading library
find_package(Threads REQUIRED)


if(MSVC)
    set(CMAKE_CXX_FLAGS "/W4")
else()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
endif()


include_directories(include)


add_executable(quickcompress
    src/main.cpp
    src/core/bit_stream.cpp
    src/core/frequency_analyzer.cpp
    src/core/huffman_tree.cpp
    src/core/encoder.cpp
    src/threading/thread_pool.cpp
)

# Link threading library
target_link_libraries(quickcompress Threads::Threads)
