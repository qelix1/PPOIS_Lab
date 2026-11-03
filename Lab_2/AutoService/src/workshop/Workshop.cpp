#include "autoservice/workshop/Workshop.h"

#include "autoservice/core/DateUtils.h"
#include "autoservice/workshop/Appointment.h"
#include "autoservice/workshop/ServiceBay.h"

namespace autoservice {

Workshop::Workshop() = default;

Workshop::Workshop(std::string id, std::string name, std::string address, std::string phone,
                   std::string workingHours)
    : id_(std::move(id)),
      name_(std::move(name)),
      address_(std::move(address)),
      phone_(std::move(phone)),
      workingHours_(std::move(workingHours)) {}

void Workshop::addBay(ServiceBay& bay) {
    for (ServiceBay* b : bays_) {
        if (b == &bay) return;
    }
    bays_.push_back(&bay);
    bay.setWorkshop(*this);
}

void Workshop::hire(Employee& employee) {
    for (Employee* e : staff_) {
        if (e == &employee) return;
    }
    staff_.push_back(&employee);
}

int Workshop::freeBays() const {
    int free = 0;
    for (const ServiceBay* b : bays_) {
        if (b && !b->isBusy()) ++free;
    }
    return free;
}

double Workshop::loadFactor() const {
    if (bays_.empty()) return 0.0;
    int busy = 0;
    for (const ServiceBay* b : bays_) {
        if (b && b->isBusy()) ++busy;
    }
    return static_cast<double>(busy) / static_cast<double>(bays_.size());
}

bool Workshop::isWorkingAt(const std::string& dateTime) const {
    // workingHours_ в формате "HH:MM-HH:MM"
    std::size_t dash = workingHours_.find('-');
    if (dash == std::string::npos) return false;
    int openMin = date::minutesOfDateTime("0000-01-01 " + workingHours_.substr(0, dash));
    int closeMin = date::minutesOfDateTime("0000-01-01 " + workingHours_.substr(dash + 1));
    int nowMin = date::minutesOfDateTime(dateTime);
    if (openMin < 0 || closeMin < 0 || nowMin < 0) return false;
    return nowMin >= openMin && nowMin < closeMin;
}

int Workshop::capacityPerDay() const {
    return static_cast<int>(bays_.size()) * 2;
}

void Workshop::scheduleAppointment(Appointment& appointment) {
    for (Appointment* a : appointments_) {
        if (a == &appointment) return;
    }
    appointments_.push_back(&appointment);
}

} // namespace autoservice
