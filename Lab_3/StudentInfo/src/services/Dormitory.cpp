#include "studentis/services/Dormitory.h"

#include <string>
#include <vector>

#include "studentis/core/Student.h"
#include "studentis/exceptions/DormitoryFullException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"
#include "studentis/services/DormRoom.h"

namespace studentis {

Dormitory::Dormitory(std::string id, std::string address, int totalRooms, Teacher* warden,
                     double pricePerMonth, std::string phone)
    : id_(std::move(id)),
      address_(std::move(address)),
      totalRooms_(totalRooms),
      warden_(warden),
      pricePerMonth_(pricePerMonth),
      phone_(std::move(phone)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор общежития не может быть пустым");
    if (address_.empty()) throw InvalidStudentDataException("адрес общежития не может быть пустым");
    if (totalRooms_ <= 0) throw InvalidStudentDataException("число комнат должно быть положительным");
    if (pricePerMonth_ <= 0.0) throw InvalidStudentDataException("стоимость проживания должна быть положительной");
}

void Dormitory::addRoom(DormRoom& room) {
    for (const DormRoom* current : rooms_) {
        if (current == &room || current->getNumber() == room.getNumber()) return;
    }
    if (static_cast<int>(rooms_.size()) >= totalRooms_) {
        throw InvalidStudentDataException("общежитие " + id_ + " не вмещает больше комнат");
    }
    room.setDormitory(this);
    rooms_.push_back(&room);
}

DormRoom* Dormitory::findFreeRoom(const std::string& gender) const {
    for (DormRoom* room : rooms_) {
        if (room == nullptr || room->getStatus() != "active" || !room->hasFreeBed()) continue;
        if (gender.empty() || gender == "mixed" || room->getGender() == "mixed" ||
            room->getGender() == gender) {
            return room;
        }
    }
    return nullptr;
}

double Dormitory::occupancyRate() const {
    int beds = 0;
    int occupied = 0;
    for (const DormRoom* room : rooms_) {
        beds += room->getCapacity();
        occupied += static_cast<int>(room->getResidents().size());
    }
    return beds == 0 ? 0.0 : 100.0 * static_cast<double>(occupied) / static_cast<double>(beds);
}

int Dormitory::freeBeds() const {
    int free = 0;
    for (const DormRoom* room : rooms_) free += room->freeBeds();
    return free;
}

void Dormitory::assignStudent(Student& student) {
    if (!student.isActive()) {
        throw InvalidStudentDataException("поселить можно только активного студента");
    }
    DormRoom* room = findFreeRoom(student.getGender());
    if (room == nullptr) throw DormitoryFullException("в общежитии " + id_ + " нет свободных мест");
    room->addResident(student);
}

bool Dormitory::evictStudent(const std::string& studentId) {
    for (DormRoom* room : rooms_) {
        if (room->removeResident(studentId)) return true;
    }
    return false;
}

double Dormitory::monthlyRevenue() const {
    int occupied = 0;
    for (const DormRoom* room : rooms_) {
        occupied += static_cast<int>(room->getResidents().size());
    }
    return static_cast<double>(occupied) * pricePerMonth_;
}

} // namespace studentis
