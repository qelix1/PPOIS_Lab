#ifndef AUTOSERVICE_INVENTORY_STOCK_RESERVATION_H
#define AUTOSERVICE_INVENTORY_STOCK_RESERVATION_H

#include <string>

namespace autoservice {

class Part;
class RepairOrder;

/**
 * @brief Резерв запчасти под заказ-наряд со сроком годности брони.
 *
 * Статусы: active → confirmed | cancelled.
 */
class StockReservation {
public:
    StockReservation();
    StockReservation(std::string id, Part* part, int quantity, RepairOrder* order, std::string expiresAt);

    /// Подтвердить резерв: количество списывается со свободного остатка.
    void confirm();
    /// Отменить резерв (при подтверждённом — вернуть на остаток).
    void cancel();
    /// Срок резерва истёк на указанную дату.
    bool isExpired(const std::string& today) const;
    /// Продлить срок резерва.
    void extend(const std::string& newDate);

    const std::string& getId() const { return id_; }
    Part* getPart() const { return part_; }
    int getQuantity() const { return quantity_; }
    RepairOrder* getOrder() const { return order_; }
    const std::string& getExpiresAt() const { return expiresAt_; }
    const std::string& getStatus() const { return status_; }

private:
    std::string id_;
    Part* part_ = nullptr;
    int quantity_ = 0;
    RepairOrder* order_ = nullptr;
    std::string expiresAt_;
    std::string status_ = "active";
};

} // namespace autoservice

#endif
