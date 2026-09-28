cmake_minimum_required(VERSION 3.14)
project(fifteen_puzzle CXX)

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0 -g)
        add_link_options(--coverage)
    else()
        message(WARNING "Coverage flags are not supported for this compiler.")
    endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(puzzle_app src/main.cpp)


add_library(puzzle_lib src/FifteenPuzzle.cpp)
target_include_directories(puzzle_lib PUBLIC include)
target_link_libraries(puzzle_app PRIVATE puzzle_lib)

# --- раскомментировать, когда появятся тесты и tests/test_fifteen_puzzle.cpp ---
include(FetchContent)
FetchContent_Declare(catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2)
FetchContent_MakeAvailable(catch2)
enable_testing()
add_executable(puzzle_tests tests/test_fifteen_puzzle.cpp)
target_link_libraries(puzzle_tests PRIVATE Catch2::Catch2WithMain puzzle_lib)
add_test(NAME puzzle_tests COMMAND puzzle_tests)

