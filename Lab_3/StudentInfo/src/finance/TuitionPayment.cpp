#include "studentis/finance/TuitionPayment.h"

#include <cstdio>

#include "studentis/core/DateUtils.h"
#include "studentis/core/Teacher.h"
#include "studentis/exceptions/InsufficientFundsException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"
#include "studentis/finance/TuitionContract.h"

namespace studentis {
namespace {

bool isKnownStatus(const std::string& status) {
    return status == "pending" || status == "confirmed" || status == "cancelled";
}

std::string formatAmount(double amount) {
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%.2f", amount);
    return std::string(buffer);
}

} // namespace

TuitionPayment::TuitionPayment(std::string id, TuitionContract* contract, double amount,
                               std::string paidAt, std::string method, std::string receiptNumber,
                               Teacher* confirmedBy, std::string status)
    : id_(std::move(id)),
      contract_(contract),
      amount_(amount),
      paidAt_(std::move(paidAt)),
      method_(std::move(method)),
      receiptNumber_(std::move(receiptNumber)),
      confirmedBy_(confirmedBy) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор платежа не может быть пустым");
    if (contract_ == nullptr) throw InvalidStudentDataException("платёж должен относиться к договору");
    if (amount_ <= 0.0) throw InsufficientFundsException("сумма платежа должна быть положительной");
    if (!date::isDate(paidAt_)) throw InvalidStudentDataException("некорректная дата платежа: " + paidAt_);
    if (method_.empty()) throw InvalidStudentDataException("способ оплаты не указан");
    if (receiptNumber_.empty()) throw InvalidStudentDataException("номер квитанции не может быть пустым");
    if (!isKnownStatus(status)) throw InvalidStudentDataException("неизвестный статус платежа: " + status);
    status_ = status;
}

void TuitionPayment::confirm(Teacher& teacher) {
    if (status_ != "pending") throw InvalidStudentDataException("подтвердить можно только ожидающий платёж");
    confirmedBy_ = &teacher;
    status_ = "confirmed";
}

void TuitionPayment::cancel() {
    if (status_ == "confirmed") throw InvalidStudentDataException("подтверждённый платёж нельзя отменить");
    status_ = "cancelled";
}

bool TuitionPayment::isConfirmed() const {
    return status_ == "confirmed";
}

bool TuitionPayment::isLate() const {
    return contract_ != nullptr && paidAt_ > contract_->getStartDate();
}

bool TuitionPayment::matchesPeriod(const std::string& startDate, const std::string& endDate) const {
    if (!date::isDate(startDate) || !date::isDate(endDate)) {
        throw InvalidStudentDataException("некорректный период платежа");
    }
    if (date::daysBetween(startDate, endDate) < 0) {
        throw InvalidStudentDataException("начало периода позже окончания");
    }
    return paidAt_ >= startDate && paidAt_ <= endDate;
}

std::string TuitionPayment::receipt() const {
    return "Квитанция №" + receiptNumber_ + " от " + paidAt_ + " на сумму " + formatAmount(amount_) +
           " руб. (" + method_ + ")";
}

std::string TuitionPayment::summary() const {
    return "платёж " + id_ + " по договору " +
           (contract_ != nullptr ? contract_->getId() : std::string("—")) + ": " +
           formatAmount(amount_) + " руб., " + status_;
}

void TuitionPayment::setMethod(const std::string& method) {
    if (method.empty()) throw InvalidStudentDataException("способ оплаты не указан");
    method_ = method;
}

void TuitionPayment::setReceiptNumber(const std::string& receiptNumber) {
    if (receiptNumber.empty()) throw InvalidStudentDataException("номер квитанции не может быть пустым");
    receiptNumber_ = receiptNumber;
}

} // namespace studentis
