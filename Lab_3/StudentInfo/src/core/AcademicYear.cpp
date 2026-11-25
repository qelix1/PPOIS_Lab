#include "studentis/core/AcademicYear.h"

#include "studentis/core/DateUtils.h"
#include "studentis/core/Semester.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

AcademicYear::AcademicYear(std::string name, std::string startDate, std::string endDate,
                           bool current)
    : name_(std::move(name)),
      startDate_(std::move(startDate)),
      endDate_(std::move(endDate)),
      current_(current) {
    if (name_.empty()) throw InvalidStudentDataException("название учебного года не может быть пустым");
    if (!date::isDate(startDate_) || !date::isDate(endDate_)) {
        throw InvalidStudentDataException("некорректные даты учебного года");
    }
    if (startDate_ > endDate_) throw InvalidStudentDataException("начало учебного года позже окончания");
}

void AcademicYear::addSemester(Semester& semester) {
    for (Semester* current : semesters_) {
        if (current == &semester) return;
    }
    semesters_.push_back(&semester);
    semester.setYear(this);
}

bool AcademicYear::containsDate(const std::string& date) const {
    if (!date::isDate(date)) return false;
    return startDate_ <= date && date <= endDate_;
}

Semester* AcademicYear::findSemester(int number) const {
    for (Semester* current : semesters_) {
        if (current->getNumber() == number) return current;
    }
    return nullptr;
}

bool AcademicYear::isCurrent() const {
    return current_;
}

long AcademicYear::durationDays() const {
    return date::daysBetween(startDate_, endDate_) + 1;
}

void AcademicYear::close() {
    current_ = false;
    for (Semester* semester : semesters_) semester->close();
}

} // namespace studentis
