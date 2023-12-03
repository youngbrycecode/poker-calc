cmake_minimum_required(VERSION 3.26)
set(exe_name "poker_calc")
project(${exe_name})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file (GLOB SRC
    "src/*.cpp"
    "include/*.h"
    "include/*.hpp"
)

add_executable(${exe_name} ${SRC})