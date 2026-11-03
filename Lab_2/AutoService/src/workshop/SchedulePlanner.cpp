#include "autoservice/workshop/SchedulePlanner.h"

#include "autoservice/exceptions/AppointmentConflictException.h"
#include "autoservice/exceptions/WorkshopException.h"
#include "autoservice/workshop/Appointment.h"

namespace autoservice {

SchedulePlanner::SchedulePlanner() = default;

SchedulePlanner::SchedulePlanner(std::string date, int slots, int capacityPerSlot)
    : date_(std::move(date)), slots_(slots), capacityPerSlot_(capacityPerSlot) {}

void SchedulePlanner::book(Appointment& appointment) {
    if (!isSlotFree(appointment.getStartsAt()))
        throw AppointmentConflictException("слот " + appointment.getStartsAt() + " уже занят");
    for (Appointment* a : appointments_) {
        if (a->conflictsWith(appointment))
            throw AppointmentConflictException("запись пересекается с существующей: " + a->getId());
    }
    appointments_.push_back(&appointment);
}

void SchedulePlanner::cancel(Appointment& appointment) {
    for (auto it = appointments_.begin(); it != appointments_.end(); ++it) {
        if (*it == &appointment) {
            appointments_.erase(it);
            return;
        }
    }
}

bool SchedulePlanner::isSlotFree(const std::string& slot) const {
    if (slot.compare(0, date_.size(), date_) != 0) return false;
    for (const std::string& blocked : blockedSlots_) {
        if (blocked == slot) return false;
    }
    for (const Appointment* a : appointments_) {
        if (a && a->getStartsAt() == slot) return false;
    }
    int hour = 9;
    int minute = 0;
    std::size_t space = slot.rfind(' ');
    if (space != std::string::npos &&
        std::sscanf(slot.c_str() + space + 1, "%d:%d", &hour, &minute) == 2) {
        (void)minute;
        int slotIndex = hour - 9;
        if (slotIndex < 0 || slotIndex >= slots_) return false;
    }
    return true;
}

std::string SchedulePlanner::findFreeSlot() const {
    for (int i = 0; i < slots_; ++i) {
        int hour = 9 + i;
        char buf[6];
        std::snprintf(buf, sizeof(buf), "%02d:00", hour);
        std::string slot = date_ + " " + buf;
        if (isSlotFree(slot)) return slot;
    }
    throw WorkshopException("на " + date_ + " нет свободных слотов");
}

int SchedulePlanner::dayLoad() const {
    int capacity = slots_ * capacityPerSlot_;
    if (capacity <= 0) return 0;
    return static_cast<int>(100 * static_cast<long>(appointments_.size()) / capacity);
}

} // namespace autoservice
