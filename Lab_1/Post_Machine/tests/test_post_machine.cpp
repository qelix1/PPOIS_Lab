#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <string>
#include <stdexcept>
#include "PostMachine.h"

// Вспомогательная функция: записать строку во временный файл
static void writeFile(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    f << content;
}

// Вспомогательная функция: удалить файл
static void removeFile(const std::string& path) {
    std::remove(path.c_str());
}

TEST_CASE("PostCommand: сравнение", "[command]") {
    PostCommand a{PostCommand::Type::MoveRight, 5, -1};
    PostCommand b{PostCommand::Type::MoveRight, 5, -1};
    PostCommand c{PostCommand::Type::MoveRight, 6, -1};

    REQUIRE(a == b);
    REQUIRE(a != c);
}

TEST_CASE("PostCommand: вывод в поток", "[command][stream]") {
    SECTION("Движение и работа с метками") {
        std::ostringstream os;
        os << PostCommand{PostCommand::Type::MoveRight, 5, -1};
        os << "|";
        os << PostCommand{PostCommand::Type::MoveLeft, 2, -1};
        os << "|";
        os << PostCommand{PostCommand::Type::PutMark, 4, -1};
        os << "|";
        os << PostCommand{PostCommand::Type::EraseMark, 7, -1};
        REQUIRE(os.str() == "> 5|< 2|1 4|0 7");
    }

    SECTION("Ветвление") {
        std::ostringstream os;
        os << PostCommand{PostCommand::Type::Branch, 3, 8};
        REQUIRE(os.str() == "? 3; 8");
    }

    SECTION("Остановка") {
        std::ostringstream os;
        os << PostCommand{PostCommand::Type::Stop, -1, -1};
        REQUIRE(os.str() == "!");
    }
}

TEST_CASE("PostMachine: loadProgram — успешные случаи", "[load][program]") {
    const std::string path = "test_program.txt";

    SECTION("Простая корректная программа") {
        writeFile(path, 
            "# Комментарий\n"
            "1. > 2\n"
            "2. 1 3\n"
            "3. !\n");
        PostMachine m;
        REQUIRE_NOTHROW(m.loadProgram(path));
        REQUIRE(m.programSize() == 3);
        removeFile(path);
    }

    SECTION("Программа с пустыми строками и комментариями") {
        writeFile(path, 
            "# Заголовок\n"
            "\n"
            "1. !\n"
            "\n"
            "# Конец\n");
        PostMachine m;
        REQUIRE_NOTHROW(m.loadProgram(path));
        REQUIRE(m.programSize() == 1);
        removeFile(path);
    }
}

TEST_CASE("PostMachine: loadProgram — ошибки", "[load][program][errors]") {
    const std::string path = "test_program_bad.txt";
    PostMachine m;

    SECTION("Несуществующий файл") {
        REQUIRE_THROWS_AS(m.loadProgram("no_such_file_xyz.txt"), std::runtime_error);
    }

    SECTION("Пустая программа") {
        writeFile(path, "# Только комментарий\n");
        REQUIRE_THROWS_AS(m.loadProgram(path), std::runtime_error);
        removeFile(path);
    }

    SECTION("Пропуск в нумерации") {
        writeFile(path, "1. !\n3. !\n");
        REQUIRE_THROWS_AS(m.loadProgram(path), std::runtime_error);
        removeFile(path);
    }

    SECTION("Нет точки в строке") {
        writeFile(path, "1 !\n");
        REQUIRE_THROWS_AS(m.loadProgram(path), std::runtime_error);
        removeFile(path);
    }

    SECTION("Неизвестная команда") {
        writeFile(path, "1. X 5\n");
        REQUIRE_THROWS_AS(m.loadProgram(path), std::runtime_error);
        removeFile(path);
    }

    SECTION("? без ;") {
        writeFile(path, "1. ? 2 3\n");
        REQUIRE_THROWS_AS(m.loadProgram(path), std::runtime_error);
        removeFile(path);
    }

    SECTION("> без аргумента") {
        writeFile(path, "1. >\n");
        REQUIRE_THROWS_AS(m.loadProgram(path), std::runtime_error);
        removeFile(path);
    }

    SECTION("Пустая команда после номера") {
        writeFile(path, "1.\n");
        REQUIRE_THROWS_AS(m.loadProgram(path), std::runtime_error);
        removeFile(path);
    }

    SECTION("Номер команды не число") {
        writeFile(path, "abc. !\n");
        REQUIRE_THROWS_AS(m.loadProgram(path), std::runtime_error);
        removeFile(path);
    }

    SECTION("Номер с мусором после цифр") {
        writeFile(path, "1x. !\n");
        REQUIRE_THROWS_AS(m.loadProgram(path), std::runtime_error);
        removeFile(path);
    }

    SECTION("Точка без номера") {
        writeFile(path, ". !\n");
        REQUIRE_THROWS_AS(m.loadProgram(path), std::runtime_error);
        removeFile(path);
    }

    SECTION("Ветвление с пустым вторым аргументом") {
        writeFile(path, "1. ? 2;\n");
        REQUIRE_THROWS_AS(m.loadProgram(path), std::runtime_error);
        removeFile(path);
    }

    SECTION("Ветвление с пустым первым аргументом") {
        writeFile(path, "1. ? ; 3\n");
        REQUIRE_THROWS_AS(m.loadProgram(path), std::runtime_error);
        removeFile(path);
    }
}

TEST_CASE("PostMachine: loadTape", "[load][tape]") {
    const std::string path = "test_tape.txt";
    PostMachine m;

    SECTION("Корректная лента") {
        writeFile(path, "0\n0 1 0 1 0\n");
        REQUIRE_NOTHROW(m.loadTape(path));
        REQUIRE(m.head() == 0);
        REQUIRE(m.tape().get(0) == Tape::EMPTY);
        REQUIRE(m.tape().get(1) == Tape::MARK);
        REQUIRE(m.tape().get(3) == Tape::MARK);
        removeFile(path);
    }

    SECTION("Несуществующий файл") {
        REQUIRE_THROWS_AS(m.loadTape("no_such_tape_xyz.txt"), std::runtime_error);
    }

    SECTION("Отсутствует позиция каретки") {
        writeFile(path, "");
        REQUIRE_THROWS_AS(m.loadTape(path), std::runtime_error);
        removeFile(path);
    }

    SECTION("Некорректное значение на ленте") {
        writeFile(path, "0\n0 2 1\n");
        REQUIRE_THROWS_AS(m.loadTape(path), std::runtime_error);
        removeFile(path);
    }
}

TEST_CASE("PostMachine: run — успешные программы", "[run]") {
    const std::string progPath = "test_run_prog.txt";
    const std::string tapePath = "test_run_tape.txt";

    SECTION("Простая программа: поставить метку и остановиться") {
        writeFile(progPath, "1. 1 2\n2. !\n");
        writeFile(tapePath, "0\n0 0 0\n");
        
        PostMachine m;
        m.loadProgram(progPath);
        m.loadTape(tapePath);
        REQUIRE_NOTHROW(m.run());
        REQUIRE(m.tape().get(0) == Tape::MARK);
        
        removeFile(progPath);
        removeFile(tapePath);
    }

    SECTION("Ветвление: переход по 0") {
        // Если ячейка пустая → поставить метку (команда 2)
        // Если ячейка занята → стереть (команда 3)
        writeFile(progPath, "1. ? 2; 3\n2. 1 4\n3. 0 4\n4. !\n");
        writeFile(tapePath, "0\n0 1\n"); // ячейка 0 пустая → пойдём в 2
        
        PostMachine m;
        m.loadProgram(progPath);
        m.loadTape(tapePath);
        REQUIRE_NOTHROW(m.run());
        REQUIRE(m.tape().get(0) == Tape::MARK);
        
        removeFile(progPath);
        removeFile(tapePath);
    }

    SECTION("Ветвление: переход по 1") {
        // Ячейка под кареткой занята → переход на команду 3 (стирание)
        writeFile(progPath, "1. ? 2; 3\n2. 1 4\n3. 0 4\n4. !\n");
        writeFile(tapePath, "1\n0 1\n"); // каретка в 1, ячейка 1 занята → пойдём в 3

        PostMachine m;
        m.loadProgram(progPath);
        m.loadTape(tapePath);
        REQUIRE_NOTHROW(m.run());
        REQUIRE(m.head() == 1);
        REQUIRE(m.tape().get(1) == Tape::EMPTY);
        REQUIRE(m.tape().empty()); // метка стёрта; если бы взяли ветку 2, run() бросил бы исключение

        removeFile(progPath);
        removeFile(tapePath);
    }

    SECTION("Движение влево/вправо") {
        writeFile(progPath, "1. > 2\n2. 1 3\n3. < 4\n4. !\n");
        writeFile(tapePath, "0\n0 0 0\n");
        
        PostMachine m;
        m.loadProgram(progPath);
        m.loadTape(tapePath);
        REQUIRE_NOTHROW(m.run());
        REQUIRE(m.head() == 0); // сдвинулись вправо, поставили метку, сдвинулись влево
        REQUIRE(m.tape().get(1) == Tape::MARK);
        
        removeFile(progPath);
        removeFile(tapePath);
    }
}

TEST_CASE("PostMachine: run — ошибки", "[run][errors]") {
    const std::string progPath = "test_err_prog.txt";
    const std::string tapePath = "test_err_tape.txt";

    SECTION("Программа не загружена") {
        PostMachine m;
        REQUIRE_THROWS_AS(m.run(), std::runtime_error);
    }

    SECTION("Превышение maxSteps (бесконечный цикл)") {
        writeFile(progPath, "1. > 1\n"); // бесконечно двигается вправо
        writeFile(tapePath, "0\n");
        
        PostMachine m;
        m.loadProgram(progPath);
        m.loadTape(tapePath);
        REQUIRE_THROWS_AS(m.run(100), std::runtime_error);
        
        removeFile(progPath);
        removeFile(tapePath);
    }

    SECTION("Переход к несуществующей команде") {
        writeFile(progPath, "1. > 99\n");
        writeFile(tapePath, "0\n");
        
        PostMachine m;
        m.loadProgram(progPath);
        m.loadTape(tapePath);
        REQUIRE_THROWS_AS(m.run(), std::runtime_error);
        
        removeFile(progPath);
        removeFile(tapePath);
    }

    SECTION("Метка в занятую ячейку") {
        writeFile(progPath, "1. 1 2\n2. !\n");
        writeFile(tapePath, "0\n1 0 0\n"); // ячейка 0 уже занята
        
        PostMachine m;
        m.loadProgram(progPath);
        m.loadTape(tapePath);
        REQUIRE_THROWS_AS(m.run(), std::runtime_error);
        
        removeFile(progPath);
        removeFile(tapePath);
    }

    SECTION("Стирание пустой ячейки") {
        writeFile(progPath, "1. 0 2\n2. !\n");
        writeFile(tapePath, "0\n0 0 0\n"); // ячейка 0 пустая
        
        PostMachine m;
        m.loadProgram(progPath);
        m.loadTape(tapePath);
        REQUIRE_THROWS_AS(m.run(), std::runtime_error);
        
        removeFile(progPath);
        removeFile(tapePath);
    }
}

TEST_CASE("PostMachine: копирование и сравнение", "[copy][equality]") {
    const std::string progPath = "test_copy_prog.txt";
    const std::string tapePath = "test_copy_tape.txt";
    writeFile(progPath, "1. !\n");
    writeFile(tapePath, "0\n1 1 0\n");

    PostMachine original;
    original.loadProgram(progPath);
    original.loadTape(tapePath);

    SECTION("Копия равна оригиналу") {
        PostMachine copy = original;
        REQUIRE(copy == original);
    }

    SECTION("Изменение копии не меняет оригинал") {
        PostMachine copy = original;
        copy.run();
        // Программа простая (только !), но состояние каретки могло измениться
        // Здесь просто проверяем, что копия не влияет на оригинал
        REQUIRE(original.head() == 0);
    }

    SECTION("Самоприсваивание") {
        original = original;
        REQUIRE(original.programSize() == 1);
    }

    SECTION("operator!=") {
        PostMachine copy = original;
        REQUIRE_FALSE(original != copy);
        REQUIRE_FALSE(copy != original);

        PostMachine other;
        REQUIRE(original != other);
        REQUIRE(other != original);

        writeFile(tapePath, "0\n0 0 1\n");
        copy.loadTape(tapePath);
        REQUIRE(copy != original);
        REQUIRE(original != copy);

        removeFile(tapePath);
    }

    removeFile(progPath);
    removeFile(tapePath);
}

TEST_CASE("PostMachine: потоковый ввод-вывод", "[stream]") {
    const std::string progPath = "test_stream_prog.txt";
    const std::string tapePath = "test_stream_tape.txt";
    writeFile(progPath, "1. !\n");
    writeFile(tapePath, "3\n0 1 0 1 0\n");

    PostMachine original;
    original.loadProgram(progPath);
    original.loadTape(tapePath);

    SECTION("Round-trip: << → >>") {
        std::stringstream ss;
        ss << original;

        PostMachine restored;
        REQUIRE(ss >> restored);
        REQUIRE(restored.head() == original.head());
        REQUIRE(restored.tape() == original.tape());
    }

    SECTION("Некорректная позиция каретки → failbit, машина не изменена") {
        std::stringstream ss("abc\n0 1 0 1 0\n");
        PostMachine m = original;
        REQUIRE_FALSE(ss >> m);
        REQUIRE(m == original);
    }

    SECTION("Некорректное значение на ленте → failbit, машина не изменена") {
        std::stringstream ss("7\n0 3 1\n");
        PostMachine m = original;
        REQUIRE_FALSE(ss >> m);
        REQUIRE(m == original);
    }

    removeFile(progPath);
    removeFile(tapePath);
}