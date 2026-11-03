#include "autoservice/core/Transmission.h"

#include "autoservice/core/Vehicle.h"
#include "autoservice/exceptions/InvalidAmountException.h"

namespace autoservice {

Transmission::Transmission() = default;

Transmission::Transmission(std::string type, int gearCount, std::string driveType, std::string manufacturer,
                           int serviceIntervalKm)
    : type_(std::move(type)),
      gearCount_(gearCount),
      driveType_(std::move(driveType)),
      manufacturer_(std::move(manufacturer)),
      serviceIntervalKm_(serviceIntervalKm) {}

bool Transmission::isAutomatic() const {
    return type_ != "MT" && type_ != "МТ" && type_ != "Manual";
}

bool Transmission::maintenanceDue(int mileageKm) const {
    if (serviceIntervalKm_ <= 0) throw InvalidAmountException("интервал обслуживания КПП не задан");
    return mileageKm >= serviceIntervalKm_;
}

int Transmission::expectedLifespanKm() const {
    if (type_ == "CVT" || type_ == "Вариатор") return 150000;
    if (type_ == "AT" || type_ == "АТ") return 250000;
    if (type_ == "DCT" || type_ == "РКП") return 300000;
    return 400000;
}

std::string Transmission::describe() const {
    return type_ + " " + std::to_string(gearCount_) + " ст., " + driveType_ + " (" + manufacturer_ + ")";
}

void Transmission::linkVehicle(Vehicle& vehicle) {
    vehicle_ = &vehicle;
}

} // namespace autoservice
