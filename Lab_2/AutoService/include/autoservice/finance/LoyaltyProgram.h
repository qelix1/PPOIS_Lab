#ifndef AUTOSERVICE_FINANCE_LOYALTY_PROGRAM_H
#define AUTOSERVICE_FINANCE_LOYALTY_PROGRAM_H

#include <string>
#include <utility>
#include <vector>

namespace autoservice {

class Client;
class Discount;

/**
 * @brief Программа лояльности: баллы за покупки, уровни, списание.
 */
class LoyaltyProgram {
public:
    LoyaltyProgram();
    LoyaltyProgram(std::string name, double pointsPerRub, int minRedeemPoints, int welcomeBonus);

    /// Начислить баллы за потраченную сумму.
    int accrue(int spentRub) const;
    /// Списать баллы клиента (минимум и достаточный баланс).
    bool redeem(int points, Client& client);
    /// Уровень клиента по количеству баллов.
    std::string tierOf(int points) const;
    /// Отключить программу.
    void deactivate();

    const std::string& getName() const { return name_; }
    double getPointsPerRub() const { return pointsPerRub_; }
    int getMinRedeemPoints() const { return minRedeemPoints_; }
    bool getActive() const { return active_; }
    int getWelcomeBonus() const { return welcomeBonus_; }
    const std::vector<std::pair<int, std::string>>& getTiers() const { return tiers_; }
    void addTier(int threshold, const std::string& name) { tiers_.emplace_back(threshold, name); }
    void setDiscount(Discount& discount) { discount_ = &discount; }
    Discount* getDiscount() const { return discount_; }

private:
    std::string name_;
    double pointsPerRub_ = 1.0;
    std::vector<std::pair<int, std::string>> tiers_;
    int minRedeemPoints_ = 100;
    bool active_ = true;
    int welcomeBonus_ = 0;
    Discount* discount_ = nullptr;
};

} // namespace autoservice

#endif
