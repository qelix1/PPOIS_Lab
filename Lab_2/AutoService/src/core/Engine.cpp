#include "autoservice/core/Engine.h"

#include <algorithm>
#include <cctype>

#include "autoservice/core/Vehicle.h"
#include "autoservice/exceptions/InvalidAmountException.h"

namespace autoservice {

Engine::Engine() = default;

Engine::Engine(std::string engineCode, std::string type, double displacementL, int powerHp, int torqueNm,
               std::string fuelType)
    : engineCode_(std::move(engineCode)),
      type_(std::move(type)),
      displacementL_(displacementL),
      powerHp_(powerHp),
      torqueNm_(torqueNm),
      fuelType_(std::move(fuelType)) {}

double Engine::powerPerLiter() const {
    if (displacementL_ <= 0.0) return 0.0;
    return static_cast<double>(powerHp_) / displacementL_;
}

bool Engine::isTurbo() const {
    std::string lower = type_;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return lower.find("turbo") != std::string::npos || lower.find("tt") != std::string::npos;
}

bool Engine::needsOilChange(int sinceLastOilKm, int intervalKm) const {
    if (intervalKm <= 0) throw InvalidAmountException("интервал замены масла должен быть положительным");
    return sinceLastOilKm >= intervalKm;
}

std::string Engine::specSummary() const {
    return engineCode_ + ": " + type_ + ", " + std::to_string(powerHp_) + " hp, " + std::to_string(torqueNm_) +
           " Nm, " + fuelType_;
}

void Engine::linkVehicle(Vehicle& vehicle) {
    vehicle_ = &vehicle;
}

} // namespace autoservice
