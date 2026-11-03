#include "autoservice/workshop/ServiceBay.h"

#include "autoservice/exceptions/ServiceBayBusyException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/orders/RepairOrder.h"

namespace autoservice {

ServiceBay::ServiceBay() = default;

ServiceBay::ServiceBay(std::string number, std::string type, int maxWeightKg)
    : number_(std::move(number)), type_(std::move(type)), maxWeightKg_(maxWeightKg) {}

void ServiceBay::assign(RepairOrder& order) {
    if (isBusy()) throw ServiceBayBusyException("пост " + number_ + " уже занят");
    currentOrder_ = &order;
    status_ = "busy";
}

void ServiceBay::free() {
    currentOrder_ = nullptr;
    status_ = "free";
}

bool ServiceBay::isBusy() const {
    return status_ == "busy";
}

bool ServiceBay::canTake(const RepairOrder& order) const {
    (void)order;
    return !isBusy();
}

std::string ServiceBay::downtimeReport() const {
    throw WorkshopException("не реализован: отчёт по простою поста " + number_);
}

void ServiceBay::addEquipment(Equipment& equipment) {
    for (Equipment* e : equipment_) {
        if (e == &equipment) return;
    }
    equipment_.push_back(&equipment);
}

} // namespace autoservice
