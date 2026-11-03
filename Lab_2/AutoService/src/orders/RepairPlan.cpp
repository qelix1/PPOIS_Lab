#include "autoservice/orders/RepairPlan.h"

#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/inventory/Part.h"
#include "autoservice/orders/WorkItem.h"
#include "autoservice/staff/Manager.h"

namespace autoservice {

RepairPlan::RepairPlan() = default;

RepairPlan::RepairPlan(std::string id, int priority, std::string createdAt)
    : id_(std::move(id)), priority_(priority), createdAt_(std::move(createdAt)) {}

void RepairPlan::addStep(WorkItem& step) {
    for (WorkItem* s : steps_) {
        if (s == &step) return;
    }
    steps_.push_back(&step);
}

void RepairPlan::addPart(Part& part) {
    for (Part* p : parts_) {
        if (p == &part) return;
    }
    parts_.push_back(&part);
}

void RepairPlan::reorder(int from, int to) {
    int size = static_cast<int>(steps_.size());
    if (from < 0 || from >= size || to < 0 || to >= size)
        throw WorkshopException("позиция этапа вне диапазона плана");
    std::iter_swap(steps_.begin() + from, steps_.begin() + to);
}

int RepairPlan::totalLaborHours() const {
    int hours = 0;
    for (const WorkItem* s : steps_) {
        if (s) hours += s->getPlannedHours();
    }
    return hours;
}

void RepairPlan::approve(Manager& manager) {
    if (steps_.empty()) throw WorkshopException("нельзя утвердить план без этапов");
    approvedBy_ = &manager;
}

} // namespace autoservice
