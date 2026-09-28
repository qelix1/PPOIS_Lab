cmake_minimum_required(VERSION 3.14)
project(post_machine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(ENABLE_COVERAGE "Покрытие тестами (gcov)" OFF)
add_executable(post_app src/main.cpp)
add_library(post_lib src/Tape.cpp src/PostMachine.cpp)
target_include_directories(post_lib PUBLIC include)


target_link_libraries(post_app PRIVATE post_lib)

include(FetchContent)
FetchContent_Declare(catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2)
FetchContent_MakeAvailable(catch2)

enable_testing()
add_executable(post_tests
    tests/test_tape.cpp
    tests/test_post_machine.cpp)
target_link_libraries(post_tests PRIVATE Catch2::Catch2WithMain post_lib)
add_test(NAME post_tests COMMAND post_tests)

if(ENABLE_COVERAGE)
    target_compile_options(post_lib PRIVATE --coverage)
    target_link_options(post_lib PRIVATE --coverage)
    target_compile_options(post_tests PRIVATE --coverage)
    target_link_options(post_tests PRIVATE --coverage)
endif()
