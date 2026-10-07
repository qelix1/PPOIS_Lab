# Генерация отчёта покрытия для Post_Machine.
# Запускается из цели `coverage` (см. CMakeLists.txt):
#   cmake --build <build-dir> --target coverage
#
# В отчёт входят только исходники библиотеки (src/PostMachine.cpp, src/Tape.cpp).
# Консольный интерфейс src/main.cpp unit-тестами не покрывается и намеренно
# исключён — как и в отчёте по Game_Fifteen.

foreach(var LCOV GENHTML TESTS BUILD_DIR SRC_DIR)
    if(NOT DEFINED ${var})
        message(FATAL_ERROR "run_coverage.cmake: не задана переменная ${var}")
    endif()
endforeach()

# 1. Сбрасываем старые данные gcov, иначе gcov сообщит о несовпадении контрольной суммы
file(GLOB_RECURSE GCDA_FILES "${BUILD_DIR}/*.gcda")
if(GCDA_FILES)
    file(REMOVE ${GCDA_FILES})
endif()

# 2. Прогоняем unit-тесты
execute_process(
    COMMAND ${TESTS}
    WORKING_DIRECTORY ${BUILD_DIR}
    RESULT_VARIABLE TESTS_RESULT)
if(NOT TESTS_RESULT EQUAL 0)
    message(FATAL_ERROR "Тесты провалились (код ${TESTS_RESULT})")
endif()

# 3. Собираем сырые данные покрытия
execute_process(
    COMMAND ${LCOV} --capture
            --directory ${BUILD_DIR}
            --output-file ${BUILD_DIR}/coverage.info
            --ignore-errors mismatch,gcov,source,unused,empty,graph,inconsistent
    RESULT_VARIABLE LCOV_RESULT)
if(NOT LCOV_RESULT EQUAL 0)
    message(FATAL_ERROR "lcov --capture завершился с ошибкой")
endif()

# 4. Оставляем только исходники библиотеки
execute_process(
    COMMAND ${LCOV}
            --extract ${BUILD_DIR}/coverage.info
            ${SRC_DIR}/src/PostMachine.cpp
            ${SRC_DIR}/src/Tape.cpp
            --output-file ${BUILD_DIR}/coverage_library.info
            --ignore-errors unused,empty
    RESULT_VARIABLE LCOV_RESULT)
if(NOT LCOV_RESULT EQUAL 0)
    message(FATAL_ERROR "lcov --extract завершился с ошибкой")
endif()

# 5. Итог в консоль + HTML-отчёт
execute_process(COMMAND ${LCOV} --list ${BUILD_DIR}/coverage_library.info)
execute_process(
    COMMAND ${GENHTML} ${BUILD_DIR}/coverage_library.info
            --output-directory ${BUILD_DIR}/coverage_report
    RESULT_VARIABLE GENHTML_RESULT)
if(NOT GENHTML_RESULT EQUAL 0)
    message(FATAL_ERROR "genhtml завершился с ошибкой")
endif()

message(STATUS "HTML-отчёт: ${BUILD_DIR}/coverage_report/index.html")
