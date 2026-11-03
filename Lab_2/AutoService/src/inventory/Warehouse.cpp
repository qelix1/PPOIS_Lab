#include "autoservice/inventory/Warehouse.h"

#include "autoservice/inventory/InventoryAudit.h"
#include "autoservice/inventory/PurchaseOrder.h"
#include "autoservice/inventory/PurchaseOrderLine.h"
#include "autoservice/inventory/WarehouseCell.h"

namespace autoservice {

Warehouse::Warehouse() = default;

Warehouse::Warehouse(std::string id, std::string name, std::string address, int capacityUnits)
    : id_(std::move(id)), name_(std::move(name)), address_(std::move(address)), capacityUnits_(capacityUnits) {}

void Warehouse::addCell(WarehouseCell& cell) {
    for (WarehouseCell* c : cells_) {
        if (c == &cell) return;
    }
    cells_.push_back(&cell);
}

WarehouseCell* Warehouse::findCell(const std::string& code) const {
    for (WarehouseCell* c : cells_) {
        if (c && c->getCode() == code) return c;
    }
    return nullptr;
}

int Warehouse::freeSpace() const {
    int free = 0;
    for (const WarehouseCell* c : cells_) {
        if (c) free += c->getCapacityUnits() - c->getUsedUnits();
    }
    return free > 0 ? free : 0;
}

void Warehouse::receiveShipment(PurchaseOrder& order) {
    // Строки заказа сами оприходуют запчасти (PurchaseOrderLine::receive).
    order.receive();
}

void Warehouse::startAudit(InventoryAudit& audit) {
    audit.setWarehouse(*this);
}

} // namespace autoservice
