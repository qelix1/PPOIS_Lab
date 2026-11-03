#include "autoservice/finance/Refund.h"

#include "autoservice/exceptions/InvalidOrderStatusException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/finance/CashRegister.h"
#include "autoservice/finance/Payment.h"
#include "autoservice/staff/Manager.h"

namespace autoservice {

Refund::Refund() = default;

Refund::Refund(std::string id, int amountRub, std::string reason, std::string createdAt)
    : id_(std::move(id)), amountRub_(amountRub), reason_(std::move(reason)), createdAt_(std::move(createdAt)) {}

void Refund::approve(Manager& manager) {
    if (amountRub_ <= 0) throw WorkshopException("сумма возврата должна быть положительной");
    if (status_ != "draft")
        throw InvalidOrderStatusException("согласовать можно только новый возврат, статус: " + status_);
    status_ = "approved";
    approvedBy_ = &manager;
}

void Refund::reject(const std::string& reason) {
    if (status_ != "draft")
        throw InvalidOrderStatusException("отклонить можно только новый возврат, статус: " + status_);
    status_ = "rejected";
    reason_ = reason;
}

bool Refund::isPending() const {
    return status_ == "draft";
}

void Refund::execute(CashRegister& cashier) {
    if (status_ != "approved")
        throw InvalidOrderStatusException("исполнить можно только согласованный возврат, статус: " + status_);
    cashier.withdraw(amountRub_);
    status_ = "executed";
}

} // namespace autoservice
