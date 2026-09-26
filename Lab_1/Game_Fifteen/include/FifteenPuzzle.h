#ifndef FIFTEEN_PUZZLE_H
#define FIFTEEN_PUZZLE_H
#include <iosfwd>
#include <array>

/**
 * @brief Игра-головоломка «Пятнашки» на поле 4×4.
 *
 * Поле содержит 16 клеток: номера 1–15 и одну пустую клетку (код 0).
 * Цель — расставить номера в порядке 1, 2, …, 15, оставив пустую клетку
 * в правом нижнем углу.
 *
 * Класс не зависит от пользовательского интерфейса: весь ввод-вывод
 * выполняется через потоковые операции operator<</operator>>.
 *
 * @invariant board_ содержит каждое значение 0..15 ровно один раз;
 *            emptyPos_ — индекс клетки со значением EMPTY.
 */


class FifteenPuzzle {
public:
    static const int SIZE  = 4;   ///< Размер стороны поля.
    static const int CELLS = 16;  ///< Общее число клеток (SIZE * SIZE).
    static const int EMPTY = 0;   ///< Код пустой клетки.

    /// @brief Создаёт головоломку со случайной решаемой расстановкой.
    FifteenPuzzle();

    /**
     * @brief Создаёт головоломку с заданной расстановкой.
     * @param cells 16 чисел — перестановка 0..15 (построчная нумерация).
     * @throw std::invalid_argument если cells — не перестановка 0..15.
     */
    explicit FifteenPuzzle(const std::array<int, CELLS>& cells);

    /// @brief Конструктор копирования.
    FifteenPuzzle(const FifteenPuzzle& other);

    /// @brief Операция присваивания копированием.
    FifteenPuzzle& operator=(const FifteenPuzzle& other);

    /// @brief Деструктор (ресурсы освобождаются средствами RAII).
    ~FifteenPuzzle() = default;

    /**
     * @brief Передвигает клетку (row, col) на место пустой.
     * @param row строка клетки (0..SIZE-1).
     * @param col столбец клетки (0..SIZE-1).
     * @return true, если ход выполнен; false, если клетка не соседствует
     *         с пустой или координаты некорректны.
     */
    bool move(int row, int col);

    /**
     * @brief Значение клетки по линейному индексу (EMPTY — пустая клетка).
     * @param index индекс клетки 0..CELLS-1.
     * @return значение клетки.
     * @throw std::out_of_range если index вне диапазона 0..CELLS-1.
     */
    int operator[](int index) const;

    /// @brief Проверяет правильность расстановки (1..15, пустая — последней).
    bool isSolved() const;

    /// @brief Сравнение расстановок на равенство.
    bool operator==(const FifteenPuzzle& rhs) const;

    /// @brief Сравнение расстановок на неравенство.
    bool operator!=(const FifteenPuzzle& rhs) const;

private:
    std::array<int, CELLS> board_;  ///< Игровое поле (построчное хранение).
    int emptyPos_;                  ///< Индекс пустой клетки.

    /// @brief Случайное перемешивание до получения решаемой расстановки.
    void shuffle();

    /// @brief Критерий разрешимости расстановки (чётность числа инверсий).
    static bool isSolvable(const std::array<int, CELLS>& b);

    /// @brief true, если клетки a и b соседние по вертикали/горизонтали.
    static bool isAdjacent(int a, int b);
};

/// @brief Записывает расстановку в поток: 4 строки по 4 числа через пробел.
std::ostream& operator<<(std::ostream& os, const FifteenPuzzle& p);

/// @brief Считывает из потока 16 чисел. Если это не перестановка 0..15,
///        устанавливается failbit, объект не изменяется.
std::istream& operator>>(std::istream& is, FifteenPuzzle& p);

#endif // FIFTEEN_PUZZLE_H