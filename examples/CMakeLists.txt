macro(cg_executable name src)
    add_executable(${name} ${src})
    target_link_libraries(${name} PUBLIC compute_graph)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${name} PRIVATE
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
        target_compile_options(${name} PRIVATE /W4)
    endif()
endmacro()

cg_executable(compute_graph_exec main.cpp)
cg_executable(calc calc.cpp)
cg_executable(autodiff autodiff.cpp)