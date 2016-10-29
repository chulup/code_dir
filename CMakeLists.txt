cmake_minimum_required(VERSION 2.8.11)
project(code-directory CXX C)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/" ${CMAKE_MODULE_PATH} )
message(STATUS "c++ compiler ... " ${CMAKE_CXX_COMPILER})

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

find_package(Boost
    1.58 
    REQUIRED
    COMPONENTS program_options system thread log
)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(src)

find_package(GTest REQUIRED)
enable_testing()
add_subdirectory(test)
