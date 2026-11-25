#include "studentis/documents/AcademicLeave.h"

#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

bool isKnownStatus(const std::string& status) {
    return status == "pending" || status == "approved" || status == "closed";
}

void validatePeriod(const std::string& startDate, const std::string& endDate) {
    if (!date::isDate(startDate) || !date::isDate(endDate)) {
        throw InvalidStudentDataException("некорректные даты академического отпуска");
    }
    if (date::daysBetween(startDate, endDate) < 0) {
        throw InvalidStudentDataException("окончание отпуска раньше начала");
    }
}

} // namespace

AcademicLeave::AcademicLeave(std::string id, Student* student, std::string startDate,
                             std::string endDate, std::string reason, std::string orderNumber,
                             std::string status, Teacher* approvedBy)
    : id_(std::move(id)),
      student_(student),
      startDate_(std::move(startDate)),
      endDate_(std::move(endDate)),
      reason_(std::move(reason)),
      orderNumber_(std::move(orderNumber)),
      approvedBy_(approvedBy) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор отпуска не может быть пустым");
    if (student_ == nullptr) throw InvalidStudentDataException("отпуск должен относиться к студенту");
    if (orderNumber_.empty()) throw InvalidStudentDataException("номер приказа не может быть пустым");
    validatePeriod(startDate_, endDate_);
    if (!isKnownStatus(status)) throw InvalidStudentDataException("неизвестный статус отпуска: " + status);
    status_ = status;
}

void AcademicLeave::approve(Teacher& teacher) {
    if (status_ != "pending") {
        throw InvalidStudentDataException("согласовать можно только ожидающий отпуск");
    }
    approvedBy_ = &teacher;
    status_ = "approved";
}

void AcademicLeave::extend(const std::string& endDate) {
    if (status_ != "approved") {
        throw InvalidStudentDataException("продлить можно только согласованный отпуск");
    }
    if (!date::isDate(endDate)) throw InvalidStudentDataException("некорректная дата окончания отпуска");
    if (date::daysBetween(endDate_, endDate) <= 0) {
        throw InvalidStudentDataException("новая дата окончания должна быть позже текущей");
    }
    endDate_ = endDate;
}

void AcademicLeave::close() {
    if (status_ != "approved") {
        throw InvalidStudentDataException("закрыть можно только согласованный отпуск");
    }
    status_ = "closed";
}

bool AcademicLeave::isActive(const std::string& date) const {
    if (status_ != "approved" || !date::isDate(date)) return false;
    return date::daysBetween(startDate_, date) >= 0 && date::daysBetween(date, endDate_) >= 0;
}

int AcademicLeave::durationDays() const {
    return static_cast<int>(date::daysBetween(startDate_, endDate_)) + 1;
}

bool AcademicLeave::overlaps(const AcademicLeave& other) const {
    return date::daysBetween(startDate_, other.endDate_) >= 0 &&
           date::daysBetween(other.startDate_, endDate_) >= 0;
}

} // namespace studentis
