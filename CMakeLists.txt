cmake_minimum_required(VERSION 3.26)
set(EXE_NAME "poker_calc")
project(${EXE_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("include")

file (GLOB SRC
    "src/*.cpp"
    "include/*.h"
    "include/*.hpp"
)

add_executable(${EXE_NAME} ${SRC})