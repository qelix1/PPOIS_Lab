#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include "FifteenPuzzle.h"

// Вспомогательная функция для красивого вывода сетки
void printBoard(const FifteenPuzzle& game) {
    std::cout << "+----+----+----+----+\n";
    for (int i = 0; i < FifteenPuzzle::SIZE; ++i) {
        for (int j = 0; j < FifteenPuzzle::SIZE; ++j) {
            int val = game[i * FifteenPuzzle::SIZE + j];
            std::cout << "| ";
            if (val == FifteenPuzzle::EMPTY) {
                std::cout << "  ";
            } else {
                if (val < 10) std::cout << " "; // Для выравнивания однозначных чисел
                std::cout << val;
            }
            std::cout << " ";
        }
        std::cout << "|\n+----+----+----+----+\n";
    }
}

// Очистка потока ввода при ошибке
void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    FifteenPuzzle game;
    int choice = 0;

    while (true) {
        std::cout << "\n=== Игра «Пятнашки» ===\n";
        std::cout << "1. Новая игра\n";
        std::cout << "2. Сделать ход (по номеру клетки)\n";
        std::cout << "3. Показать поле\n";
        std::cout << "4. Проверить, собрана ли игра\n";
        std::cout << "5. Сохранить в файл\n";
        std::cout << "6. Загрузить из файла\n";
        std::cout << "7. Выход\n";
        std::cout << "Выберите пункт: ";

        if (!(std::cin >> choice)) {
            std::cout << "Ошибка ввода. Пожалуйста, введите число.\n";
            clearInput();
            continue;
        }

        switch (choice) {
            case 1:
                game = FifteenPuzzle(); // Создаём новую случайную игру
                std::cout << "Новая игра начата!\n";
                break;

            case 2: {
                if (game.isSolved()) {
                    std::cout << "Игра уже собрана! Начните новую игру.\n";
                    break;
                }
                int tile;
                std::cout << "Введите номер клетки для перемещения (1-15): ";
                if (!(std::cin >> tile)) {
                    std::cout << "Ошибка ввода.\n";
                    clearInput();
                    break;
                }
                if (tile < 1 || tile > 15) {
                    std::cout << "Некорректный номер. Допустимо от 1 до 15.\n";
                    break;
                }

                bool found = false;
                // Ищем клетку с введённым номером через operator[]
                for (int i = 0; i < FifteenPuzzle::CELLS; ++i) {
                    if (game[i] == tile) {
                        int row = i / FifteenPuzzle::SIZE;
                        int col = i % FifteenPuzzle::SIZE;
                        try {
                            if (game.move(row, col)) {
                                std::cout << "Ход выполнен.\n";
                            } else {
                                std::cout << "Невозможно переместить эту клетку (не соседняя с пустой).\n";
                            }
                        } catch (const std::exception& e) {
                            std::cout << "Ошибка при ходе: " << e.what() << "\n";
                        }
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    std::cout << "Клетка с таким номером не найдена.\n";
                }
                break;
            }

            case 3:
                printBoard(game);
                break;

            case 4:
                if (game.isSolved()) {
                    std::cout << "Поздравляем! Пятнашки собраны!\n";
                } else {
                    std::cout << "Пятнашки ещё не собраны.\n";
                }
                break;

            case 5: {
                std::string filename;
                std::cout << "Введите имя файла для сохранения: ";
                std::cin >> filename;
                std::ofstream outFile(filename); // RAII: файл закроется сам
                if (!outFile) {
                    std::cout << "Не удалось открыть файл для записи.\n";
                    break;
                }
                outFile << game;
                std::cout << "Игра сохранена в " << filename << "\n";
                break;
            }

            case 6: {
                std::string filename;
                std::cout << "Введите имя файла для загрузки: ";
                std::cin >> filename;
                std::ifstream inFile(filename); // RAII: файл закроется сам
                if (!inFile) {
                    std::cout << "Не удалось открыть файл для чтения.\n";
                    break;
                }
                FifteenPuzzle temp;
                if (inFile >> temp) {
                    game = temp;
                    std::cout << "Игра успешно загружена!\n";
                } else {
                    std::cout << "Ошибка чтения данных из файла. Игра не изменена.\n";
                }
                break;
            }

            case 7:
                std::cout << "До свидания!\n";
                return 0;

            default:
                std::cout << "Некорректный пункт меню. Попробуйте снова.\n";
                break;
        }
    }
    return 0;
}