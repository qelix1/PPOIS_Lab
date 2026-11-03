#include "autoservice/finance/Invoice.h"

#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/InvalidOrderStatusException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/finance/Discount.h"
#include "autoservice/finance/InvoiceLine.h"

namespace autoservice {

Invoice::Invoice() = default;

Invoice::Invoice(std::string number, std::string createdAt, std::string dueAt)
    : number_(std::move(number)), createdAt_(std::move(createdAt)), dueAt_(std::move(dueAt)) {}

void Invoice::addLine(InvoiceLine& line) {
    for (InvoiceLine* l : lines_) {
        if (l == &line) return;
    }
    lines_.push_back(&line);
    line.setInvoice(*this);
}

void Invoice::recalcTotal() {
    int sum = 0;
    for (InvoiceLine* l : lines_) {
        if (!l) continue;
        l->recalc();
        sum += l->getTotalRub();
    }
    if (discount_) sum = sum * (100 - discount_->getPercent()) / 100;
    totalRub_ = sum;
}

void Invoice::markPaid(int amountRub) {
    if (amountRub <= 0) throw InvalidAmountException("сумма оплаты должна быть положительной");
    if (status_ != "issued")
        throw InvalidOrderStatusException("оплатить можно только выставленный счёт, статус: " + status_);
    paidRub_ += amountRub;
    if (paidRub_ >= totalRub_) status_ = "paid";
}

bool Invoice::isOverdue(const std::string& today) const {
    return status_ == "issued" && !dueAt_.empty() && today > dueAt_;
}

void Invoice::applyDiscount(const Discount& discount) {
    if (!discount.isValid(date::today()))
        throw WorkshopException("скидка " + discount.getCode() + " недействительна сегодня");
    discount_ = &discount;
    recalcTotal();
}

int Invoice::balance() const {
    int debt = totalRub_ - paidRub_;
    return debt > 0 ? debt : 0;
}

void Invoice::issue() {
    if (status_ != "draft")
        throw InvalidOrderStatusException("выставить можно только черновик счёта, статус: " + status_);
    if (lines_.empty()) throw WorkshopException("нельзя выставить пустой счёт");
    recalcTotal();
    status_ = "issued";
}

} // namespace autoservice
