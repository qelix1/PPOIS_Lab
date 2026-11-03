#include "autoservice/finance/Payment.h"

#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/InvalidOrderStatusException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/finance/CashRegister.h"
#include "autoservice/finance/Invoice.h"

namespace autoservice {

Payment::Payment() = default;

Payment::Payment(std::string id, int amountRub, std::string method, std::string paidAt)
    : id_(std::move(id)),
      amountRub_(amountRub),
      method_(std::move(method)),
      paidAt_(std::move(paidAt)) {}

void Payment::authorize() {
    if (amountRub_ <= 0) throw InvalidAmountException("сумма платежа должна быть положительной");
    if (!invoice_) throw WorkshopException("платёж не привязан к счёту");
    if (status_ != "draft")
        throw InvalidOrderStatusException("авторизовать можно только новый платёж, статус: " + status_);
    status_ = "authorized";
}

void Payment::confirm() {
    if (status_ != "authorized")
        throw InvalidOrderStatusException("провести можно только авторизованный платёж, статус: " + status_);
    status_ = "completed";
    invoice_->markPaid(amountRub_);
}

void Payment::cancel() {
    if (status_ == "completed")
        throw InvalidOrderStatusException("завершённый платёж нельзя отменить");
    status_ = "cancelled";
}

std::string Payment::receipt() const {
    return "Чек " + id_ + ": " + std::to_string(amountRub_) + " руб., " + method_ + " (" + status_ + ")";
}

bool Payment::isRefundable(const std::string& today) const {
    if (status_ != "completed") return false;
    return date::daysBetween(paidAt_, today) <= 30;
}

} // namespace autoservice
