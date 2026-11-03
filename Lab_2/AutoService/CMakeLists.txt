cmake_minimum_required(VERSION 3.14)
project(auto_service CXX)

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

file(GLOB_RECURSE AUTOSERVICE_SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(REMOVE_ITEM AUTOSERVICE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_library(autoservice_lib ${AUTOSERVICE_SOURCES})
target_include_directories(autoservice_lib PUBLIC include)

add_executable(autoservice_app src/main.cpp)
target_link_libraries(autoservice_app PRIVATE autoservice_lib)

if(ENABLE_COVERAGE)
    # PUBLIC: флаги наследуются autoservice_app и autoservice_tests. Если задать их
    # только PRIVATE на autoservice_lib, линковка падает с
    # undefined reference на __gcov_init/__gcov_exit.
    target_compile_options(autoservice_lib PUBLIC --coverage -O0 -g)
    target_link_options(autoservice_lib PUBLIC --coverage)
endif()

include(FetchContent)
FetchContent_Declare(catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2)
FetchContent_MakeAvailable(catch2)

enable_testing()
add_executable(autoservice_tests
    tests/test_core.cpp
    tests/test_staff.cpp
    tests/test_orders.cpp
    tests/test_inventory.cpp
    tests/test_finance.cpp
    tests/test_workshop.cpp)
target_link_libraries(autoservice_tests PRIVATE Catch2::Catch2WithMain autoservice_lib)
add_test(NAME autoservice_tests COMMAND autoservice_tests)

if(ENABLE_COVERAGE)
    find_program(LCOV_EXECUTABLE lcov)
    find_program(GENHTML_EXECUTABLE genhtml)
    if(LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
        # cmake --build <build-dir> --target coverage
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND}
                    -DLCOV=${LCOV_EXECUTABLE}
                    -DGENHTML=${GENHTML_EXECUTABLE}
                    -DTESTS=$<TARGET_FILE:autoservice_tests>
                    -DBUILD_DIR=${CMAKE_BINARY_DIR}
                    -DSRC_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                    -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/run_coverage.cmake
            DEPENDS autoservice_tests
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Генерация отчёта покрытия (lcov)"
            VERBATIM)
    else()
        message(STATUS "lcov/genhtml не найдены — цель coverage недоступна")
    endif()
endif()
