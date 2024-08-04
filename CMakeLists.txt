cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(compute_graph
        LANGUAGES CXX
        VERSION 0.0.1)

# low version cmake does not support PROJECT_IS_TOP_LEVEL
if (${CMAKE_VERSION} VERSION_LESS "3.12.0")
    if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        set(PROJECT_IS_TOP_LEVEL TRUE)
    else()
        set(PROJECT_IS_TOP_LEVEL FALSE)
    endif ()
endif()


# ================ options ================
option(CG_BUILD_EXAMPLES "Build examples" ${PROJECT_IS_TOP_LEVEL})


# ================ target ================
add_library(compute_graph INTERFACE)
target_include_directories(compute_graph INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/compute_graph/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if (MSVC)
    target_compile_options(compute_graph INTERFACE /Zc:preprocessor)
endif()

# ================ examples ================
if (CG_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# ================ install ================
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(version_config_file ${CMAKE_CURRENT_BINARY_DIR}/compute_graphConfigVersion.cmake)
set(project_config_file ${CMAKE_CURRENT_BINARY_DIR}/compute_graphConfig.cmake)

write_basic_package_version_file(
    ${version_config_file}
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(DIRECTORY compute_graph/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS compute_graph
        EXPORT  compute_graphTargets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT compute_graphTargets
        FILE compute_graphTargets.cmake
        NAMESPACE compute_graph::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/compute_graph)
install(FILES ${project_config_file} ${version_config_file}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/compute_graph)

configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/compute_graphConfig.cmake.in
               ${project_config_file}
               @ONLY)