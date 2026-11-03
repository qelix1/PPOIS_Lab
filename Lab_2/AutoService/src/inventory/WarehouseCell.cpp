#include "autoservice/inventory/WarehouseCell.h"

#include "autoservice/exceptions/InsufficientStockException.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/inventory/Part.h"

namespace autoservice {

WarehouseCell::WarehouseCell() = default;

WarehouseCell::WarehouseCell(std::string code, std::string zone, int capacityUnits)
    : code_(std::move(code)), zone_(std::move(zone)), capacityUnits_(capacityUnits) {}

void WarehouseCell::put(Part& part, int qty) {
    if (qty <= 0) throw InvalidAmountException("кол-во в ячейку должно быть положительным");
    if (usedUnits_ + qty > capacityUnits_) throw WorkshopException("ячейка " + code_ + " переполнена");
    usedUnits_ += qty;
    for (Part* p : contents_) {
        if (p == &part) return;
    }
    contents_.push_back(&part);
    part.setCell(*this);
}

void WarehouseCell::take(Part& part, int qty) {
    if (qty <= 0) throw InvalidAmountException("кол-во из ячейки должно быть положительным");
    if (qty > usedUnits_) throw InsufficientStockException("в ячейке " + code_ + " недостаточно единиц");
    usedUnits_ -= qty;
    (void)part;
}

bool WarehouseCell::isFull() const {
    return usedUnits_ >= capacityUnits_;
}

double WarehouseCell::utilization() const {
    if (capacityUnits_ <= 0) return 0.0;
    return static_cast<double>(usedUnits_) / static_cast<double>(capacityUnits_);
}

} // namespace autoservice
