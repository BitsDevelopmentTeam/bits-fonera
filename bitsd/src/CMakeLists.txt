
cmake_minimum_required(VERSION 2.6)
project(Bitsd)

set(CMAKE_BUILD_TYPE Release)

# This adds -DDEBUG_MODE to redirect serial port to console
add_definitions(-DDEBUG_MODE)

## Target
set(BITSD_SRCS main.cpp bitsd.cpp core.cpp configfile.cpp base64.cpp)
add_executable(bitsd ${BITSD_SRCS})

## Link libraries
set(BOOST_LIBS system)
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
target_link_libraries(bitsd ${Boost_LIBRARIES})
find_package(Threads REQUIRED)
target_link_libraries(bitsd ${CMAKE_THREAD_LIBS_INIT})
