#include "autoservice/orders/Service.h"

#include "autoservice/exceptions/InvalidAmountException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/orders/ServiceCategory.h"

namespace autoservice {

Service::Service() = default;

Service::Service(std::string code, std::string name, int basePriceRub, int durationHours,
                 int requiredSkillLevel, bool periodic)
    : code_(std::move(code)),
      name_(std::move(name)),
      basePriceRub_(basePriceRub),
      durationHours_(durationHours),
      requiredSkillLevel_(requiredSkillLevel),
      periodic_(periodic) {}

int Service::priceFor(int hours) const {
    if (hours <= 0) throw InvalidAmountException("трудоёмкость должна быть положительной");
    if (durationHours_ <= 0) return basePriceRub_;
    return static_cast<int>(static_cast<double>(basePriceRub_) * static_cast<double>(hours) /
                            static_cast<double>(durationHours_));
}

bool Service::requiresCertification(int mechanicSkillLevel) const {
    return requiredSkillLevel_ > mechanicSkillLevel;
}

bool Service::isDue(int sinceServiceKm, int intervalKm) const {
    if (intervalKm <= 0) throw InvalidAmountException("интервал услуги должен быть положительным");
    return periodic_ && sinceServiceKm >= intervalKm;
}

void Service::rename(const std::string& newName) {
    if (newName.empty()) throw WorkshopException("новое название услуги не может быть пустым");
    name_ = newName;
}

void Service::assignCategory(ServiceCategory& category) {
    category_ = &category;
    category.addService(*this);
}

} // namespace autoservice
