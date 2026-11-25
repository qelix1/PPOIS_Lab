#include "studentis/services/Event.h"

#include <string>
#include <vector>

#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

bool isKnownStatus(const std::string& status) {
    return status == "planned" || status == "ongoing" || status == "completed" ||
           status == "cancelled";
}

/// Минуты от условного нуля для строки «YYYY-MM-DD HH:MM»; ошибка → −1.
long absoluteMinutes(const std::string& dateTime) {
    const long day = date::dayNumber(dateTime);
    const int minutes = date::minutesOfDateTime(dateTime);
    if (day < 0 || minutes < 0) return -1;
    return day * 1440L + static_cast<long>(minutes);
}

} // namespace

Event::Event(std::string id, std::string title, std::string description, std::string startAt,
             std::string endAt, Teacher* organizer, Classroom* location, int maxParticipants,
             std::string status)
    : id_(std::move(id)),
      title_(std::move(title)),
      description_(std::move(description)),
      startAt_(std::move(startAt)),
      endAt_(std::move(endAt)),
      organizer_(organizer),
      location_(location),
      maxParticipants_(maxParticipants),
      status_(std::move(status)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор мероприятия не может быть пустым");
    if (title_.empty()) throw InvalidStudentDataException("название мероприятия не может быть пустым");
    const long start = absoluteMinutes(startAt_);
    const long end = absoluteMinutes(endAt_);
    if (start < 0 || end < 0) throw InvalidStudentDataException("некорректные дата и время мероприятия");
    if (end <= start) throw InvalidStudentDataException("окончание мероприятия должно быть позже начала");
    if (maxParticipants_ <= 0) {
        throw InvalidStudentDataException("число участников мероприятия должно быть положительным");
    }
    if (!isKnownStatus(status_)) throw InvalidStudentDataException("неизвестный статус мероприятия: " + status_);
}

bool Event::registerStudent(Student& student) {
    if (status_ == "cancelled") return false;
    if (!student.isActive()) return false;
    for (const Student* participant : participants_) {
        if (participant == &student || participant->getStudentId() == student.getStudentId()) return false;
    }
    if (isFull()) return false;
    participants_.push_back(&student);
    return true;
}

bool Event::unregister(const std::string& studentId) {
    bool found = false;
    for (auto it = participants_.begin(); it != participants_.end(); ++it) {
        if ((*it)->getStudentId() == studentId) {
            participants_.erase(it);
            found = true;
            break;
        }
    }
    for (auto it = attended_.begin(); it != attended_.end(); ++it) {
        if ((*it)->getStudentId() == studentId) {
            attended_.erase(it);
            break;
        }
    }
    return found;
}

void Event::markAttended(Student& student) {
    if (status_ == "cancelled") throw InvalidStudentDataException("мероприятие отменено");
    bool registered = false;
    for (const Student* participant : participants_) {
        if (participant == &student || participant->getStudentId() == student.getStudentId()) {
            registered = true;
            break;
        }
    }
    if (!registered) throw InvalidStudentDataException("студент не записан на мероприятие");
    for (const Student* visitor : attended_) {
        if (visitor == &student || visitor->getStudentId() == student.getStudentId()) return;
    }
    attended_.push_back(&student);
}

bool Event::isFull() const {
    return static_cast<int>(participants_.size()) >= maxParticipants_;
}

bool Event::isUpcoming(const std::string& date) const {
    if (status_ != "planned" || !date::isDate(date)) return false;
    return date::dayNumber(startAt_) >= date::dayNumber(date);
}

int Event::durationMinutes() const {
    const long start = absoluteMinutes(startAt_);
    const long end = absoluteMinutes(endAt_);
    return (start < 0 || end < 0) ? 0 : static_cast<int>(end - start);
}

double Event::attendancePercent() const {
    if (participants_.empty()) return 0.0;
    return 100.0 * static_cast<double>(attended_.size()) / static_cast<double>(participants_.size());
}

bool Event::conflictsWith(const Event& other) const {
    const long start = absoluteMinutes(startAt_);
    const long end = absoluteMinutes(endAt_);
    const long otherStart = absoluteMinutes(other.startAt_);
    const long otherEnd = absoluteMinutes(other.endAt_);
    if (start < 0 || end < 0 || otherStart < 0 || otherEnd < 0) return false;
    return start < otherEnd && otherStart < end;
}

void Event::cancel() {
    if (status_ == "completed") throw InvalidStudentDataException("завершённое мероприятие нельзя отменить");
    status_ = "cancelled";
}

} // namespace studentis
