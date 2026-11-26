cmake_minimum_required(VERSION 3.16)
project(EEditor-open VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(EE_BUILD_SHARED "Build LibCommon as a shared library" ON)
option(EE_BUILD_TESTS "Build unit tests" ON)

add_subdirectory(LibCommon)

if(EE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

