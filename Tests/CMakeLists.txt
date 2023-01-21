cmake_minimum_required(VERSION 3.24.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(Tests)

add_executable(Tests main.cpp)

target_include_directories(Tests PUBLIC "../include")