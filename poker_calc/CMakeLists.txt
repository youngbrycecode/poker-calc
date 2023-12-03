file (GLOB SRC
    "*cpp"
)

file (GLOB HEADER_FILES
    "include/*.h"    
    "include/*.hpp"
)

add_library(poker_calc_lib ${SRC} ${HEADER_FILES})
target_include_directories(poker_calc_lib PUBLIC include)
