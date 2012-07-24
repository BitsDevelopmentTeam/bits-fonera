
cmake_minimum_required(VERSION 2.6)
project(PNGPP)

## Targets
set(READ_SRCS read.cpp)
add_executable(read ${READ_SRCS})

## Link libraries
find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
target_link_libraries(read ${PNG_LIBRARY})
