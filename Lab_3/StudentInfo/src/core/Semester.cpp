#include "studentis/core/Semester.h"

#include "studentis/core/AcademicYear.h"
#include "studentis/core/DateUtils.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {

Semester::Semester(int number, AcademicYear* year, std::string startDate, std::string endDate,
                   std::string examsStart, std::string examsEnd, std::string status)
    : number_(number),
      year_(year),
      startDate_(std::move(startDate)),
      endDate_(std::move(endDate)),
      examsStart_(std::move(examsStart)),
      examsEnd_(std::move(examsEnd)),
      status_(std::move(status)) {
    if (number_ < 1 || number_ > 12) throw InvalidStudentDataException("номер семестра должен быть от 1 до 12");
    if (!date::isDate(startDate_) || !date::isDate(endDate_)) {
        throw InvalidStudentDataException("некорректные даты семестра");
    }
    if (startDate_ > endDate_) throw InvalidStudentDataException("начало семестра позже окончания");
    if (!date::isDate(examsStart_) || !date::isDate(examsEnd_)) {
        throw InvalidStudentDataException("некорректные даты сессии");
    }
    if (examsStart_ > examsEnd_) throw InvalidStudentDataException("начало сессии позже окончания");
}

bool Semester::containsDate(const std::string& date) const {
    if (!date::isDate(date)) return false;
    return startDate_ <= date && date <= endDate_;
}

bool Semester::isExamPeriod(const std::string& date) const {
    if (!date::isDate(date)) return false;
    return examsStart_ <= date && date <= examsEnd_;
}

int Semester::weeksCount() const {
    return static_cast<int>((date::daysBetween(startDate_, endDate_) + 1) / 7);
}

bool Semester::canEditGrades() const {
    return status_ == "active" && !isExamPeriod(date::today());
}

void Semester::close() {
    status_ = "closed";
}

bool Semester::isActive() const {
    return status_ == "active";
}

long Semester::daysUntilExams(const std::string& date) const {
    if (!date::isDate(date)) throw InvalidStudentDataException("некорректная дата: " + date);
    long days = date::daysBetween(date, examsStart_);
    return days > 0 ? days : 0;
}

} // namespace studentis
