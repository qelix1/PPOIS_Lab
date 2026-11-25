#include "studentis/finance/FinanceAccount.h"

#include <algorithm>

#include "studentis/core/DateUtils.h"
#include "studentis/core/Student.h"
#include "studentis/exceptions/InsufficientFundsException.h"
#include "studentis/exceptions/InvalidStudentDataException.h"
#include "studentis/exceptions/PaymentNotFoundException.h"
#include "studentis/finance/ScholarshipAward.h"
#include "studentis/finance/TuitionContract.h"
#include "studentis/finance/TuitionPayment.h"

namespace studentis {
namespace {

constexpr double kMoneyEpsilon = 0.005;

} // namespace

FinanceAccount::FinanceAccount(Student* student) : student_(student) {
    if (student_ == nullptr) throw InvalidStudentDataException("счёт должен принадлежать студенту");
}

void FinanceAccount::charge(double amount) {
    if (amount <= 0.0) throw InsufficientFundsException("сумма начисления должна быть положительной");
    balance_ -= amount;
}

void FinanceAccount::registerPayment(TuitionPayment& payment) {
    for (const TuitionPayment* current : transactions_) {
        if (current == &payment) return;
    }
    if (payment.getAmount() <= 0.0) {
        throw InsufficientFundsException("сумма платежа должна быть положительной");
    }
    if (!payment.isConfirmed()) {
        throw InvalidStudentDataException("зарегистрировать можно только подтверждённый платёж");
    }
    if (payment.getContract() == nullptr) {
        throw PaymentNotFoundException("платёж не привязан к договору");
    }
    if (payment.getContract()->getStudent() != student_) {
        throw InvalidStudentDataException("платёж относится к договору другого студента");
    }
    if (!canPay(payment.getAmount())) {
        throw InsufficientFundsException("сумма платежа превышает остаток долга");
    }
    balance_ += payment.getAmount();
    transactions_.push_back(&payment);
}

bool FinanceAccount::canPay(double amount) const {
    return amount > 0.0 && amount <= debt() + kMoneyEpsilon;
}

double FinanceAccount::debt() const {
    return std::max(0.0, -balance_);
}

std::vector<TuitionPayment*> FinanceAccount::historyBetween(const std::string& startDate,
                                                            const std::string& endDate) const {
    if (!date::isDate(startDate) || !date::isDate(endDate)) {
        throw InvalidStudentDataException("некорректный период истории платежей");
    }
    if (date::daysBetween(startDate, endDate) < 0) {
        throw InvalidStudentDataException("начало периода позже окончания");
    }
    std::vector<TuitionPayment*> result;
    for (TuitionPayment* payment : transactions_) {
        if (payment != nullptr && payment->getPaidAt() >= startDate &&
            payment->getPaidAt() <= endDate) {
            result.push_back(payment);
        }
    }
    return result;
}

bool FinanceAccount::hasDebt() const {
    return debt() > kMoneyEpsilon;
}

void FinanceAccount::applyScholarship(ScholarshipAward& award) {
    if (award.getStudent() != student_) {
        throw InvalidStudentDataException("стипендия назначена другому студенту");
    }
    if (!award.isPaid()) throw InvalidStudentDataException("зачислить можно только выплаченную стипендию");
    if (award.getAmount() <= 0.0) {
        throw InsufficientFundsException("сумма стипендии должна быть положительной");
    }
    balance_ += award.getAmount();
}

} // namespace studentis
