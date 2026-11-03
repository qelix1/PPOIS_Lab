#ifndef AUTOSERVICE_INVENTORY_WAREHOUSE_H
#define AUTOSERVICE_INVENTORY_WAREHOUSE_H

#include <string>
#include <vector>

namespace autoservice {

class WarehouseCell;
class InventoryAudit;
class Employee;
class PurchaseOrder;
class Part;

/**
 * @brief Склад запчастей: ячейки, вместимость, приёмка поставок.
 */
class Warehouse {
public:
    Warehouse();
    Warehouse(std::string id, std::string name, std::string address, int capacityUnits);

    /// Добавить ячейку хранения.
    void addCell(WarehouseCell& cell);
    /// Найти ячейку по коду (nullptr — не найдена).
    WarehouseCell* findCell(const std::string& code) const;
    /// Свободная вместимость по всем ячейкам.
    int freeSpace() const;
    /// Принять поставку: оприходовать все строки заказа.
    void receiveShipment(PurchaseOrder& order);
    /// Открыть инвентаризацию на складе.
    void startAudit(InventoryAudit& audit);

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getAddress() const { return address_; }
    int getCapacityUnits() const { return capacityUnits_; }
    const std::vector<WarehouseCell*>& getCells() const { return cells_; }
    Employee* getSupervisor() const { return supervisor_; }
    void setSupervisor(Employee& supervisor) { supervisor_ = &supervisor; }

private:
    std::string id_;
    std::string name_;
    std::string address_;
    std::vector<WarehouseCell*> cells_;
    int capacityUnits_ = 0;
    Employee* supervisor_ = nullptr;
};

} // namespace autoservice

#endif
