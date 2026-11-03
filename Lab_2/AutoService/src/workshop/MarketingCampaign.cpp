#include "autoservice/workshop/MarketingCampaign.h"

#include "autoservice/exceptions/InvalidAmountException.h"

namespace autoservice {

MarketingCampaign::MarketingCampaign() = default;

MarketingCampaign::MarketingCampaign(std::string id, std::string name, std::string startsAt, std::string endsAt,
                                     int budgetRub)
    : id_(std::move(id)),
      name_(std::move(name)),
      startsAt_(std::move(startsAt)),
      endsAt_(std::move(endsAt)),
      budgetRub_(budgetRub) {}

bool MarketingCampaign::isActive(const std::string& today) const {
    return today >= startsAt_ && today <= endsAt_;
}

void MarketingCampaign::spend(int rub) {
    if (rub <= 0) throw InvalidAmountException("расход акции должен быть положительным");
    if (rub > remainingBudget()) throw InvalidAmountException("бюджет акции исчерпан");
    spentRub_ += rub;
}

int MarketingCampaign::remainingBudget() const {
    int left = budgetRub_ - spentRub_;
    return left > 0 ? left : 0;
}

std::string MarketingCampaign::status(const std::string& today) const {
    if (today < startsAt_) return "upcoming";
    if (isActive(today)) return "active";
    return "ended";
}

} // namespace autoservice
