#include "autoservice/workshop/EquipmentMaintenance.h"

#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

EquipmentMaintenance::EquipmentMaintenance() = default;

EquipmentMaintenance::EquipmentMaintenance(std::string id, std::string type, int costRub)
    : id_(std::move(id)), type_(std::move(type)), costRub_(costRub) {}

void EquipmentMaintenance::complete(const std::string& date) {
    if (type_.empty()) throw WorkshopException("не задан тип ТО оборудования");
    performedAt_ = date;
}

void EquipmentMaintenance::scheduleNext(const std::string& date) {
    nextDue_ = date;
}

bool EquipmentMaintenance::isOverdue(const std::string& today) const {
    return !nextDue_.empty() && today > nextDue_;
}

std::string EquipmentMaintenance::summarize() const {
    throw WorkshopException("не реализован: сводка ТО " + id_);
}

} // namespace autoservice
