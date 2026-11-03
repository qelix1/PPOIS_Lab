#include "autoservice/orders/Estimate.h"

#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/InvalidOrderStatusException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/finance/Discount.h"
#include "autoservice/orders/EstimateLine.h"
#include "autoservice/staff/Manager.h"

namespace autoservice {

Estimate::Estimate() = default;

Estimate::Estimate(std::string number, std::string createdAt)
    : number_(std::move(number)), createdAt_(std::move(createdAt)) {}

void Estimate::addLine(EstimateLine& line) {
    for (EstimateLine* l : lines_) {
        if (l == &line) return;
    }
    lines_.push_back(&line);
    line.setEstimate(*this);
}

void Estimate::removeLine(const EstimateLine& line) {
    for (auto it = lines_.begin(); it != lines_.end(); ++it) {
        if (*it == &line) {
            lines_.erase(it);
            return;
        }
    }
}

void Estimate::recalcTotal() {
    int sum = 0;
    for (EstimateLine* l : lines_) {
        if (!l) continue;
        l->recalc();
        sum += l->getLineTotalRub();
    }
    if (discount_) sum = sum * (100 - discount_->getPercent()) / 100;
    totalRub_ = sum;
}

void Estimate::approve(Manager& manager) {
    if (status_ != "draft")
        throw InvalidOrderStatusException("утвердить можно только черновик сметы, статус: " + status_);
    status_ = "approved";
    approvedBy_ = &manager;
}

void Estimate::reject() {
    if (status_ != "draft")
        throw InvalidOrderStatusException("отклонить можно только черновик сметы, статус: " + status_);
    status_ = "rejected";
}

void Estimate::applyDiscount(const Discount& discount) {
    if (!discount.isValid(date::today()))
        throw WorkshopException("скидка " + discount.getCode() + " недействительна сегодня");
    discount_ = &discount;
    recalcTotal();
}

} // namespace autoservice
