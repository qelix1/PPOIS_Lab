#include "autoservice/finance/LoyaltyProgram.h"

#include <algorithm>

#include "autoservice/core/Client.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

LoyaltyProgram::LoyaltyProgram() = default;

LoyaltyProgram::LoyaltyProgram(std::string name, double pointsPerRub, int minRedeemPoints, int welcomeBonus)
    : name_(std::move(name)),
      pointsPerRub_(pointsPerRub),
      minRedeemPoints_(minRedeemPoints),
      welcomeBonus_(welcomeBonus) {}

int LoyaltyProgram::accrue(int spentRub) const {
    if (spentRub <= 0) throw InvalidAmountException("сумма покупки должна быть положительной");
    return static_cast<int>(static_cast<double>(spentRub) * pointsPerRub_);
}

bool LoyaltyProgram::redeem(int points, Client& client) {
    if (!active_) throw WorkshopException("программа лояльности отключена");
    if (points <= 0) throw InvalidAmountException("списание баллов должно быть положительным");
    if (points < minRedeemPoints_)
        throw WorkshopException("минимальное списание — " + std::to_string(minRedeemPoints_) + " баллов");
    client.spendBonus(points);
    return true;
}

std::string LoyaltyProgram::tierOf(int points) const {
    std::string best = "базовый";
    int bestThreshold = -1;
    for (const auto& tier : tiers_) {
        if (points >= tier.first && tier.first > bestThreshold) {
            bestThreshold = tier.first;
            best = tier.second;
        }
    }
    return best;
}

void LoyaltyProgram::deactivate() {
    active_ = false;
}

} // namespace autoservice
