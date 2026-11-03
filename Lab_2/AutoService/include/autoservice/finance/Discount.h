#ifndef AUTOSERVICE_FINANCE_DISCOUNT_H
#define AUTOSERVICE_FINANCE_DISCOUNT_H

#include <string>

namespace autoservice {

class Client;
class Estimate;
class Invoice;

/**
 * @brief Скидка: процент, срок действия, лимит использований, мин. сумма заказа.
 */
class Discount {
public:
    Discount();
    Discount(std::string code, int percent, std::string validUntil, int maxUses, int minOrderRub);

    /// Скидка действительна на указанную дату (не блок, лимит и срок соблюдены).
    bool isValid(const std::string& today) const;
    /// Израсходовать одно применение скидки.
    void use();
    /// Скидка применима к сумме заказа.
    bool canApplyTo(int amountRub) const;
    /// Осталось ли использований.
    int remainingUses() const;
    /// Заблокировать скидку с причиной.
    void block(const std::string& reason);

    const std::string& getCode() const { return code_; }
    int getPercent() const { return percent_; }
    const std::string& getValidUntil() const { return validUntil_; }
    int getMaxUses() const { return maxUses_; }
    int getUsedCount() const { return usedCount_; }
    int getMinOrderRub() const { return minOrderRub_; }
    const std::string& getReason() const { return reason_; }
    bool getBlocked() const { return blocked_; }

private:
    std::string code_;
    int percent_ = 0;
    std::string validUntil_;
    int maxUses_ = 0;
    int usedCount_ = 0;
    int minOrderRub_ = 0;
    std::string reason_;
    bool blocked_ = false;
};

} // namespace autoservice

#endif
