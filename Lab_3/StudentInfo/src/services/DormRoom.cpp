#include "studentis/services/DormRoom.h"

#include <string>
#include <vector>

#include "studentis/core/Student.h"
#include "studentis/exceptions/DormitoryFullException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"
#include "studentis/services/Dormitory.h"

namespace studentis {
namespace {

bool isKnownGender(const std::string& gender) {
    return gender == "male" || gender == "female" || gender == "mixed";
}

bool isKnownStatus(const std::string& status) {
    return status == "active" || status == "closed";
}

} // namespace

DormRoom::DormRoom(std::string number, Dormitory* dormitory, int capacity, int floor,
                   std::string gender, std::string status)
    : number_(std::move(number)),
      dormitory_(dormitory),
      capacity_(capacity),
      floor_(floor),
      gender_(std::move(gender)),
      status_(std::move(status)) {
    if (number_.empty()) throw InvalidStudentDataException("номер комнаты не может быть пустым");
    if (capacity_ <= 0) throw InvalidStudentDataException("вместимость комнаты должна быть положительной");
    if (floor_ < 1) throw InvalidStudentDataException("этаж комнаты должен быть положительным");
    if (!isKnownGender(gender_)) throw InvalidStudentDataException("неизвестный пол комнаты: " + gender_);
    if (!isKnownStatus(status_)) throw InvalidStudentDataException("неизвестный статус комнаты: " + status_);
}

void DormRoom::addResident(Student& student) {
    if (status_ != "active") throw InvalidStudentDataException("комната " + number_ + " закрыта");
    for (const Student* current : residents_) {
        if (current == &student || current->getStudentId() == student.getStudentId()) return;
    }
    if (isFull()) throw DormitoryFullException("в комнате " + number_ + " нет свободных мест");
    if (gender_ != "mixed" && gender_ != student.getGender()) {
        throw DormitoryFullException("комната " + number_ + " не предназначена для пола студента");
    }
    residents_.push_back(&student);
}

bool DormRoom::removeResident(const std::string& studentId) {
    for (auto it = residents_.begin(); it != residents_.end(); ++it) {
        if ((*it)->getStudentId() == studentId) {
            residents_.erase(it);
            return true;
        }
    }
    return false;
}

bool DormRoom::hasFreeBed() const {
    return freeBeds() > 0;
}

bool DormRoom::isFull() const {
    return static_cast<int>(residents_.size()) >= capacity_;
}

int DormRoom::freeBeds() const {
    const int free = capacity_ - static_cast<int>(residents_.size());
    return free > 0 ? free : 0;
}

bool DormRoom::containsStudent(const Student& student) const {
    for (const Student* current : residents_) {
        if (current == &student || current->getStudentId() == student.getStudentId()) return true;
    }
    return false;
}

double DormRoom::monthlyPrice() const {
    return dormitory_ == nullptr ? 0.0 : dormitory_->getPricePerMonth();
}

} // namespace studentis
