#include "autoservice/staff/Mechanic.h"

#include <algorithm>

#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/MechanicNotAvailableException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/orders/RepairOrder.h"
#include "autoservice/orders/Service.h"
#include "autoservice/orders/WorkItem.h"
#include "autoservice/staff/Skill.h"

namespace autoservice {

Mechanic::Mechanic() = default;

Mechanic::Mechanic(std::string id, std::string fullName, int experienceYears, std::string specialization)
    : Employee(std::move(id), std::move(fullName), "", date::today(), 60000, "Механик", experienceYears),
      specialization_(std::move(specialization)) {}

void Mechanic::acceptOrder(RepairOrder& order) {
    if (isOverloaded(40)) throw MechanicNotAvailableException("механик перегружен: >40 часов в неделю");
    order.assignMechanic(*this);
    for (const WorkItem* w : order.getWorks()) {
        if (w) currentLoadHours_ += w->getPlannedHours();
    }
}

void Mechanic::completeWork(WorkItem& work) {
    work.complete(work.getPlannedHours());
    currentLoadHours_ = std::max(0, currentLoadHours_ - work.getPlannedHours());
    ++completedOrders_;
}

void Mechanic::addCertification(Skill& skill) {
    for (Skill* s : certifications_) {
        if (s == &skill) return;
    }
    certifications_.push_back(&skill);
}

bool Mechanic::isOverloaded(int maxHours) const {
    return currentLoadHours_ > maxHours;
}

double Mechanic::averageRepairTime() const {
    throw WorkshopException("не реализовано: нет учёта времени выполнения ремонтов");
}

bool Mechanic::canPerform(const Service& service) const {
    int best = 0;
    for (const Skill* s : certifications_) {
        if (s) best = std::max(best, s->getLevel());
    }
    return best >= service.getRequiredSkillLevel();
}

} // namespace autoservice
