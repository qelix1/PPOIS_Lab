#include "studentis/finance/ScholarshipAward.h"

#include <cstdio>

#include "studentis/core/DateUtils.h"
#include "studentis/core/Semester.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/InsufficientFundsException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"
#include "studentis/finance/Scholarship.h"

namespace studentis {
namespace {

bool isKnownStatus(const std::string& status) {
    return status == "pending" || status == "approved" || status == "paid" ||
           status == "cancelled";
}

std::string formatAmount(double amount) {
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%.2f", amount);
    return std::string(buffer);
}

} // namespace

ScholarshipAward::ScholarshipAward(std::string id, Scholarship* scholarship, Student* student,
                                   Semester* semester, double amount, std::string awardedAt,
                                   std::string status, std::string orderNumber)
    : id_(std::move(id)),
      scholarship_(scholarship),
      student_(student),
      semester_(semester),
      amount_(amount),
      awardedAt_(std::move(awardedAt)),
      orderNumber_(std::move(orderNumber)) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор назначения не может быть пустым");
    if (scholarship_ == nullptr) {
        throw InvalidStudentDataException("назначение должно относиться к стипендии");
    }
    if (student_ == nullptr) {
        throw InvalidStudentDataException("назначение должно относиться к студенту");
    }
    if (semester_ == nullptr) {
        throw InvalidStudentDataException("назначение должно относиться к семестру");
    }
    if (amount_ <= 0.0) throw InsufficientFundsException("сумма стипендии должна быть положительной");
    if (!date::isDate(awardedAt_)) throw InvalidStudentDataException("некорректная дата назначения");
    if (!isKnownStatus(status)) {
        throw InvalidStudentDataException("неизвестный статус назначения: " + status);
    }
    status_ = status;
    if ((status_ == "approved" || status_ == "paid") && orderNumber_.empty()) {
        throw InvalidStudentDataException("для утверждённого назначения нужен номер приказа");
    }
}

void ScholarshipAward::approve(std::string orderNumber) {
    if (status_ != "pending") throw InvalidStudentDataException("утвердить можно только ожидающее назначение");
    if (orderNumber.empty()) throw InvalidStudentDataException("номер приказа не может быть пустым");
    orderNumber_ = std::move(orderNumber);
    status_ = "approved";
}

void ScholarshipAward::pay() {
    if (status_ != "approved") throw InvalidStudentDataException("выплатить можно только утверждённую стипендию");
    status_ = "paid";
}

void ScholarshipAward::cancel() {
    if (status_ == "paid") throw InvalidStudentDataException("выплаченную стипендию нельзя отменить");
    status_ = "cancelled";
}

bool ScholarshipAward::isPaid() const {
    return status_ == "paid";
}

bool ScholarshipAward::matchesSemester(const Semester& semester) const {
    return semester_ == &semester;
}

std::string ScholarshipAward::summary() const {
    return "назначение " + id_ + " (" + (scholarship_ != nullptr ? scholarship_->getName() : "—") +
           ") на " + formatAmount(amount_) + " руб., " + status_;
}

} // namespace studentis
