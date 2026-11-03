#ifndef AUTOSERVICE_STAFF_MANAGER_H
#define AUTOSERVICE_STAFF_MANAGER_H

#include <string>
#include <vector>

#include "autoservice/staff/Employee.h"

namespace autoservice {

class Discount;
class RepairOrder;
class Mechanic;
class Invoice;
class MarketingCampaign;

/**
 * @brief Менеджер мастерской: распределение работ, скидки, счета.
 */
class Manager : public Employee {
public:
    Manager();
    Manager(std::string id, std::string fullName, std::string region, int discountLimitPercent);

    /// Согласовать скидку в пределах полномочий; сверх лимита — исключение.
    void approveDiscount(Discount& discount);
    /// Назначить механика на заказ-наряд.
    void assignMechanic(RepairOrder& order, Mechanic& mechanic);
    /// Отменить заказ-наряд.
    void cancelOrder(RepairOrder& order);
    /// Выставить счёт к оплате.
    void releaseInvoice(Invoice& invoice);
    /// Запустить маркетинговую акцию с положительным бюджетом.
    void startCampaign(MarketingCampaign& campaign);
    /// Средняя продуктивность бригады механиков.
    double teamProductivity(const std::vector<Mechanic*>& mechanics) const;

    const std::string& getRegion() const { return region_; }
    int getDiscountLimitPercent() const { return discountLimitPercent_; }
    int getManagedClients() const { return managedClients_; }
    int getActiveOrders() const { return activeOrders_; }
    int getBonusPlanRub() const { return bonusPlanRub_; }

private:
    int managedClients_ = 0;
    int activeOrders_ = 0;
    int discountLimitPercent_ = 10;
    int bonusPlanRub_ = 0;
    std::string region_;
};

} // namespace autoservice

#endif
