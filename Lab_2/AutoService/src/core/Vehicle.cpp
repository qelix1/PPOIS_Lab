#include "autoservice/core/Vehicle.h"

#include "autoservice/core/Client.h"
#include "autoservice/core/Engine.h"
#include "autoservice/core/Transmission.h"
#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/InvalidVinException.h"

namespace autoservice {
namespace {

// Транслитерация символов VIN по ISO 3779; недопустимый символ → -1.
int transliterate(char c) {
    static const char* letters = "ABCDEFGHJKLMNPRSTUVWXYZ";
    static const int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4, 5, 7, 9, 2, 3, 4, 5, 6, 7, 8, 9};
    if (c >= '0' && c <= '9') return c - '0';
    for (std::size_t i = 0; i < 23; ++i) {
        if (letters[i] == c) return values[i];
    }
    return -1;
}

// Проверка контрольной цифры VIN (позиция 9), алгоритм NHTSA/ISO 3779.
bool checkDigitValid(const std::string& vin) {
    static const int weights[17] = {8, 7, 6, 5, 4, 3, 2, 10, 0, 9, 8, 7, 6, 5, 4, 3, 2};
    int sum = 0;
    for (int i = 0; i < 17; ++i) {
        int value = transliterate(vin[static_cast<std::size_t>(i)]);
        if (value < 0) return false;
        sum += value * weights[i];
    }
    int remainder = sum % 11;
    char expected = remainder == 10 ? 'X' : static_cast<char>('0' + remainder);
    return vin[8] == expected;
}

} // namespace

Vehicle::Vehicle() = default;

Vehicle::Vehicle(std::string vin, int year, std::string color)
    : vin_(std::move(vin)), year_(year), color_(std::move(color)) {}

void Vehicle::validateVin() const {
    if (vin_.size() != 17) throw InvalidVinException("VIN должен содержать ровно 17 символов");
    for (char c : vin_) {
        if (transliterate(c) < 0) throw InvalidVinException("VIN содержит недопустимый символ (допустимы без I, O, Q)");
    }
    if (!checkDigitValid(vin_)) throw InvalidVinException("не совпала контрольная цифра VIN");
}

void Vehicle::increaseMileage(int km) {
    if (km <= 0) throw InvalidAmountException("пробег может только увеличиваться");
    mileageKm_ += km;
}

bool Vehicle::needsServiceByMileage(int intervalKm) const {
    if (intervalKm <= 0) throw InvalidAmountException("интервал ТО должен быть положительным");
    return mileageKm_ - lastServiceKm_ >= intervalKm;
}

bool Vehicle::isRoadworthy(int currentYear) const {
    return year_ >= currentYear - 20 && mileageKm_ < 500000;
}

double Vehicle::averageMonthlyMileage(int ownershipMonths) const {
    if (ownershipMonths <= 0) throw InvalidAmountException("срок владения должен быть положительным");
    return static_cast<double>(mileageKm_) / static_cast<double>(ownershipMonths);
}

void Vehicle::assignOwner(Client& owner) {
    owner_ = &owner;
    owner.registerVehicle(*this);
}

void Vehicle::attachEngine(Engine& engine) {
    engine_ = &engine;
}

void Vehicle::attachTransmission(Transmission& transmission) {
    transmission_ = &transmission;
}

} // namespace autoservice
