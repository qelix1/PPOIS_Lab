#include "studentis/finance/TuitionContract.h"

#include "studentis/core/DateUtils.h"
#include "studentis/core/Specialty.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/InsufficientFundsException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

bool isKnownStatus(const std::string& status) {
    return status == "draft" || status == "active" || status == "terminated";
}

} // namespace

TuitionContract::TuitionContract(std::string id, Student* student, Specialty* specialty,
                                 std::string signedAt, std::string startDate, std::string endDate,
                                 double annualAmount, double discountPercent, std::string status)
    : id_(std::move(id)),
      student_(student),
      specialty_(specialty),
      signedAt_(std::move(signedAt)),
      startDate_(std::move(startDate)),
      endDate_(std::move(endDate)),
      annualAmount_(annualAmount),
      discountPercent_(discountPercent) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор договора не может быть пустым");
    if (student_ == nullptr) throw InvalidStudentDataException("договор должен относиться к студенту");
    if (specialty_ == nullptr) {
        throw InvalidStudentDataException("договор должен относиться к специальности");
    }
    if (!date::isDate(signedAt_)) throw InvalidStudentDataException("некорректная дата подписания договора");
    if (!validateDates()) throw InvalidStudentDataException("некорректные даты договора");
    setAnnualAmount(annualAmount);
    if (discountPercent_ < 0.0 || discountPercent_ > 100.0) {
        throw InvalidStudentDataException("скидка должна быть от 0 до 100 процентов");
    }
    setStatus(status);
}

double TuitionContract::yearlyTotal() const {
    return annualAmount_ * (1.0 - discountPercent_ / 100.0);
}

double TuitionContract::semesterAmount() const {
    return yearlyTotal() / 2.0;
}

bool TuitionContract::isActive() const {
    return status_ == "active";
}

void TuitionContract::applyDiscount(double percent) {
    if (percent < 0.0 || percent > 100.0) {
        throw InvalidStudentDataException("скидка должна быть от 0 до 100 процентов");
    }
    discountPercent_ = percent;
}

void TuitionContract::terminate() {
    status_ = "terminated";
}

long TuitionContract::daysUntilExpiry(const std::string& date) const {
    if (!date::isDate(date)) throw InvalidStudentDataException("некорректная дата: " + date);
    return date::daysBetween(date, endDate_);
}

double TuitionContract::remainingAmount(const std::string& date) const {
    if (!date::isDate(date)) throw InvalidStudentDataException("некорректная дата: " + date);
    if (status_ == "terminated" || date::daysBetween(date, endDate_) < 0) return 0.0;
    if (date::daysBetween(date, startDate_) >= 0) return yearlyTotal();
    const long totalDays = date::daysBetween(startDate_, endDate_);
    if (totalDays <= 0) return yearlyTotal();
    return yearlyTotal() * static_cast<double>(date::daysBetween(date, endDate_)) /
           static_cast<double>(totalDays);
}

bool TuitionContract::validateDates() const {
    return date::isDate(signedAt_) && date::isDate(startDate_) && date::isDate(endDate_) &&
           date::daysBetween(startDate_, endDate_) >= 0;
}

void TuitionContract::setSignedAt(const std::string& signedAt) {
    if (!date::isDate(signedAt)) throw InvalidStudentDataException("некорректная дата подписания договора");
    signedAt_ = signedAt;
}

void TuitionContract::setStartDate(const std::string& startDate) {
    if (!date::isDate(startDate)) throw InvalidStudentDataException("некорректная дата начала договора");
    if (date::isDate(endDate_) && date::daysBetween(startDate, endDate_) < 0) {
        throw InvalidStudentDataException("начало договора позже окончания");
    }
    startDate_ = startDate;
}

void TuitionContract::setEndDate(const std::string& endDate) {
    if (!date::isDate(endDate)) throw InvalidStudentDataException("некорректная дата окончания договора");
    if (date::isDate(startDate_) && date::daysBetween(startDate_, endDate) < 0) {
        throw InvalidStudentDataException("окончание договора раньше начала");
    }
    endDate_ = endDate;
}

void TuitionContract::setAnnualAmount(double annualAmount) {
    if (annualAmount <= 0.0) {
        throw InsufficientFundsException("годовая стоимость обучения должна быть положительной");
    }
    annualAmount_ = annualAmount;
}

void TuitionContract::setStatus(const std::string& status) {
    if (!isKnownStatus(status)) throw InvalidStudentDataException("неизвестный статус договора: " + status);
    status_ = status;
}

} // namespace studentis
