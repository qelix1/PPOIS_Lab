#ifndef TAPE_H
#define TAPE_H

#include <unordered_map>
#include <iosfwd> 
#include <cstddef>

/**
 * @brief Класс, представляющий бесконечную ленту машины Поста.
 * 
 * Использует разреженное хранение (std::unordered_map) для имитации
 * бесконечной ленты. Ячейки, которых нет в map, считаются пустыми (EMPTY).
 */
class Tape {
public:
    /// @brief Значение пустой ячейки.
    static constexpr int EMPTY = 0;
    
    /// @brief Значение заполненной ячейки (метка).
    static constexpr int MARK = 1;

    /**
     * @brief Конструктор по умолчанию. Создает пустую ленту.
     */
    Tape() = default;

    /**
     * @brief Конструктор копирования.
     * @param other Лента для копирования.
     */
    Tape(const Tape& other) = default;

    /**
     * @brief Оператор присваивания копированием.
     * @param other Лента для присваивания.
     * @return Ссылка на текущий объект.
     */
    Tape& operator=(const Tape& other) = default;

    /**
     * @brief Деструктор по умолчанию.
     */
    ~Tape() = default;

    /**
     * @brief Получить значение ячейки по индексу.
     * @param index Индекс ячейки (может быть отрицательным).
     * @return MARK (1), если ячейка заполнена, иначе EMPTY (0).
     */
    int get(int index) const;

    /**
     * @brief Установить значение ячейки.
     * @param index Индекс ячейки.
     * @param value Значение (EMPTY или MARK).
     * @throw std::invalid_argument Если value не равно EMPTY или MARK.
     */
    void set(int index, int value);

    /**
     * @brief Очистить всю ленту (сделать все ячейки пустыми).
     */
    void clear();

    /**
     * @brief Проверить, пуста ли лента.
     * @return true, если на ленте нет ни одной метки, иначе false.
     */
    bool empty() const;

    /**
     * @brief Получить количество меток на ленте.
     * @return Количество ячеек со значением MARK.
     */
    std::size_t marks() const;

    /**
     * @brief Получить максимальный индекс, на котором стоит метка.
     * @return Максимальный индекс или -1, если лента пуста.
     */
    int maxIndex() const;

    /**
     * @brief Оператор сравнения на равенство.
     * @param rhs Другая лента.
     * @return true, если ленты идентичны.
     */
    bool operator==(const Tape& rhs) const;

    /**
     * @brief Оператор сравнения на неравенство.
     * @param rhs Другая лента.
     * @return true, если ленты различаются.
     */
    bool operator!=(const Tape& rhs) const;

private:
    /// @brief Разреженное хранилище ячеек. Ключ — индекс, значение — 0 или 1.
    std::unordered_map<int, int> cells_;
};

/**
 * @brief Оператор вывода ленты в поток.
 * 
 * Выводит ячейки от 0 до maxIndex() в формате: 0 1 0 1...
 * Если лента пуста, выводит пустую строку.
 * 
 * @param os Выходной поток.
 * @param tape Лента для вывода.
 * @return Ссылка на выходной поток.
 */
std::ostream& operator<<(std::ostream& os, const Tape& tape);

/**
 * @brief Оператор ввода ленты из потока.
 * 
 * Читает последовательность чисел (0 или 1) и заполняет ленту.
 * При ошибке ввода объект tape не изменяется.
 * 
 * @param is Входной поток.
 * @param tape Лента для заполнения.
 * @return Ссылка на входной поток.
 */
std::istream& operator>>(std::istream& is, Tape& tape);

#endif 