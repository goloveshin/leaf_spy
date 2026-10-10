cmake_minimum_required(VERSION 4.0)

add_library(
    leaf_spy_example_exceptions
    INTERFACE
)

target_include_directories(
    leaf_spy_example_exceptions
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/exceptions
)

target_compile_features(
    leaf_spy_example_exceptions
    INTERFACE
        cxx_std_23
)

target_link_libraries(
    leaf_spy_example_exceptions
    INTERFACE
        Boost::leaf
)

add_library(
    leaf_spy_example_libraries
    INTERFACE
)

target_include_directories(
    leaf_spy_example_libraries
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/library_A
        ${CMAKE_CURRENT_SOURCE_DIR}/library_B
        ${CMAKE_CURRENT_SOURCE_DIR}/library_C
)

target_link_libraries(
    leaf_spy_example_libraries
    INTERFACE
        leaf_spy_example_exceptions
)

add_executable(
    leaf_spy_example
    executable/main.cpp
)

target_link_libraries(
    leaf_spy_example
    PRIVATE
        leaf_spy::leaf_spy
        leaf_spy_example_libraries
)
