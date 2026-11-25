#include "studentis/academic/Lesson.h"

#include "studentis/academic/Classroom.h"
#include "studentis/academic/Course.h"
#include "studentis/core/DateUtils.h"
#include "studentis/core/StudentGroup.h"
#include "studentis/exceptions/ClassroomCapacityException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

int minutesOf(const std::string& time) {
    return date::minutesOfDateTime("2000-01-01 " + time);
}

bool isKnownType(const std::string& type) {
    return type == "lecture" || type == "practice" || type == "lab" || type == "seminar";
}

void validateInterval(const std::string& date, const std::string& startTime,
                      const std::string& endTime) {
    if (!date::isDate(date)) throw InvalidStudentDataException("некорректная дата занятия: " + date);
    const int start = minutesOf(startTime);
    const int end = minutesOf(endTime);
    if (start < 0 || end < 0) throw InvalidStudentDataException("некорректное время занятия");
    if (end <= start) throw InvalidStudentDataException("окончание занятия должно быть позже начала");
}

} // namespace

Lesson::Lesson(std::string id, Course* course, std::string date, std::string startTime,
               std::string endTime, std::string type, Classroom* classroom, std::string topic,
               bool conducted)
    : id_(std::move(id)),
      course_(course),
      date_(std::move(date)),
      startTime_(std::move(startTime)),
      endTime_(std::move(endTime)),
      type_(std::move(type)),
      classroom_(classroom),
      topic_(std::move(topic)),
      conducted_(conducted) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор занятия не может быть пустым");
    validateInterval(date_, startTime_, endTime_);
    if (!isKnownType(type_)) throw InvalidStudentDataException("неизвестный тип занятия: " + type_);
    if (classroom_ != nullptr) classroom_->book(*this);
}

int Lesson::durationMinutes() const {
    return minutesOf(endTime_) - minutesOf(startTime_);
}

bool Lesson::overlaps(const Lesson& other) const {
    if (date_ != other.date_) return false;
    const int start = minutesOf(startTime_);
    const int end = minutesOf(endTime_);
    const int otherStart = minutesOf(other.startTime_);
    const int otherEnd = minutesOf(other.endTime_);
    return start < otherEnd && otherStart < end;
}

void Lesson::moveTo(const std::string& date, const std::string& startTime,
                    const std::string& endTime) {
    validateInterval(date, startTime, endTime);
    date_ = date;
    startTime_ = startTime;
    endTime_ = endTime;
}

void Lesson::assignClassroom(Classroom& classroom) {
    if (course_ != nullptr && course_->getGroup() != nullptr && !classroom.fits(*course_->getGroup())) {
        throw ClassroomCapacityException("группа не помещается в аудиторию " + classroom.getRoomNumber());
    }
    if (!classroom.supports(*this)) {
        throw ClassroomCapacityException("аудитория " + classroom.getRoomNumber() +
                                         " не оборудована для занятия типа " + type_);
    }
    if (classroom_ == &classroom) return;
    if (classroom_ != nullptr) classroom_->unbook(*this);
    classroom_ = &classroom;
    classroom_->book(*this);
}

void Lesson::markConducted() {
    conducted_ = true;
}

bool Lesson::isLecture() const {
    return type_ == "lecture";
}

bool Lesson::isOn(const std::string& date) const {
    return date_ == date;
}

std::string Lesson::summary() const {
    return date_ + " " + startTime_ + "-" + endTime_ + ", " + type_ + ": " + topic_;
}

} // namespace studentis
