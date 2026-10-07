cmake_minimum_required(VERSION 3.14)
project(post_machine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# База компиляций для clangd/IDE. Без неё clangd не видит флагов
# (-I, -std=c++17) и подсвечивает красным tests/*.cpp.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_COVERAGE "Покрытие тестами (gcov)" OFF)

# clangd ищет compile_commands.json только в каталогах выше исходного файла,
# но не в build/. Поэтому оставляем ссылку в корне проекта (она в .gitignore).
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR
   AND NOT EXISTS ${CMAKE_SOURCE_DIR}/compile_commands.json)
    file(CREATE_LINK ${CMAKE_BINARY_DIR}/compile_commands.json
         ${CMAKE_SOURCE_DIR}/compile_commands.json SYMBOLIC)
endif()

add_executable(post_app src/main.cpp)
add_library(post_lib src/Tape.cpp src/PostMachine.cpp)
target_include_directories(post_lib PUBLIC include)


target_link_libraries(post_app PRIVATE post_lib)

if(ENABLE_COVERAGE)
    # PUBLIC: флаги наследуются post_app и post_tests. Если задать их
    # только PRIVATE на post_lib, линковка post_app падает с
    # undefined reference на __gcov_init/__gcov_exit.
    target_compile_options(post_lib PUBLIC --coverage -O0 -g)
    target_link_options(post_lib PUBLIC --coverage)
endif()

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
    find_program(LCOV_EXECUTABLE lcov)
    find_program(GENHTML_EXECUTABLE genhtml)
    if(LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
        # cmake --build <build-dir> --target coverage
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND}
                    -DLCOV=${LCOV_EXECUTABLE}
                    -DGENHTML=${GENHTML_EXECUTABLE}
                    -DTESTS=$<TARGET_FILE:post_tests>
                    -DBUILD_DIR=${CMAKE_BINARY_DIR}
                    -DSRC_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                    -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/run_coverage.cmake
            DEPENDS post_tests
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Генерация отчёта покрытия (lcov)"
            VERBATIM)
    else()
        message(STATUS "lcov/genhtml не найдены — цель coverage недоступна")
    endif()
endif()
