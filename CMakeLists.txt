cmake_minimum_required(VERSION 4.0)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(leaf_spy LANGUAGES CXX)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED YES)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(LEAF_SPY_IS_TOP_LEVEL ON)
else()
    set(LEAF_SPY_IS_TOP_LEVEL OFF)
endif()

# the tests and the example are built by default only when leaf_spy is the top-level project
option(LEAF_SPY_BUILD_TESTS "Build leaf_spy tests and example" ${LEAF_SPY_IS_TOP_LEVEL})

add_library(leaf_spy INTERFACE)
add_library(leaf_spy::leaf_spy ALIAS leaf_spy)

target_include_directories(
    leaf_spy
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(
    leaf_spy
    INTERFACE
        cxx_std_20
)

if(NOT TARGET Boost::leaf)
    include(FetchContent)
    set(BOOST_SUPERPROJECT_VERSION 1.92.0)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
    FetchContent_Declare(
        boost_leaf
        GIT_REPOSITORY https://github.com/boostorg/leaf.git
        GIT_TAG boost-1.92.0
        EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(boost_leaf)
endif()

target_link_libraries(
    leaf_spy
    INTERFACE
        Boost::leaf
)

if(LEAF_SPY_BUILD_TESTS)
    if(NOT TARGET doctest::doctest)
        include(FetchContent)
        set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
        FetchContent_Declare(
            doctest
            GIT_REPOSITORY https://github.com/doctest/doctest.git
            GIT_TAG v2.5.0
            EXCLUDE_FROM_ALL
        )
        FetchContent_MakeAvailable(doctest)
    endif()

    add_executable(
        leaf_spy_tests
        tests/test_leaf_spy.cpp
    )
    target_link_libraries(
        leaf_spy_tests
        PRIVATE
            leaf_spy::leaf_spy
            doctest::doctest
    )
    target_compile_definitions(
        leaf_spy_tests
        PRIVATE
            DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
    )

    add_subdirectory(example)
endif()
