cmake_minimum_required(VERSION 3.29)
project(compute_graph)

set(CMAKE_CXX_STANDARD 17)

add_library(compute_graph INTERFACE)

target_include_directories(compute_graph INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/compute_graph/include)

add_executable(compute_graph_exec main.cpp)
target_link_libraries(compute_graph_exec PUBLIC compute_graph)