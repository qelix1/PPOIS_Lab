#ifndef POST_MACHINE_H
#define POST_MACHINE_H

#include <vector>
#include <string>
#include <iosfwd>
#include <cstddef>
#include "Tape.h"

/**
 * @brief Структура, представляющая одну команду машины Поста.
 */
struct PostCommand {
    /**
     * @brief Тип команды.
     */
    enum class Type {
        MoveRight,  ///< Сдвинуть каретку вправо (>)
        MoveLeft,   ///< Сдвинуть каретку влево (<)
        PutMark,    ///< Поставить метку (1)
        EraseMark,  ///< Стереть метку (0)
        Branch,     ///< Условный переход (?)
        Stop        ///< Остановка (!)
    };

    Type type;      ///< Тип команды
    int arg1 = -1;  ///< Первый аргумент (номер команды для перехода или движения)
    int arg2 = -1;  ///< Второй аргумент (используется только для Branch)

    /**
     * @brief Оператор сравнения на равенство.
     */
    bool operator==(const PostCommand& rhs) const;
    
    /**
     * @brief Оператор сравнения на неравенство.
     */
    bool operator!=(const PostCommand& rhs) const;
};

/**
 * @brief Оператор вывода команды в поток (для отладки).
 */
std::ostream& operator<<(std::ostream& os, const PostCommand& cmd);

/**
 * @brief Класс, реализующий машину Поста.
 * 
 * Хранит ленту, положение каретки и программу. 
 * Позволяет загружать программу и ленту из файлов, а также выполнять программу.
 */
class PostMachine {
public:
    /**
     * @brief Конструктор по умолчанию.
     */
    PostMachine() = default;

    /**
     * @brief Конструктор копирования.
     */
    PostMachine(const PostMachine& other) = default;

    /**
     * @brief Оператор присваивания копированием.
     */
    PostMachine& operator=(const PostMachine& other) = default;

    /**
     * @brief Деструктор по умолчанию.
     */
    ~PostMachine() = default;

    /**
     * @brief Загрузить программу из файла.
     * 
     * Формат файла: строки вида "N. команда", где N — номер строки.
     * Поддерживаются комментарии (начинаются с #).
     * 
     * @param filename Путь к файлу с программой.
     * @throw std::runtime_error Если файл не открывается, программа пуста, 
     *        нарушена нумерация строк или встречена неизвестная команда.
     */
    void loadProgram(const std::string& filename);

    /**
     * @brief Загрузить начальное состояние ленты из файла.
     * 
     * Формат файла: первая строка — положение каретки (целое число),
     * последующие строки — содержимое ленты (0 и 1).
     * 
     * @param filename Путь к файлу с лентой.
     * @throw std::runtime_error Если файл не открывается или данные некорректны.
     */
    void loadTape(const std::string& filename);

    /**
     * @brief Запустить выполнение программы.
     * 
     * @param maxSteps Максимальное количество шагов (защита от бесконечного цикла).
     * @throw std::runtime_error Если во время выполнения произошла ошибка:
     *        - попытка поставить метку в занятую ячейку,
     *        - попытка стереть пустую ячейку,
     *        - переход к несуществующей команде,
     *        - превышение лимита maxSteps.
     */
    void run(int maxSteps = 10000);

    /**
     * @brief Получить текущее положение каретки.
     */
    int head() const;

    /**
     * @brief Получить константную ссылку на ленту.
     */
    const Tape& tape() const;

    /**
     * @brief Получить количество команд в программе.
     */
    std::size_t programSize() const;

    /**
     * @brief Оператор сравнения на равенство (сравнивает ленту, каретку и программу).
     */
    bool operator==(const PostMachine& rhs) const;

    /**
     * @brief Оператор сравнения на неравенство.
     */
    bool operator!=(const PostMachine& rhs) const;

private:
    Tape tape_;                         ///< Лента машины
    int head_ = 0;                      ///< Положение каретки
    std::vector<PostCommand> program_;  ///< Программа (набор команд)

    /**
     * @brief Разобрать одну строку программы в команду.
     * 
     * @param line Строка вида "> 5" или "? 3; 7".
     * @return PostCommand Разобранная команда.
     * @throw std::runtime_error Если синтаксис команды неверен.
     */
    static PostCommand parseCommand(const std::string& line);
};

/**
 * @brief Оператор вывода состояния машины в поток.
 * 
 * Выводит положение каретки и ленту.
 */
std::ostream& operator<<(std::ostream& os, const PostMachine& machine);

/**
 * @brief Оператор ввода состояния машины из потока.
 * 
 * Читает положение каретки и ленту. При ошибке объект не изменяется.
 */
std::istream& operator>>(std::istream& is, PostMachine& machine);

#endif // POST_MACHINE_H