cmake_minimum_required(VERSION 3.16)
project(termgl)

set(CMAKE_CXX_STANDARD 14)

find_package (Threads)
add_executable(termgl main.cpp)
target_link_libraries (termgl ${CMAKE_THREAD_LIBS_INIT})
