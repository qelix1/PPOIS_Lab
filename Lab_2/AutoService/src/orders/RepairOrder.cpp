#include "autoservice/orders/RepairOrder.h"

#include <algorithm>

#include "autoservice/exceptions/InvalidOrderStatusException.h"
#include "autoservice/orders/Estimate.h"
#include "autoservice/orders/WorkItem.h"

namespace autoservice {

RepairOrder::RepairOrder() = default;

RepairOrder::RepairOrder(std::string number, std::string createdAt, std::string deadlineAt)
    : number_(std::move(number)), createdAt_(std::move(createdAt)), deadlineAt_(std::move(deadlineAt)) {}

void RepairOrder::addWork(WorkItem& work) {
    for (WorkItem* w : works_) {
        if (w == &work) return;
    }
    works_.push_back(&work);
}

void RepairOrder::removeWork(const WorkItem& work) {
    for (auto it = works_.begin(); it != works_.end(); ++it) {
        if (*it == &work) {
            works_.erase(it);
            return;
        }
    }
}

int RepairOrder::calculateTotal() const {
    int total = 0;
    for (const WorkItem* w : works_) {
        if (w) total += w->getPriceRub();
    }
    return total;
}

void RepairOrder::changeStatus(const std::string& next) {
    static const std::pair<const char*, std::vector<const char*>> transitions[] = {
        {"new", {"in_progress", "cancelled"}},
        {"in_progress", {"on_hold", "closed", "cancelled"}},
        {"on_hold", {"in_progress", "cancelled", "closed"}},
        {"closed", {"in_progress"}},
        {"cancelled", {}},
    };
    for (const auto& tr : transitions) {
        if (status_ == tr.first) {
            if (std::find(tr.second.begin(), tr.second.end(), next) != tr.second.end()) {
                status_ = next;
                return;
            }
            throw InvalidOrderStatusException("недопустимый переход статуса: " + status_ + " -> " + next);
        }
    }
    throw InvalidOrderStatusException("неизвестный статус заказа: " + status_);
}

bool RepairOrder::isOverdue(const std::string& today) const {
    if (status_ == "closed" || status_ == "cancelled") return false;
    return !deadlineAt_.empty() && today > deadlineAt_;
}

void RepairOrder::assignMechanic(Mechanic& mechanic) {
    mechanic_ = &mechanic;
}

void RepairOrder::attachEstimate(Estimate& estimate) {
    estimate_ = &estimate;
    estimate.setOrder(*this);
}

void RepairOrder::close() {
    changeStatus("closed");
}

void RepairOrder::reopen() {
    changeStatus("in_progress");
}

} // namespace autoservice
