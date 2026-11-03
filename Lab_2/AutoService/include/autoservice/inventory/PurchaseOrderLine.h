#ifndef AUTOSERVICE_INVENTORY_PURCHASE_ORDER_LINE_H
#define AUTOSERVICE_INVENTORY_PURCHASE_ORDER_LINE_H

#include <string>

namespace autoservice {

class Part;
class PurchaseOrder;

/**
 * @brief Строка заказа поставщику: запчасть, количество, цена закупки.
 */
class PurchaseOrderLine {
public:
    PurchaseOrderLine();
    PurchaseOrderLine(Part* part, int quantity, int unitCostRub);

    /// Пересчитать сумму строки.
    void recalc();
    /// Оприходовать поступившее количество.
    void receive(int qty);
    /// Позиция полностью получена.
    bool isFullyReceived() const;
    /// Остаток неполученного количества.
    int remaining() const;
    /// Короткая сверка по строке для отчёта.
    std::string shortReport() const;

    Part* getPart() const { return part_; }
    int getQuantity() const { return quantity_; }
    int getUnitCostRub() const { return unitCostRub_; }
    int getLineTotalRub() const { return lineTotalRub_; }
    int getReceivedQty() const { return receivedQty_; }
    PurchaseOrder* getOrder() const { return order_; }
    void setOrder(PurchaseOrder& order) { order_ = &order; }
    void setQuantity(int qty) { quantity_ = qty; }

private:
    Part* part_ = nullptr;
    int quantity_ = 0;
    int unitCostRub_ = 0;
    int lineTotalRub_ = 0;
    int receivedQty_ = 0;
    PurchaseOrder* order_ = nullptr;
};

} // namespace autoservice

#endif
