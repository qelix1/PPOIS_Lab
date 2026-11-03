#ifndef AUTOSERVICE_CORE_CLIENT_H
#define AUTOSERVICE_CORE_CLIENT_H

#include <string>
#include <vector>

namespace autoservice {

class Vehicle;
class Discount;
class LoyaltyProgram;

/**
 * @brief Клиент автомастерской: контакты, автомобили, бонусные баллы.
 */
class Client {
public:
    Client();
    Client(std::string id, std::string fullName, std::string phone, std::string email);

    /// Привязать автомобиль к клиенту (связь «клиент владеет автомобилем»).
    void registerVehicle(Vehicle& vehicle);
    /// Начислить бонусные баллы за выполненный ремонт.
    void addBonus(int points);
    /// Списать бонусные баллы (смена скидки); при нехватке — исключение.
    void spendBonus(int points);
    /// Обновить контакты; оба пустых поля — ошибка данных клиента.
    void updateContact(const std::string& phone, const std::string& email);
    /// Клиент считается лояльным при наличии хотя бы minPoints баллов.
    bool isLoyal(int minPoints) const;
    /// Уровень в программе лояльности по текущему балансу.
    std::string loyaltyTier(const LoyaltyProgram& program) const;
    /// Можно ли применить скидку сегодня: она активна и клиент не заблокирован.
    bool canUseDiscount(const Discount& discount, const std::string& today) const;

    int getBonusPoints() const { return bonusPoints_; }
    const std::string& getId() const { return id_; }
    const std::string& getFullName() const { return fullName_; }
    const std::string& getPhone() const { return phone_; }
    const std::string& getEmail() const { return email_; }
    const std::string& getRegisteredAt() const { return registeredAt_; }
    const std::string& getStatus() const { return status_; }
    std::size_t getVehicleCount() const { return vehicles_.size(); }

private:
    std::string id_;
    std::string fullName_;
    std::string phone_;
    std::string email_;
    std::string registeredAt_;
    int bonusPoints_ = 0;
    std::string status_ = "active";
    std::string notes_;
    std::vector<Vehicle*> vehicles_;
};

} // namespace autoservice

#endif
