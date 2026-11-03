#ifndef AUTOSERVICE_INVENTORY_PURCHASE_ORDER_H
#define AUTOSERVICE_INVENTORY_PURCHASE_ORDER_H

#include <string>
#include <vector>

namespace autoservice {

class Supplier;
class PurchaseOrderLine;
class Warehouse;
class Employee;

/**
 * @brief Заказ поставщику запчастей.
 *
 * Статусы: draft → ordered → received; можно отменить.
 */
class PurchaseOrder {
public:
    PurchaseOrder();
    PurchaseOrder(std::string number, std::string createdAt, std::string expectedAt);

    /// Добавить строку заказа.
    void addLine(PurchaseOrderLine& line);
    /// Убрать строку заказа.
    void removeLine(const PurchaseOrderLine& line);
    /// Пересчитать итоговую сумму заказа.
    void recalcTotal();
    /// Отправить заказ поставщику.
    void submit();
    /// Приять поставку: строки оприходуются на склад.
    void receive();
    /// Отменить заказ.
    void cancel();
    /// Поставка не принята и срок вышел.
    bool isLate(const std::string& today) const;

    const std::string& getNumber() const { return number_; }
    const std::string& getCreatedAt() const { return createdAt_; }
    const std::string& getExpectedAt() const { return expectedAt_; }
    const std::string& getStatus() const { return status_; }
    Supplier* getSupplier() const { return supplier_; }
    const std::vector<PurchaseOrderLine*>& getLines() const { return lines_; }
    int getTotalCostRub() const { return totalCostRub_; }
    Employee* getCreatedBy() const { return createdBy_; }
    void setSupplier(Supplier& supplier) { supplier_ = &supplier; }
    void setCreatedBy(Employee& employee) { createdBy_ = &employee; }

private:
    std::string number_;
    std::string createdAt_;
    std::string status_ = "draft";
    Supplier* supplier_ = nullptr;
    std::vector<PurchaseOrderLine*> lines_;
    std::string expectedAt_;
    int totalCostRub_ = 0;
    Employee* createdBy_ = nullptr;
};

} // namespace autoservice

#endif
