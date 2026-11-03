#include "autoservice/orders/EstimateLine.h"

#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/inventory/Part.h"
#include "autoservice/orders/Service.h"

namespace autoservice {

EstimateLine::EstimateLine() = default;

EstimateLine::EstimateLine(std::string description, int quantity, int unitPriceRub)
    : description_(std::move(description)), quantity_(quantity), unitPriceRub_(unitPriceRub) {}

void EstimateLine::recalc() {
    if (quantity_ <= 0) throw InvalidAmountException("количество в строке сметы должно быть положительным");
    if (unitPriceRub_ < 0) throw InvalidAmountException("цена в строке сметы не может быть отрицательной");
    lineTotalRub_ = quantity_ * unitPriceRub_;
}

bool EstimateLine::isPartsLine() const {
    return part_ != nullptr;
}

bool EstimateLine::isLaborLine() const {
    return service_ != nullptr && part_ == nullptr;
}

std::string EstimateLine::describe() const {
    return description_ + " x" + std::to_string(quantity_) + " = " + std::to_string(lineTotalRub_) + " руб.";
}

} // namespace autoservice
