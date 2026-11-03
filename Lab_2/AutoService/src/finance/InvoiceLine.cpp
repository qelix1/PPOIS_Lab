#include "autoservice/finance/InvoiceLine.h"

#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/inventory/Part.h"
#include "autoservice/orders/WorkItem.h"

namespace autoservice {

InvoiceLine::InvoiceLine() = default;

InvoiceLine::InvoiceLine(std::string description, int quantity, int unitPriceRub)
    : description_(std::move(description)), quantity_(quantity), unitPriceRub_(unitPriceRub) {}

void InvoiceLine::recalc() {
    if (quantity_ <= 0) throw InvalidAmountException("количество в строке счёта должно быть положительным");
    if (unitPriceRub_ < 0) throw InvalidAmountException("цена в строке счёта не может быть отрицательной");
    totalRub_ = quantity_ * unitPriceRub_;
}

bool InvoiceLine::isPartsLine() const {
    return part_ != nullptr;
}

bool InvoiceLine::isLaborLine() const {
    return work_ != nullptr && part_ == nullptr;
}

std::string InvoiceLine::describe() const {
    return description_ + " x" + std::to_string(quantity_) + " = " + std::to_string(totalRub_) + " руб.";
}

} // namespace autoservice
