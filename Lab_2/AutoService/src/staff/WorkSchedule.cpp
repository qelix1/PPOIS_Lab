#include "autoservice/staff/WorkSchedule.h"

#include "autoservice/staff/Employee.h"
#include "autoservice/staff/Shift.h"

namespace autoservice {

WorkSchedule::WorkSchedule() = default;

WorkSchedule::WorkSchedule(int weekNumber, std::string publishedAt, int version)
    : weekNumber_(weekNumber), publishedAt_(std::move(publishedAt)), version_(version) {}

void WorkSchedule::addShift(Shift& shift) {
    for (Shift* s : shifts_) {
        if (s == &shift) return;
    }
    shifts_.push_back(&shift);
}

void WorkSchedule::removeShift(Shift& shift) {
    for (auto it = shifts_.begin(); it != shifts_.end(); ++it) {
        if (*it == &shift) {
            shifts_.erase(it);
            return;
        }
    }
}

bool WorkSchedule::isEmployeeFree(Employee& employee, const std::string& day) const {
    for (const Shift* s : shifts_) {
        if (s && s->getEmployee() == &employee && s->getStartsAt().compare(0, day.size(), day) == 0) {
            return false;
        }
    }
    return true;
}

int WorkSchedule::hoursPerWeek(Employee& employee) const {
    int minutes = 0;
    for (const Shift* s : shifts_) {
        if (s && s->getEmployee() == &employee) minutes += s->durationMinutes();
    }
    return minutes / 60;
}

} // namespace autoservice
