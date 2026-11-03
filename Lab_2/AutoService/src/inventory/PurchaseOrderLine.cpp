#include "autoservice/inventory/PurchaseOrderLine.h"

#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/inventory/Part.h"

namespace autoservice {

PurchaseOrderLine::PurchaseOrderLine() = default;

PurchaseOrderLine::PurchaseOrderLine(Part* part, int quantity, int unitCostRub)
    : part_(part), quantity_(quantity), unitCostRub_(unitCostRub) {}

void PurchaseOrderLine::recalc() {
    if (quantity_ <= 0) throw InvalidAmountException("количество в заказе должно быть положительным");
    if (unitCostRub_ < 0) throw InvalidAmountException("цена закупки не может быть отрицательной");
    lineTotalRub_ = quantity_ * unitCostRub_;
}

void PurchaseOrderLine::receive(int qty) {
    if (qty <= 0) throw InvalidAmountException("приёмка должна быть положительной");
    if (receivedQty_ + qty > quantity_)
        throw WorkshopException("принято больше, чем заказано по строке");
    receivedQty_ += qty;
    if (part_) part_->restock(qty);
}

bool PurchaseOrderLine::isFullyReceived() const {
    return quantity_ > 0 && receivedQty_ >= quantity_;
}

int PurchaseOrderLine::remaining() const {
    int left = quantity_ - receivedQty_;
    return left > 0 ? left : 0;
}

std::string PurchaseOrderLine::shortReport() const {
    std::string sku = part_ ? part_->getSku() : "?";
    return sku + ": " + std::to_string(receivedQty_) + "/" + std::to_string(quantity_);
}

} // namespace autoservice
