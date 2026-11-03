#include "autoservice/inventory/Part.h"

#include "autoservice/core/CarModel.h"
#include "autoservice/exceptions/InsufficientStockException.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/inventory/PartCompatibility.h"

namespace autoservice {

Part::Part() = default;

Part::Part(std::string sku, std::string name, int priceRub, int stockQty, int minStock, std::string unit)
    : sku_(std::move(sku)),
      name_(std::move(name)),
      priceRub_(priceRub),
      stockQty_(stockQty),
      minStock_(minStock),
      unit_(std::move(unit)) {}

void Part::reserve(int qty) {
    if (qty <= 0) throw InvalidAmountException("количество резерва должно быть положительным");
    if (qty > stockQty_) throw InsufficientStockException("запчасть " + sku_ + ": недостаточно на складе");
    stockQty_ -= qty;
}

void Part::release(int qty) {
    if (qty <= 0) throw InvalidAmountException("количество возврата должно быть положительным");
    stockQty_ += qty;
}

void Part::restock(int qty) {
    if (qty <= 0) throw InvalidAmountException("поступление должно быть положительным");
    stockQty_ += qty;
}

bool Part::isLowStock() const {
    return stockQty_ <= minStock_;
}

int Part::sellPrice() const {
    return priceRub_ * (100 + markupPercent_) / 100;
}

bool Part::compatibleWith(const CarModel& model) const {
    for (const PartCompatibility* c : compatibilities_) {
        if (c && c->fits(model)) return true;
    }
    return false;
}

} // namespace autoservice
