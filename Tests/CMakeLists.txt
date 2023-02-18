cmake_minimum_required(VERSION 3.24)

#lib requires c++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
add_compile_options(-g -O0 -Wall -fprofile-arcs -ftest-coverage --coverage)

set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)

project(Tests)

add_executable(Tests)

target_sources(Tests 
    PUBLIC src/main.cpp
    PUBLIC src/AdjacencyListTest.cpp
    PUBLIC src/MapInterfaceTest.cpp
    PUBLIC src/SetInterfaceTest.cpp
    )

target_include_directories(Tests PUBLIC 
    ../include 
    ${GTEST_INCLUDE_DIRS}
    )

#gtest stuff
find_package(GTest REQUIRED)
target_link_libraries(Tests ${GTEST_LIBRARIES} pthread gcov)

#define object directory - is there a better way?
set(OBJECT_DIR ${CMAKE_SOURCE_DIR}/CMakeFiles/Tests.dir/src)

#run the tests
add_custom_command(
    TARGET Tests POST_BUILD
    COMMAND ./Tests
    COMMAND mkdir -p gcoverage
    COMMAND mkdir -p lcoverage
    )

#gcov execution
add_custom_command(
    TARGET Tests POST_BUILD
    COMMAND echo "========= GCOV ========="
    COMMAND gcov -b ${CMAKE_SOURCE_DIR}/src/*.cpp -o ${OBJECT_DIR} >> gcovresults.txt
    COMMAND echo "gcov results output to gcoverage/gcovresults.txt"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/gcoverage
    )

add_custom_command(
    TARGET Tests POST_BUILD
    COMMAND echo "========= LCOV =========="
    COMMAND lcov -c -d ${OBJECT_DIR} -o main_coverage.info
    COMMAND genhtml main_coverage.info -o .
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/lcoverage
    )

#clean-all target and cleaning additional files
add_custom_target(clean-all
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    COMMAND rm -f ${OBJECT_DIR}/*.gcno
    COMMAND rm -f ${OBJECT_DIR}/*.gcda
    COMMAND rm -r gcoverage
    COMMAND rm -r lcoverage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN FILES gcoverage)