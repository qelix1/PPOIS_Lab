#ifndef STUDENTIS_ACADEMIC_CLASSROOM_H
#define STUDENTIS_ACADEMIC_CLASSROOM_H

#include <string>
#include <vector>

namespace studentis {

class Lesson;
class StudentGroup;

/**
 * @brief Аудитория: вместимость, тип и оборудование.
 */
class Classroom {
public:
    Classroom() = default;
    Classroom(std::string id, std::string building, std::string roomNumber, int capacity,
              std::string type, bool hasProjector, bool hasComputers);

    /// Помещается ли группа по числу активных студентов.
    bool fits(const StudentGroup& group) const;
    /// Подходит ли аудитория для занятия: лекции нужен проектор, лаборатории — компьютеры.
    bool supports(const Lesson& lesson) const;
    /// Свободна ли аудитория в интервал даты.
    bool isAvailable(const std::string& date, const std::string& startTime,
                     const std::string& endTime) const;
    /// Заполненность аудитории группой в процентах (0..100).
    double utilizationPercent(const StudentGroup& group) const;
    /// Тип аудитории совпадает с запросом.
    bool matchesType(const std::string& type) const;
    /// Переименовать аудиторию (номер); пустой номер — ошибка данных.
    void rename(const std::string& roomNumber);

    const std::string& getId() const { return id_; }
    const std::string& getBuilding() const { return building_; }
    const std::string& getRoomNumber() const { return roomNumber_; }
    int getCapacity() const { return capacity_; }
    const std::string& getType() const { return type_; }
    bool hasProjector() const { return hasProjector_; }
    bool hasComputers() const { return hasComputers_; }

    void setCapacity(int capacity) { capacity_ = capacity; }
    void setHasProjector(bool hasProjector) { hasProjector_ = hasProjector; }
    void setHasComputers(bool hasComputers) { hasComputers_ = hasComputers; }

private:
    friend class Lesson;

    /// Учесть занятие в аудитории.
    void book(Lesson& lesson);
    /// Снять занятие с учёта.
    void unbook(const Lesson& lesson);

    std::string id_;
    std::string building_;
    std::string roomNumber_;
    int capacity_ = 0;
    std::string type_;
    bool hasProjector_ = false;
    bool hasComputers_ = false;
    // Приватный помощник: занятые интервалы для проверки доступности.
    std::vector<Lesson*> lessons_;
};

} // namespace studentis

#endif
