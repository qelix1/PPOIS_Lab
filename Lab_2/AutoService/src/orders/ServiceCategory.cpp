#include "autoservice/orders/ServiceCategory.h"

#include "autoservice/orders/Service.h"

namespace autoservice {

ServiceCategory::ServiceCategory() = default;

ServiceCategory::ServiceCategory(std::string id, std::string name, std::string description)
    : id_(std::move(id)), name_(std::move(name)), description_(std::move(description)) {}

void ServiceCategory::addService(Service& service) {
    for (Service* s : services_) {
        if (s == &service) return;
    }
    services_.push_back(&service);
}

void ServiceCategory::removeService(Service& service) {
    for (auto it = services_.begin(); it != services_.end(); ++it) {
        if (*it == &service) {
            services_.erase(it);
            return;
        }
    }
}

Service* ServiceCategory::findByCode(const std::string& code) const {
    for (Service* s : services_) {
        if (s && s->getCode() == code) return s;
    }
    return nullptr;
}

int ServiceCategory::totalPriceFrom() const {
    int total = 0;
    for (const Service* s : services_) {
        if (s) total += s->getBasePriceRub();
    }
    return total;
}

} // namespace autoservice
