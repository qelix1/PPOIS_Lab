# Лабораторная работа №1. Основы ООП

Два независимых C++17-проекта с меню в консоли, юнит-тестами (Catch2),
покрытием >90% и документацией Doxygen.

## Проекты

### Game_Fifteen — игра «Пятнашки»

Класс `FifteenPuzzle`: случайная решаемая расстановка 4×4, ход клеткой,
проверка собранности, сравнение, потоковый ввод-вывод.

### Post_Machine — машина Поста

Классы `PostMachine`, `Tape`, `PostCommand`: загрузка программы и ленты
из файлов, интерпретация команд (`>`, `<`, `1`, `0`, `? a; b`, `!`),
защита от бесконечного цикла (`maxSteps`).

## Сборка и запуск

Команды выполняются внутри каталога проекта (`Game_Fifteen/` или `Post_Machine/`).

```bash
# сборка
cmake -S . -B build
cmake --build build

# запуск приложения (интерактивное меню)
./build/puzzle_app     # Пятнашки
./build/post_app       # Машина Поста

# тесты
ctest --test-dir build --output-on-failure
```

## Покрытие тестами

Требуется `lcov` и `genhtml`.

```bash
cmake -S . -B build-cov -DENABLE_COVERAGE=ON
cmake --build build-cov
cmake --build build-cov --target coverage   # отчёт: build-cov/coverage_report/index.html
```

## Документация (Doxygen)

```bash
doxygen Doxyfile       # результат: doc/html/index.html
```

## Структура

```
Game_Fifteen/
  include/FifteenPuzzle.h   # интерфейс класса
  src/FifteenPuzzle.cpp     # реализация
  src/main.cpp              # консольное меню
  tests/                    # Catch2-тесты
  Doxyfile
Post_Machine/
  include/                  # PostMachine.h, Tape.h
  src/                      # PostMachine.cpp, Tape.cpp, main.cpp
  tests/                    # Catch2-тесты
  programs/                 # примеры программ и лент
  Doxyfile
```
