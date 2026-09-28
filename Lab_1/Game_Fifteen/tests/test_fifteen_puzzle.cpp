#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <stdexcept>
#include <array>
//#include <algorithm>
#include "FifteenPuzzle.h"

// Вспомогательная функция для проверки, что массив является перестановкой 0..15
bool isPermutation(const std::array<int, FifteenPuzzle::CELLS>& arr) {
    std::array<bool, FifteenPuzzle::CELLS> seen{};
    for (int val : arr) {
        if (val < 0 || val >= FifteenPuzzle::CELLS || seen[val]) return false;
        seen[val] = true;
    }
    return true;
}

TEST_CASE("Конструкторы FifteenPuzzle", "[constructor]") {
    SECTION("Конструктор по умолчанию создает решаемую перестановку") {
        FifteenPuzzle game;
        std::array<int, FifteenPuzzle::CELLS> cells;
        for (int i = 0; i < FifteenPuzzle::CELLS; ++i) {
            cells[i] = game[i];
        }
        REQUIRE(isPermutation(cells));
    }

    SECTION("Конструктор от массива принимает корректную перестановку") {
        std::array<int, FifteenPuzzle::CELLS> valid = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
        REQUIRE_NOTHROW(FifteenPuzzle(valid));
    }

    SECTION("Конструктор от массива бросает invalid_argument при дубликате") {
        std::array<int, FifteenPuzzle::CELLS> dup = {1, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
        REQUIRE_THROWS_AS(FifteenPuzzle(dup), std::invalid_argument);
    }

    SECTION("Конструктор от массива бросает invalid_argument при значении > 15") {
        std::array<int, FifteenPuzzle::CELLS> big = {16, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
        REQUIRE_THROWS_AS(FifteenPuzzle(big), std::invalid_argument);
    }

    SECTION("Конструктор от массива бросает invalid_argument при отрицательном значении") {
        std::array<int, FifteenPuzzle::CELLS> neg = {-1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
        REQUIRE_THROWS_AS(FifteenPuzzle(neg), std::invalid_argument);
    }
}

TEST_CASE("operator[]", "[operator]") {
    std::array<int, FifteenPuzzle::CELLS> valid = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
    FifteenPuzzle game(valid);

    SECTION("Границы 0 и 15") {
        REQUIRE(game[0] == 1);
        REQUIRE(game[15] == 0);
    }

    SECTION("Выход за границы: 16 и -1") {
        REQUIRE_THROWS_AS(game[16], std::out_of_range);
        REQUIRE_THROWS_AS(game[-1], std::out_of_range);
    }
}

TEST_CASE("Метод move", "[move]") {
    // Решенное состояние: пустая клетка в правом нижнем углу (индекс 15)
    std::array<int, FifteenPuzzle::CELLS> solved = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
    FifteenPuzzle game(solved);

    SECTION("Соседняя клетка (15) перемещается успешно") {
        // 15 находится на позиции (3, 2), пустая на (3, 3)
        REQUIRE(game.move(3, 2) == true);
        REQUIRE(game[15] == 15); // 15 переместилась на индекс 15? Wait, move swaps with EMPTY. Index 15 was EMPTY. New EMPTY is at 3,2.
        REQUIRE(game[14] == 0); // Теперь EMPTY на индексе 14
    }

    SECTION("Дальняя клетка не перемещается") {
        REQUIRE(game.move(0, 0) == false); // Клетка 1 далеко от пустой
    }

    SECTION("Пустая клетка не перемещается") {
        REQUIRE(game.move(3, 3) == false); // Пустая клетка
    }

    SECTION("Выход за границы") {
        REQUIRE(game.move(4, 0) == false);
        REQUIRE(game.move(-1, 0) == false);
    }
}

TEST_CASE("isSolved", "[isSolved]") {
    std::array<int, FifteenPuzzle::CELLS> solved = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
    
    SECTION("Собранная позиция возвращает true") {
        FifteenPuzzle game(solved);
        REQUIRE(game.isSolved() == true);
    }

    SECTION("После хода возвращает false") {
        FifteenPuzzle game(solved);
        game.move(3, 2); // Делаем ход
        REQUIRE(game.isSolved() == false);
    }
}

TEST_CASE("Копирование и оператор ==", "[copy][equality]") {
    std::array<int, FifteenPuzzle::CELLS> valid = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
    FifteenPuzzle original(valid);

    SECTION("Копия равна оригиналу") {
        FifteenPuzzle copy = original;
        REQUIRE(copy == original);
    }

    SECTION("Изменение копии не меняет оригинал") {
        FifteenPuzzle copy = original;
        copy.move(3, 2);
        REQUIRE(copy != original);
        REQUIRE(original[14] == 15); // Оригинал не изменился
    }

    SECTION("Самоприсваивание") {
        original = original;
        REQUIRE(original == original);
    }
}

TEST_CASE("Потоковый ввод-вывод (Round-trip)", "[stream]") {
    std::array<int, FifteenPuzzle::CELLS> valid = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
    FifteenPuzzle original(valid);

    std::stringstream ss;
    ss << original;

    FifteenPuzzle restored;
    REQUIRE(ss >> restored);
    REQUIRE(restored == original);
}

TEST_CASE("Ошибки ввода в поток", "[stream][errors]") {
    std::array<int, FifteenPuzzle::CELLS> valid = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
    FifteenPuzzle game(valid);
    FifteenPuzzle backup = game;

    SECTION("Не перестановка (дубликат) -> failbit, объект не изменен") {
        std::stringstream ss("1 1 3 4 5 6 7 8 9 10 11 12 13 14 15 0");
        REQUIRE_FALSE(ss >> game);
        REQUIRE(ss.fail());
        REQUIRE(game == backup); // Объект не должен был измениться
    }

    SECTION("Не хватает чисел -> failbit, объект не изменен") {
        std::stringstream ss("1 2 3 4 5");
        REQUIRE_FALSE(ss >> game);
        REQUIRE(ss.fail());
        REQUIRE(game == backup);
    }
}