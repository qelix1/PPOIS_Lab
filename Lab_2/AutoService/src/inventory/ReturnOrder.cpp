#include "autoservice/inventory/ReturnOrder.h"

#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/InvalidOrderStatusException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/inventory/Part.h"

namespace autoservice {

ReturnOrder::ReturnOrder() = default;

ReturnOrder::ReturnOrder(std::string number, std::string createdAt, std::string reason)
    : number_(std::move(number)), createdAt_(std::move(createdAt)), reason_(std::move(reason)) {}

void ReturnOrder::addPart(Part& part) {
    if (!isReturnable(date::today()))
        throw WorkshopException("срок возврата поставщику истёк");
    for (Part* p : parts_) {
        if (p == &part) return;
    }
    parts_.push_back(&part);
}

void ReturnOrder::submit() {
    if (status_ != "draft")
        throw InvalidOrderStatusException("отправить можно только черновик возврата, статус: " + status_);
    if (!supplier_) throw WorkshopException("возврат без поставщика");
    if (parts_.empty()) throw WorkshopException("возврат без запчастей");
    status_ = "submitted";
}

bool ReturnOrder::isReturnable(const std::string& today) const {
    long age = date::daysBetween(createdAt_, today);
    return age >= 0 && age <= 14;
}

int ReturnOrder::refundAmount() const {
    int total = 0;
    for (const Part* p : parts_) {
        if (p) total += p->getPriceRub();
    }
    return total;
}

} // namespace autoservice
