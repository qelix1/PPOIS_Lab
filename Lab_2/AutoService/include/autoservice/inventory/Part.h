#ifndef AUTOSERVICE_INVENTORY_PART_H
#define AUTOSERVICE_INVENTORY_PART_H

#include <string>
#include <vector>

namespace autoservice {

class Supplier;
class WarehouseCell;
class PartCompatibility;
class CarModel;

/**
 * @brief Запчасть на складе: остаток, минимум, цена, совместимость.
 */
class Part {
public:
    Part();
    Part(std::string sku, std::string name, int priceRub, int stockQty, int minStock, std::string unit);

    /// Зарезервировать количество под заказ; нехватка — InsufficientStockException.
    void reserve(int qty);
    /// Вернуть от reservations количество на свободный остаток.
    void release(int qty);
    /// Оприходовать поступление на склад.
    void restock(int qty);
    /// Остаток достиг или ниже минимального.
    bool isLowStock() const;
    /// Отпускная цена с наценкой.
    int sellPrice() const;
    /// Есть ли совместимость с моделью автомобиля.
    bool compatibleWith(const CarModel& model) const;

    const std::string& getSku() const { return sku_; }
    const std::string& getName() const { return name_; }
    int getPriceRub() const { return priceRub_; }
    int getStockQty() const { return stockQty_; }
    int getMinStock() const { return minStock_; }
    const std::string& getUnit() const { return unit_; }
    double getWeightKg() const { return weightKg_; }
    Supplier* getSupplier() const { return supplier_; }
    WarehouseCell* getCell() const { return cell_; }
    int getMarkupPercent() const { return markupPercent_; }
    const std::vector<PartCompatibility*>& getCompatibilities() const { return compatibilities_; }
    void setSupplier(Supplier& supplier) { supplier_ = &supplier; }
    void setCell(WarehouseCell& cell) { cell_ = &cell; }
    void setMarkupPercent(int percent) { markupPercent_ = percent; }
    void setWeightKg(double kg) { weightKg_ = kg; }
    void addCompatibility(PartCompatibility& compatibility) { compatibilities_.push_back(&compatibility); }

private:
    std::string sku_;
    std::string name_;
    int priceRub_ = 0;
    int stockQty_ = 0;
    int minStock_ = 0;
    std::string unit_;
    double weightKg_ = 0.0;
    Supplier* supplier_ = nullptr;
    WarehouseCell* cell_ = nullptr;
    int markupPercent_ = 20;
    std::vector<PartCompatibility*> compatibilities_;
};

} // namespace autoservice

#endif
