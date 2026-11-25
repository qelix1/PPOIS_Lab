#include "studentis/finance/StudentFine.h"

#include <algorithm>
#include <cstdio>

#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/InsufficientFundsException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"

namespace studentis {
namespace {

constexpr double kMoneyEpsilon = 0.005;

bool isKnownStatus(const std::string& status) {
    return status == "issued" || status == "paid" || status == "waived";
}

std::string formatAmount(double amount) {
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%.2f", amount);
    return std::string(buffer);
}

} // namespace

StudentFine::StudentFine(std::string id, Student* student, std::string reason, double amount,
                         std::string issuedAt, std::string dueDate, double paidAmount,
                         std::string status)
    : id_(std::move(id)),
      student_(student),
      reason_(std::move(reason)),
      amount_(amount),
      issuedAt_(std::move(issuedAt)),
      dueDate_(std::move(dueDate)),
      paidAmount_(paidAmount) {
    if (id_.empty()) throw InvalidStudentDataException("идентификатор штрафа не может быть пустым");
    if (student_ == nullptr) throw InvalidStudentDataException("штраф должен относиться к студенту");
    if (reason_.empty()) throw InvalidStudentDataException("причина штрафа не указана");
    if (amount_ <= 0.0) throw InsufficientFundsException("сумма штрафа должна быть положительной");
    if (!date::isDate(issuedAt_)) throw InvalidStudentDataException("некорректная дата выставления штрафа");
    if (!date::isDate(dueDate_)) throw InvalidStudentDataException("некорректный срок оплаты штрафа");
    if (date::daysBetween(issuedAt_, dueDate_) < 0) {
        throw InvalidStudentDataException("срок оплаты раньше даты выставления штрафа");
    }
    if (paidAmount_ < 0.0 || paidAmount_ > amount_ + kMoneyEpsilon) {
        throw InsufficientFundsException("оплаченная часть штрафа должна быть от 0 до суммы штрафа");
    }
    if (paidAmount_ > amount_) paidAmount_ = amount_;
    if (!isKnownStatus(status)) throw InvalidStudentDataException("неизвестный статус штрафа: " + status);
    status_ = status;
    if (status_ == "paid" && remaining() > kMoneyEpsilon) {
        throw InvalidStudentDataException("статус paid требует полной оплаты штрафа");
    }
    if (remaining() <= kMoneyEpsilon && status_ != "waived") status_ = "paid";
}

void StudentFine::addPayment(double amount) {
    if (status_ == "waived") throw InvalidStudentDataException("списанный штраф нельзя оплачивать");
    if (amount <= 0.0) throw InsufficientFundsException("сумма платежа должна быть положительной");
    if (amount > remaining() + kMoneyEpsilon) {
        throw InsufficientFundsException("переплата по штрафу: остаток " + formatAmount(remaining()) +
                                         " руб.");
    }
    paidAmount_ += amount;
    if (remaining() <= kMoneyEpsilon) {
        paidAmount_ = amount_;
        status_ = "paid";
    }
}

double StudentFine::remaining() const {
    return std::max(0.0, amount_ - paidAmount_);
}

bool StudentFine::isOverdue(const std::string& date) const {
    if (!date::isDate(date)) throw InvalidStudentDataException("некорректная дата: " + date);
    if (status_ == "waived" || isPaid()) return false;
    return date::daysBetween(dueDate_, date) > 0;
}

bool StudentFine::isPaid() const {
    return remaining() <= kMoneyEpsilon;
}

void StudentFine::waive() {
    if (isPaid()) throw InvalidStudentDataException("погашенный штраф нельзя списать");
    status_ = "waived";
}

std::string StudentFine::summary() const {
    return "штраф " + id_ + ": " + reason_ + ", " + formatAmount(amount_) + " руб., остаток " +
           formatAmount(remaining()) + " руб., " + status_;
}

} // namespace studentis
