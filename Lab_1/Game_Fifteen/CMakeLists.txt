cmake_minimum_required(VERSION 3.14)
project(fifteen_puzzle CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(puzzle_app src/main.cpp)

# --- раскомментировать, когда появится src/FifteenPuzzle.cpp ---
# add_library(puzzle_lib src/FifteenPuzzle.cpp)
# target_include_directories(puzzle_lib PUBLIC include)
# target_link_libraries(puzzle_app PRIVATE puzzle_lib)

# --- раскомментировать, когда появятся тесты и tests/test_fifteen_puzzle.cpp ---
# include(FetchContent)
# FetchContent_Declare(catch2
#     GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#     GIT_TAG v3.5.2)
# FetchContent_MakeAvailable(catch2)
# enable_testing()
# add_executable(puzzle_tests tests/test_fifteen_puzzle.cpp)
# target_link_libraries(puzzle_tests PRIVATE Catch2::Catch2WithMain puzzle_lib)
# add_test(NAME puzzle_tests COMMAND puzzle_tests)