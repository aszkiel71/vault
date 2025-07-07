cmake_minimum_required(VERSION 3.16)

project(QuickCompress)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# External libraries
# include(FetchContent) # No longer needed

# indicators library for progress bars
# FetchContent_Declare(
#     indicators
#     GIT_REPOSITORY https://github.com/p-ranav/indicators.git
#     GIT_TAG v2.3
# )
# FetchContent_MakeAvailable(indicators) # No longer needed

add_subdirectory(external/indicators)

# Find threading library
# find_package(Threads REQUIRED) # No longer needed


if(MSVC)
    set(CMAKE_CXX_FLAGS "/W4 /EHsc")
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
)

# Link threading library
target_link_libraries(quickcompress PRIVATE indicators)
