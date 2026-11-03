#include "autoservice/finance/FinancialTransaction.h"

#include "autoservice/exceptions/InvalidAmountException.h"

namespace autoservice {

FinancialTransaction::FinancialTransaction() = default;

FinancialTransaction::FinancialTransaction(std::string id, std::string type, int amountRub, std::string createdAt)
    : id_(std::move(id)), type_(std::move(type)), amountRub_(amountRub), createdAt_(std::move(createdAt)) {
    if (amountRub_ <= 0) throw InvalidAmountException("сумма операции должна быть положительной");
}

bool FinancialTransaction::isIncoming() const {
    return type_ == "income";
}

void FinancialTransaction::reverse(const std::string& reason) {
    type_ = isIncoming() ? "expense" : "income";
    if (comment_.empty()) {
        comment_ = "реверс: " + reason;
    } else {
        comment_ += "; реверс: " + reason;
    }
}

std::string FinancialTransaction::describe() const {
    return id_ + " " + type_ + " " + std::to_string(amountRub_) + " руб." +
           (comment_.empty() ? "" : " (" + comment_ + ")");
}

int FinancialTransaction::signedAmount() const {
    return isIncoming() ? amountRub_ : -amountRub_;
}

} // namespace autoservice
