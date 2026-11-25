#include "studentis/documents/Diploma.h"

#include <cstddef>

#include "studentis/core/DateUtils.h"
#include "studentis/core/Specialty.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/InvalidGradeException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"
#include "studentis/exceptions/StudentNotFoundException.h"

namespace studentis {
namespace {

bool allDigits(const std::string& value, std::size_t length) {
    if (value.size() != length) return false;
    for (char symbol : value) {
        if (symbol < '0' || symbol > '9') return false;
    }
    return true;
}

bool validNumberFormat(const std::string& number, const std::string& series) {
    return allDigits(number, 6) && allDigits(series, 4);
}

} // namespace

Diploma::Diploma(std::string id, Student* student, Specialty* specialty, std::string number,
                 std::string series, std::string issueDate, double finalAverage, bool honors,
                 std::string thesisTopic, int thesisScore)
    : id_(std::move(id)),
      student_(student),
      specialty_(specialty),
      number_(std::move(number)),
      series_(std::move(series)),
      issueDate_(std::move(issueDate)),
      finalAverage_(finalAverage),
      honors_(honors),
      thesisTopic_(std::move(thesisTopic)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор диплома не может быть пустым");
    if (student_ == nullptr) throw InvalidStudentDataException("диплом должен относиться к студенту");
    if (finalAverage_ < 0.0 || finalAverage_ > 100.0) {
        throw InvalidGradeException("средний балл диплома должен быть от 0 до 100");
    }
    if (!issueDate_.empty() && !date::isDate(issueDate_)) {
        throw InvalidStudentDataException("некорректная дата выдачи диплома");
    }
    if (!number_.empty() || !series_.empty()) {
        if (!validNumberFormat(number_, series_)) {
            throw InvalidStudentDataException("номер диплома — 6 цифр, серия — 4 цифры");
        }
    }
    setThesisScore(thesisScore);
}

bool Diploma::isHonors() const {
    return honors_ && finalAverage_ >= 85.0;
}

std::string Diploma::qualification() const {
    return specialty_ != nullptr ? specialty_->getQualification() : std::string();
}

bool Diploma::isIssued() const {
    return !number_.empty() && !issueDate_.empty();
}

void Diploma::registerIssue(const std::string& issueDate, const std::string& number,
                            const std::string& series) {
    if (student_ == nullptr) throw StudentNotFoundException("студент диплома не найден");
    if (specialty_ == nullptr) throw InvalidStudentDataException("специальность диплома не указана");
    if (!date::isDate(issueDate)) throw InvalidStudentDataException("некорректная дата выдачи диплома");
    if (!validNumberFormat(number, series)) {
        throw InvalidStudentDataException("номер диплома — 6 цифр, серия — 4 цифры");
    }
    issueDate_ = issueDate;
    number_ = number;
    series_ = series;
}

std::string Diploma::summary() const {
    std::string result = "Диплом " + series_ + " " + number_;
    result += student_ != nullptr ? " выдан " + student_->shortName() : "";
    result += specialty_ != nullptr ? ", специальность " + specialty_->getName() : "";
    result += issueDate_.empty() ? ", не выдан" : ", дата выдачи " + issueDate_;
    if (isHonors()) result += " (с отличием)";
    return result;
}

bool Diploma::validateNumber() const {
    return validNumberFormat(number_, series_);
}

bool Diploma::matchesSpecialty(const Specialty& specialty) const {
    return specialty_ != nullptr && specialty_->matchesCode(specialty.getCode());
}

void Diploma::setThesisScore(int thesisScore) {
    if (thesisScore < 0 || thesisScore > 100) {
        throw InvalidGradeException("оценка за дипломную работу должна быть от 0 до 100");
    }
    thesisScore_ = thesisScore;
}

} // namespace studentis
