#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <stdexcept>
#include <array>
//#include <algorithm>
#include "FifteenPuzzle.h"

using Board = std::array<int, FifteenPuzzle::CELLS>;

// Вспомогательная функция для проверки, что массив является перестановкой 0..15
bool isPermutation(const Board& arr) {
    std::array<bool, FifteenPuzzle::CELLS> seen{};
    for (int val : arr) {
        if (val < 0 || val >= FifteenPuzzle::CELLS || seen[val]) return false;
        seen[val] = true;
    }
    return true;
}

Board boardOf(const FifteenPuzzle& game) {
    Board cells{};
    for (int i = 0; i < FifteenPuzzle::CELLS; ++i) cells[i] = game[i];
    return cells;
}

// Критерий разрешимости 15-пазла: чётность числа инверсий и чётность
// номера строки пустой клетки (снизу, начиная с 1) должны различаться.
bool isSolvableBoard(const Board& cells) {
    int inversions = 0;
    for (int i = 0; i < FifteenPuzzle::CELLS; ++i)
        for (int j = i + 1; j < FifteenPuzzle::CELLS; ++j)
            if (cells[i] != 0 && cells[j] != 0 && cells[i] > cells[j]) ++inversions;

    int emptyIndex = 0;
    for (int i = 0; i < FifteenPuzzle::CELLS; ++i)
        if (cells[i] == 0) { emptyIndex = i; break; }

    const int rowFromBottom = FifteenPuzzle::SIZE - emptyIndex / FifteenPuzzle::SIZE;
    return (inversions % 2) != (rowFromBottom % 2);
}

TEST_CASE("Конструкторы FifteenPuzzle", "[constructor]") {
    SECTION("Конструктор по умолчанию создает решаемую перестановку") {
        FifteenPuzzle game;
        const Board cells = boardOf(game);
        REQUIRE(isPermutation(cells));
        REQUIRE(isSolvableBoard(cells));
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

TEST_CASE("Случайная расстановка", "[constructor][shuffle]") {
    const Board first = boardOf(FifteenPuzzle());
    bool seenDifferent = false;
    for (int i = 0; i < 50; ++i) {
        FifteenPuzzle game;
        const Board cells = boardOf(game);
        REQUIRE(isPermutation(cells));
        REQUIRE(isSolvableBoard(cells));
        seenDifferent = seenDifferent || cells != first;
    }
    REQUIRE(seenDifferent);

    // Перемешивание должно корректно запомнить позицию пустой клетки.
    FifteenPuzzle game;
    const Board cells = boardOf(game);
    int emptyIndex = 0;
    for (int i = 0; i < FifteenPuzzle::CELLS; ++i)
        if (cells[i] == 0) { emptyIndex = i; break; }
    const int row = emptyIndex / FifteenPuzzle::SIZE;
    const int col = emptyIndex % FifteenPuzzle::SIZE;
    const int neighborRow = (row == 0) ? 1 : row - 1;
    REQUIRE(game.move(neighborRow, col));
    REQUIRE(game[emptyIndex] != 0);
    REQUIRE(game[neighborRow * FifteenPuzzle::SIZE + col] == 0);
}

TEST_CASE("operator[]", "[operator]") {
    std::array<int, FifteenPuzzle::CELLS> valid = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
    FifteenPuzzle game(valid);

    SECTION("Границы 0 и 15") {
        REQUIRE(game[0] == 1);
        REQUIRE(game[15] == 0);
    }

    SECTION("Все клетки возвращаются в порядке расстановки") {
        for (int i = 0; i < FifteenPuzzle::CELLS; ++i)
            REQUIRE(game[i] == valid[i]);
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

TEST_CASE("Метод move: все направления", "[move]") {
    // Пустая клетка в центре (1,1), вокруг неё — все четыре соседа.
    const Board centered = {1, 2, 3, 4, 5, 0, 7, 8, 9, 10, 11, 12, 13, 14, 15, 6};

    SECTION("Вверх") {
        FifteenPuzzle game(centered);
        REQUIRE(game.move(0, 1));
        REQUIRE(game[5] == 2);
        REQUIRE(game[1] == 0);
    }

    SECTION("Вниз") {
        FifteenPuzzle game(centered);
        REQUIRE(game.move(2, 1));
        REQUIRE(game[5] == 10);
        REQUIRE(game[9] == 0);
    }

    SECTION("Влево") {
        FifteenPuzzle game(centered);
        REQUIRE(game.move(1, 0));
        REQUIRE(game[5] == 5);
        REQUIRE(game[4] == 0);
    }

    SECTION("Вправо") {
        FifteenPuzzle game(centered);
        REQUIRE(game.move(1, 2));
        REQUIRE(game[5] == 7);
        REQUIRE(game[6] == 0);
    }

    SECTION("По диагонали и через клетку — нельзя") {
        FifteenPuzzle game(centered);
        REQUIRE_FALSE(game.move(2, 2));
        REQUIRE_FALSE(game.move(1, 3));
        REQUIRE(game[5] == 0);
        REQUIRE(game[10] == 11);
        REQUIRE(game[7] == 8);
    }

    SECTION("Границы поля") {
        FifteenPuzzle game(centered);
        REQUIRE_FALSE(game.move(-1, 1));
        REQUIRE_FALSE(game.move(1, -1));
        REQUIRE_FALSE(game.move(4, 1));
        REQUIRE_FALSE(game.move(1, 4));
        REQUIRE(game[5] == 0);
    }

    SECTION("Угол: ровно два соседа") {
        const Board topLeft = {0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 1};
        FifteenPuzzle game(topLeft);
        REQUIRE_FALSE(game.move(1, 1)); // диагональ
        REQUIRE(game.move(0, 1));       // вправо
        REQUIRE(game[0] == 2);
        REQUIRE(game[1] == 0);
        REQUIRE(game.move(1, 1));       // вниз от новой позиции пустой
        REQUIRE(game[1] == 6);
        REQUIRE(game[5] == 0);
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

    SECTION("Перепутана одна пара — false") {
        const Board swapped = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 14, 0};
        REQUIRE_FALSE(FifteenPuzzle(swapped).isSolved());
    }

    SECTION("Пустая клетка не в конце — false") {
        const Board emptyFirst = {0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 1};
        REQUIRE_FALSE(FifteenPuzzle(emptyFirst).isSolved());
    }

    SECTION("Ход и обратный ход — снова собрано") {
        FifteenPuzzle game(solved);
        REQUIRE(game.move(3, 2));
        REQUIRE_FALSE(game.isSolved());
        REQUIRE(game.move(3, 3));
        REQUIRE(game.isSolved());
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

TEST_CASE("Присваивание копированием другого состояния", "[copy][assignment]") {
    const Board solved = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
    FifteenPuzzle source(solved);
    REQUIRE(source.move(3, 2)); // пустая клетка теперь в индексе 14

    FifteenPuzzle target(solved); // пустая клетка в индексе 15
    target = source;

    REQUIRE(target == source);
    REQUIRE_FALSE(target.isSolved());

    // Если emptyPos_ не скопирован, клетка (3,3) не считается соседней с пустой.
    REQUIRE(target.move(3, 3));
    REQUIRE(target[14] == 15);
    REQUIRE(target[15] == 0);
    REQUIRE(target.isSolved());

    // Источник присваиванием не изменился.
    REQUIRE(source[14] == 0);
    REQUIRE(source[15] == 15);
}

TEST_CASE("Неравенство разных расстановок", "[equality]") {
    const Board solved = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
    const Board emptyBeforeLast = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0, 15};

    REQUIRE(FifteenPuzzle(solved) != FifteenPuzzle(emptyBeforeLast));
    REQUIRE_FALSE(FifteenPuzzle(solved) == FifteenPuzzle(emptyBeforeLast));
    REQUIRE(FifteenPuzzle(solved) == FifteenPuzzle(solved));
    REQUIRE_FALSE(FifteenPuzzle(solved) != FifteenPuzzle(solved));
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

TEST_CASE("Формат записи в поток", "[stream]") {
    const Board solved = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};

    std::stringstream ss;
    ss << FifteenPuzzle(solved);

    REQUIRE(ss.str() == "1 2 3 4\n5 6 7 8\n9 10 11 12\n13 14 15 0\n");
}

TEST_CASE("Round-trip незавершённой игры", "[stream]") {
    const Board solved = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};
    FifteenPuzzle original(solved);
    REQUIRE(original.move(3, 2)); // пустая клетка в индексе 14

    std::stringstream ss;
    ss << original;

    FifteenPuzzle restored(solved); // заведомо другое состояние
    REQUIRE(ss >> restored);
    REQUIRE(restored == original);

    // Позиция пустой клетки восстановлена: ход из (3,3) должен быть возможен.
    REQUIRE(restored.move(3, 3));
    REQUIRE(restored.isSolved());
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

    SECTION("Значение больше 15 -> failbit, объект не изменен") {
        std::stringstream ss("16 2 3 4 5 6 7 8 9 10 11 12 13 14 15 0");
        REQUIRE_FALSE(ss >> game);
        REQUIRE(ss.fail());
        REQUIRE(game == backup);
    }

    SECTION("Отрицательное значение -> failbit, объект не изменен") {
        std::stringstream ss("-1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 0");
        REQUIRE_FALSE(ss >> game);
        REQUIRE(ss.fail());
        REQUIRE(game == backup);
    }

    SECTION("Нечисловой ввод -> failbit, объект не изменен") {
        std::stringstream ss("a b c d e f g h i j k l m n o p");
        REQUIRE_FALSE(ss >> game);
        REQUIRE(ss.fail());
        REQUIRE(game == backup);
    }

    SECTION("Пустой поток -> failbit, объект не изменен") {
        std::stringstream ss;
        REQUIRE_FALSE(ss >> game);
        REQUIRE(ss.fail());
        REQUIRE(game == backup);
    }

    SECTION("После ошибки позиция пустой клетки не испорчена") {
        std::stringstream ss("1 1 3 4 5 6 7 8 9 10 11 12 13 14 15 0");
        REQUIRE_FALSE(ss >> game);
        REQUIRE(game == backup);
        REQUIRE(game.move(3, 2));
        REQUIRE(game[14] == 0);
        REQUIRE(game[15] == 15);
    }
}