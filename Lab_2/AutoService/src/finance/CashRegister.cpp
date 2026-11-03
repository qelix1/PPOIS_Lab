#include "autoservice/finance/CashRegister.h"

#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/PaymentFailedException.h"
#include "autoservice/finance/FinancialTransaction.h"

namespace autoservice {

CashRegister::CashRegister() = default;

CashRegister::CashRegister(std::string id, std::string cashierName)
    : id_(std::move(id)), cashierName_(std::move(cashierName)) {}

void CashRegister::open(const std::string& date) {
    if (isOpen()) throw PaymentFailedException("касса уже открыта");
    openedAt_ = date;
    closedAt_.clear();
}

void CashRegister::close(const std::string& date) {
    if (!isOpen()) throw PaymentFailedException("касса не открыта");
    closedAt_ = date;
}

void CashRegister::deposit(int rub) {
    if (rub <= 0) throw InvalidAmountException("сумма прихода должна быть положительной");
    if (!isOpen()) throw PaymentFailedException("касса закрыта");
    balanceRub_ += rub;
}

void CashRegister::withdraw(int rub) {
    if (rub <= 0) throw InvalidAmountException("сумма расхода должна быть положительной");
    if (!isOpen()) throw PaymentFailedException("касса закрыта");
    if (rub > balanceRub_) throw PaymentFailedException("недостаточно средств в кассе");
    balanceRub_ -= rub;
}

void CashRegister::recordTransaction(FinancialTransaction& transaction) {
    transaction.setRegister(*this);
    for (FinancialTransaction* t : transactions_) {
        if (t == &transaction) return;
    }
    transactions_.push_back(&transaction);
}

int CashRegister::dayTotal() const {
    int total = 0;
    for (const FinancialTransaction* t : transactions_) {
        if (t) total += t->signedAmount();
    }
    return total;
}

} // namespace autoservice
