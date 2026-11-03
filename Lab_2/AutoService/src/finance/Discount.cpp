#include "autoservice/finance/Discount.h"

#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

Discount::Discount() = default;

Discount::Discount(std::string code, int percent, std::string validUntil, int maxUses, int minOrderRub)
    : code_(std::move(code)),
      percent_(percent),
      validUntil_(std::move(validUntil)),
      maxUses_(maxUses),
      minOrderRub_(minOrderRub) {}

bool Discount::isValid(const std::string& today) const {
    if (blocked_) return false;
    if (maxUses_ > 0 && usedCount_ >= maxUses_) return false;
    if (!validUntil_.empty() && today > validUntil_) return false;
    return percent_ > 0;
}

void Discount::use() {
    if (!isValid(date::today())) throw WorkshopException("скидка " + code_ + " не может быть применена");
    ++usedCount_;
}

bool Discount::canApplyTo(int amountRub) const {
    return amountRub >= minOrderRub_;
}

int Discount::remainingUses() const {
    int left = maxUses_ - usedCount_;
    return left > 0 ? left : 0;
}

void Discount::block(const std::string& reason) {
    blocked_ = true;
    reason_ = reason;
}

} // namespace autoservice
