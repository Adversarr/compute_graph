cmake_minimum_required(VERSION 3.29)
project(compute_graph)

set(CMAKE_CXX_STANDARD 17)

add_library(compute_graph STATIC
        ${CMAKE_CURRENT_LIST_DIR}/compute_graph/source/node.cpp)

target_include_directories(compute_graph PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/compute_graph/include)

add_executable(compute_graph_exec main.cpp)
