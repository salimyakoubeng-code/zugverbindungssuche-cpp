cmake_minimum_required(VERSION 3.16)

project(RailConnect VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(rail src/rail.cpp)

target_include_directories(rail PUBLIC include)

if(MSVC)
    target_compile_options(rail PRIVATE /W4)
else()
    target_compile_options(
        rail PRIVATE -Wall -Wextra -Wpedantic
    )
endif()

add_executable(railconnect src/main.cpp)

target_link_libraries(railconnect PRIVATE rail)

include(CTest)

if(BUILD_TESTING)
    add_executable(rail_tests tests/test_rail.cpp)

    target_link_libraries(rail_tests PRIVATE rail)

    add_test(
        NAME rail_unit_tests
        COMMAND rail_tests
    )

    set_tests_properties(
        rail_unit_tests
        PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endif()
