#include "autoservice/core/Client.h"

#include "autoservice/core/Vehicle.h"
#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/InvalidClientDataException.h"
#include "autoservice/finance/Discount.h"
#include "autoservice/finance/LoyaltyProgram.h"

namespace autoservice {

Client::Client() : Client("", "", "", "") {}

Client::Client(std::string id, std::string fullName, std::string phone, std::string email)
    : id_(std::move(id)),
      fullName_(std::move(fullName)),
      phone_(std::move(phone)),
      email_(std::move(email)),
      registeredAt_(date::today()) {}

void Client::registerVehicle(Vehicle& vehicle) {
    for (Vehicle* v : vehicles_) {
        if (v == &vehicle) return;
    }
    vehicles_.push_back(&vehicle);
}

void Client::addBonus(int points) {
    if (points <= 0) throw InvalidAmountException("бонусы могут начисляться только положительным числом");
    bonusPoints_ += points;
}

void Client::spendBonus(int points) {
    if (points <= 0) throw InvalidAmountException("сумма списания должна быть положительной");
    if (points > bonusPoints_) throw InvalidAmountException("недостаточно бонусных баллов");
    bonusPoints_ -= points;
}

void Client::updateContact(const std::string& phone, const std::string& email) {
    if (phone.empty() && email.empty()) throw InvalidClientDataException("контакты клиента не могут быть пустыми");
    if (!phone.empty()) phone_ = phone;
    if (!email.empty()) email_ = email;
}

bool Client::isLoyal(int minPoints) const {
    return bonusPoints_ >= minPoints;
}

std::string Client::loyaltyTier(const LoyaltyProgram& program) const {
    return program.tierOf(bonusPoints_);
}

bool Client::canUseDiscount(const Discount& discount, const std::string& today) const {
    return status_ == "active" && discount.isValid(today);
}

} // namespace autoservice
