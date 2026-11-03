#include "autoservice/orders/Fault.h"

#include <algorithm>

#include "autoservice/core/DateUtils.h"

namespace autoservice {

Fault::Fault() = default;

Fault::Fault(std::string code, std::string description, int severity, std::string unit, std::string foundAt)
    : code_(std::move(code)),
      description_(std::move(description)),
      severity_(severity < 1 ? 1 : (severity > 5 ? 5 : severity)),
      unit_(std::move(unit)),
      foundAt_(std::move(foundAt)) {}

void Fault::escalate() {
    severity_ = std::min(5, severity_ + 1);
}

void Fault::markFixed(const std::string& date) {
    fixedAt_ = date;
}

bool Fault::isCritical() const {
    return severity_ >= 4;
}

long Fault::daysOpen(const std::string& today) const {
    if (fixedAt_.empty()) return date::daysBetween(foundAt_, today);
    return date::daysBetween(foundAt_, fixedAt_);
}

void Fault::linkOrder(RepairOrder& order) {
    order_ = &order;
}

} // namespace autoservice
