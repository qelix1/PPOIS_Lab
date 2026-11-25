#ifndef STUDENTIS_SERVICES_DORMITORY_H
#define STUDENTIS_SERVICES_DORMITORY_H

#include <string>
#include <vector>

namespace studentis {

class DormRoom;
class Student;
class Teacher;

/**
 * @brief Общежитие: комнаты, комендант и стоимость проживания.
 */
class Dormitory {
public:
    Dormitory() = default;
    Dormitory(std::string id, std::string address, int totalRooms, Teacher* warden,
              double pricePerMonth, std::string phone);

    /// Добавить комнату; сверх totalRooms_ — InvalidStudentDataException.
    void addRoom(DormRoom& room);
    /// Первая комната с подходящим полом и свободным местом или nullptr.
    DormRoom* findFreeRoom(const std::string& gender) const;
    /// Заполненность общежития в процентах (0..100).
    double occupancyRate() const;
    /// Свободные места во всех комнатах.
    int freeBeds() const;
    /// Поселить студента; свободной комнаты нет — DormitoryFullException.
    void assignStudent(Student& student);
    /// Выселить студента из общежития; true — если он был поселен.
    bool evictStudent(const std::string& studentId);
    /// Месячная выручка по занятым местам.
    double monthlyRevenue() const;

    const std::string& getId() const { return id_; }
    const std::string& getAddress() const { return address_; }
    int getTotalRooms() const { return totalRooms_; }
    Teacher* getWarden() const { return warden_; }
    double getPricePerMonth() const { return pricePerMonth_; }
    const std::string& getPhone() const { return phone_; }
    const std::vector<DormRoom*>& getRooms() const { return rooms_; }

    void setWarden(Teacher* warden) { warden_ = warden; }
    void setPricePerMonth(double pricePerMonth) { pricePerMonth_ = pricePerMonth; }
    void setPhone(const std::string& phone) { phone_ = phone; }

private:
    std::string id_;
    std::string address_;
    int totalRooms_ = 0;
    Teacher* warden_ = nullptr;
    double pricePerMonth_ = 0.0;
    std::string phone_;
    std::vector<DormRoom*> rooms_;
};

} // namespace studentis

#endif
