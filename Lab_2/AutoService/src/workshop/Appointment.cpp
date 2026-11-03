#include "autoservice/workshop/Appointment.h"

#include "autoservice/core/DateUtils.h"
#include "autoservice/exceptions/InvalidOrderStatusException.h"
#include "autoservice/exceptions/WorkshopException.h"

namespace autoservice {

Appointment::Appointment() = default;

Appointment::Appointment(std::string id, std::string startsAt, std::string endsAt)
    : id_(std::move(id)), startsAt_(std::move(startsAt)), endsAt_(std::move(endsAt)) {}

void Appointment::reschedule(const std::string& newStart, const std::string& newEnd) {
    if (status_ != "booked")
        throw InvalidOrderStatusException("перенести можно только активную запись, статус: " + status_);
    startsAt_ = newStart;
    endsAt_ = newEnd;
}

void Appointment::cancel(const std::string& reason) {
    if (status_ == "done" || status_ == "cancelled")
        throw InvalidOrderStatusException("завершённую/отменённую запись нельзя отменить: " + status_);
    if (reason.empty()) throw WorkshopException("нужна причина отмены записи");
    cancelReason_ = reason;
    status_ = "cancelled";
}

void Appointment::checkIn() {
    if (status_ != "booked")
        throw InvalidOrderStatusException("заехать можно только по записи, статус: " + status_);
    status_ = "checked_in";
}

void Appointment::checkOut() {
    if (status_ != "checked_in")
        throw InvalidOrderStatusException("уехать можно только после заезда, статус: " + status_);
    status_ = "done";
}

int Appointment::durationMinutes() const {
    int startMinutes = date::minutesOfDateTime(startsAt_);
    int endMinutes = date::minutesOfDateTime(endsAt_);
    if (startMinutes < 0 || endMinutes < 0) return 0;
    long dayDiff = date::dayNumber(endsAt_) - date::dayNumber(startsAt_);
    int total = static_cast<int>(dayDiff) * 1440 + (endMinutes - startMinutes);
    return total > 0 ? total : 0;
}

bool Appointment::conflictsWith(const Appointment& other) const {
    if (date::dayNumber(startsAt_) != date::dayNumber(other.startsAt_)) return false;
    return startsAt_ < other.endsAt_ && other.startsAt_ < endsAt_;
}

} // namespace autoservice
