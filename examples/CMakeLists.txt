add_executable(compute_graph_exec main.cpp)
target_link_libraries(compute_graph_exec PUBLIC compute_graph)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(compute_graph_exec PRIVATE
            -Wall -Wextra -Weffc++
            -Werror=uninitialized
            -Werror=return-type
            -Wconversion -Wsign-compare
            -Werror=unused-result
            -Werror=suggest-override
            -Wzero-as-null-pointer-constant
            -Wmissing-declarations
            -Wold-style-cast -Werror=vla
            -Wnon-virtual-dtor)
endif()

if (MSVC)
    target_compile_options(compute_graph_exec PRIVATE /W4)
endif()