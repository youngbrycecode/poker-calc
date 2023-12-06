cmake_minimum_required(VERSION 3.25)

set(EXE_NAME "poker_sim")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_subdirectory(poker_calc)
add_subdirectory(tests)

include_directories(poker_calc/include)
add_executable(${EXE_NAME} main.cpp ${HEADER_FILES})
target_link_libraries(${EXE_NAME} poker_calc_lib)