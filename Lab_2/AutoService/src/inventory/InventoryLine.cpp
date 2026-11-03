#include "autoservice/inventory/InventoryLine.h"

#include "autoservice/inventory/Part.h"

namespace autoservice {

InventoryLine::InventoryLine() = default;

InventoryLine::InventoryLine(Part* part, int expectedQty, int actualQty)
    : part_(part), expectedQty_(expectedQty), actualQty_(actualQty) {}

int InventoryLine::diff() const {
    return actualQty_ - expectedQty_;
}

bool InventoryLine::isMatch() const {
    return diff() == 0;
}

std::string InventoryLine::describe() const {
    std::string sku = part_ ? part_->getSku() : "?";
    return sku + ": ожидалось " + std::to_string(expectedQty_) + ", найдено " + std::to_string(actualQty_);
}

} // namespace autoservice
