#ifndef AUTOSERVICE_WORKSHOP_SCHEDULE_PLANNER_H
#define AUTOSERVICE_WORKSHOP_SCHEDULE_PLANNER_H

#include <string>
#include <vector>

namespace autoservice {

class Appointment;
class RepairOrder;
class Mechanic;
class ServiceBay;

/**
 * @brief Планировщик расписания на день: слоты, брони, блокировки.
 */
class SchedulePlanner {
public:
    SchedulePlanner();
    SchedulePlanner(std::string date, int slots, int capacityPerSlot);

    /// Забронировать слот под запись; конфликт — AppointmentConflictException.
    void book(Appointment& appointment);
    /// Снять бронь.
    void cancel(Appointment& appointment);
    /// Первый свободный слот дня (исключение — свободных нет).
    std::string findFreeSlot() const;
    /// Свободен ли слот времени.
    bool isSlotFree(const std::string& slot) const;
    /// Загрузка дня в процентах.
    int dayLoad() const;

    const std::string& getDate() const { return date_; }
    int getSlots() const { return slots_; }
    int getCapacityPerSlot() const { return capacityPerSlot_; }
    const std::vector<Appointment*>& getAppointments() const { return appointments_; }
    const std::vector<std::string>& getBlockedSlots() const { return blockedSlots_; }
    void blockSlot(const std::string& slot) { blockedSlots_.push_back(slot); }

private:
    std::string date_;
    int slots_ = 8;
    int capacityPerSlot_ = 1;
    std::vector<Appointment*> appointments_;
    std::vector<std::string> blockedSlots_;
};

} // namespace autoservice

#endif
