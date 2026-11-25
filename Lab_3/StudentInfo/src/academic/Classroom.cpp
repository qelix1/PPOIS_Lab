#include "studentis/academic/Classroom.h"

#include "studentis/academic/Lesson.h"
#include "studentis/core/DateUtils.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

int minutesOf(const std::string& time) {
    return date::minutesOfDateTime("2000-01-01 " + time);
}

} // namespace

Classroom::Classroom(std::string id, std::string building, std::string roomNumber, int capacity,
                     std::string type, bool hasProjector, bool hasComputers)
    : id_(std::move(id)),
      building_(std::move(building)),
      roomNumber_(std::move(roomNumber)),
      capacity_(capacity),
      type_(std::move(type)),
      hasProjector_(hasProjector),
      hasComputers_(hasComputers) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор аудитории не может быть пустым");
    if (building_.empty()) throw InvalidStudentDataException("корпус аудитории не может быть пустым");
    if (roomNumber_.empty()) throw InvalidStudentDataException("номер аудитории не может быть пустым");
    if (capacity_ <= 0) throw InvalidStudentDataException("вместимость аудитории должна быть положительной");
    if (type_.empty()) throw InvalidStudentDataException("тип аудитории не может быть пустым");
}

bool Classroom::fits(const StudentGroup& group) const {
    return group.activeCount() <= capacity_;
}

bool Classroom::supports(const Lesson& lesson) const {
    if (lesson.isLecture()) return hasProjector_;
    if (lesson.getType() == "lab") return hasComputers_;
    return true;
}

bool Classroom::isAvailable(const std::string& date, const std::string& startTime,
                            const std::string& endTime) const {
    const int start = minutesOf(startTime);
    const int end = minutesOf(endTime);
    if (!date::isDate(date) || start < 0 || end < 0 || end <= start) return false;
    for (const Lesson* lesson : lessons_) {
        if (lesson->getDate() != date) continue;
        const int lessonStart = minutesOf(lesson->getStartTime());
        const int lessonEnd = minutesOf(lesson->getEndTime());
        if (start < lessonEnd && lessonStart < end) return false;
    }
    return true;
}

double Classroom::utilizationPercent(const StudentGroup& group) const {
    if (capacity_ <= 0) return 0.0;
    const double percent = 100.0 * group.activeCount() / capacity_;
    return percent > 100.0 ? 100.0 : percent;
}

bool Classroom::matchesType(const std::string& type) const {
    return type_ == type;
}

void Classroom::rename(const std::string& roomNumber) {
    if (roomNumber.empty()) throw InvalidStudentDataException("номер аудитории не может быть пустым");
    roomNumber_ = roomNumber;
}

void Classroom::book(Lesson& lesson) {
    for (Lesson* current : lessons_) {
        if (current == &lesson) return;
    }
    lessons_.push_back(&lesson);
}

void Classroom::unbook(const Lesson& lesson) {
    for (auto it = lessons_.begin(); it != lessons_.end(); ++it) {
        if (*it == &lesson) {
            lessons_.erase(it);
            return;
        }
    }
}

} // namespace studentis
