#ifndef STUDENTIS_SERVICES_DORMROOM_H
#define STUDENTIS_SERVICES_DORMROOM_H

#include <string>
#include <vector>

namespace studentis {

class Dormitory;
class Student;

/**
 * @brief Комната общежития: вместимость, этаж, пол жильцов и жильцы.
 *
 * Пол комнаты: male, female или mixed. Статусы: active, closed.
 */
class DormRoom {
public:
    DormRoom() = default;
    DormRoom(std::string number, Dormitory* dormitory, int capacity, int floor,
             std::string gender, std::string status = "active");

    /// Поселить студента; дубль — no-op, нет мест или чужой пол — DormitoryFullException.
    void addResident(Student& student);
    /// Выселить студента по номеру; true — если он проживал в комнате.
    bool removeResident(const std::string& studentId);
    /// В комнате есть свободное место.
    bool hasFreeBed() const;
    /// Комната заполнена.
    bool isFull() const;
    /// Число свободных мест.
    int freeBeds() const;
    /// Проживает ли студент в комнате.
    bool containsStudent(const Student& student) const;
    /// Стоимость проживания в месяц по тарифу общежития.
    double monthlyPrice() const;

    const std::string& getNumber() const { return number_; }
    Dormitory* getDormitory() const { return dormitory_; }
    int getCapacity() const { return capacity_; }
    int getFloor() const { return floor_; }
    const std::string& getGender() const { return gender_; }
    const std::vector<Student*>& getResidents() const { return residents_; }
    const std::string& getStatus() const { return status_; }

    void setDormitory(Dormitory* dormitory) { dormitory_ = dormitory; }
    void setCapacity(int capacity) { capacity_ = capacity; }
    void setStatus(const std::string& status) { status_ = status; }

private:
    std::string number_;
    Dormitory* dormitory_ = nullptr;
    int capacity_ = 0;
    int floor_ = 1;
    std::string gender_ = "mixed";
    std::vector<Student*> residents_;
    std::string status_ = "active";
};

} // namespace studentis

#endif
