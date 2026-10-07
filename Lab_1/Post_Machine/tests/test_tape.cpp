#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <stdexcept>
#include "Tape.h"

TEST_CASE("Tape: пустая лента по умолчанию", "[tape]") {
    Tape tape;
    REQUIRE(tape.empty());
    REQUIRE(tape.marks() == 0);
    REQUIRE(tape.maxIndex() == -1);
    REQUIRE(tape.get(0) == Tape::EMPTY);
    REQUIRE(tape.get(100) == Tape::EMPTY);
    REQUIRE(tape.get(-100) == Tape::EMPTY);
}

TEST_CASE("Tape: set и get", "[tape]") {
    Tape tape;

    SECTION("Поставить и прочитать метку") {
        tape.set(5, Tape::MARK);
        REQUIRE(tape.get(5) == Tape::MARK);
        REQUIRE_FALSE(tape.empty());
        REQUIRE(tape.marks() == 1);
    }

    SECTION("Стереть метку") {
        tape.set(5, Tape::MARK);
        tape.set(5, Tape::EMPTY);
        REQUIRE(tape.get(5) == Tape::EMPTY);
        REQUIRE(tape.empty());
        REQUIRE(tape.marks() == 0);
    }

    SECTION("Отрицательные индексы") {
        tape.set(-3, Tape::MARK);
        REQUIRE(tape.get(-3) == Tape::MARK);
        REQUIRE(tape.marks() == 1);
    }

    SECTION("Некорректное значение → invalid_argument") {
        REQUIRE_THROWS_AS(tape.set(0, 7), std::invalid_argument);
        REQUIRE_THROWS_AS(tape.set(0, -1), std::invalid_argument);
        REQUIRE_THROWS_AS(tape.set(0, 2), std::invalid_argument);
    }

    SECTION("Повторная установка метки не увеличивает marks()") {
        tape.set(1, Tape::MARK);
        tape.set(1, Tape::MARK);
        REQUIRE(tape.marks() == 1);
    }
}

TEST_CASE("Tape: maxIndex", "[tape]") {
    Tape tape;
    
    SECTION("Только положительные индексы") {
        tape.set(0, Tape::MARK);
        tape.set(10, Tape::MARK);
        tape.set(3, Tape::MARK);
        REQUIRE(tape.maxIndex() == 10);
    }

    SECTION("Отрицательные индексы игнорируются при поиске максимума") {
        tape.set(-5, Tape::MARK);
        tape.set(2, Tape::MARK);
        REQUIRE(tape.maxIndex() == 2);
    }

    SECTION("Только отрицательные индексы") {
        tape.set(-5, Tape::MARK);
        tape.set(-1, Tape::MARK);
        REQUIRE(tape.maxIndex() == -1); // положительных индексов нет → -1
    }
}

TEST_CASE("Tape: clear", "[tape]") {
    Tape tape;
    tape.set(0, Tape::MARK);
    tape.set(100, Tape::MARK);
    tape.set(-50, Tape::MARK);
    REQUIRE_FALSE(tape.empty());

    tape.clear();
    REQUIRE(tape.empty());
    REQUIRE(tape.marks() == 0);
    REQUIRE(tape.get(0) == Tape::EMPTY);
    REQUIRE(tape.get(100) == Tape::EMPTY);
}

TEST_CASE("Tape: копирование и сравнение", "[tape]") {
    Tape original;
    original.set(1, Tape::MARK);
    original.set(5, Tape::MARK);

    SECTION("Копия равна оригиналу") {
        Tape copy = original;
        REQUIRE(copy == original);
    }

    SECTION("Изменение копии не влияет на оригинал") {
        Tape copy = original;
        copy.set(10, Tape::MARK);
        REQUIRE(copy != original);
        REQUIRE(original.get(10) == Tape::EMPTY);
    }

    SECTION("Самоприсваивание") {
        original = original;
        REQUIRE(original.get(1) == Tape::MARK);
    }
}

TEST_CASE("Tape: потоковый ввод-вывод", "[tape]") {
    Tape original;
    original.set(0, Tape::MARK);
    original.set(1, Tape::EMPTY); // Не создаст ключ
    original.set(2, Tape::MARK);
    original.set(3, Tape::MARK);

    SECTION("Round-trip: << → >>") {
        std::stringstream ss;
        ss << original;

        Tape restored;
        REQUIRE(ss >> restored);
        REQUIRE(restored == original);
    }

    SECTION("Пустая лента выводится пустой строкой") {
        Tape empty;
        std::stringstream ss;
        ss << empty;
        REQUIRE(ss.str().empty());
    }

    SECTION("Чтение чисел, отличных от 0/1 → failbit") {
        std::stringstream ss("0 1 2 0 1");
        Tape tape;
        REQUIRE_FALSE(ss >> tape);
        REQUIRE(ss.fail());
    }

    SECTION("Чтение из пустого потока → успех, лента становится пустой") {
        Tape tape;
        tape.set(0, Tape::MARK);
        tape.set(2, Tape::MARK);

        std::stringstream ss("");
        REQUIRE(ss >> tape);
        REQUIRE(tape.empty());
        REQUIRE(tape.marks() == 0);
    }

    SECTION("Нечисловой ввод → failbit, лента не изменена") {
        Tape tape;
        tape.set(0, Tape::MARK);
        Tape backup = tape;

        std::stringstream ss("0 1 x");
        REQUIRE_FALSE(ss >> tape);
        REQUIRE(ss.fail());
        REQUIRE(tape == backup);
    }
}