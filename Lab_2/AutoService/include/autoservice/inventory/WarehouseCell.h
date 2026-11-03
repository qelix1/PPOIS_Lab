#ifndef AUTOSERVICE_INVENTORY_WAREHOUSE_CELL_H
#define AUTOSERVICE_INVENTORY_WAREHOUSE_CELL_H

#include <string>
#include <vector>

namespace autoservice {

class Part;

/**
 * @brief Ячейка склада (стеллаж/место): вместимость и занятость.
 */
class WarehouseCell {
public:
    WarehouseCell();
    WarehouseCell(std::string code, std::string zone, int capacityUnits);

    /// Принять запчасть в ячейку; переполнение — исключение.
    void put(Part& part, int qty);
    /// Выдать запчасть из ячейки; нехватка — InsufficientStockException.
    void take(Part& part, int qty);
    /// Ячейка заполнена полностью.
    bool isFull() const;
    /// Заполненность ячейки 0..1.
    double utilization() const;

    const std::string& getCode() const { return code_; }
    const std::string& getZone() const { return zone_; }
    int getCapacityUnits() const { return capacityUnits_; }
    int getUsedUnits() const { return usedUnits_; }
    int getReservedUnits() const { return reservedUnits_; }
    const std::vector<Part*>& getContents() const { return contents_; }

private:
    std::string code_;
    std::string zone_;
    int capacityUnits_ = 0;
    int usedUnits_ = 0;
    int reservedUnits_ = 0;
    std::vector<Part*> contents_;
};

} // namespace autoservice

#endif
