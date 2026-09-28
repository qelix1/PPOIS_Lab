#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <stdexcept>
#include "PostMachine.h"

// Вспомогательная функция очистки потока при ошибке ввода
void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Красивый вывод состояния машины
void printState(const PostMachine& machine) {
    std::cout << "Позиция каретки: " << machine.head() << "\n";
    std::cout << "Размер программы: " << machine.programSize() << " команд\n";
    std::cout << "Состояние ленты:\n  ";
    
    if (machine.tape().empty()) {
        std::cout << "(лента пуста)\n";
    } else {
        // Выводим ленту от минимального до максимального индекса
        // Наша Tape умеет выводить только от 0 до maxIndex(), 
        // но для наглядности выведем диапазон вокруг каретки
        int from = machine.head() - 5;
        int to = machine.head() + 10;
        for (int i = from; i <= to; ++i) {
            if (i == machine.head()) {
                std::cout << "[" << machine.tape().get(i) << "]";
            } else {
                std::cout << " " << machine.tape().get(i) << " ";
            }
        }
        std::cout << "\n  (в скобках [ ] — текущая ячейка под кареткой)\n";
    }
}

int main() {
    PostMachine machine;
    int choice = 0;

    while (true) {
        std::cout << "\n=== Машина Поста ===\n";
        std::cout << "1. Загрузить программу из файла\n";
        std::cout << "2. Загрузить ленту из файла\n";
        std::cout << "3. Показать текущее состояние\n";
        std::cout << "4. Запустить программу\n";
        std::cout << "5. Сохранить состояние в файл\n";
        std::cout << "6. Выход\n";
        std::cout << "Выберите пункт: ";

        if (!(std::cin >> choice)) {
            std::cout << "Ошибка ввода. Введите число.\n";
            clearInput();
            continue;
        }
        clearInput(); //

        switch (choice) {
            case 1: {
                std::string filename;
                std::cout << "Введите путь к файлу программы: ";
                std::getline(std::cin, filename);
                if (filename.empty()) {
                    std::cout << "Пустой путь — отмена.\n";
                    break;
                }
                try {
                    machine.loadProgram(filename);
                    std::cout << "Программа успешно загружена. Команд: " 
                              << machine.programSize() << "\n";
                } catch (const std::exception& e) {
                    std::cout << "[Ошибка загрузки программы] " << e.what() << "\n";
                }
                break;
            }

            case 2: {
                std::string filename;
                std::cout << "Введите путь к файлу ленты: ";
                std::getline(std::cin, filename);
                if (filename.empty()) {
                    std::cout << "Пустой путь — отмена.\n";
                    break;
                }
                try {
                    machine.loadTape(filename);
                    std::cout << "Лента успешно загружена.\n";
                } catch (const std::exception& e) {
                    std::cout << "[Ошибка загрузки ленты] " << e.what() << "\n";
                }
                break;
            }

            case 3:
                printState(machine);
                break;

            case 4: {
                if (machine.programSize() == 0) {
                    std::cout << "Сначала загрузите программу (пункт 1).\n";
                    break;
                }
                std::cout << "Введите максимальное число шагов (Enter = 10000): ";
                std::string stepsStr;
                std::getline(std::cin, stepsStr);
                
                int maxSteps = 10000;
                if (!stepsStr.empty()) {
                    try {
                        maxSteps = std::stoi(stepsStr);
                        if (maxSteps <= 0) {
                            std::cout << "Число шагов должно быть > 0. Использую 10000.\n";
                            maxSteps = 10000;
                        }
                    } catch (...) {
                        std::cout << "Некорректное число. Использую 10000.\n";
                    }
                }

                std::cout << "Запуск...\n";
                try {
                    machine.run(maxSteps);
                    std::cout << "Программа завершилась командой ! (остановка).\n";
                    printState(machine);
                } catch (const std::exception& e) {
                    std::cout << "[Ошибка выполнения] " << e.what() << "\n";
                    printState(machine);
                }
                break;
            }

            case 5: {
                std::string filename;
                std::cout << "Введите путь к файлу для сохранения: ";
                std::getline(std::cin, filename);
                if (filename.empty()) {
                    std::cout << "Пустой путь — отмена.\n";
                    break;
                }
                std::ofstream outFile(filename);
                if (!outFile) {
                    std::cout << "[Ошибка] Не удалось открыть файл для записи.\n";
                    break;
                }
                outFile << machine;
                std::cout << "Состояние сохранено в " << filename << "\n";
                break;
            }

            case 6:
                std::cout << "Выход.\n";
                return 0;

            default:
                std::cout << "Некорректный пункт меню.\n";
                break;
        }
    }
    return 0;
}