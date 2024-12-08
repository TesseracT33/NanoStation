cmake_minimum_required(VERSION 3.13)

project(NanoStationTest LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(NanoStationTest
    test_register_allocator.cpp    
)

target_link_libraries(NanoStationTest
    ${NANOSTATION_LIB}
    gmock
    gtest
    gtest_main
)
