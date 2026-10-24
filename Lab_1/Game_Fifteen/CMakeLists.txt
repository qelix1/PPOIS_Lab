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

# Install targets
install(TARGETS puzzle_app DESTINATION bin)

include(FetchContent)
FetchContent_Declare(catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2)
FetchContent_MakeAvailable(catch2)
enable_testing()
add_executable(puzzle_tests tests/test_fifteen_puzzle.cpp)
target_link_libraries(puzzle_tests PRIVATE Catch2::Catch2WithMain puzzle_lib)
add_test(NAME puzzle_tests COMMAND puzzle_tests)

if(ENABLE_COVERAGE)
    find_program(LCOV_EXECUTABLE lcov)
    find_program(GENHTML_EXECUTABLE genhtml)
    if(LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
        # cmake --build <build-dir> --target coverage
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND}
                    -DLCOV=${LCOV_EXECUTABLE}
                    -DGENHTML=${GENHTML_EXECUTABLE}
                    -DTESTS=$<TARGET_FILE:puzzle_tests>
                    -DBUILD_DIR=${CMAKE_BINARY_DIR}
                    -DSRC_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                    -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/run_coverage.cmake
            DEPENDS puzzle_tests
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Генерация отчёта покрытия (lcov)"
            VERBATIM)
    else()
        message(STATUS "lcov/genhtml не найдены — цель coverage недоступна")
    endif()
endif()

