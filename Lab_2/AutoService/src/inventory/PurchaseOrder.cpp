#include "autoservice/inventory/PurchaseOrder.h"

#include "autoservice/exceptions/InvalidOrderStatusException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/inventory/PurchaseOrderLine.h"

namespace autoservice {

PurchaseOrder::PurchaseOrder() = default;

PurchaseOrder::PurchaseOrder(std::string number, std::string createdAt, std::string expectedAt)
    : number_(std::move(number)), createdAt_(std::move(createdAt)), expectedAt_(std::move(expectedAt)) {}

void PurchaseOrder::addLine(PurchaseOrderLine& line) {
    for (PurchaseOrderLine* l : lines_) {
        if (l == &line) return;
    }
    lines_.push_back(&line);
    line.setOrder(*this);
}

void PurchaseOrder::removeLine(const PurchaseOrderLine& line) {
    for (auto it = lines_.begin(); it != lines_.end(); ++it) {
        if (*it == &line) {
            lines_.erase(it);
            return;
        }
    }
}

void PurchaseOrder::recalcTotal() {
    int sum = 0;
    for (PurchaseOrderLine* l : lines_) {
        if (!l) continue;
        l->recalc();
        sum += l->getLineTotalRub();
    }
    totalCostRub_ = sum;
}

void PurchaseOrder::submit() {
    if (status_ != "draft")
        throw InvalidOrderStatusException("отправить можно только черновик, статус: " + status_);
    if (!supplier_) throw WorkshopException("заказ поставщику без поставщика");
    if (lines_.empty()) throw WorkshopException("заказ поставщику без строк");
    status_ = "ordered";
}

void PurchaseOrder::receive() {
    if (status_ != "ordered")
        throw InvalidOrderStatusException("приять можно только отправленный заказ, статус: " + status_);
    for (PurchaseOrderLine* l : lines_) {
        if (l) l->receive(l->getQuantity());
    }
    status_ = "received";
}

void PurchaseOrder::cancel() {
    if (status_ == "received")
        throw InvalidOrderStatusException("принятую поставку нельзя отменить");
    status_ = "cancelled";
}

bool PurchaseOrder::isLate(const std::string& today) const {
    if (status_ == "received" || status_ == "cancelled") return false;
    return !expectedAt_.empty() && today > expectedAt_;
}

} // namespace autoservice
