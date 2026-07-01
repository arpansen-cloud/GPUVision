cmake_minimum_required(VERSION 3.16)

project(GPUVision VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(GPUVISION_BUILD_TESTS "Build GPUVision tests" ON)

add_library(gpuvision_core
    src/benchmark.cpp
    src/engine.cpp
    src/json_export.cpp
    src/metrics.cpp
    src/scheduler.cpp
    src/sm.cpp
    src/workload.cpp
    src/warp.cpp
)

target_include_directories(gpuvision_core PUBLIC include)

if(MSVC)
    target_compile_options(gpuvision_core PRIVATE /W4 /WX)
else()
    target_compile_options(gpuvision_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(gpuvision src/main.cpp)
target_link_libraries(gpuvision PRIVATE gpuvision_core)

if(MSVC)
    target_compile_options(gpuvision PRIVATE /W4 /WX)
else()
    target_compile_options(gpuvision PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

if(GPUVISION_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(gpuvision_tests
        tests/benchmark_tests.cpp
        tests/engine_tests.cpp
        tests/json_export_tests.cpp
        tests/scheduler_tests.cpp
        tests/workload_tests.cpp
    )
    target_link_libraries(gpuvision_tests PRIVATE gpuvision_core GTest::gtest_main)

    if(MSVC)
        target_compile_options(gpuvision_tests PRIVATE /W4 /WX)
    else()
        target_compile_options(gpuvision_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()

    include(GoogleTest)
    gtest_discover_tests(gpuvision_tests)
endif()
