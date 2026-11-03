#include "autoservice/staff/Shift.h"

#include "autoservice/core/DateUtils.h"

namespace autoservice {

Shift::Shift() = default;

Shift::Shift(std::string startsAt, std::string endsAt, Employee* employee, ServiceBay* bay)
    : startsAt_(std::move(startsAt)), endsAt_(std::move(endsAt)), employee_(employee), bay_(bay) {}

bool Shift::overlapsWith(const Shift& other) const {
    if (date::dayNumber(startsAt_) != date::dayNumber(other.startsAt_)) return false;
    return startsAt_ < other.endsAt_ && other.startsAt_ < endsAt_;
}

int Shift::durationMinutes() const {
    int startMinutes = date::minutesOfDateTime(startsAt_);
    int endMinutes = date::minutesOfDateTime(endsAt_);
    if (startMinutes < 0 || endMinutes < 0) return 0;
    long dayDiff = date::dayNumber(endsAt_) - date::dayNumber(startsAt_);
    int total = static_cast<int>(dayDiff) * 1440 + (endMinutes - startMinutes);
    return total > 0 ? total : 0;
}

bool Shift::isFinished(const std::string& now) const {
    return now >= endsAt_;
}

} // namespace autoservice
