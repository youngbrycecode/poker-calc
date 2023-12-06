include(FetchContent)

FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(decktest DeckTests.cpp ${HEADER_FILES})
target_link_libraries(decktest gtest_main poker_calc_lib)
add_test(decktest_gtest decktest dt decktest)

add_executable(classifytests ClassifyTests.cpp ${HEADER_FILES})
target_link_libraries(classifytests gtest_main poker_calc_lib)
add_test(classifytests_gtest d_gtestecktest ct classifytests)