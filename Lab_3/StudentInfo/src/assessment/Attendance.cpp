#include "studentis/assessment/Attendance.h"

#include "studentis/academic/Lesson.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/AttendanceException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

bool isKnownStatus(const std::string& status) {
    return status == "present" || status == "absent" || status == "excused" || status == "late";
}

} // namespace

Attendance::Attendance(std::string id, Lesson* lesson, Student* student, std::string status,
                       int minutesLate, std::string reason, Teacher* markedBy)
    : id_(std::move(id)),
      lesson_(lesson),
      student_(student),
      status_(std::move(status)),
      minutesLate_(minutesLate),
      reason_(std::move(reason)),
      markedBy_(markedBy) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор отметки не может быть пустым");
    if (lesson_ == nullptr) throw AttendanceException("отметка должна относиться к занятию");
    if (student_ == nullptr) throw AttendanceException("отметка должна принадлежать студенту");
    if (!isKnownStatus(status_)) throw AttendanceException("неизвестный статус посещаемости: " + status_);
    if (minutesLate_ < 0) throw AttendanceException("опоздание не может быть отрицательным");
    if (status_ != "late") minutesLate_ = 0;
}

bool Attendance::isPresent() const {
    return status_ == "present" || status_ == "late";
}

bool Attendance::isAbsent() const {
    return status_ == "absent";
}

bool Attendance::isExcused() const {
    return status_ == "excused";
}

void Attendance::markExcused(const std::string& reason) {
    if (reason.empty()) throw AttendanceException("уважительная причина не указана");
    reason_ = reason;
    status_ = "excused";
    minutesLate_ = 0;
}

int Attendance::latePenalty() const {
    if (status_ == "late") return minutesLate_ > 100 ? 100 : minutesLate_;
    if (status_ == "absent") return 100;
    return 0;
}

std::string Attendance::summary() const {
    const std::string name = student_ != nullptr ? student_->shortName() : "—";
    std::string text = "посещаемость " + id_ + ": " + name + " — " + status_;
    if (status_ == "late") text += " на " + std::to_string(minutesLate_) + " мин";
    if (!reason_.empty()) text += ", причина: " + reason_;
    return text;
}

} // namespace studentis
