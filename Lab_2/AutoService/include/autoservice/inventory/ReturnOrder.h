#ifndef AUTOSERVICE_INVENTORY_RETURN_ORDER_H
#define AUTOSERVICE_INVENTORY_RETURN_ORDER_H

#include <string>
#include <vector>

namespace autoservice {

class Supplier;
class Part;
class PurchaseOrder;

/**
 * @brief Возврат запчастей поставщику (в течение срока возврата).
 *
 * Статусы: draft → submitted.
 */
class ReturnOrder {
public:
    ReturnOrder();
    ReturnOrder(std::string number, std::string createdAt, std::string reason);

    /// Добавить запчасть в возврат.
    void addPart(Part& part);
    /// Отправить возврат поставщику.
    void submit();
    /// Возврат ещё допустим по сроку (14 дней от даты создания).
    bool isReturnable(const std::string& today) const;
    /// Сумма возврата по закупочным ценам запчастей.
    int refundAmount() const;

    const std::string& getNumber() const { return number_; }
    const std::string& getCreatedAt() const { return createdAt_; }
    const std::string& getReason() const { return reason_; }
    const std::string& getStatus() const { return status_; }
    Supplier* getSupplier() const { return supplier_; }
    const std::vector<Part*>& getParts() const { return parts_; }
    void setSupplier(Supplier& supplier) { supplier_ = &supplier; }
    void setPurchaseOrder(PurchaseOrder& order) { purchaseOrder_ = &order; }

private:
    std::string number_;
    std::string createdAt_;
    std::string reason_;
    std::string status_ = "draft";
    Supplier* supplier_ = nullptr;
    std::vector<Part*> parts_;
    PurchaseOrder* purchaseOrder_ = nullptr;
};

} // namespace autoservice

#endif
