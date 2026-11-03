#ifndef AUTOSERVICE_WORKSHOP_MARKETING_CAMPAIGN_H
#define AUTOSERVICE_WORKSHOP_MARKETING_CAMPAIGN_H

#include <string>

namespace autoservice {

class Discount;
class Client;

/**
 * @brief Маркетинговая акция: период, бюджет, привязанная скидка.
 */
class MarketingCampaign {
public:
    MarketingCampaign();
    MarketingCampaign(std::string id, std::string name, std::string startsAt, std::string endsAt, int budgetRub);

    /// Акция активна на указанную дату.
    bool isActive(const std::string& today) const;
    /// Списать средства из бюджета акции.
    void spend(int rub);
    /// Остаток бюджета.
    int remainingBudget() const;
    /// Статус акции словами: upcoming / active / ended.
    std::string status(const std::string& today) const;

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getStartsAt() const { return startsAt_; }
    const std::string& getEndsAt() const { return endsAt_; }
    int getBudgetRub() const { return budgetRub_; }
    int getSpentRub() const { return spentRub_; }
    Discount* getDiscount() const { return discount_; }
    void setDiscount(Discount& discount) { discount_ = &discount; }
    void setAudienceNote(const std::string& note) { audienceNote_ = note; }
    const std::string& getAudienceNote() const { return audienceNote_; }

private:
    std::string id_;
    std::string name_;
    std::string startsAt_;
    std::string endsAt_;
    int budgetRub_ = 0;
    int spentRub_ = 0;
    Discount* discount_ = nullptr;
    std::string audienceNote_;
};

} // namespace autoservice

#endif
